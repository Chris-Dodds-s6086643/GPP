#include "Networking.h"
#include <sstream>
#include <string>
#include "Message.h"


Networking::Networking(ThreadSafeQueue<std::string>& incomingMessageQueue) : incomingsQueue(incomingMessageQueue)
{
	hint->sin_family = AF_INET;
	hint->sin_port = htons(54000);
	hint->sin_addr.S_un.S_addr = INADDR_ANY;

	threads = new std::vector<std::thread*>();
	sockets = new std::vector<SOCKET*>();

	serverRunning = new std::atomic<bool>(true);
	socketsConnected = new ThreadSafeQueue<SOCKET*>();
	activeStatus.push_back(new std::atomic<bool>(true));

	playersMapHashFn = playersMap.hash_function();
	playersMapHashFnTheSecond = playersMapTheSecond.getHashFunction();
}

Networking::~Networking()
{
	delete(threads);
	delete(sockets);
	delete(serverRunning);
	delete(socketsConnected);
	delete(winSockData);
	delete(winSockVersion);
	delete(listener);
	delete(hint);
	delete(client);
	delete(clientSize);
	delete(clientSocket);
}

bool Networking::InitialiseServer()
{
	*winSockVersion = MAKEWORD(2, 2);
	int windowsSocketCheck = WSAStartup(*winSockVersion, winSockData);
	if (windowsSocketCheck != 0)
	{
		std::cerr << "winsock failed to initialise";
		return false;
	}
	listener = new SOCKET(socket(AF_INET, SOCK_STREAM, 0));
	sockets->push_back(listener);
	if (*sockets->at((int)socketSecurityEnum::listeningSocket) == INVALID_SOCKET)
	{
		std::cerr << "Can't Create a Socket";
		return false;
	}
	std::cout << "sizeof(*hint) " << sizeof(*hint) << "\n";


	int result = bind(*listener, (sockaddr*) hint, sizeof(*hint));
	if (result == SOCKET_ERROR)
	{
		std::cout << "socket error in binding: \n" << WSAGetLastError() << "\n";
		return 0;
	}
	//std::thread threadTheFirst([this] { listenForConnections2(*this->serverRunning, *this->socketsConnected, *this->listener); });
	threads->push_back(new std::thread (&Networking::listenForConnections2, this));
	//threadTheFirst.join();
	//this->ListenForConnections();
	threads->push_back(new std::thread(&Networking::GameLoopTheThird, this));
}

