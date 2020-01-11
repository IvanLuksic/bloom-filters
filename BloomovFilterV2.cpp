#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<list>
#include "md5.h"
#include "murmur3_fnv.h"
using namespace std;

class BloomFilter
{
public:
	BloomFilter(int,int);
	BloomFilter(int);
	vector<bool> filter;
	int check(string);
	int insert(string);
	int clear();
	int countTrue();
	int numOfSimulatedHashFunctions;
	int setSeed(unsigned int);
	bool WithKirschMitzenmacher;// 0-obicno 1-kirtsch mitschemacher
protected:
	int velfiltera;
	unsigned int seed;//za murmur
};

int BloomFilter::setSeed(unsigned int s)
{
	seed = s;
	return 0;
}
int BloomFilter::countTrue()
{
	int br = 0;
	for (bool j : filter) if (j == true) br++;
	return br;
}
int BloomFilter::clear()
{
	fill(filter.begin(), filter.end(), 0);
	return 0;
}
int BloomFilter::check(string data)
{
	if (WithKirschMitzenmacher == 0)
	{
		if (filter[FNV1a(data) % velfiltera] == 1 && filter[MuRMuR3(data, seed) % velfiltera] == 1) return 1;
		else return 0;
	}
	else//km optimizacija
	{
		uint32_t mur = MuRMuR3(data, seed);
		uint32_t fnv = FNV1a(data);
		int ind = 0;
		for (int i = 0; i < numOfSimulatedHashFunctions; i++)
		{
			if (filter[(mur + i*fnv + i*i) % velfiltera ] == 1) ind++;
		}
		if (ind == 3) return 1;
		else return 0;
	}

}//vraca nulu ako nije, vraca 1 ako je
int BloomFilter::insert(string data)
{
	if (WithKirschMitzenmacher == 0)
	{
		filter[FNV1a(data) % velfiltera] = 1;
		filter[MuRMuR3(data,seed) % velfiltera] = 1;
	}
	else//kirtz mitschenmacher - izmesarena imena
	{
		uint32_t mur = MuRMuR3(data, seed);
		uint32_t fnv = FNV1a(data);
		for (int i = 0; i < numOfSimulatedHashFunctions; i++)
		{
			filter[ (mur + i*fnv + i*i) % velfiltera ] = 1;
		}
		return 0;
	}

}//Vraca nulu nakon sta doda element u set
BloomFilter::BloomFilter(int velicina,int kolko)//konstruktor za filter s KM optimiacijom
{
	velfiltera = velicina;
	seed = 123;
	WithKirschMitzenmacher = 1;
	numOfSimulatedHashFunctions = kolko;
	filter.resize(velfiltera, 0);
}
BloomFilter::BloomFilter(int velicina)//konstruktor za filter bez KM optimizacije
{
	velfiltera = velicina;
	seed = 123;
	WithKirschMitzenmacher = 0;
	numOfSimulatedHashFunctions = 0;
	filter.resize(velfiltera, 0);
}

ostream& operator<<(ostream &out, BloomFilter &bloom)
{
	for (bool j : bloom.filter) out << " " << j;
	return out;
}

int main()
{
	BloomFilter km(1200000,18), bez(6700000);//prvi filter koristi 5 puta manje memorije ali koristi KM optimizaciju
	int fp1=0, fp2 = 0;
	ifstream unos("74402.txt"),provjera("10000.txt");
	list<string> redci;
	string redak;

	while (unos>>redak) km.insert(redak);
	unos.clear();
	unos.seekg(0, ios::beg);
	while (unos>>redak) bez.insert(redak);

	while (provjera >> redak) if (km.check(redak) == 1) fp1++;
	provjera.clear();
	provjera.seekg(0, ios::beg);
	while (provjera >> redak) if (bez.check(redak) == 1) fp2++;

	cout << "Bez optimizacije: 6.7M bitova False positives(na 10K nesadrzanih elemenata):"<< fp2 << endl;
	cout << "Kirsch.Mitzenmacher: 1.2M bitova False positives(na 10K nesadrzanih elemenata):" << fp1 << endl;
	system("pause");
	return 0;

}
