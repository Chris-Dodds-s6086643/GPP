#include "InputManager.h"

void InputManager::GameLoop()
{
	std::string lineGot;
	while (running)
	{
		if (terminalNeedsUpdating)
		{
			switch (GetGameState())
			{
				case GameState::WaitingForMatch:
				{
					opponentID = ErrorInt;
					std::cout << ">Waiting to be placed into a match with another player;";
					break;
				}
				case GameState::WaitingForThisPlayerInput:
				{
					do
					{
						std::cout << ">Please Enter Your Desired choice (R or 0 for Rock, P or 1 for Paper, S or 2 for Scissors) \n>";
						std::getline(std::cin, lineGot);
						if (lineGot.length() > 0)
						{
							lineGot = std::toupper(lineGot[0]);
						}
					} while (lineGot != "R" && lineGot != "P" && lineGot != "S" &&
						lineGot != "0" && lineGot != "1" && lineGot != "2");
					setPlayerInput(lineGot);
					std::cout << ">Thank you, you selected: " << toStringMessageInput(thisPlayerInput) << "\n";
					if (OpponentInputs != MessageInputs::InputError)
					{
						SetGameState(GameState::MatchResult);
					}
					else
					{
						SetGameState(GameState::WaitingForOpponentinput);
					}
					break;
				}
				case GameState::WaitingForOpponentinput:
				{
					std::cout << "Waiting for Opponents Input; Thank you for your patience\n";
					break;
				}
				case GameState::MatchResult:
				{
					Result r = DetermineGameResult();
					std::cout << "You Selected: " << toStringMessageInput(thisPlayerInput) << ", Your Opponent Selected: " << toStringMessageInput(OpponentInputs) << "\n";
					switch (r)
					{
					case Result::Loss:
						std::cout << "Unfortunately this time you lost.\n";
						SetGameState(GameState::WaitingForMatch);
						break;
					case Result::Win:
						std::cout << "Congratulations, this game is yours.\n";
						SetGameState(GameState::WaitingForMatch);
						break;
					case Result::Draw:
						std::cout << "There was a draw please select again";
						SetGameState(GameState::WaitingForThisPlayerInput);
						break;
					default:
						break;
					}
					thisPlayerInput = MessageInputs::InputError;
					OpponentInputs = MessageInputs::InputError;
					std::vector<int> parameters{ (int)r };
					Message endGameMessage(thisClientID, MessagePurpose::GameEnd, parameters);
					endGameMessage.setOpponentID(opponentID);
					networking->SendMessageToServer(endGameMessage);
					break;
				}
				default: 
				{
					std::cerr << "GameState is Error, the fuck?";
					break;
				}
			}
		}
		//while (incomingMessageQueue.isEmpty()) {}
		HandleMessages();
	}
}

void InputManager::setPlayerInput(std::string stringInput)
{
	if (stringInput == "R")
	{
		thisPlayerInput = MessageInputs::Rock;
	}
	else if (stringInput == "P")
	{
		thisPlayerInput = MessageInputs::Paper;
	}
	else if (stringInput == "S")
	{
		thisPlayerInput = MessageInputs::Scissors;
	}
	else
	{
		thisPlayerInput = (MessageInputs)std::stoi(stringInput);
	}
	std::vector<int> input;
	input.push_back((int)thisPlayerInput);
	Message inputTakenMessage(thisClientID, MessagePurpose::Input, input);
	inputTakenMessage.setOpponentID(opponentID);
	networking->SendMessageToServer(inputTakenMessage);
}

//Loops through the messages on the queue and alters the gameState to the relevant setting.
void InputManager::HandleMessages()
{
	Message messageFromQueue;
	while (incomingMessageQueue.tryPop(messageFromQueue))
	{
		//std::cout << "\nMessage Received: " << messageFromQueue.ToString() << "\n";
		switch (messageFromQueue.GetMessagePurpose())
		{
			case MessagePurpose::Join:
			{
				if (messageFromQueue.GetOpponentID() != ErrorInt)
				{
					opponentID = messageFromQueue.GetOpponentID();
					SetGameState(GameState::WaitingForThisPlayerInput);
				}
				else
				{
					thisClientID = messageFromQueue.GetID();
				}
				break;
			}
			case MessagePurpose::Quit:
			{
				SetGameState(GameState::WaitingForMatch);
				break;
			}
			case MessagePurpose::Input:
			{
				OpponentInputs = (MessageInputs)(messageFromQueue.GetParameters().at(0));
				if (thisPlayerInput != MessageInputs::InputError)
				{
					SetGameState(GameState::MatchResult);
				}
				else
				{
					SetGameState(GameState::WaitingForThisPlayerInput);
				}
				break;
			}
			case MessagePurpose::GameEnd:
			{
				if (messageFromQueue.GetOpponentID() == ErrorInt)
				{
					SetGameState(GameState::WaitingForMatch);
				}
				else
				{
					SetGameState(GameState::WaitingForThisPlayerInput);
				}
				break;
			}
			case MessagePurpose::Error:
			{
				std::cerr << "error in pulling message from queue";
				break;
			}
			default:
			{
				break;
			}
		}
	}
}

Result InputManager::DetermineGameResult()
{
	if (thisPlayerInput == OpponentInputs)
	{
		return Result::Draw;
	}
	if (thisPlayerInput == MessageInputs::Rock)
	{
		if (OpponentInputs == MessageInputs::Scissors)
		{
			return Result::Win;
		}
		else if (OpponentInputs == MessageInputs::Paper)
		{
			return Result::Loss;
		}
	}
	else if (thisPlayerInput == MessageInputs::Paper)
	{
		if (OpponentInputs == MessageInputs::Rock)
		{
			return Result::Win;
		}
		else if (OpponentInputs == MessageInputs::Scissors)
		{
			return Result::Loss;
		}
	}
	else if (thisPlayerInput == MessageInputs::Scissors)
	{
		if (OpponentInputs == MessageInputs::Paper)
		{
			return Result::Win;
		}
		else if (OpponentInputs == MessageInputs::Rock)
		{
			return Result::Loss;
		}
	}
}

std::string InputManager::toStringMessageInput(MessageInputs messageInput)
{
	switch (messageInput)
	{
		case Rock:
		{
			return "Rock";
		}
		case Paper:
		{
			return "Paper";
		}
		case Scissors:
		{
			return "Scissors";
		}
		default:
		{
			return "Error, how on earth have you managed that.";
		}
	}
}
