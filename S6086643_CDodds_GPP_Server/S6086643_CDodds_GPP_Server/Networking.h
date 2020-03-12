#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <string>
#include <thread>
#include <vector>
#include "ThreadSafeQueue.h"
#include <atomic>
#include <unordered_map>
#include "ClientPlayer.h"
#include "ThreadSafeUnordoredMap.h"	


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

	std::vector<std::atomic<bool>*> activeStatus;

	std::unordered_map<int, ClientPlayer*> playersMap;
	std::unordered_map<int, ClientPlayer*>::hasher playersMapHashFn;

	ThreadSafeUnorderedMap<int, ClientPlayer*> playersMapTheSecond;
	std::unordered_map<int, ClientPlayer*>::hasher playersMapHashFnTheSecond;

	mutable std::mutex playersMapMutex;

	ThreadSafeQueue<int> threadSafePlayerKeysToRemove;


public:
	Networking(ThreadSafeQueue<std::string>& incomingMessageQueue);

	~Networking();

	bool InitialiseServer();

	void ListenForConnections();

	void listenForConnections2();

	void Listen();

	void OneSocketReceive(int socketNumber);

	void OneSocketReceiveTheSecond(int socketNumber);

	void OneSocketReceiveTheThird(int socketNumber);

	void GameLoop();
	void GameLoopTheSecond();
	void GameLoopTheThird();
	void EchoMessage(int senderID, std::string message);

	void EchoMessageTheSecond(int senderID, std::string message);

	void EchoMessageTheThird(int senderID, std::string message);

	void sendToClient(ClientPlayer* client, std::string message);

	void setActive(int ID);
	void setActiveTheSecond(int ID);
};

