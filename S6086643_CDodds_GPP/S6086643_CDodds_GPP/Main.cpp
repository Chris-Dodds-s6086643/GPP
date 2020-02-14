#include <SFML\Graphics.hpp>
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "Networking.h"
#include "ThreadSafeQueue.h"

#pragma comment(lib, "ws2_32.lib")


int main()
{
	sf::RenderWindow window(sf::VideoMode(960, 540), "SFML works!");
	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	ThreadSafeQueue<std::string> tsq;
	Networking networking(tsq);

	while (window.isOpen())
	{

		if (sf::Mouse::getPosition(window).x <= window.getSize().x && sf::Mouse::getPosition().x >= 0
			&& sf::Mouse::getPosition(window).y <= window.getSize().y && sf::Mouse::getPosition().y >= 0)
		{
			std::cout << "x: " << sf::Mouse::getPosition(window).x << ", y: " << sf::Mouse::getPosition(window).y << "\n";
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
	return 0;
}