#include "GameController.h"
#include "Message.h"

void GameController::Update()
{
	while (true)
	{
		while (!messageQueue.isEmpty())
		{
			std::string outString;
			messageQueue.tryPop(outString);
			int senderID = 0;
			//HandleMessage(outString);
			std::vector<std::string> splitString = Message::SplitString(outString);
			std::string stringID = splitString.at(Message::messageSegmets::id);
			senderID = std::stoi(stringID);
			if (splitString.size() > 1 && splitString.at(1) == "QUIT")
			{
				networking.setActiveTheSecond(senderID);
			}
			networking.EchoMessageTheThird(senderID, outString);
		}
	}
}

//while (!incomingsQueue.isEmpty())
//{
//	std::string outString;
//	incomingsQueue.tryPop(outString);
//	std::cout << outString << "\n";
//	for (size_t i = 1; i < sockets->size(); i++)
//	{
//		if (std::stoi(outString.substr(0,1)) != i)
//		{
//			send(*sockets->at(i), outString.c_str(), (outString.length() + 1), 0);
//		}
//	}
//}

void GameController::HandleMessage(std::string& message)
{
	//std::vector<std::string*> stringSegments = Message::SplitString(outString);
	//switch ((Message::messageTypes)(std::stoi(*stringSegments.at(Message::messageSegmets::messageType))))
	//{
	//	case Message::messageTypes::movement:
	//	{
	//		break;
	//	}
	//	case Message::messageTypes::cardClick:
	//	{
	//		break;
	//	}
	//	case Message::messageTypes::cardRelease:
	//	{
	//		break;
	//	}
	//	case Message::messageTypes::stateUpdate:
	//	{
	//		break;
	//	}
	//	default:
	//	{
	//		std::cout << "ERROR In Message Handling;";
	//	}
	//}
}
