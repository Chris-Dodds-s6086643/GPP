#pragma once
#include <string>
#include "WinSock2.h"
#include <thread>
#include <atomic>
#include "Message.h"
#include "KeyModulusPair.h"

class ClientPlayer
{
private:
	int playerID;
	MessageInputs input;
	std::string name;
	SOCKET* playerSocket;
	std::thread* playerReceiveThread;
	std::atomic<bool> isPlayerActive;
	KeyModulusPair* publicKey;
public:
	/**	
	Creates a blank ClientPlayer;
	
	* With an input set to error value 'E', the name set to a default of "buttons" TY RKG, 
	* and the atomic bool isPlayerActive set to True;
	* @param inPlayerSocket Socket Pointer which the player has connected on.
	* @param inPlayerRecieveThread Potentially incorrectly spelled pointer to the thread where the server is listening to this clients messages.
	* @param inPlayerID ID that the player is held as, should be ensured to be unique in some way before being set.
	*/
	ClientPlayer(SOCKET* inPlayerSocket, std::thread* inPlayerRecieveThread, int inPlayerID) :
		playerID(inPlayerID),
		input(MessageInputs::InputError),
		name("Buttons"),
		playerSocket(inPlayerSocket),
		playerReceiveThread(inPlayerRecieveThread),
		isPlayerActive(true),
		publicKey(nullptr)
	{};

	bool IsPlayerActive() 
	{
		while(!isPlayerActive.is_lock_free()) {}
		return isPlayerActive.load();
	}

	void setPlayerInactive()
	{
		while (!isPlayerActive.is_lock_free()) {}
		isPlayerActive.store(false);
	}

	int GetPlayerID() { return playerID; }

	SOCKET* GetSocket() { return playerSocket; }

	void SetInput(MessageInputs inMessageInput) { input = inMessageInput; }
	
	MessageInputs GetInput() { return input; }

	void  SetPublicKey(KeyModulusPair* inPublicKey) { publicKey = inPublicKey; }

	KeyModulusPair GetPublicKey() { return *publicKey; }

	~ClientPlayer()
	{
		delete playerSocket;
		playerReceiveThread->join();
		delete playerReceiveThread;
	}
};

