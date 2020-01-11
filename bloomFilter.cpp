#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include "murmur3_fnv.h"
#include "md5.h"
//#include "sha1.h"
#include<ctime>
#include <chrono>
#include<vector>
#include<fstream>
#include<list>

using namespace std;

class BloomFilter
{
public:
	BloomFilter(int);
	vector<bool> filter;
	int check(string);
	int insertFasterHashes(string);
	int checkMinMax(int duration, int& hashMin, int& hashMax);
	int setFirstTime(int duration, int& hashMin, int& hashMax);
	int getStatsF();
	int countTrue();
	int setSeed(unsigned int);
protected:
	int velfiltera;
	unsigned int seed;//za murmur
	int durationFasterHashes;

	//Indeksi 0 - ukupno, 1 - minimum, 2 - max
	int murmurTimes[3];
	int fnvTimes[3];


};

int BloomFilter::getStatsF() {

	cout << "Murmur3 average time on 1000 items (in nanoseconds): " << murmurTimes[0]/1000 << " maximum time: " << murmurTimes[2] << " minimum time: " << murmurTimes[1] << endl;

	cout << "Fnv average time on 1000 items (in nanoseconds): " << fnvTimes[0] / 1000 << " maximum time: " << fnvTimes[2] << " minimum time: " << fnvTimes[1] << endl;

	cout << "Complete Bloom addition time on 1000 (in microseconds)" << durationFasterHashes/1000 << endl;
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


int BloomFilter::check(string data)
{
	if ((filter[MuRMuR3(data, seed) % velfiltera] == 1) && (filter[FNV1a(data) % velfiltera] == 1))
	{
		cout << "String '" << data << "' je mozda u setu." << endl;
		return 1;
	}
	else
	{
		cout << "String '" << data << "' sigurno nije u setu." << endl;
		return 0;
	}
}//vraca nulu ako nije, vraca 1 ako je

int BloomFilter::insertFasterHashes(string data)
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

	checkMinMax(durationMM3.count(), murmurTimes[1],murmurTimes[2]);
	checkMinMax(durationFNV.count(), fnvTimes[1], fnvTimes[2]);

	murmurTimes[0] += durationMM3.count();
	fnvTimes[0] += durationFNV.count();

	durationFasterHashes += durationMM3.count() + durationFNV.count();
	return 0;
}//Vraca nulu nakon sta doda element u set


BloomFilter::BloomFilter(int velicina)
{
	velfiltera = velicina;
	seed = 123;
	filter.resize(velfiltera, 0);
	fnvTimes[0] = 0;
	durationFasterHashes = 0;
	fnvTimes[1] = 0;
	murmurTimes[0] = 0;
	fnvTimes[2] = 0;

}

ostream& operator<<(ostream &out, BloomFilter &bloom)
{
	for (bool j : bloom.filter) out << " " << j;
	return out;
}


int main() {

	BloomFilter iko(88500);
	int brojac = 0;
	ifstream unos("data1000.txt");
	ifstream provjera("provjera1500.txt");
	list<string> redci;
	string redak;
	while (unos >> redak) iko.insertFasterHashes(redak);
	//cout << iko << endl;
	//while (provjera >> redak) if (iko.check(redak) == 1) brojac++;
	//cout << "Imamo " << iko.countTrue() << " jedinica!" << endl;
	//cout << "Datoteka 'data1000.txt' sadrzi " << brojac << " rijeci iz datoteke 'provjera1500.txt'." << endl;

	iko.getStatsF();

	system("PAUSE");
	return 0;
}




