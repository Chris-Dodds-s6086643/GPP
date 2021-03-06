#include "Encryption.h"
#include <iostream>
#include <array>
#include <time.h>
#include <stdlib.h>
#include "Utility.h"
#include <bitset>
#include <math.h>

/**
* Contains the necessary variables, generated on construction, to encrypt and decrypt both integers and the integers contained within the Message class of the RockPaperScissorsServer/Client;
* Using the RSA algorithm but without any big integer libraries.
* Should really be made more generic and reusable in terms of the message class being manipulated.
*/
Encryption::Encryption()
{
	srand(time(NULL));
#pragma region Define member Variables to error value (avoids annoying warning);
	//awful hackaround to get some basic prime numbers to work from.
	primeNumbers =
	{
		11, 13, 17, 19, 23, 29, /*31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199*/
	};
	primeNumberOne = ErrorInt;
	primeNumberTwo = ErrorInt;
	publicKey = ErrorInt;
	privateKey = ErrorInt;
	keyModulus = ErrorInt;
	phiKeyModulus = ErrorInt;
#pragma endregion
	SetFixedPrimeNumbers();
	GeneratePublicKey();
	GeneratePrivateKey();
}

#pragma region SetUpFunctions
void Encryption::SetFixedPrimeNumbers()
{
	primeNumberOne = 17;
	primeNumberTwo = 29;
}

void Encryption::GenerateRandomPrimeNumbers()
{
	int primeNumberIndexOne = rand() % primeNumbers.size();
	int primeNumberIndexTwo = rand() % primeNumbers.size();
	if (primeNumbers.size() < 2)
	{
		std::cerr << "primeNumbers Array seems a bit small as size is: " << primeNumbers.size() << "\n";
		return;
	}
	while (primeNumberIndexTwo == primeNumberIndexOne)
	{
		primeNumberIndexTwo = rand() % primeNumbers.size();
	}
	primeNumberOne = primeNumbers[primeNumberIndexOne];
	primeNumberTwo = primeNumbers[primeNumberIndexTwo];
}

void Encryption::GeneratePublicKey()
{
	keyModulus = (long long int)primeNumberOne * (long long int)primeNumberTwo;
	long long int phiKeyModulus = ((long long int)primeNumberOne - 1) * ((long long int)primeNumberTwo - 1);
	// public key must be coprime with modulus, > 1 and < numberOfCoPrimeNumbersWithModulus;
	for (int i = 3; i < phiKeyModulus; i++) // must be > 2 for a decent key, 3 is usual for public keys.
	{
		if (IsRelativelyPrime(i, keyModulus) && IsRelativelyPrime(i, phiKeyModulus))
		{
			publicKey = i;
			break;
		}
	}
	if (publicKey == ErrorInt)
	{
		throw "publicKey not set: ERROR;\n"; // really shouldn't happen;
	}
}

//basically EXTENDED EUCLIDIAN ALGORITHM, apparently, derived from: https://www.youtube.com/watch?v=Z8M2BTscoD4 09:49 - ~14:22; e(public key)  * d(private key) mod phi == 1;
void Encryption::GeneratePrivateKey()
{
//#pragma region Find The First Relevant Key (de(mod thi(N)))
//	int firstRelevantKey = ErrorInt;
//	for (int i = 1; i <= keyModulus; i++)
//	{
//		int de = publicKey * i;
//		int modRemainder = de % phiKeyModulus;
//		if (modRemainder == 1)
//		{
//			firstRelevantKey = i;
//			break;
//		}
//	}
//	if (firstRelevantKey == ErrorInt)
//	{
//		std::cerr << "no relevant key found, ERROR \n";
//		return;
//	}
//#pragma endregion
//	int randomStepsToAdd = rand() % 15 + 1; // add minimum 1 step and max 15, increases complexity or something.
//	privateKey = (firstRelevantKey + (randomStepsToAdd * keyModulus));
	enum section {
		top = 0,
		middle = 1,
		bottom = 2
	};

	int columnOne[3] = { phiKeyModulus, publicKey, 0 };
	int columnTwo[3] = { phiKeyModulus, 1, 0 };

	while (columnOne[middle] != 1)
	{
#pragma region Calculate new bottom columns
		int columnOneDivision = (columnOne[top] / columnOne[middle]);
		columnOne[bottom] = (columnOne[top] - (columnOne[middle] * columnOneDivision));
		columnTwo[bottom] = (columnTwo[top] - (columnTwo[middle] * columnOneDivision));
		if (columnTwo[bottom] < 0)
		{
			columnTwo[bottom] = phiKeyModulus + columnTwo[bottom];
		}
#pragma endregion

#pragma region move rows up and clear bottoms;
		columnOne[top] = columnOne[middle];
		columnOne[middle] = columnOne[bottom];
		columnOne[bottom] = 0;

		columnTwo[top] = columnTwo[middle];
		columnTwo[middle] = columnTwo[bottom];
		columnTwo[bottom] = 0;
#pragma endregion
	}
	privateKey = columnTwo[middle];
}
#pragma endregion

