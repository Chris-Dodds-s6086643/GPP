#pragma once
#include "ClientPlayer.h"

class Match
{
public:
	int ID;
	ClientPlayer& playerOne;
	ClientPlayer& playerTwo;

	Match(int inID, ClientPlayer& inPlayerOne, ClientPlayer& inPlayerTwo) : ID(inID), playerOne(inPlayerOne), playerTwo(inPlayerTwo) {}
};