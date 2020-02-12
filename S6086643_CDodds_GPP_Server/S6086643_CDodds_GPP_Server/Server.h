#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>

#pragma comment (lib, "ws2_32.lib")

class Server
{
private:
	WSADATA* winSockData = new WSADATA();
	WORD* winSockVersion = new WORD();

	SOCKET* listener;

	sockaddr_in* hint = new sockaddr_in();

	sockaddr_in* client = new sockaddr_in();
	int* clientSize = new int(0);

	SOCKET* clientSocket;

	char* host[NI_MAXHOST];
	char* service[NI_MAXHOST];

	char* buffer[4096];

	fd_set* master;

public:
	Server();

	bool InitialiseServer();

	void ListenForConnections();

	void Listen();

	void GameLoop();
};

