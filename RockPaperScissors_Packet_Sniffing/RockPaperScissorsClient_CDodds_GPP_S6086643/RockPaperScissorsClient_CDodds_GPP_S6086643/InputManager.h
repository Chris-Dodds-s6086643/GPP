#pragma once
#include "ThreadSafeQueue.h"
#include "Message.h"
#include "Networking.h"
#include <atomic>

enum class GameState : int
{
	WaitingForMatch = 0,
	WaitingForThisPlayerInput = 1,
	WaitingForOpponentinput = 2,
	MatchResult = 3
};

enum class Result : int
{
	Loss = 0,
	Win = 1,
	Draw = 2
};

class InputManager
{
private:
	ThreadSafeQueue<Message>& incomingMessageQueue;
	Networking* networking;
	std::atomic<bool> running;
	GameState gameState;
	MessageInputs thisPlayerInput;
	MessageInputs OpponentInputs;
	int id;
	int opponentID;
public:
	InputManager(ThreadSafeQueue<Message>& inIncomingMessageQueue) :
		incomingMessageQueue(inIncomingMessageQueue),
		networking(nullptr),
		running(true),
		gameState(GameState::WaitingForMatch),
		thisPlayerInput(MessageInputs::InputError),
		OpponentInputs(MessageInputs::InputError),
		id(ErrorInt)
	{
		networking = new Networking(incomingMessageQueue);
		GameLoop();
	}

	void GameLoop();

	void setPlayerInput(std::string stringInput);

	void HandleMessages();

	Result DetermineGameResult();

	std::string toStringMessageInput(MessageInputs messageInput);

	~InputManager() {}
};

