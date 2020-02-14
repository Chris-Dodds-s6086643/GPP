#include "Server.h"

Server::Server()
{
	hint->sin_family = AF_INET;
	hint->sin_port = htons(54000);
	hint->sin_addr.S_un.S_addr = INADDR_ANY;

	threads = new std::vector<std::thread*>();
	sockets = new std::vector<SOCKET*>();

	serverRunning = new std::atomic<bool>(true);
	socketsConnected = new ThreadSafeQueue<SOCKET*>();

	sockets = new std::vector<SOCKET*>();
	threads = new std::vector<std::thread*>();
}

bool Server::InitialiseServer()
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
	std::thread threadTheFirst(&Server::listenForConnections2, this);
	threadTheFirst.join();
	//this->ListenForConnections();
	GameLoop();
}

void Server::ListenForConnections()
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

void Server::listenForConnections2()
{
	listen(*listener, SOMAXCONN);
	char newHost[NI_MAXHOST];
	char newService[NI_MAXSERV];
	int numSockets = 0;
	while (*serverRunning && numSockets < 2)
	{
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
	std::cout << "TWO CONNECTIONS ACCOMPLISHED: " << socketsConnected << "\n";
}


void Server::Listen()
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

void Server::OneSocketReceive(int socketNumber)
{
	char buffer[4096];
	bool clientIsActive = true;

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
			break;
		}
		bytesReceived++;

		std::cout << std::string(buffer, 0, bytesReceived) << "\n";

		send(*this->sockets->at(socketNumber), buffer, bytesReceived, 0);
	}
}

void Server::GameLoop()
{
	while(*serverRunning)
	{
		while (!socketsConnected->isEmpty())
		{
			SOCKET* newSocketChecker = new SOCKET();
			socketsConnected->tryPop(newSocketChecker);
			sockets->push_back(newSocketChecker);
			int sizeOfSockets = sockets->size();
			threads->push_back(new std::thread([this] { OneSocketReceive(sockets->size() - 1); }));
			//std::thread threadTheFirst(&Server::listenForConnections2, this);
		}

	}
}
