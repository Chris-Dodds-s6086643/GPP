#pragma once
#include "Card.h"
#include "Utility.h"
#include "TextureHolder.h"


void Card::DrawCard(bool isFaceUp, sf::RenderWindow& window, TextureHolder& textureHolder, float xPostion, float yPosition)
{

	if (isFaceUp)
	{
/*		sf::Font fontasaurus;
		fontasaurus.loadFromFile(relativeResourcePath + "segoeui.ttf");
		sf::Texture& card = textureHolder.cardSide[(int)CardSide::Front];
		sf::Texture& icon = textureHolder.suitIcon[(int)suit];
		sideSpritePtr = new sf::Sprite(card);
		sf::Sprite cardIcon(icon);
		sideSpritePtr->setPosition(xPostion, yPosition);
		cardIcon.setPosition(sideSpritePtr->getPosition().x + (cardIcon.getTextureRect().width / 2), sideSpritePtr->getPosition().y + 4);
		textPtr = new sf::Text("",fontasaurus);
		
		textPtr->setOrigin(textPtr->getLocalBounds().width / 2, textPtr->getLocalBounds().width / 2);
		int textOffset = 0;
		if ((int)suit % 2 == 0)
		{
			textPtr->setFillColor(sf::Color::Black);
		}
		else
		{
			textPtr->setFillColor(sf::Color::Red);
		}
		switch (value)
		{
		case ace:
			textPtr->setString("A");
			break;
		case ten:
			textPtr->setString("10");
			textOffset = 3;
			break;
		case jack:
			textPtr->setString("J");
			textOffset = 2;
			break;
		case queen:
			textPtr->setString("Q");
			textOffset = 2;
			break;
		case king:
			textPtr->setString("K");
			textOffset = 2;
			break;
		default:
			textPtr->setString(std::to_string((int)value));
			textOffset = 2;
			break;
		}
		textPtr->setOrigin(textPtr->getLocalBounds().width / 2, textPtr->getLocalBounds().height / 2);
		textPtr->setPosition(sideSpritePtr->getPosition().x + (sideSpritePtr->getTextureRect().width / 2) - textOffset, sideSpritePtr->getPosition().y + (sideSpritePtr->getTextureRect().height  /2));
		*/
		if (sideSpritePtr == nullptr || iconSpritePtr == nullptr || textPtr == nullptr)
		{
			FormCardFaceTexture(true);
		}
		sf::Font bob;
		bob.loadFromFile(relativeResourcePath + "segoeui.ttf");
		textPtr->setFont(bob);
		window.draw(*sideSpritePtr);
		window.draw(*iconSpritePtr);
		window.draw(*textPtr);
	}
	else
	{
		/*sf::Texture& card = textureHolder.cardSide[(int)CardSide::Back];
		sf::Sprite spriteForDrawing(card);*/
		window.draw(*sideSpritePtr);
	}
}

void Card::FormCardFaceTexture(bool isFaceUp)
{
	if (isFaceUp)
	{
		if (textureHolder.cardSide[0].getSize().x < 10)
		{
			std::cout << "I've no idea what I am doing here";
		}
		sf::Texture& card = textureHolder.cardSide[(int)CardSide::Front];
		sf::Texture& icon = textureHolder.suitIcon[(int)suit];
		sideSpritePtr = new sf::Sprite(card);
		iconSpritePtr = new sf::Sprite(icon);
		iconSpritePtr->setPosition(sideSpritePtr->getPosition().x + (iconSpritePtr->getTextureRect().width / 2), sideSpritePtr->getPosition().y + 4);
		sf::Font f;
		f.loadFromFile(relativeResourcePath + "segoeui.ttf");
		textPtr = new sf::Text("", f);

		textPtr->setOrigin(textPtr->getLocalBounds().width / 2, textPtr->getLocalBounds().width / 2);
		if ((int)suit % 2 == 0)
		{
			textPtr->setFillColor(sf::Color::Black);
		}
		else
		{
			textPtr->setFillColor(sf::Color::Red);
		}
		switch (value)
		{
		case ace:
			textPtr->setString("A");
			break;
		case ten:
			textPtr->setString("10");
			cardTextOffset = 3;
			break;
		case jack:
			textPtr->setString("J");
			cardTextOffset = 2;
			break;
		case queen:
			textPtr->setString("Q");
			cardTextOffset = 2;
			break;
		case king:
			textPtr->setString("K");
			cardTextOffset = 2;
			break;
		default:
			textPtr->setString(std::to_string((int)value));
			cardTextOffset = 2;
			break;
		}
		textPtr->setOrigin(textPtr->getLocalBounds().width / 2, textPtr->getLocalBounds().height / 2);
		textPtr->setPosition(sideSpritePtr->getPosition().x + (sideSpritePtr->getTextureRect().width / 2) - cardTextOffset, sideSpritePtr->getPosition().y + (sideSpritePtr->getTextureRect().height / 2));
	}
	else
	{
		sf::Texture& card = textureHolder.cardSide[(int)CardSide::Back];
		sideSpritePtr = new sf::Sprite(card);
	}
}

sf::Sprite* Card::getCardSprite(bool isFaceUp)
{
	if (isFaceUp)
	{
		FormCardFaceTexture(isFaceUp);
	}
	return sideSpritePtr;
}

void Card::setCardPosition(float posX, float posY)
{
	sideSpritePtr->setPosition(posX, posY);
	if (textPtr != nullptr && iconSpritePtr != nullptr)
	{
		//card is face down.
		textPtr->setPosition(sideSpritePtr->getPosition().x + (sideSpritePtr->getTextureRect().width / 2) - cardTextOffset, sideSpritePtr->getPosition().y + (sideSpritePtr->getTextureRect().height / 2));
		iconSpritePtr->setPosition(sideSpritePtr->getPosition().x + (iconSpritePtr->getTextureRect().width / 2), sideSpritePtr->getPosition().y + 4);
	}
}

//void Card::DrawCard2(bool isFaceUp, sf::RenderWindow& window, TextureHolder& textureHolder)
//{
//	sf::Texture& card = textureHolder.cardSide[(int)CardSide::Back];
//	sf::Sprite spriteForDrawing(card);
//	window.draw(spriteForDrawing);
//}
