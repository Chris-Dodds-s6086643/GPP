#pragma once
#include <vector>
#include "Card.h"
#include <random>

class Deck
{
private:
	std::vector<Card> cards;

	std::default_random_engine randomEngine;
public:
	Deck();

	~Deck();

	void FillDeck();

	void Shuffle();

	void Draw(Card& poppedCard);
};

