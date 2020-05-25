#pragma once
#include <array>
#include "Message.h"
// formed from a mixture of https://www.youtube.com/watch?v=oOcTVTpUsPQ, and its part 1 https://www.youtube.com/watch?v=4zahvcJ9glg also
// testing working status alongside https://www.cs.drexel.edu/~jpopyack/Courses/CSP/Fa17/notes/10.1_Cryptography/RSAWorksheetv4e.html and https://www.wolframalpha.com/
class Encryption
{
private:
    std::array<int, 3> primeNumbers;
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
    void GenerateKeys();
    //void GeneratePublicKey();
    //void GeneratePrivateKey();
    // maybe the next two should be more generic and in utility or a mathsUtility static class but I'm not smart enough to know what else they could be used for
    long long int ModularExponentiation(long long int integer, long long int desiredPower);
    long long int ModularExponentiation(long long int integer, long long int desiredPower, long long int modulus);
    bool AreCoPrime(int numberToCheck, long long int numberToCheckAgainst);
public:
    int EncryptOrDecipherInt(int value, long long int key, long long int modulus);
    Message EncryptOrDecipherMessage(Message message, long long int key, long long int modulus);
    int EncryptOrDecipherIntWithPrivateKey(int intToEncryptOrDecipher);
    int EncryptOrDecipherIntWithPublicKey(int intToEncryptOrDecipher);
    Message EncryptOrDecipherMessageWithPrivateKey(Message messageToEncrypt);
    Message EncryptOrDecipherMessageWithPublicKey(Message messageToEncrypt);
    long long int GetPublicKey() { return publicKey; }
    long long int GetKeyModulus() { return keyModulus; }
    ~Encryption();
};
