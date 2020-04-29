#pragma once
#include <string>
#include <functional>
#include <SFML/Graphics/Font.hpp>
const std::string relativeResourcePath = "../../Resources/";

class Utility
{
private:
	//sf::Font* solutionFont;
	sf::Font* theFonz = nullptr;
public:
	Utility()
	{
		theFonz = new sf::Font();
		theFonz->loadFromFile(relativeResourcePath + "segoeui.ttf");
	}
	sf::Font* getFont()
	{
		return theFonz;
	}
};
