#pragma once
#include "Utility.h"
#include <SFML/Graphics.hpp>
#include <sstream>
#include <iostream>

enum CardSuit
{
	spades = 0,
	diamonds = 1,
	clubs = 2,
	hearts = 3,
	EndingForLoop = 4
};
enum CardSide
{
	Back = 0,
	Front = 1,
	Ending = 2
};

const class TextureHolder
{
private: 
	const char* suitsInOrderAsStrings[4]{ "Spades", "Diamonds", "Clubs", "Hearts" };
	const char* cardSideInOrder[2]{ "Back", "Front" };
	std::string directory = "../../Resources/";
public:
	TextureHolder()
	{
		for (int i = Back; i != Ending; i++)
		{
			std::stringstream filePath;
			filePath << relativeResourcePath << "Card" << cardSideInOrder[i] << ".png";
			if (!cardSide[i].loadFromFile(filePath.str()))
			{
				std::cerr << "inelligable path: " << filePath.str() << "\n";
			}
		}

		for (int i = spades; i != EndingForLoop; i++)
		{
			std::stringstream filePath;
			filePath << directory << suitsInOrderAsStrings[i] << "Icon.png";
			if (!suitIcon[i].loadFromFile(filePath.str()))
			{
				std::cerr << "inelligable path: " << filePath.str() << "\n";
			}
		}
	}

	sf::Texture cardSide[2];

	sf::Texture suitIcon[4];
};

