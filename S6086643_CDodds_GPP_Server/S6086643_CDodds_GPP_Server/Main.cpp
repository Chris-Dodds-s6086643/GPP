#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include "Networking.h"
#include "Deck.h"
#include "GameController.h"




int main()
{
	ThreadSafeQueue<std::string> messageQueue;
	Networking s(messageQueue);
	Deck* d = new Deck();
	GameController* g = new GameController(messageQueue, s);
	s.InitialiseServer();
	std::thread thread(&GameController::Update, g);
	while (true)
	{

	}
	return 0;
}

int mai()
{
	//initialise winsock
	WSADATA winSockData;
	WORD winSockVersion = MAKEWORD(2, 2);

	int winSockCheck = WSAStartup(winSockVersion, &winSockData);
	if (winSockCheck != 0)
	{
		std::cerr << "winsock failed to initialise";
		return 0;
	}

	//create listener socket
	SOCKET listener = socket(AF_INET, SOCK_STREAM, 0);

	if (listener == INVALID_SOCKET)
	{
		std::cerr << "Can't Create a Socket";
		return 0;
	}

	// bind socket
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	int result = bind(listener, (sockaddr*)& hint, sizeof(hint));

	listen(listener, SOMAXCONN);

	sockaddr_in client;
	int clientSize = sizeof(client);

	SOCKET clientSocket = accept(listener, (sockaddr*)& client, &clientSize);
	if (clientSocket == INVALID_SOCKET)
	{
		return 0;
	}

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	ZeroMemory(host, NI_MAXHOST);
	ZeroMemory(service, NI_MAXSERV);

	if (getnameinfo((sockaddr*)& client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
	{
		std::cout << "Connected On Port " << service << std::endl;
	}
	else
	{
		inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
		std::cout << host << " Connected on port " << ntohs(client.sin_port) << "\n";
	}

	//closesocket(listener);

	char buffer[4096];

	while (true)
	{
		ZeroMemory(buffer, 4096);
		/*std::cout << "listening\n";*/
		int bytesReceived = recv(clientSocket, buffer, 4096, 0);
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
		std::cout << std::string(buffer, 0, bytesReceived) << "\n";

		send(clientSocket, buffer, bytesReceived, 0);


	}

	closesocket(clientSocket);

	WSACleanup();
}