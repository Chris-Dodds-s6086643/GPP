#pragma once
#include <string>
#include <vector>
class Message
{

public:
	enum messageTypes 
	{
		movement = 0,
		cardClick = 1,
		cardRelease = 2,
		stateUpdate = 3,
	};
	enum messageSegmets
	{
		id = 0,
		messageType = 1
	};

	std::string formMessage();

	static void handleMessage(std::string message);

	static std::vector<std::string> SplitString(std::string message);

};

