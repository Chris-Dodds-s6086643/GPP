#pragma once

struct KeyModulusPair
{
private:
	long long int key;
	long long int modulus;
public:
	KeyModulusPair(long long int inKey, long long int inModulus) : key(inKey), modulus(inModulus) {}

	long long int GetKey() { return key; }

	long long int GetModulus() { return modulus; }
};