#pragma once
#include "Utility.h"
#include <SFML\Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#include "Networking.h"
#include "ThreadSafeQueue.h"
#include "TextureHolder.h"
#include "Card.h"
#include "Utility.h"


#pragma comment(lib, "ws2_32.lib")

float fastSquareRoot(float number);

sf::Vector2f DirectionVector(sf::Vector2i origin, sf::Vector2i newLocation);

void gameSetupAndRunForTestingOnlyFuck();

int main()
{
	gameSetupAndRunForTestingOnlyFuck();

	return 0;
}

void gameSetupAndRunForTestingOnlyFuck()
{
	sf::RenderWindow window(sf::VideoMode(960, 540), "SFML works!");
	TextureHolder t;
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	ThreadSafeQueue<std::string> tsq;
	Networking networking(tsq);
	int ID = 0;
	float scale = 0.01f;
	sf::Vector2i oldMousePos{ 0,0 };
	sf::Vector2i mousePos{ 0,0 };
	sf::Vector2f oldMouseDirection{ 0.0f,0.0f };
	sf::Vector2f mouseDirection{ 0.0f,0.0f };
	sf::CircleShape circle(0.5f);
	Utility utility;

	Card cardSharksHearts[13] =
	{
		Card(CardSuit::hearts, CardValue::ace, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::two, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::three, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::four, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::five, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::six, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::seven, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::eight, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::nine, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::ten, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::jack, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::queen, t, utility.getFont()),
		Card(CardSuit::hearts, CardValue::king, t, utility.getFont())
	};

	Card cardSharksSpades[13] =
	{
		Card(CardSuit::spades, CardValue::ace, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::two, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::three, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::four, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::five, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::six, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::seven, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::eight, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::nine, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::ten, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::jack, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::queen, t, utility.getFont()),
		Card(CardSuit::spades, CardValue::king, t, utility.getFont())
	};


	Card cardSharksDiamonds[13] =
	{
		Card(CardSuit::diamonds, CardValue::ace, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::two, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::three, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::four, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::five, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::six, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::seven, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::eight, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::nine, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::ten, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::jack, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::queen, t, utility.getFont()),
		Card(CardSuit::diamonds, CardValue::king, t, utility.getFont())
	};


	Card cardSharksClubs[13] =
	{
		Card(CardSuit::clubs, CardValue::ace, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::two, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::three, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::four, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::five, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::six, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::seven, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::eight, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::nine, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::ten, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::jack, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::queen, t, utility.getFont()),
		Card(CardSuit::clubs, CardValue::king, t, utility.getFont())
	};


	Card* affectedCard = nullptr;
	sf::Font fontyPython;
	fontyPython.loadFromFile(relativeResourcePath + "segoeui.ttf");
	window.setFramerateLimit(0);
	while (window.isOpen())
	{
		std::string message;
		while (tsq.tryPop(message))
		{
			std::cout << "SERVER>" << message << "\n";
			if (message.substr(0, 2) == "ID")
			{
				ID = std::stoi(message.substr(message.length() - 1, 1));
				networking.setID(ID);
			}
		}
		oldMousePos = mousePos;
		mousePos = sf::Mouse::getPosition(window);
		if (mousePos.x <= window.getSize().x && mousePos.x >= 0
			&& mousePos.y <= window.getSize().y && mousePos.y >= 0)
		{
			oldMouseDirection = mouseDirection;
			mouseDirection = DirectionVector(oldMousePos, mousePos);
			if (mouseDirection.x != oldMouseDirection.x || mouseDirection.y != oldMouseDirection.y)
			{
				std::ostringstream messageToSend;
				//std::cout << "direction vector was: " << oldMouseDirection.x << ", " << oldMouseDirection.y << "\nnow: " << mouseDirection.x << ", " << mouseDirection.y << "\n";
				messageToSend << ID << "::0::" << mouseDirection.x << "::" << mouseDirection.y;
				std::string stringToSend = messageToSend.str();
				networking.Send(stringToSend);
			}
		}
		window.clear();

		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::MouseButtonPressed)
			{
				/*for (int i = 0; i < 13; i++)
				{*/
				//	std::cout << i << ": " << "X: " << cardSharksClubs[i].getRect().left << " , Y: " << cardSharksClubs[i].getRect().top << ", Width: " << cardSharksClubs[i].getRect().width << ", Height: " << cardSharksClubs[i].getRect().height << "\n";
				//	if (cardSharksClubs[i].getRect().contains(mousePos.x, mousePos.y))
				//	{
				//		if (affectedCard != nullptr)
				//		{
				//			affectedCard->~Card();
				//		}
				//		delete(affectedCard);
				//		affectedCard = new Card(cardSharksClubs[i]);
				//		break;
				//	}
				//	if (cardSharksHearts[i].getRect().contains(mousePos.x, mousePos.y))
				//	{
				//		delete(affectedCard);
				//		affectedCard = new Card(cardSharksHearts[i]);
				//		break;
				//	}
				//	if (cardSharksSpades[i].getRect().contains(mousePos.x, mousePos.y))
				//	{
				//		delete(affectedCard);
				//		affectedCard = new Card(cardSharksSpades[i]);
				//		break;
				//	}
				//	if (cardSharksDiamonds[i].getRect().contains(mousePos.x, mousePos.y))
				//	{
				//		delete(affectedCard);
				//		affectedCard = new Card(cardSharksDiamonds[i]);
				//		break;
				//	}
				//}
				//if (affectedCard != nullptr)
				//{
				//	std::cout << affectedCard->toString() << "\n";
				//}
			}
			if (event.type == sf::Event::MouseButtonReleased)
			{
				delete(affectedCard);
				affectedCard = nullptr;
			}
			//if (event.type == sf::Event::MouseMoved)
			//{
			//	if (affectedCard != nullptr)
			//	{
			//		affectedCard->setCardPosition(event.mouseMove.x , event.mouseMove.y);
			//		/*affectedCard->DrawCard(false, window, t, 100, 100);*/
			//	}
	
			//}


			
		}

		window.draw(shape);
		for (int i = 0; i < 13; i++)
		{
			if (cardSharksClubs[i].getCardSprite(false) == nullptr)
			{
				cardSharksSpades[i].FormCardFaceTexture(true);
			}
			cardSharksSpades[i].setCardPosition(float(i * 50), 0);
			cardSharksSpades[i].DrawCard(true, window, t, (float)i * 50, 0);
		}
		for (int i = 0; i < 13; i++)
		{
			if (cardSharksDiamonds[i].getCardSprite(false) == nullptr)
			{
				cardSharksDiamonds[i].FormCardFaceTexture(true);
			}
			cardSharksDiamonds[i].setCardPosition(float(i * 50), 70);
			cardSharksDiamonds[i].DrawCard(true, window, t, (float)i * 50, 70);
		}
		for (int i = 0; i < 13; i++)
		{

			if (cardSharksClubs[i].getCardSprite(true) == nullptr)
			{
				cardSharksClubs[i].FormCardFaceTexture(true);
			}
			cardSharksClubs[i].setCardPosition(float(i * 50), 140);
			cardSharksClubs[i].DrawCard(true, window, t, (float)i * 50, 140);

		}
		for (int i = 0; i < 13; i++)
		{
			if (cardSharksHearts[i].getCardSprite(false) == nullptr)
			{
				cardSharksHearts[i].FormCardFaceTexture(true);
			}
			cardSharksHearts[i].setCardPosition(float(i * 50), 210);
			cardSharksHearts[i].DrawCard(true, window, t, (float)i * 50, 210);
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
		{
			/*mousePos.x = event.mouseMove.x;
			mousePos.y = event.mouseMove.y;*/
			for (int i = 0; i < 13; i++)
			{
				//std::cout << i << ": " << "X: " << cardSharksClubs[i].getRect().left << " , Y: " << cardSharksClubs[i].getRect().top << ", Width: " << cardSharksClubs[i].getRect().width << ", Height: " << cardSharksClubs[i].getRect().height << "\n";
				if (cardSharksClubs[i].getRect().contains(mousePos.x, mousePos.y))
				{
					//delete(affectedCard);
					affectedCard = new Card(cardSharksClubs[i]);
					break;
				}
				if (cardSharksHearts[i].getRect().contains(mousePos.x, mousePos.y))
				{
					delete(affectedCard);
					affectedCard = new Card(cardSharksHearts[i]);
					break;
				}
				if (cardSharksSpades[i].getRect().contains(mousePos.x, mousePos.y))
				{
					delete(affectedCard);
					affectedCard = new Card(cardSharksSpades[i]);
					break;
				}
				if (cardSharksDiamonds[i].getRect().contains(mousePos.x, mousePos.y))
				{
					delete(affectedCard);
					affectedCard = new Card(cardSharksDiamonds[i]);
					break;
				}
			}
			//if (affectedCard != nullptr)
			//{
			//	//affectedCard->DrawCard(false, window, t, sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			//	sf::Vector2f newpos = sf::Vector2f(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
			//	//affectedCard->FormCardFaceTexture(true);
			//	affectedCard->setCardPosition(newpos.x, newpos.y);
			//	affectedCard->DrawCard(false, window, t, 100, 100);
			//	//window.draw(*affectedCard->getCardSprite(true));
			//}
		}
		

		if (affectedCard != nullptr)
		{
			affectedCard->setCardPosition(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y);
			affectedCard->DrawCard(false, window, t, 0, 0);
		}

		window.display();
	}
}

