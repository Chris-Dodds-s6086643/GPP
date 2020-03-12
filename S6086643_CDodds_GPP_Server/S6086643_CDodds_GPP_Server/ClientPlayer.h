#pragma once
#include "Card.h"
#include <vector>
#include <thread>
#include <WinSock2.h>
#include <atomic>

#pragma comment (lib, "ws2_32.lib")


class ClientPlayer
{
private:
	std::vector<Card*> hand;
	
	int playerHashID;
	SOCKET* playerSocket;
	std::thread* playerReceiveThread;
	std::atomic<bool>* isPlayerActive;

public:
	ClientPlayer(SOCKET* newPlayerSocket, std::thread* newPlayerThread, int hashID) : isPlayerActive(new std::atomic<bool>(true)), playerReceiveThread(newPlayerThread), playerSocket(newPlayerSocket), playerHashID(hashID) {};

	SOCKET getPlayerSocket() { return *playerSocket; };

	//cause encapsulation is not allowed with threads apparently.
	std::thread* getPlayerThread() { return playerReceiveThread; };

	bool getIsPlayerActive() 
	{
		while (!(isPlayerActive->is_lock_free())) {}
		return isPlayerActive->load();
	}

	void setPlayerInactive()
	{
		while (!(isPlayerActive->is_lock_free())) {}
		isPlayerActive->store(false);
	}

	~ClientPlayer()
	{
		for(int i = 0; i < hand.size(); i++)
		{
			delete(hand[i]);	
		}
		delete(playerSocket);
		delete(isPlayerActive);
		playerReceiveThread->join();
		delete(playerReceiveThread);
	}

	
};

