#include "Networking.h"
#include <sstream>


Networking::~Networking()
{
	while (!listening->is_lock_free()) {}
	listening->store(false);
	//std::ostringstream quitMessage;
	//quitMessage << _id << "::QUIT";
	//Send(quitMessage.c_str());
	std::ostringstream quitMessage;
	quitMessage << _id << "::QUIT::";
	std::string sendingMessage = quitMessage.str();
	Send(sendingMessage);
	listeningThread->join();
	delete(listeningThread);
}

void Networking::NetworkingStartUp()
{
	int winSockResult = WSAStartup(winSockVersion, &winSockData);
	if (winSockResult != 0)
	{
		return;
	}
	serverSocket = new SOCKET(socket(AF_INET, SOCK_STREAM, 0));


	if(*serverSocket == INVALID_SOCKET)
	{
		return;
	}

	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, serverIPAddress.c_str(), &hint.sin_addr);

	int connectionResult = connect(*serverSocket, (sockaddr*)& hint, sizeof(hint));
	if (connectionResult == SOCKET_ERROR)
	{
		std::cerr << "SOCKET ERROR";
		closesocket(*serverSocket);
		WSACleanup();
		return;
	}

	char buffer[4096];
	std::string userInput;
	listeningThread = new std::thread(&Networking::Listen, this);
}

bool Networking::Send(std::string& message)
{
	int sendResult = send(*serverSocket, message.c_str(), message.length() + 1, 0);
	if (sendResult == 0)
	{
		return false;
	}
	return true;

}

void Networking::Listen()
{
	char buffer[4096];
	while(*listening)
	{
		ZeroMemory(buffer, 4096);
		int bytesReceived = recv(*serverSocket, buffer, 4096, 0);
		if (bytesReceived > 0)
		{
			bytesReceived++;
			stringSafeQueue.push(std::string(buffer));
		}
	}
}