float fastSquareRoot(float number)
{
	long i;
	float x2{ 0 }, y{ 0 };
	const float threeHalfs = 1.5f;

	x2 *= 0.5f;

	y = number;
	i = *(long*)& y;
	i = 0x5f3759df - (i >> 1);
	y = *(float*)& i;
	y *= (threeHalfs - (x2 * y * y));
	return y;
}

sf::Vector2f DirectionVector(sf::Vector2i origin, sf::Vector2i newLocation)
{
	sf::Vector2i differenceVector = newLocation - origin;

	double scale = 0.01f;
	float magnitude = std::sqrt((float)(differenceVector.x * differenceVector.x) + (differenceVector.y * differenceVector.y));
	magnitude = magnitude > 0 ? magnitude : 1;
	//if (std::abs(differenceVector.x) > 0 || std::abs(differenceVector.y) > 0)
	//{
	//	//std::cout << "differenceVector = " << differenceVector.x << ", " << differenceVector.y << "\n" << "magnitude:" << magnitude;
	//	//std::cout << "OUTPUT: X: " << differenceVector.x / magnitude << ", Y: " << differenceVector.y / magnitude << "\n";
	//}
	return sf::Vector2f((int)((differenceVector.x / magnitude)/scale) * scale, (int)((differenceVector.y / magnitude) / scale) * scale);
}