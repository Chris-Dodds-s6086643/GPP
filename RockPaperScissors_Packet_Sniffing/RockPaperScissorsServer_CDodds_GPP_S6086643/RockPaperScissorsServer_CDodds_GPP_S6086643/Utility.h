#pragma once
#include <string>
#include <vector>
#include <iostream>

static enum class Result : int
{
	Loss = 0,
	Win = 1,
	Draw = 2
};

static std::string StringDelimiter = "::";
static std::string StringEndingDelimiter = "~~";
static int ErrorInt = -8;

static std::vector<std::string> SplitString(std::string& stringToSplit, std::string delimiter)
{
	std::vector<std::string> outStringVector;

	int position = stringToSplit.find(delimiter);
	while (position != std::string::npos)
	{
		std::string section = std::string(stringToSplit.substr(0, position));
		outStringVector.push_back(section);
		stringToSplit.erase(0, position + delimiter.length());
		position = stringToSplit.find(delimiter);
	}
	if(stringToSplit != "")
		outStringVector.push_back(stringToSplit);
	return outStringVector;
}