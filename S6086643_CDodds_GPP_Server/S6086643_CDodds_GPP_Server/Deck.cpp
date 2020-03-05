#include "Deck.h"
#define DECKSIZE int(52)

Deck::Deck()
{
	this->cards.resize(0);
	this->cards.reserve(DECKSIZE);
	FillDeck();
	randomEngine = std::default_random_engine{};
	this->Shuffle();
}

Deck::~Deck()
{
}

void Deck::FillDeck()
{
	for (int suit = (int)CardSuit::spades; suit <= (int)CardSuit::hearts; suit++)
	{
		for (int value = (int)CardValue::ace; value <= (int)CardValue::king; value++)
		{
			this->cards.push_back(Card(suit, value));
		}
	}
}

void Deck::Shuffle()
{
	std::shuffle(std::begin(cards), std::end(cards), randomEngine);
}

void Deck::Draw(Card& poppedCard)
{
	if (cards.size() == 0)
	{
		FillDeck();
	}
	poppedCard = cards.back();
	cards.pop_back();
}