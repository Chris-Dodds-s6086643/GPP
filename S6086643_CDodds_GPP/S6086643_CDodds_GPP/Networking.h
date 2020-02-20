#pragma once
#include "ThreadSafeQueue.h"
#include <string>
#include <WS2tcpip.h>
#include <iostream>
#include <atomic>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

class Networking
{


private:
	ThreadSafeQueue<std::string>& stringSafeQueue;
	std::string serverIPAddress = "127.0.0.1";
	int port = 54000;
	WSAData winSockData;
	WORD winSockVersion = MAKEWORD(2, 2);
	sockaddr_in hint;
	SOCKET* clientSocket;
	std::atomic<bool>* listening;
	std::thread* listeningThread;
	


public:

	Networking(ThreadSafeQueue<std::string>& threadSafeQueue) : stringSafeQueue(threadSafeQueue)
	{
		listening = new std::atomic<bool>(true);
		NetworkingStartUp();
	}

	void NetworkingStartUp();

	bool Send(std::string &message);

	void Listen();

	~Networking();
};

