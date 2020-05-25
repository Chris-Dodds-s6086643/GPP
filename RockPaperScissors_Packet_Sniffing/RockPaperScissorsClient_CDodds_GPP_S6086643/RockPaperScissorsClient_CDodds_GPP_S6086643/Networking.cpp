#include "Networking.h"

void Networking::SendMessageToServer(Message message)
{
	if (message.GetMessagePurpose() != MessagePurpose::PublicKeyMessage)
	{
		message = encryption->EncryptOrDecipherMessage(message, serverPublicKey, serverPublicModulus);
		message = encryption->EncryptOrDecipherMessageWithPrivateKey(message);
		int sendResult = send(*serverSocket, message.ToString().c_str(), message.ToString().length() + 1, 0);
		if (sendResult == 0)
			std::cerr << "ERROR SENDING MESSAGE: " << message.ToString() << "\n";
	}
	else
	{
		std::vector<int> newParams;
		newParams.push_back(encryption->EncryptOrDecipherIntWithPrivateKey(ErrorInt));
		newParams.push_back(encryption->GetPublicKey());
		newParams.push_back(encryption->GetKeyModulus());
		Message newMessageToSend = Message(message.GetID(), MessagePurpose::PublicKeyMessage, newParams);
		int sendResult = send(*serverSocket, newMessageToSend.ToString().c_str(), newMessageToSend.ToString().length() + 1, 0);
		if (sendResult == 0)
			std::cerr << "ERROR SENDING MESSAGE: " << newMessageToSend.ToString() << "\n";
	}
	//std::cout << "\n Message sent: " << message.ToString();
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
				Message incomingMessage = Message(messages.at(i));
				if (incomingMessage.GetMessagePurpose() == MessagePurpose::PublicKeyMessage &&
					encryption->EncryptOrDecipherInt(incomingMessage.GetParameters().at(0), incomingMessage.GetParameters().at(1), incomingMessage.GetParameters().at(2)) == ErrorInt)
				{
					serverPublicKey = incomingMessage.GetParameters().at(1);
					serverPublicModulus = incomingMessage.GetParameters().at(2);
				}
				else
				{
					incomingMessage = encryption->EncryptOrDecipherMessageWithPrivateKey(incomingMessage);
					incomingMessage = encryption->EncryptOrDecipherMessage(incomingMessage, serverPublicKey, serverPublicModulus);
				}
				incomingMessageQueue.push(incomingMessage);
			}
		}
	}
}
