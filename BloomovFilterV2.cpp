#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include <chrono>
#include<list>
#include "./Hash-functions/md5.h"
#include "murmur3_fnv.h"
//#include "sha1.h"
using namespace std;

class BloomFilter
{
public:
	BloomFilter(int,int);
	BloomFilter(int);
	vector<bool> filter;
	int check(string);
	int insert(string);
	int insertKM(string);
	int getStatsF();
	int countTrue();
	int checkMinMax(int duration, int& hashMin, int& hashMax);
	int setFirstTime(int duration, int& hashMin, int& hashMax);
	int numOfSimulatedHashFunctions;
	int setSeed(unsigned int);
	bool WithKirschMitzenmacher;// 0-obicno 1-kirtsch mitschemacher
protected:
	int velfiltera;
	unsigned int seed;//za murmur

	int durationFasterHashes;

	//Indeksi 0 - ukupno, 1 - minimum, 2 - max
	int murmurTimes[3];
	int fnvTimes[3];
};


int BloomFilter::getStatsF() {

	cout << "Murmur3 average time on 1000 items (in nanoseconds): " << murmurTimes[0] / 1000 << " maximum time: " << murmurTimes[2] << " minimum time: " << murmurTimes[1] << endl;

	cout << "Fnv average time on 1000 items (in nanoseconds): " << fnvTimes[0] / 1000 << " maximum time: " << fnvTimes[2] << " minimum time: " << fnvTimes[1] << endl;

	cout << "Complete Bloom addition time on 1000 (in microseconds)" << durationFasterHashes / 1000 << endl;
	cout << "Average Bloom addition time on 1000 (in nanoseconds)" << durationFasterHashes / 1000 << endl;

	return 0;
}

int BloomFilter::checkMinMax(int duration, int& hashMin, int& hashMax) {

	if (duration > hashMax) {
		hashMax = duration;
	}
	else if (duration < hashMin) {
		hashMin = duration;
	}

	return 0;
}

int BloomFilter::setFirstTime(int duration, int& hashMin, int& hashMax) {
	hashMin = duration;
	hashMax = duration;

	return 0;
}
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

int BloomFilter::insertKM(string data) {

	auto startMurmur3 = chrono::high_resolution_clock::now();
	uint32_t mur = MuRMuR3(data, seed);
	auto endMurmur3 = chrono::high_resolution_clock::now();



	auto startFnv = chrono::high_resolution_clock::now();
	uint32_t fnv = FNV1a(data);
	auto endFnv = chrono::high_resolution_clock::now();


	for (int i = 0; i < numOfSimulatedHashFunctions; i++)
	{
		filter[(mur + i * fnv + i * i) % velfiltera] = 1;
	}


	auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);
	auto durationFNV = chrono::duration_cast<chrono::nanoseconds>(endFnv - startFnv);


	if (murmurTimes[0] == 0 && fnvTimes[0] == 0) {

		setFirstTime(durationMM3.count(), murmurTimes[1], murmurTimes[2]);
		setFirstTime(durationFNV.count(), fnvTimes[1], fnvTimes[2]);
	}

	checkMinMax(durationMM3.count(), murmurTimes[1], murmurTimes[2]);
	checkMinMax(durationFNV.count(), fnvTimes[1], fnvTimes[2]);

	murmurTimes[0] += durationMM3.count();
	fnvTimes[0] += durationFNV.count();

	durationFasterHashes += durationMM3.count() + durationFNV.count();

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
		if (ind == numOfSimulatedHashFunctions) return 1;
		else return 0;


	}

}//vraca nulu ako nije, vraca 1 ako je
int BloomFilter::insert(string data)
{
	if (WithKirschMitzenmacher == 0)
	{
		auto startMurmur3 = chrono::high_resolution_clock::now();
		filter[MuRMuR3(data, seed) % velfiltera] = 1;
		auto endMurmur3 = chrono::high_resolution_clock::now();



		auto startFnv = chrono::high_resolution_clock::now();
		filter[FNV1a(data) % velfiltera] = 1;
		auto endFnv = chrono::high_resolution_clock::now();

		auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);
		auto durationFNV = chrono::duration_cast<chrono::nanoseconds>(endFnv - startFnv);

		if (murmurTimes[0] == 0 && fnvTimes[0] == 0) {

			setFirstTime(durationMM3.count(), murmurTimes[1], murmurTimes[2]);
			setFirstTime(durationFNV.count(), fnvTimes[1], fnvTimes[2]);
		}

		checkMinMax(durationMM3.count(), murmurTimes[1], murmurTimes[2]);
		checkMinMax(durationFNV.count(), fnvTimes[1], fnvTimes[2]);

		murmurTimes[0] += durationMM3.count();
		fnvTimes[0] += durationFNV.count();

		durationFasterHashes += durationMM3.count() + durationFNV.count();
	}
	else//kirtz mitschenmacher - izmesarena imena
	{
		insertKM(data);
	}

	return 0;
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
	BloomFilter km(12000000, 12), bez(6700000);//prvi filter koristi 5 puta manje memorije ali koristi KM optimizaciju
	int fp1=0, fp2 = 0;
	ifstream unos("./data/74402.txt"),provjera("./data/10000.txt");
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

	km.getStatsF();
	cout << "Bez optimizacije: 6.7M bitova False positives(na 10K nesadrzanih elemenata):"<< fp2 << endl;
	cout << "Kirsch.Mitzenmacher: 1.2M bitova False positives(na 10K nesadrzanih elemenata):" << fp1 << endl;
	system("pause");
	return 0;

}
