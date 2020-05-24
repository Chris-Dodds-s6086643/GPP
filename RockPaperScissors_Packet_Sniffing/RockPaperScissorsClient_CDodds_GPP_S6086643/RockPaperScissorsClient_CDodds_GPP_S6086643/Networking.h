#pragma once
#include "ThreadSafeQueue.h"
#include "Message.h"
#include <WS2tcpip.h>
#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#pragma comment (lib, "ws2_32.lib")

class Networking
{
private:
#pragma region Receiving Messages Variables
	ThreadSafeQueue<Message>& incomingMessageQueue;
	std::thread* listeningThread;
	std::atomic<bool> listening;
#pragma endregion

#pragma region Server Connection Variables
	std::string serverIPAddress;
	int port;
	sockaddr_in hint;
	WSADATA windowsSocketData;
	WORD windowsSocketVersion;
	SOCKET* serverSocket;
#pragma endregion
public:
	Networking(ThreadSafeQueue<Message>& inIncomingMessageQueue) :
		incomingMessageQueue(inIncomingMessageQueue),
		listeningThread(nullptr),
		listening(true),
		serverIPAddress("127.0.0.1"),
		port(54000),
		hint(),
		windowsSocketData(),
		windowsSocketVersion(MAKEWORD(2,2)),
		serverSocket(nullptr)
	{
	#pragma region Establish Connection To Server & initialise Relevant Variables
		int windowsSocketStartupResult = WSAStartup(windowsSocketVersion, &windowsSocketData);
		if (windowsSocketStartupResult != 0)
			return;
		
		serverSocket = new SOCKET(socket(AF_INET, SOCK_STREAM, 0));
		if (*serverSocket == INVALID_SOCKET)
			return;
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		inet_pton(AF_INET, serverIPAddress.c_str(), &hint.sin_addr);

		int connectionResult = connect(*serverSocket, (sockaddr*)&hint, sizeof(hint));
		if (connectionResult == SOCKET_ERROR)
		{
			std::cerr << "SOCKET ERROR\n" + WSAGetLastError();
			closesocket(*serverSocket);
			WSACleanup();
			return;
		}
		else
		{
			std::cout << "CONNECTED;\n";
		}
	#pragma endregion

		listeningThread = new std::thread(&Networking::ListenForMessages, this);
	}

	void SendMessageToServer(Message message);

	void ListenForMessages();

	void StopListening()
	{
		listening.store(false);
	}

	~Networking()
	{
		listening.store(false);
		listeningThread->join();
		delete listeningThread;
		delete serverSocket;
	}
};

