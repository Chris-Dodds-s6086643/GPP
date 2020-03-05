#pragma once
#include <string>
#include <iostream>
enum CardSuit
{
	spades = 0,
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
public:
	Card() {
		std::cout << "SOMETHING HAS GONE WRONG";
	}
	Card(int inSuit, int inValue) : suit((CardSuit)inSuit), value((CardValue)inValue), seen(new bool[4]) {}

	CardSuit suit;
	CardValue value;
	bool* seen;

	std::string toString();
};