#pragma region Encryption and Decipher functions
//only needs to be int due to the prime numbers currently being small so the modulus (moodulus-1 being the max result of the calculation) can't need long long int really.
int Encryption::EncryptOrDecipherInt(int value, long long int key)
{
	return ModularExponentiation(value, key);
}

Message Encryption::EncryptOrDecipherMessage(Message message, long long int key)
{
	int encryptedSenderID = EncryptOrDecipherInt(message.GetID(), key);
	int encryptedMessagePurpose = EncryptOrDecipherInt((int)message.GetMessagePurpose(), key);
	int encryptedOpponentID = EncryptOrDecipherInt(message.GetOpponentID(), key);
	std::vector<int> encryptedParamaters;
	for (int i = 0; i < message.GetParameters().size(); i++)
	{
		encryptedParamaters.push_back(EncryptOrDecipherInt(message.GetParameters()[i], key));
	}
	return Message(encryptedSenderID, (MessagePurpose)encryptedMessagePurpose, encryptedOpponentID, encryptedParamaters);
}

//in theory removes need for big int library, still would need a large integer library for an industry standard RSA method.
long long int Encryption::ModularExponentiation(long long int integer, long long int desiredPower)
{
#pragma region Calculate base^(binary powers for as many as the desiredPower represented in binary);
	//make binary representation the desiredPower as string to use to determine which results need to be multiplied; Could probably be smaller than 4096 but oh well;
	std::string desiredPowerBinaryString = std::bitset<4096>(desiredPower).to_string();
	//removing leading 0s
	desiredPowerBinaryString.erase(0, desiredPowerBinaryString.find("1"));
	std::vector<long long int> results;
	results.resize(desiredPowerBinaryString.length());
	for (int i = desiredPowerBinaryString.length() - 1; i >= 0; i--) //work through binary in reverse so that the indexes match with the binary results position; least significant first;
	{
		//if first index use the given integer (given integer ^ 1 = integer) as there is no previous result to ^2;
		long long int powResult = (i == desiredPowerBinaryString.length() - 1) ? integer : pow(results.at(i + 1), 2);
		//turnery as standard % seems to have issues with negatives and apparently this is equal; Source = https://youtu.be/Z8M2BTscoD4?t=772: extrapolated from the explanation of calculating matching keys for RSA.
		long long int modResult = powResult >= 0 ? powResult % keyModulus : keyModulus - powResult;
		//set relevant index in results to (base ^ (i(exponent))) mod modulus; or the result of the previous ^ 2 mod modulus; as the maths still holds;
		results.at(i) = modResult;
	}
#pragma endregion

#pragma region Times together the relevant results for the desired binary power;
	long long int finalResult = 1; // =1 for all the *=ing;
	for (int ii = 0; ii < results.size(); ii++)
	{
		finalResult *= desiredPowerBinaryString[ii] == '1' ? results.at(ii) : 1;
	}
#pragma endregion
	//return modulus of total results mod modulus to keep balance in the force;
	return finalResult >= 0 ? (finalResult % keyModulus) : (keyModulus - finalResult);
}
#pragma endregion

//extrapolated from https://www.youtube.com/watch?v=p5gn2hj51hs; maths function used to check the validity of the created key.
bool Encryption::IsRelativelyPrime(int initialDivisor, long long int initialDivisee)
{
#pragma region Find Greatest Common Divisor
	long long int smallerNum = initialDivisor < initialDivisee ? initialDivisor : initialDivisee;
	long long int largerNum = initialDivisor > initialDivisee ? initialDivisor : initialDivisee;
	long long int modRemainder = ErrorInt;
	long long int newRemainder = ErrorInt;
	do
	{
		newRemainder = largerNum % smallerNum;
		modRemainder = (newRemainder != 0) ? newRemainder : modRemainder; //so if newRemainder == 0 keep the previousRemainder.
		largerNum = smallerNum;
		smallerNum = newRemainder;
	} while (newRemainder != 0);
#pragma endregion
	// the last non-zero remainder is the greatest common divisor, if this is 1 then they are coprime / relatively prime;
	return modRemainder == 1;
}

#pragma region Public calls to Encryption and deciphering with stated key to maintain Encapsulation
int Encryption::EncryptOrDecipherIntWithPrivateKey(int intToEncryptOrDecipher)
{
	return EncryptOrDecipherInt(intToEncryptOrDecipher, privateKey);
}

int Encryption::EncryptOrDecipherIntWithPublicKey(int intToEncryptOrDecipher)
{
	return EncryptOrDecipherInt(intToEncryptOrDecipher, publicKey);
}

Message Encryption::EncryptOrDecipherMessageWithPrivateKey(Message messageToEncrypt)
{
	return EncryptOrDecipherMessage(messageToEncrypt, privateKey);
}

Message Encryption::EncryptOrDecipherMessageWithPublicKey(Message messageToEncrypt)
{
	return EncryptOrDecipherMessage(messageToEncrypt, publicKey);
}
#pragma endregion

Encryption::~Encryption() {}
