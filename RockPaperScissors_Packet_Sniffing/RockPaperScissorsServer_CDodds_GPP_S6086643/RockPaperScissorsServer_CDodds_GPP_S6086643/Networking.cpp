#include "Networking.h"
#include <functional>
#include <sstream>
#include "Message.h"

void Networking::ListenForConnections()
{
	while (this->serverRunning) // replace true with running atomic bool;
	{
		listen(*serverConnectionListeningSocket, SOMAXCONN); //pauses until something is received;
		sockaddr_in newClient;
		int newClientSize = sizeof(newClient);
		SOCKET* newSocket = new SOCKET(accept(*serverConnectionListeningSocket, (sockaddr*)&newClient, &newClientSize));
		if (*newSocket == INVALID_SOCKET)
		{
			delete newSocket;
			std::cerr << "CLIENT SOCKET FAILED TO CREATE \n" << WSAGetLastError() << "\n";
		}
		else
		{
			ZeroMemory(host, NI_MAXHOST);
			ZeroMemory(service, NI_MAXSERV);
			newClientSize = sizeof(newClient);
			if (getnameinfo((sockaddr*)&newClient, newClientSize, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0)
			{
				std::cout << "Connection on port: " << service << "\n";
			}
			else
			{
				inet_ntop(AF_INET, &newClient.sin_addr, host, NI_MAXHOST);
				std::cout << host << " Connected On Port " << ntohs(newClient.sin_port) << "\n";
			}
			//Create new ClientPlayer, with a unique ID, storing their socket, a thread that listens and puts their messages onto the messageQueue;
			int newID = clientMap.GetNextAvailableKeyId();
			ClientPlayer* newPlayer = new ClientPlayer(newSocket, new std::thread([this, newID] { ReceiveFromOnePlayer(newID); }), newID);
			clientMap.Emplace(newID, newPlayer);

			//create a message to be sent to the joining player containing the public key. Would need a hash/proof value to prevent man-in-the-middle attacks.
			std::vector<int> parametersVector;
			//using ErrorInt encrypted with the privateKey as a makeshift proof of identity.
			parametersVector.push_back(encryption->EncryptOrDecipherIntWithPrivateKey(ErrorInt));
			parametersVector.push_back(encryption->GetPublicKey());
			parametersVector.push_back(encryption->GetKeyModulus());
			Message publicKeyMessage(newID, MessagePurpose::PublicKeyMessage, parametersVector);
			SendMessageToClientPlayerByID(newID, publicKeyMessage);
		}
	}
}

void Networking::ReceiveFromOnePlayer(int playerID)
{
	char buffer[4096];
	while (!this->serverRunning.is_lock_free()) {}
	while (this->serverRunning && clientMap.AccessValue(playerID)->IsPlayerActive())
	{
		ZeroMemory(buffer, 4096);
		int bytesReceived = recv(*(clientMap.AccessValue(playerID)->GetSocket()), buffer, 4096, 0); // hangs here.
		if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error in Recieving on playter ID: " << playerID << "\n" << WSAGetLastError() << "\n";
			std::vector<int> params;
			Message quitMessage(playerID, MessagePurpose::Quit, params);
			EchoMessage(quitMessage);
			break;
		}
		bytesReceived++;
		std::string bufferString = std::string(buffer);
		std::vector<std::string> messageStrings = SplitString(bufferString, StringEndingDelimiter);
		for (int i = 0; i < messageStrings.size(); i++)
		{
			Message potentiallyEncryptedIncomingMessage = Message(messageStrings.at(i));
			//if purpose is readable it isn't totally encrypted; otherwise  it should be the publicKeyMessage and have params of:\n
			// [0] client encrypted ErrorInt with their private key, [1] unencrypted public key, [2] unencrypted modulus
			if (potentiallyEncryptedIncomingMessage.GetMessagePurpose() == MessagePurpose::PublicKeyMessage &&
				encryption->EncryptOrDecipherInt(potentiallyEncryptedIncomingMessage.GetParameters().at(0), potentiallyEncryptedIncomingMessage.GetParameters().at(1), potentiallyEncryptedIncomingMessage.GetParameters().at(2)) == ErrorInt)
			{
				clientMap.AccessValue(playerID)->SetPublicKey(new KeyModulusPair(potentiallyEncryptedIncomingMessage.GetParameters().at(1), potentiallyEncryptedIncomingMessage.GetParameters().at(2)));
				this->messageQueue.push(potentiallyEncryptedIncomingMessage);
			}
			else
			{
				//decrypt with client key first
				Message incomingMessage = encryption->EncryptOrDecipherMessage(Message(messageStrings.at(i)), clientMap.AccessValue(playerID)->GetPublicKey().GetKey(), clientMap.AccessValue(playerID)->GetPublicKey().GetModulus());
				//decrypt with server private key
				incomingMessage = encryption->EncryptOrDecipherMessageWithPrivateKey(incomingMessage);
				if (incomingMessage.GetMessagePurpose() == MessagePurpose::Quit)
				{
					clientMap.AccessValue(playerID)->setPlayerInactive();
				}
				this->messageQueue.push(encryption->EncryptOrDecipherMessageWithPrivateKey(incomingMessage));
			}
		}
	}
}