void Networking::ListenForConnections()
{
	listen(*listener, SOMAXCONN);
	clientSize = new int(sizeof(*client));
	int numSockets = 0;
	clientSocket = new SOCKET(accept(*listener, (sockaddr*) client, clientSize));
	if (*clientSocket == INVALID_SOCKET)
	{
		std::cout << "LAST ERROR \n" << WSAGetLastError() << "\n";
		return;
	}
	else
	{
		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (getnameinfo((sockaddr*) client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
		{
			std::cout << "Connected On Port " << service << std::endl;
		}
		else
		{
			inet_ntop(AF_INET, &client->sin_addr, host, NI_MAXHOST);
			std::cout << host << " Connected on port " << ntohs(client->sin_port) << "\n";
		}
		numSockets++;
	}
	Listen();
}

void Networking::listenForConnections2()
{
	char newHost[NI_MAXHOST];
	char newService[NI_MAXSERV];
	int numSockets = 0;
	while (*serverRunning)
	{
		listen(*listener, SOMAXCONN);
		sockaddr_in newClient;
		int newClientSize = sizeof(newClient);
		//sockaddr_in* client = new sockaddr_in();
		SOCKET newClientSocket = accept(*listener, (sockaddr*) &newClient, &newClientSize);
		if (newClientSocket == INVALID_SOCKET)
		{
			std::cerr << "ERROR WITH THE Client 'accept' function\n" << WSAGetLastError() << "\n";
		}
		else
		{
			ZeroMemory(host, NI_MAXHOST);
			ZeroMemory(service, NI_MAXSERV);

			if (getnameinfo((sockaddr*)& newClient, sizeof(newClient), newHost, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			{
				std::cout << "Connected On Port " << service << std::endl;
			}
			else
			{
				inet_ntop(AF_INET, &newClient.sin_addr, host, NI_MAXHOST);
				std::cout << host << " Connected on port " << ntohs(newClient.sin_port) << "\n";
			}
			socketsConnected->push(new SOCKET(newClientSocket));
			numSockets++;
		}
	}
}


void Networking::Listen()
{

	while (true)
	{
		ZeroMemory(&buffer, 4096);
		/*std::cout << "listening\n";*/
		int bytesReceived = recv(*clientSocket, buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV \n" << WSAGetLastError() << "\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "quitting";
			break;
		}
		bytesReceived += 1;
		std::cout << std::string(buffer, 0, bytesReceived) << "\n";
		

		send(*clientSocket, buffer, bytesReceived, 0);
	}

	closesocket(*clientSocket);

	WSACleanup();
}

void Networking::OneSocketReceive(int socketNumber)
{
	char buffer[4096];
	bool clientIsActive = true;

	std::ostringstream idMessage;
	idMessage << "ID::" << socketNumber;
	std::string idMessageString = idMessage.str();
	send(*this->sockets->at(socketNumber), idMessageString.c_str(), idMessageString.length(), 0);
	while(this->serverRunning && (this->activeStatus.at(socketNumber)->load() == true))
	{
		ZeroMemory(&buffer, 4096);
		if ((*this->activeStatus.at(socketNumber)) == false)
		{
			std::cout << "quitting";

		}
		int bytesReceived = recv(*this->sockets->at(socketNumber), buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV \n" << WSAGetLastError() << "\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "0 bytes received";
		}
		else
		{
			bytesReceived++;
			//std::ostringstream outMessage;
			//outMessage << socketNumber << ": " << buffer;
			std::string incomingString = std::string(buffer, 0, bytesReceived);
			incomingsQueue.push(incomingString);
			if (incomingString.find("QUIT"))
			{
				std::cout << "ending socket: " << socketNumber << "\n";
				delete(this->sockets->at(socketNumber));
				this->sockets->erase(this->sockets->begin() + (socketNumber));
				//delete(this->activeStatus.at(socketNumber));
				//this->activeStatus.erase(activeStatus.begin() + (socketNumber - 1));
				return;
			}
		}

		//std::cout << std::string(buffer, 0, bytesReceived) << "\n";

		//send(*this->sockets->at(socketNumber), buffer, bytesReceived, 0);
	}
}

void Networking::OneSocketReceiveTheSecond(int socketNumber)
{
	char buffer[4096];
	bool clientIsActive = true;

	std::ostringstream idMessage;
	idMessage << "ID::" << socketNumber;
	std::string idMessageString = idMessage.str();
	send(this->playersMap[socketNumber]->getPlayerSocket(), idMessageString.c_str(), idMessageString.length(), 0);
	while (this->serverRunning)
	{
		ZeroMemory(&buffer, 4096);
		if (this->playersMap[socketNumber]->getIsPlayerActive() == false)
		{
			std::cout << "quitting";

		}
		int bytesReceived = recv(this->playersMap[socketNumber]->getPlayerSocket(), buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV \n" << WSAGetLastError() << "\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "0 bytes received";
		}
		else
		{
			bytesReceived++;
			//std::ostringstream outMessage;
			//outMessage << socketNumber << ": " << buffer;
			std::string incomingString = std::string(buffer, 0, bytesReceived);
			incomingsQueue.push(incomingString);
			if (Message::SplitString(incomingString).at(1) == "QUIT")
			{
				std::cout << "ending socket: " << socketNumber << "\n";
				threadSafePlayerKeysToRemove.push(socketNumber);
				playersMap[socketNumber]->setPlayerInactive();
				//delete(this->activeStatus.at(socketNumber));
				//this->activeStatus.erase(activeStatus.begin() + (socketNumber - 1));
				return;
			}
		}

		//std::cout << std::string(buffer, 0, bytesReceived) << "\n";

		//send(*this->sockets->at(socketNumber), buffer, bytesReceived, 0);
	}
}

void Networking::OneSocketReceiveTheThird(int socketNumber)
{
	char buffer[4096];
	bool clientIsActive = true;

	std::ostringstream idMessage;
	idMessage << "ID::" << socketNumber;
	std::string idMessageString = idMessage.str();
	send(this->playersMapTheSecond.accessValue(socketNumber)->getPlayerSocket(), idMessageString.c_str(), idMessageString.length(), 0);
	while (this->serverRunning)
	{
		ZeroMemory(&buffer, 4096);
		if (this->playersMapTheSecond.accessValue(socketNumber)->getIsPlayerActive() == false)
		{
			std::cout << "quitting";

		}
		int bytesReceived = recv(this->playersMapTheSecond.accessValue(socketNumber)->getPlayerSocket(), buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV \n" << WSAGetLastError() << "\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "0 bytes received";
		}
		else
		{
			bytesReceived++;
			//std::ostringstream outMessage;
			//outMessage << socketNumber << ": " << buffer;
			std::string incomingString = std::string(buffer, 0, bytesReceived);
			incomingsQueue.push(incomingString);
			std::vector<std::string> splitString = Message::SplitString(incomingString);
			if (splitString.size() >= 1)
			{
				if (Message::SplitString(incomingString).at(1) == "QUIT")
				{
					std::cout << "ending socket: " << socketNumber << "\n";
					threadSafePlayerKeysToRemove.push(socketNumber);
					this->playersMapTheSecond.accessValue(socketNumber)->setPlayerInactive();
					//delete(this->activeStatus.at(socketNumber));
					//this->activeStatus.erase(activeStatus.begin() + (socketNumber - 1));
					return;
				}
			}
		}

		//std::cout << std::string(buffer, 0, bytesReceived) << "\n";

		//send(*this->sockets->at(socketNumber), buffer, bytesReceived, 0);
	}
}

void Networking::GameLoop()
{
	while(*serverRunning)
	{
		while (!socketsConnected->isEmpty())
		{
			SOCKET* newSocketChecker = new SOCKET();
			socketsConnected->tryPop(newSocketChecker);
			activeStatus.push_back(new std::atomic<bool>(true));
			sockets->push_back(newSocketChecker);
			threads->push_back(new std::thread([this] { OneSocketReceive(sockets->size() - 1); }));
			std::cout << "Socket ID: " << sockets->size() - 1 << " running on thread: " << threads->back()->get_id() << "\n";
		}

		for (int i = 1; i <= activeStatus.size() - 1; i++)
		{
			if (*activeStatus.at(i) == false)
			{
				threads->at(i + 1)->join();
				delete(this->activeStatus.at(i));
				//activeStatus.shrink_to_fit();
				activeStatus.erase(activeStatus.begin() + (i - 1));
				std::cout << "ThreadBeingDeletedID" << threads->at(i + 1)->get_id() << "\n";
				delete(threads->at(i + 1));
				threads->erase(threads->begin() + (i));
			}
		}
		
		//while (!incomingsQueue.isEmpty())
		//{
		//	std::string outString;
		//	incomingsQueue.tryPop(outString);
		//	std::cout << outString << "\n";
		//	for (size_t i = 1; i < sockets->size(); i++)
		//	{
		//		if (std::stoi(outString.substr(0,1)) != i)
		//		{
		//			send(*sockets->at(i), outString.c_str(), (outString.length() + 1), 0);
		//		}
		//	}
		//}

	}
}

void Networking::GameLoopTheSecond()
{
	while (*serverRunning)
	{
		while (!socketsConnected->isEmpty())
		{
			SOCKET* newSocketChecker = new SOCKET();
			socketsConnected->tryPop(newSocketChecker);
			int newID = 0;
			while (playersMap.find(newID) != playersMap.end())
			{
				newID++;
			}
			playersMap.emplace(newID, new ClientPlayer(newSocketChecker, new std::thread([this, newID] { OneSocketReceiveTheSecond(newID); }), playersMapHashFn(newID)));
			std::cout << "Socket ID: " << sockets->size() - 1 << " running on thread: " << threads->back()->get_id() << "\n";
		}

		//somehow close and delete any threads and or players that have closed the connection;

		while (!threadSafePlayerKeysToRemove.isEmpty())
		{
			int key = 0;
			threadSafePlayerKeysToRemove.tryPop(key);
			std::cout << key << ": is being handled: " << playersMap[key]->getPlayerSocket() << "\n";
			delete(playersMap[key]);
			playersMap.erase(key);
		}
	}
}

void Networking::GameLoopTheThird()
{
	while (*serverRunning)
	{
		while (!socketsConnected->isEmpty())
		{
			SOCKET* newSocketChecker = new SOCKET();
			socketsConnected->tryPop(newSocketChecker);
			int newID = playersMapTheSecond.getNextAvailableKeyId();
			//playersMap.emplace(newID, new ClientPlayer(newSocketChecker, new std::thread([this, newID] { OneSocketReceiveTheSecond(newID); }), playersMapHashFn(newID)));
			playersMapTheSecond.emplace(newID, new ClientPlayer(newSocketChecker, new std::thread([this, newID] { OneSocketReceiveTheThird(newID); }), playersMapHashFnTheSecond(newID)));
			//std::cout << "Socket ID: " << sockets->size() - 1 << " running on thread: " << threads->back()->get_id() << "\n";
		}

		//somehow close and delete any threads and or players that have closed the connection;

		while (!threadSafePlayerKeysToRemove.isEmpty())
		{
			int key = 0;
			threadSafePlayerKeysToRemove.tryPop(key);
			std::cout << key << ": is being handled: " << playersMapTheSecond.accessValue(key)->getPlayerSocket() << "\n";
			delete(playersMapTheSecond.accessValue(key));
			playersMapTheSecond.removeValueAtKey(key);
		}
	}
}


void Networking::EchoMessage(int senderID, std::string message)
{
	//starts at 1 as the 0 slot is used for the listening socket and therefore does not need to be sent messages.
	for (size_t i = 1; i < sockets->size(); i++)
	{
		if (senderID != i)
		{
			send(*sockets->at(i), message.c_str(), (message.length() + 1), 0);
		}
	}
}

void Networking::EchoMessageTheSecond(int senderID, std::string message)
{
	//starts at 1 as the 0 slot is used for the listening socket and therefore does not need to be sent messages.
	for (size_t i = 1; i < playersMap.size(); i++)
	{
		if (senderID != i)
		{
			send(this->playersMapTheSecond.accessValue(i)->getPlayerSocket(), message.c_str(), (message.length() + 1), 0);
		}
	}
	
}

void Networking::EchoMessageTheThird(int senderID, std::string message)
{
	//starts at 1 as the 0 slot is used for the listening socket and therefore does not need to be sent messages.
	std::function<void(ClientPlayer * cp, std::string message)> sendFunc = [](ClientPlayer* cp, std::string messageToSend)
	{
		send(cp->getPlayerSocket(), messageToSend.c_str(), (messageToSend.length() + 1), 0);
	};

	playersMapTheSecond.functionOverMap(sendFunc, message, senderID);

}


void Networking::sendToClient(ClientPlayer* client, std::string message)
{
	send(client->getPlayerSocket(), message.c_str(), (message.length() + 1), 0);
}

void Networking::setActive(int ID)
{
	while (!activeStatus.at(ID)->is_lock_free())
	{

	}
	activeStatus.at(ID)->store(false);
}

void Networking::setActiveTheSecond(int ID)
{
	//if (playersMap.at(ID)->getIsPlayerActive())
	//{
	//	playersMap.at(ID)->setPlayerInactive();
	//}
}
