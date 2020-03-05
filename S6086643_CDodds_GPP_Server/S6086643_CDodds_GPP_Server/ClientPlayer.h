#pragma once
#include "Card.h"
#include <vector>
class ClientPlayer
{
private:
	int id;
	std::vector<Card*> hand;
};

