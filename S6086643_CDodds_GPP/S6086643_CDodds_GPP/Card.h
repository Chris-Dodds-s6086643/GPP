#pragma once
#include "Utility.h"
#include <SFML\Graphics.hpp>
#include "TextureHolder.h"
#include <string>
#include <sstream>


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
private:
	CardSuit suit;
	CardValue value;
	bool seen;
	//sf::FloatRect cardRect;
	sf::Sprite* sideSpritePtr;
	sf::Sprite* iconSpritePtr;
	//sf::Font* textFontPtr;
	sf::Text* textPtr;
	int cardTextOffset;

	TextureHolder& textureHolder;
	//sf::Texture* cardTextures[6]; // Front, Back, Spades, diamonds, clubs, hearts;
public:
	Card(CardSuit cardSuit, CardValue cardValue, TextureHolder& cardTextureHolder, sf::Font* fontPtr): 
		suit(cardSuit), 
		value(cardValue), 
		seen(false), 
		textureHolder(cardTextureHolder),
		sideSpritePtr(nullptr),
		iconSpritePtr(nullptr),
		//textFontPtr(fontPtr),
		textPtr(nullptr),
		cardTextOffset(0){}
	void DrawCard(bool isFaceUp, sf::RenderWindow& window, TextureHolder& textureHolder, float xPosition, float yPosition);
	sf::FloatRect getRect()
	{
		return sideSpritePtr->getGlobalBounds();
	}
	void setPosition(sf::Vector2f& newPos)
	{
		sideSpritePtr->setPosition(newPos);
	}
	void DrawCard2(bool isFaceUp, sf::RenderWindow& window);

	void FormCardFaceTexture(bool isFaceUp);

	sf::Sprite* getCardSprite(bool isFaceUp);

	void setCardPosition(float posX, float posY);

	std::string toString()
	{
		std::stringstream ss;
		switch (value)
		{
		case ace:
			ss << "Ace";
			break;
		case jack:
			ss << "Jack";
			break;
		case queen:
			ss << "Queen";
			break;
		case king:
			ss << "King";
			break;
		default:
			ss << value;
			break;
		}
		switch (suit)
		{
		case spades:
			ss << " of Spades;";
			break;
		case diamonds:
			ss << " of Diamonds;";
			break;
		case clubs:
			ss << " of Clubs;";
			break;
		case hearts:
			ss << " of Hearts;";
			break;
		default:
			ss << "ERROR IN toString for card in suit part";
			break;
		}
		return ss.str();
	}

	~Card()
	{
		delete(sideSpritePtr);
		delete(iconSpritePtr);
		//delete(textFontPtr);
		delete(textPtr);
	}

};

