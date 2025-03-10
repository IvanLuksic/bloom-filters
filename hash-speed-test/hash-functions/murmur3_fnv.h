#pragma once
#include <iostream>
#include<string>
using namespace std;

uint32_t FNV1a(string data)
{
	//fnv posebno oraduje svaki bajt - oktet bitova - tocno jedn char
	const uint32_t offset = 2166136261;//fnv konstante
	const uint32_t fnvprime = 16777619;
	uint32_t hash = offset;

	for (int i = 0; i < data.length(); i++)
	{
		uint8_t valChar = data[i];
		hash ^= valChar;
		hash *= fnvprime;
	}
	return hash;
}
uint32_t MuRMuR3(string data, unsigned int seed)//murmur3 funkcionira sa ukljucenim saltiranjem salt=seed
{
	uint32_t hash = seed;
	const uint32_t c1 = 0xcc9e2d51;
	const uint32_t c2 = 0x1b873593;
	const uint32_t c3 = 0x85ebca6b;
	const uint32_t c4 = 0xc2b2ae35;
	const uint32_t n = 0xe6546b64;
	const  int r1 = 15;
	const  int r2 = 13;
	const int m = 5;
	int numOfBlocks = data.length() / 4;
	int numOfRemaining = data.length() % 4;

	for (int i = 0; i < numOfBlocks; i++)
	{
		uint32_t k = data[(4 * i) + 0] + (data[(4 * i) + 1] << 8) + (data[(4 * i) + 2] << 16) + (data[(4 * i) + 3] << 24);// K je odlomak od 4 bytea = abcd 97 +98<<8 + 99<<16 +100<<24 prakticki radi ovakav broj 100999897
		k *= c1;
		k = _rotl(k, r1);
		k *= c2;
		hash ^= k;
		hash = _rotl(hash, r2);
		hash = hash * m + n;
	}

	uint32_t k = 0;
	switch (numOfRemaining)
	{
	case 3: k ^= data[4 * numOfBlocks + 2] << 16;//manipulacija preostalim bitovima
	case 2: k ^= data[4 * numOfBlocks + 1] << 8;
	case 1: k ^= data[4 * numOfBlocks + 0];
		k *= c1;
		k = _rotl(k, r1);
		k *= c2;
		hash ^= k;
	}

	hash ^= data.length();
	//zavrsno premetanje-veca uniformnost
	hash ^= (hash >> 16);
	hash *= c3;
	hash ^= (hash >> 13);
	hash *= c4;
	hash ^= (hash >> 16);

	return hash;
}