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
		/*11, 13, 17,*/ 19, 23, 29, /*31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199*/
	};
	primeNumberOne = ErrorInt;
	primeNumberTwo = ErrorInt;
	publicKey = ErrorInt;
	privateKey = ErrorInt;
	keyModulus = ErrorInt;
	phiKeyModulus = ErrorInt;
#pragma endregion
	GenerateRandomPrimeNumbers();
	GenerateKeys();
}

#pragma region Prime number setting and key generation
//should use random but as the  limit  to the  numbers represented is (primeNumberOne - 1)*(primeNumberTwo - 1) allowing the use of really small  values seems dumb, also anything as high as 23 & 29 cause overflow errors;
void Encryption::SetFixedPrimeNumbers()
{
	primeNumberOne = 19;
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
	primeNumberOne = primeNumberIndexOne > primeNumberIndexTwo ? primeNumbers[primeNumberIndexOne] : primeNumbers[primeNumberIndexTwo];
	primeNumberTwo = primeNumberOne == primeNumbers[primeNumberIndexOne] ? primeNumbers[primeNumberIndexTwo] : primeNumbers[primeNumberIndexOne];
}

void Encryption::GenerateKeys()
{
	keyModulus = (long long int)primeNumberOne * (long long int)primeNumberTwo;
	phiKeyModulus = ((long long int)primeNumberOne - 1) * ((long long int)primeNumberTwo - 1);
	int possibleKeyContainer = (phiKeyModulus * 2) + 1;
	while (privateKey == ErrorInt && publicKey == ErrorInt && !(possibleKeyContainer >= phiKeyModulus * 20))
	{
		for (int i = 3; i < (possibleKeyContainer >> 1); i++)
		{
			if (possibleKeyContainer % i == 0)
			{
				int divisor = possibleKeyContainer / i;
				if (AreCoPrime(i, phiKeyModulus), AreCoPrime(divisor, phiKeyModulus), AreCoPrime(i, divisor))
				{
					publicKey = i;
					privateKey = divisor;
					break;
				}
			}
		}
		possibleKeyContainer += phiKeyModulus;
	}
}

#pragma region Deprecated Key generations, not enough emphasis on the coprimeness of the keys with each other and the phi(modulus);
//void Encryption::GeneratePublicKey()
//{
//	keyModulus = (long long int)primeNumberOne * (long long int)primeNumberTwo;
//	phiKeyModulus = ((long long int)primeNumberOne - 1) * ((long long int)primeNumberTwo - 1);
//	// public key must be coprime with modulus, > 1 and < numberOfCoPrimeNumbersWithModulus;
//	for (int i = 3; i < phiKeyModulus; i++) // must be > 2 for a decent key, 3 is usual for public keys.
//	{
//		if (AreCoPrime(i, keyModulus) && AreCoPrime(i, phiKeyModulus))
//		{
//			publicKey = i;
//			break;
//		}
//	}
//	if (publicKey == ErrorInt)
//	{
//		throw "publicKey not set: ERROR;\n"; // really shouldn't happen;
//	}
//}
//
////basically EXTENDED EUCLIDIAN ALGORITHM, apparently, derived from: https://www.youtube.com/watch?v=Z8M2BTscoD4 09:49 - ~14:22; e(public key)  * d(private key) mod phi == 1;
//void Encryption::GeneratePrivateKey()
//{
////#pragma region Find The First Relevant Key (de(mod thi(N)))
////	int firstRelevantKey = ErrorInt;
////	for (int i = 1; i <= keyModulus; i++)
////	{
////		int de = publicKey * i;
////		int modRemainder = de % phiKeyModulus;
////		if (modRemainder == 1)
////		{
////			firstRelevantKey = i;
////			break;
////		}
////	}
////	if (firstRelevantKey == ErrorInt)
////	{
////		std::cerr << "no relevant key found, ERROR \n";
////		return;
////	}
////#pragma endregion
////	int randomStepsToAdd = rand() % 15 + 1; // add minimum 1 step and max 15, increases complexity or something.
////	privateKey = (firstRelevantKey + (randomStepsToAdd * keyModulus));
//	enum section {
//		top = 0,
//		middle = 1,
//		bottom = 2
//	};
//
//	int columnOne[3] = { phiKeyModulus, publicKey, 0 };
//	int columnTwo[3] = { phiKeyModulus, 1, 0 };
//
//	while (columnOne[middle] != 1)
//	{
//#pragma region Calculate new bottom columns
//		int columnOneDivision = (columnOne[top] / columnOne[middle]);
//		columnOne[bottom] = (columnOne[top] - (columnOne[middle] * columnOneDivision));
//		columnTwo[bottom] = (columnTwo[top] - (columnTwo[middle] * columnOneDivision));
//		if (columnTwo[bottom] < 0)
//		{
//			columnTwo[bottom] = phiKeyModulus + columnTwo[bottom];
//		}
//#pragma endregion
//
//#pragma region move rows up and clear bottoms;
//		columnOne[top] = columnOne[middle];
//		columnOne[middle] = columnOne[bottom];
//		columnOne[bottom] = 0;
//
//		columnTwo[top] = columnTwo[middle];
//		columnTwo[middle] = columnTwo[bottom];
//		columnTwo[bottom] = 0;
//#pragma endregion
//	}
//	privateKey = columnTwo[middle];
//}
#pragma endregion

