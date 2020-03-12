#include <SFML\Graphics.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <WS2tcpip.h>
#include "Networking.h"
#include "ThreadSafeQueue.h"


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
				std::cout << "direction vector was: " << oldMouseDirection.x << ", " << oldMouseDirection.y << "\nnow: " << mouseDirection.x << ", " << mouseDirection.y << "\n";
				messageToSend << ID << "::0::" << mouseDirection.x << "::" << mouseDirection.y;
				std::string stringToSend = messageToSend.str();
				networking.Send(stringToSend);
			}
		}
		sf::Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear();
		window.draw(shape);
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