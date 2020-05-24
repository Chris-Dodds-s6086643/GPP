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
	int thisClientID;
	int opponentID;
	bool terminalNeedsUpdating;
public:
	InputManager(ThreadSafeQueue<Message>& inIncomingMessageQueue) :
		incomingMessageQueue(inIncomingMessageQueue),
		networking(nullptr),
		running(true),
		gameState(GameState::WaitingForMatch),
		thisPlayerInput(MessageInputs::InputError),
		OpponentInputs(MessageInputs::InputError),
		thisClientID(ErrorInt),
		terminalNeedsUpdating(true)
	{
		networking = new Networking(incomingMessageQueue);
		GameLoop();
	}

	BOOL WINAPI CtrlHandler(DWORD fdwCtrlType);

	void GameLoop();

	void setPlayerInput(std::string stringInput);

	void HandleMessages();

	Result DetermineGameResult();

	GameState GetGameState() 
	{
		terminalNeedsUpdating = false;
		return gameState; 
	}

	void SetGameState(GameState inGameState)
	{
		gameState = inGameState;
		terminalNeedsUpdating = true;
	}

	std::string toStringMessageInput(MessageInputs messageInput);

	~InputManager() 
	{
		std::vector<int> params;
		Message quitMessage(thisClientID, MessagePurpose::Quit, params);
		quitMessage.setOpponentID(opponentID);
		networking->SendMessageToServer(quitMessage);
		delete networking;
	}
};