#pragma endregion

#pragma region Maths utility esque functions
//in theory removes need for big int library, still would need a large integer library for an industry standard RSA method.
long long int Encryption::ModularExponentiation(long long int integer, long long int desiredPower, long long int modulus)
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
		long long int modResult = powResult >= 0 ? powResult % modulus : modulus - powResult;
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
	return finalResult >= 0 ? (finalResult % modulus) : (modulus - finalResult);
}

//extrapolated from https://www.youtube.com/watch?v=p5gn2hj51hs; maths function used to check the validity of the created key.
bool Encryption::AreCoPrime(int lhs, long long int rhs)
{
#pragma region Find Greatest Common Divisor
	long long int smallerNum = lhs < rhs ? lhs : rhs;
	long long int largerNum = lhs < rhs ? rhs : lhs;
	long long int newRemainder = ErrorInt;
	long long int previousRemainder = ErrorInt;
	//rem = big % little
	//if rem != 0 big = little && little = rem;
	//if rem == 0 last non-zero remainder is the greatest common divisor;
	do
	{
		previousRemainder = newRemainder;
		newRemainder = largerNum % smallerNum;
		largerNum = smallerNum;
		smallerNum = newRemainder;
	} while (newRemainder != 0);
#pragma endregion
	// the last non-zero remainder is the greatest common divisor, if this is 1 then they are coprime / relatively prime;
	return previousRemainder == 1;
}
#pragma endregion

#pragma region Encryption Decription
int Encryption::EncryptOrDecipherInt(int value, long long int key, long long int modulus)
{
	return ModularExponentiation(value, key, modulus);
}

Message Encryption::EncryptOrDecipherMessage(Message message, long long int key, long long int modulus)
{
	int encryptedSenderID = EncryptOrDecipherInt(message.GetID(), key, modulus);
	int encryptedMessagePurpose = EncryptOrDecipherInt((int)message.GetMessagePurpose(), key, modulus);
	int encryptedOpponentID = EncryptOrDecipherInt(message.GetOpponentID(), key, modulus);
	std::vector<int> encryptedParamaters;
	for (int i = 0; i < message.GetParameters().size(); i++)
	{
		encryptedParamaters.push_back(EncryptOrDecipherInt(message.GetParameters()[i], key, modulus));
	}
	return Message(encryptedSenderID, (MessagePurpose)encryptedMessagePurpose, encryptedOpponentID, encryptedParamaters);
}

#pragma region Public calls to Encryption and deciphering with stated key to maintain Encapsulation
int Encryption::EncryptOrDecipherIntWithPrivateKey(int intToEncryptOrDecipher)
{
	return EncryptOrDecipherInt(intToEncryptOrDecipher, privateKey, keyModulus);
}

int Encryption::EncryptOrDecipherIntWithPublicKey(int intToEncryptOrDecipher)
{
	return EncryptOrDecipherInt(intToEncryptOrDecipher, publicKey, keyModulus);
}

Message Encryption::EncryptOrDecipherMessageWithPrivateKey(Message messageToEncrypt)
{
	return EncryptOrDecipherMessage(messageToEncrypt, privateKey, keyModulus);
}

Message Encryption::EncryptOrDecipherMessageWithPublicKey(Message messageToEncrypt)
{
	return EncryptOrDecipherMessage(messageToEncrypt, publicKey, keyModulus);
}
#pragma endregion
#pragma endregion



Encryption::~Encryption() {}
