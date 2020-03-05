#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include "ThreadSafeQueue.h"
#include <atomic>

#pragma comment (lib, "ws2_32.lib")

enum socketSecurityEnum {
	listeningSocket = 0
};

class Networking
{
private:
	WSADATA* winSockData = new WSADATA();
	WORD* winSockVersion = new WORD();

	SOCKET* listener;

	sockaddr_in* hint = new sockaddr_in();

	sockaddr_in* client = new sockaddr_in();
	int* clientSize = new int(0);

	SOCKET* clientSocket;

	char host[NI_MAXHOST];
	char service[NI_MAXHOST];

	char buffer[4096];

	std::vector<std::thread*>* threads;

	std::vector<SOCKET*>* sockets;

	ThreadSafeQueue<std::string>& incomingsQueue;

	ThreadSafeQueue<SOCKET*>* socketsConnected;

	std::atomic<bool>* serverRunning;

public:
	Networking(ThreadSafeQueue<std::string>& incomingMessageQueue);

	~Networking();

	bool InitialiseServer();

	void ListenForConnections();

	void listenForConnections2();

	void Listen();

	void OneSocketReceive(int socketNumber);

	void GameLoop();
	void EchoMessage(int senderID, std::string message);
};

