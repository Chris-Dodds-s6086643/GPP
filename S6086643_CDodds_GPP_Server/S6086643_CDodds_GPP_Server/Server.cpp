#include "Server.h"

Server::Server()
{
	hint->sin_family = AF_INET;
	hint->sin_port = htons(54000);
	hint->sin_addr.S_un.S_addr = INADDR_ANY;
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
	if (*listener == INVALID_SOCKET)
	{
		std::cerr << "Can't Create a Socket";
		return false;
	}

	bind(*listener, (sockaddr*)& hint, sizeof(hint));
	this->ListenForConnections();
}

void Server::ListenForConnections()
{
	listen(*listener, SOMAXCONN);
	
	
	do 
	{
		clientSocket = new SOCKET(accept(*listener, (sockaddr*)& client, clientSize));

	} while (*clientSocket == INVALID_SOCKET);
	if(*clientSocket != INVALID_SOCKET)
	{
		clientSize = new int(sizeof(client));

		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);

		if (getnameinfo((sockaddr*)& client, sizeof(client), *host, NI_MAXHOST, *service, NI_MAXSERV, 0) == 0)
		{
			std::cout << "Connected On Port " << service << std::endl;
		}
		else
		{
			inet_ntop(AF_INET, &client->sin_addr, *host, NI_MAXHOST);
			std::cout << host << " Connected on port " << ntohs(client->sin_port) << "\n";
		}
		Listen();
	}
	
}

void Server::Listen()
{
	while (true)
	{
		ZeroMemory(buffer, 4096);
		/*std::cout << "listening\n";*/
		int bytesReceived = recv(*clientSocket, *buffer, 4096, 0);
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "ERRROR IN RECV";
			break;
		}
		if (bytesReceived == 0)
		{
			std::cout << "quitting";
			break;
		}
		bytesReceived += 1;
		//std::cout << std::string(buffer, 0, (int*)&bytesReceived) << "\n";

		send(*clientSocket, *buffer, bytesReceived, 0);


	}

	closesocket(*clientSocket);

	WSACleanup();
}

void Server::GameLoop()
{
}
