#pragma once
#include "ThreadSafeQueue.h"
#include <string>
#include <WS2tcpip.h>
#include <iostream>

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



public:

	Networking(ThreadSafeQueue<std::string>& threadSafeQueue) : stringSafeQueue(threadSafeQueue)
	{
		NetworkingStartUp();
	}

	void NetworkingStartUp();

	~Networking();
};

