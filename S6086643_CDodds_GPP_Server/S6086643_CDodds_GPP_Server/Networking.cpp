#include "Networking.h"
#include <sstream>
#include <string>

Networking::Networking(ThreadSafeQueue<std::string>& incomingMessageQueue) : incomingsQueue(incomingMessageQueue)
{
	hint->sin_family = AF_INET;
	hint->sin_port = htons(54000);
	hint->sin_addr.S_un.S_addr = INADDR_ANY;

	threads = new std::vector<std::thread*>();
	sockets = new std::vector<SOCKET*>();

	serverRunning = new std::atomic<bool>(true);
	socketsConnected = new ThreadSafeQueue<SOCKET*>();
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
	threads->push_back(new std::thread(&Networking::GameLoop, this));
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
	while(this->serverRunning && clientIsActive)
	{
		ZeroMemory(&buffer, 4096);

		int bytesReceived = recv(*this->sockets->at(socketNumber), buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV \n" << WSAGetLastError() << "\n";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "quitting";
			clientIsActive = false;
			this->sockets->erase(this->sockets->begin() + (socketNumber - 1));
		}
		else
		{
			bytesReceived++;
			//std::ostringstream outMessage;
			//outMessage << socketNumber << ": " << buffer;
			incomingsQueue.push(std::string(buffer, 0, bytesReceived));
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
			sockets->push_back(newSocketChecker);
			threads->push_back(new std::thread([this] { OneSocketReceive(sockets->size() - 1); }));
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
