#pragma once
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include "ClientPlayer.h"
#include "ThreadSafeQueue.h"
#include "ThreadSafeVector.h"
#include "ThreadSafeUnorderedMap.h"
#include "Message.h"
#include <string>
#include "Match.h"
#include "Encryption.h"
#pragma comment (lib, "ws2_32.lib")

class Networking
{
private:
#pragma region Listening Socket Variables
	WSADATA* winSockData;
	WORD* winSockVersion;
	SOCKET* serverConnectionListeningSocket;
	sockaddr_in* hint;

	//used to accept connections so relevant to listening socket.
	char host[NI_MAXHOST];
	char service[NI_MAXSERV];

	std::atomic<bool> serverRunning;
#pragma endregion

#pragma region Client Storage Variables
	std::vector<std::thread*> threads;
	//ThreadSafeVector<ClientPlayer*> clients;
	ThreadSafeUnorderedMap<int, ClientPlayer*, Message> clientMap;
	std::vector<int> playerIDsWaitingForMatch;
#pragma endregion

#pragma region Message Handling Variables
	char buffer[4096];
	ThreadSafeQueue<Message> messageQueue;
	Encryption* encryption;
#pragma endregion
public:
	void ListenForConnections();

	Networking()
	{
#pragma region Initialise Variables
		winSockData = new WSADATA();
		winSockVersion = new WORD(MAKEWORD(2,2));
		hint = new sockaddr_in();
		ZeroMemory(host, NI_MAXHOST);
		ZeroMemory(service, NI_MAXSERV);
		ZeroMemory(buffer, 4096);
		serverRunning.store(true);
		encryption = new Encryption();
#pragma endregion

#pragma region initialise and bind listening socket
		// ensures  the windows socket has been loaded and started properly.
		int windowsSocketCheck = WSAStartup(*winSockVersion, winSockData);
		if (windowsSocketCheck != 0)
		{
			std::cerr << "WINDOWS SOCKET STARTUP FAILED";
			return;
		}
		//instantiate the listening socket as a socket using IPV4 and the idea that it will read data, I believe.
		serverConnectionListeningSocket = new SOCKET(socket(AF_INET, SOCK_STREAM, 0));
		if (*serverConnectionListeningSocket == INVALID_SOCKET)
		{
			std::cerr << "Listening Socket Cannot be created";
			return;
		}
		//attempt to bind the listening socket to the address of the server? (I think that's what hint is anyways);
		hint->sin_family = AF_INET;
		hint->sin_port = htons(54000);
		hint->sin_addr.S_un.S_addr = INADDR_ANY;
		int result = bind(*serverConnectionListeningSocket, (sockaddr*)hint, sizeof(*hint));
		if (result == SOCKET_ERROR)
		{
			std::cerr << "ERROR BINDING SOCKET \n" << WSAGetLastError() << "\n";
			return;
		}
#pragma endregion

#pragma region Begin Listening Thread
		std::cout << "Listening Socket Bound, now listening for Connections\n";
		//ListenForConnections();
		std::thread* newThread = new std::thread([this] { ListenForConnections(); });
		//threads.push_back(newThread);
		GameLoop();
#pragma endregion
	}

	void ReceiveFromOnePlayer(int socketIndex);

	void GameLoop();

	void EchoMessage(Message messageToEcho);

	void SendMessageToClientPlayerByID(int playerID, Message messageToSend);

	std::function<void(ClientPlayer*, Message)> SendMessageToClientPlayer = [](ClientPlayer* clientPlayer, Message messageToSend) {
		send(*(clientPlayer->GetSocket()), messageToSend.ToString().c_str(), messageToSend.ToString().length(), 0);
	};

	~Networking()
	{
		while (!serverRunning.is_lock_free()) {}
		serverRunning.store(false);
		delete winSockData;
		delete winSockVersion;
		delete hint;
		delete encryption;
		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i] != nullptr)
			{
				threads[i]->join();
				delete threads[i];
			}
		}
	}
};