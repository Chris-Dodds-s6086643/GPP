#include <SFML\Graphics.hpp>
#include <iostream>
#include <string>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib");


int main()
{
	//sf::RenderWindow window(sf::VideoMode(960, 540), "SFML works!");
	//sf::CircleShape shape(100.f);
	//shape.setFillColor(sf::Color::Green);

	//while (window.isOpen())
	//{
	//	sf::Event event;
	//	while (window.pollEvent(event))
	//	{
	//		if (event.type == sf::Event::Closed)
	//			window.close();
	//	}

	//	window.clear();
	//	window.draw(shape);
	//	window.display();
	//}

	//TO BE REFACTORED INTO A SEPERATE THINGY M'BOB

	std::string serverIPAddress = "127.0.0.1";
	int port = 54000;

	WSAData winSockData;
	WORD winSockVersion = MAKEWORD(2, 2);

	int winSockResult = WSAStartup(winSockVersion, &winSockData);

	if (winSockResult != 0)
	{
		return 0;
	}

	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (clientSocket == INVALID_SOCKET)
	{
		return 0;
	}

	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, serverIPAddress.c_str(), &hint.sin_addr);

	int connectionResult = connect(clientSocket, (sockaddr*)& hint, sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "SOCKET ERROR";
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	char buffer[4096];
	std::string userInput;

	do {
		std::cout << ">";
		std::getline(std::cin, userInput);
		
		if (userInput.size() > 0)
		{
			int sendResult = send(clientSocket, userInput.c_str(), userInput.size() + 1, 0);
			if (sendResult != SOCKET_ERROR)
			{
				ZeroMemory(buffer, 4096);
				int bytesReceived = recv(clientSocket, buffer, 4096, 0);
				if (bytesReceived > 0)
				{
					std::cout << "SERVER> " << std::string(buffer, 0, bytesReceived) << "\n";
				}
			}

		}

		

	} while (userInput.size() > 0);

	return 0;
}