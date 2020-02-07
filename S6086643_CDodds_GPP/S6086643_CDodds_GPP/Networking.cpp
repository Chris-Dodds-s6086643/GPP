#include "Networking.h"

Networking::~Networking()
{
}

void Networking::NetworkingStartUp()
{
	int winSockResult = WSAStartup(winSockVersion, &winSockData);
	if (winSockResult != 0)
	{
		return;
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);


	if (clientSocket == INVALID_SOCKET)
	{
		return;
	}

	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, serverIPAddress.c_str(), &hint.sin_addr);

	int connectionResult = connect(clientSocket, (sockaddr*)& hint, sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "SOCKET ERROR";
		closesocket(clientSocket);
		WSACleanup();
		return;
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

}