#pragma once
#include <sstream>
#include <vector>
#include "Utility.h"

enum class MessagePurpose
{
	Join = 0,
	Quit = 1,
	Input = 2,
	GameEnd = 3,
	Error = 4
};

enum MessageSegments
{
	SenderID = 0,
	Purpose = 1,
	OpponentID = 2,
	FirstVariable = 3
};

enum MessageInputs
{
	Rock = 0,
	Paper = 1,
	Scissors = 2,
	InputError
};

class Message
{
private:
	int senderID;
	MessagePurpose messagePurpose;
	int opponentID;
	std::vector<int> parameters;
public:
	Message() :
		senderID(ErrorInt),
		messagePurpose(MessagePurpose::Error),
		opponentID(ErrorInt)
	{}

	Message(std::string stringToConvert)
	{
		if(stringToConvert.find(StringEndingDelimiter) != std::string::npos)
			stringToConvert.erase(stringToConvert.find(StringEndingDelimiter) + StringEndingDelimiter.length());
		std::vector<std::string> splitString = SplitString(stringToConvert, StringDelimiter);
		senderID = std::stoi(splitString[MessageSegments::SenderID]);
		messagePurpose = MessagePurpose(std::stoi(splitString[MessageSegments::Purpose]));
		opponentID = std::stoi(splitString[MessageSegments::OpponentID]);
		for (int i = int(MessageSegments::FirstVariable); i < int(splitString.size()); i++)
		{
			try
			{
				parameters.push_back(std::stoi(splitString[i]));
			}
			catch (const std::exception& e)
			{
				std::cout << e.what();
			}
		}
	}

	Message(int inSenderID, MessagePurpose inMessagePurpose ,std::vector<int> inParamaters) :
		senderID(inSenderID),
		messagePurpose(inMessagePurpose),
		opponentID(ErrorInt),
		parameters(inParamaters)
	{}

	int GetID()
	{
		return senderID;
	}

	MessagePurpose GetMessagePurpose()
	{
		return messagePurpose;
	}

	std::vector<int> GetParameters()
	{
		return parameters;
	}

	int GetOpponentID()
	{
		return opponentID;
	}

	void setOpponentID(int inOpponentID)
	{
		opponentID = inOpponentID;
	}

	std::string ToString()
	{
		std::stringstream outString;
		outString << senderID << StringDelimiter << (int)messagePurpose << StringDelimiter << opponentID;
		for (int i = 0; i < int(parameters.size()); i++)
		{
			outString << StringDelimiter << (int)parameters[i];
		}
		outString << StringEndingDelimiter;
		return outString.str();
	}

	
};