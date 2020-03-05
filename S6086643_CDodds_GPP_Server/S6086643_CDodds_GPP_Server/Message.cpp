#include "Message.h"
#include <iostream>

std::string Message::formMessage()
{
	return std::string();
}

void Message::handleMessage(std::string message)
{
	std::vector<std::string> stringSegments = SplitString(message);
	switch ((messageTypes)(std::stoi(stringSegments.at(messageSegmets::messageType))))
	{
	case messageTypes::movement:
		{
			break;
		}
		case messageTypes::cardClick:
		{
			break;
		}
		case messageTypes::cardRelease:
		{
			break;
		}
		case messageTypes::stateUpdate:
		{
			break;
		}
		default:
		{
			std::cout << "ERROR In Message Handling;";
		}
	}
}

std::vector<std::string> Message::SplitString(std::string message)
{
	int pos = 0;
	std::vector<std::string> stringVector;
	std::string delimiter = "::";
	while (pos = message.find(delimiter) != std::string::npos)
	{
		stringVector.push_back(std::string(message.substr(0, pos)));
		message.erase(0, pos + delimiter.length());
	}
	return stringVector;

}
