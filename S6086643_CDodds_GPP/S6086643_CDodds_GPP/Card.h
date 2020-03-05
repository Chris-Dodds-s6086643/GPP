#pragma once
enum CardSuit
{
	aces = 0,
	diamonds = 1,
	clubs = 2,
	hearts = 3
};
enum CardValue
{
	ace = 1,
	two = 2,
	three = 3,
	four = 4,
	five = 5,
	six = 6,
	seven = 7,
	eight = 8,
	nine = 9,
	ten = 10,
	jack = 11,
	queen = 12,
	king = 13
};

class Card
{
	CardSuit suit;
	CardValue value;
	bool seen;
};