void Networking::GameLoop()
{
	while (!this->serverRunning.is_lock_free()) {}
	while (this->serverRunning)
	{
		Message messageFromQueue;
		while (messageQueue.tryPop(messageFromQueue))
		{
			//when a key gets here it should be decrypted;
			switch (messageFromQueue.GetMessagePurpose())
			{
			case MessagePurpose::Join:
			{
				playerIDsWaitingForMatch.push_back(messageFromQueue.GetID());
				break;
			}
			case MessagePurpose::Quit:
			{
				clientMap.RemoveValueAtKey(messageFromQueue.GetID());
				if (messageFromQueue.GetOpponentID() != ErrorInt)
				{
					SendMessageToClientPlayerByID(messageFromQueue.GetOpponentID(), messageFromQueue); 
				}
				break;
			}
			case MessagePurpose::Input:
			{
				//clientMap.AccessValue(messageFromQueue.GetID())->SetInput(MessageInputs(messageFromQueue.GetParameters().at(0)));
				SendMessageToClientPlayerByID(messageFromQueue.GetOpponentID(), messageFromQueue);
				break;
			}
			case MessagePurpose::GameEnd:
			{
				int opponentID = ErrorInt;
				switch (Result(messageFromQueue.GetParameters().at(0)))
				{
					case Result::Loss:
					{
						// I guess you'd update some statistics; if nothing changes here put both this and Result::Win into default;
						playerIDsWaitingForMatch.push_back(messageFromQueue.GetID());
						break;
					}
					case Result::Win:
					{
						// I guess you'd update some statistics;
						playerIDsWaitingForMatch.push_back(messageFromQueue.GetID());
						break;
					}
					case Result::Draw:
					{
						opponentID = messageFromQueue.GetOpponentID();
						break;
					}
					default:
					{
						break;
					}
				}
				std::vector<int> params;
				Message endGameAcknowledgement(ErrorInt, MessagePurpose::GameEnd, params);
				endGameAcknowledgement.setOpponentID(opponentID);
				SendMessageToClientPlayerByID(messageFromQueue.GetID(), endGameAcknowledgement);
				break;
			}
			case MessagePurpose::Error:
			{
				std::cerr << "error in pulling message from queue";
				break;
			}
			case  MessagePurpose::PublicKeyMessage:
			{
				std::vector<int> params;
				Message acknowledgementJoinMessage = Message(messageFromQueue.GetID(), MessagePurpose::Join, params);
				SendMessageToClientPlayerByID(messageFromQueue.GetID(), acknowledgementJoinMessage);
				break;
			}
			default:
				break;
			}
			

			//EchoMessage(messageFromQueue);
		}
		while (playerIDsWaitingForMatch.size() >= 2)
		{
			int idOne = playerIDsWaitingForMatch.at(0);
			int idTwo = playerIDsWaitingForMatch.at(1);
			playerIDsWaitingForMatch.erase(playerIDsWaitingForMatch.begin());
			playerIDsWaitingForMatch.erase(playerIDsWaitingForMatch.begin());
			std::vector<int> inParameters;
			Message idOneJoinMessage(ErrorInt, MessagePurpose::Join, inParameters);
			idOneJoinMessage.setOpponentID(idTwo);
			SendMessageToClientPlayerByID(idOne, idOneJoinMessage);
			Message idTwoJoinMessage(ErrorInt, MessagePurpose::Join, inParameters);
			idTwoJoinMessage.setOpponentID(idOne);
			SendMessageToClientPlayerByID(idTwo, idTwoJoinMessage);
			playerIDsWaitingForMatch.shrink_to_fit();
		}
	} 
}

void Networking::EchoMessage(Message messageToEcho)
{
	clientMap.FunctionWithStringOverMapExcludingGivenKey(SendMessageToClientPlayer, messageToEcho, messageToEcho.GetID());
}

void Networking::SendMessageToClientPlayerByID(int playerID, Message messageToSend)
{
	if (messageToSend.GetMessagePurpose() != MessagePurpose::PublicKeyMessage)
	{
		messageToSend = encryption->EncryptOrDecipherMessageWithPrivateKey(messageToSend);
		messageToSend = encryption->EncryptOrDecipherMessage(messageToSend, clientMap.AccessValue(playerID)->GetPublicKey().GetKey(), clientMap.AccessValue(playerID)->GetPublicKey().GetModulus());
	}
	send(*(clientMap.AccessValue(playerID)->GetSocket()), messageToSend.ToString().c_str(), messageToSend.ToString().length(), 0);
	std::cout << "Sending: " << messageToSend.ToString() << " to player with ID: " << playerID << "\n";
}

