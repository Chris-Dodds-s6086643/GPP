#pragma once
#include <array>
#include "Message.h"
// formed from a mixture of https://www.youtube.com/watch?v=oOcTVTpUsPQ, and its part 1 https://www.youtube.com/watch?v=4zahvcJ9glg
class Encryption
{
private:
    std::array<int, 6> primeNumbers;
    int primeNumberOne;
    int primeNumberTwo;
    long long int privateKey;
    long long int publicKey;
    long long int keyModulus;
    long long int phiKeyModulus;
public:
    Encryption();
private:
    void SetFixedPrimeNumbers();
    void GenerateRandomPrimeNumbers();
    void GeneratePublicKey();
    void GeneratePrivateKey();
    //private functions to avoid repeating code within but as they use the key functions no reason for them to be elsewhere.
    int EncryptOrDecipherInt(int value, long long int key);
    Message EncryptOrDecipherMessage(Message message, long long int key);
    // maybe the next two should be more generic and in utility or a mathsUtility static class but I'm not smart enough to know what else they could be used for
    long long int ModularExponentiation(long long int integer, long long int desiredPower); 
    bool IsRelativelyPrime(int numberToCheck, long long int numberToCheckAgainst);
public:
    int EncryptOrDecipherIntWithPrivateKey(int intToEncryptOrDecipher);
    int EncryptOrDecipherIntWithPublicKey(int intToEncryptOrDecipher);
    Message EncryptOrDecipherMessageWithPrivateKey(Message messageToEncrypt);
    Message EncryptOrDecipherMessageWithPublicKey(Message messageToEncrypt);
    ~Encryption();
};
