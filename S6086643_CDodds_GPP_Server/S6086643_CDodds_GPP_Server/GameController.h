#pragma once
#include "ThreadSafeQueue.h"
#include "ClientPlayer.h"
#include "Networking.h"
class GameController
{
private:
	ThreadSafeQueue<std::string>& messageQueue;

	std::vector<ClientPlayer*> clients;

	Networking& networking;

	int turnID;
public:
	GameController(ThreadSafeQueue<std::string>& queue, Networking& inNetworking) : messageQueue(queue), networking(inNetworking) {}

	~GameController(){}

	void Update();

	void HandleMessage(std::string& message);

};



