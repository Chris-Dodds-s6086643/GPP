#include "Networking.h"

void Networking::SendMessageToServer(Message message)
{
	int sendResult = send(*serverSocket, message.ToString().c_str(), message.ToString().length() + 1, 0);
	if (sendResult == 0)
		std::cerr << "ERROR SENDING MESSAGE: " << message.ToString() << "\n";
	std::cout << "\n Message sent: " << message.ToString();
}

void Networking::ListenForMessages()
{
	char buffer[4096];
	while (listening)
	{
		ZeroMemory(buffer, 4096);
		int bytesReceieved = recv(*serverSocket, buffer, 4096, 0);
		if (bytesReceieved > 0)
		{
			std::string bufferString = std::string(buffer);
			std::vector<std::string> messages = SplitString(bufferString, StringEndingDelimiter);
			for (int i = 0; i < messages.size(); i++)
			{
				incomingMessageQueue.push(Message(messages.at(i)));
			}
		}
	}
}
