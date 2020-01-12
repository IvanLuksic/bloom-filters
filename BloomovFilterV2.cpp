#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include<chrono>
#include<list>
#include"./Hash-functions/md5.h"
#include"murmur3_fnv.h"
//#include "sha1.h"
using namespace std;

class BloomFilter
{
public:
	BloomFilter(int,int);
	BloomFilter(int);
	vector<bool> filter;
	int check(string);
	//int insert(string);
	int insertKM(string);
	int insertPlainF(string);
	int insertPlainS(string);
	int getStatsF();
	int countTrue();
	int checkMinMax(int, int&, int&);
	int setFirstTime(int, int& , int& );
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
	int md5Times[3];
};

//Racunanje prosjecnih i potpunih vremena dodavanja
int BloomFilter::getStatsF() {

	cout << "Murmur3 average time on 74000 items (in nanoseconds): " << murmurTimes[0] / 74400 << " maximum time: " << murmurTimes[2] << " minimum time: " << murmurTimes[1] << endl;

	cout << "Fnv average time on 1000 items (in nanoseconds): " << fnvTimes[0] / 74400 << " maximum time: " << fnvTimes[2] << " minimum time: " << fnvTimes[1] << endl;

	return 0;
}

//Funkcija koja provjera je li vrijeme izvrsavanja minimum ili maximum za odredeni hash
int BloomFilter::checkMinMax(int duration, int& hashMin, int& hashMax) {

	if (duration > hashMax) {
		hashMax = duration;
	}
	else if (duration < hashMin) {
		hashMin = duration;
	}

	return 0;
}


//Za postavljanje min i max vrijednosti na prvi element 
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

//Poziva se ako je km 1, km dodavanje 
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

//Poziva se ako je km 0 i odabrani brzi hashevi, obicno dodavanje 
int BloomFilter::insertPlainF(string data) {

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

	return 0;
}

//Poziva se ako je km 0 i odabrani spori hashevi, obicno dodavanje 
int BloomFilter::insertPlainS(string data) {

	auto startMd5 = chrono::high_resolution_clock::now();
	filter[MuRMuR3(data, seed) % velfiltera] = 1;
	auto endMd5 = chrono::high_resolution_clock::now();



	auto startFnv = chrono::high_resolution_clock::now();
	filter[FNV1a(data) % velfiltera] = 1;
	auto endFnv = chrono::high_resolution_clock::now();

	auto durationMd5 = chrono::duration_cast<chrono::nanoseconds>(endMd5 - startMd5);
	auto durationFNV = chrono::duration_cast<chrono::nanoseconds>(endFnv - startFnv);

	if (md5Times[0] == 0 && fnvTimes[0] == 0) {

		setFirstTime(durationMd5.count(), md5Times[1], md5Times[2]);
		setFirstTime(durationFNV.count(), fnvTimes[1], fnvTimes[2]);
	}

	checkMinMax(durationMd5.count(), md5Times[1], md5Times[2]);
	checkMinMax(durationFNV.count(), fnvTimes[1], fnvTimes[2]);

	md5Times[0] += durationMd5.count();
	fnvTimes[0] += durationFNV.count();

	durationFasterHashes += durationMd5.count() + durationFNV.count();



	return 0;
}

//Provjera postoji li element u filteru
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


BloomFilter::BloomFilter(int velicina,int kolko)//konstruktor za filter s KM optimiacijom
{
	velfiltera = velicina;
	seed = 123;
	WithKirschMitzenmacher = 1;
	numOfSimulatedHashFunctions = kolko;
	filter.resize(velfiltera, 0);
	murmurTimes[0] = 0;
	murmurTimes[1] = 0;
	murmurTimes[2] = 0;
}
BloomFilter::BloomFilter(int velicina)//konstruktor za filter bez KM optimizacije
{
	velfiltera = velicina;
	seed = 123;
	WithKirschMitzenmacher = 0;
	numOfSimulatedHashFunctions = 0;
	filter.resize(velfiltera, 0);
	murmurTimes[0] = 0;
	murmurTimes[1] = 0;
	murmurTimes[2] = 0;
}



ostream& operator<<(ostream &out, BloomFilter &bloom)
{
	for (bool j : bloom.filter) out << " " << j;
	return out;
}

//Provjerava koliko filter daje false positiva za datoteku s rijecima koje nisu u filteru
int checkForFalsePositive(string file , BloomFilter filter) {

	ifstream checkFile(file);
	string line;
	int falsePositive = 0;

	if (!checkFile) {
		cout << "Kriv unos!" << endl;
	}



	while (checkFile >> line) if (filter.check(line) == 1) falsePositive++;

	
	cout << endl << "Filter vraca " << falsePositive << " netocnih" << endl;

	
	checkFile.clear();
	checkFile.seekg(0, ios::beg);

	return 0;
}


int chooseDifferentHashes(const int hashChoice, const string file, BloomFilter filter){
	
	ifstream fileChoosen(file);
	string line;


	if (hashChoice == 1) {

		if (!filter.WithKirschMitzenmacher) {
			while (fileChoosen >> line)
			{
				
				filter.insertPlainF(line);
			}
		}
		else {
			while (fileChoosen >> line)
			{
				filter.insertKM(line);
			}

		}
	}
	else if (hashChoice == 2) {

		if (!filter.WithKirschMitzenmacher) {
			while (fileChoosen >> line)
			{
				filter.insertPlainS(line);
			}
		}
		else {
			while (fileChoosen >> line)
			{
				filter.insertKM(line);
			}

		}



	}

	fileChoosen.clear();
	fileChoosen.seekg(0, ios::beg);


	return 0;
}

string chooseFile() {

	int data;
	string file;
	cout << "Odaberite koju datoteku zelite doadati 1) 1k 2) 10k 3) 100k 4) 1M: " << endl;
	cin >> data;

	switch (data)
	{
	case 1:
		file = "./data/data-1k";
		break;
	case 2:
		file = "./data/data-10k";
		break;
	case 3:
		file = "./data/data-100k";
		break;
	case 4:
		file = "./data/data-1M";
		break;
	default:
		cout << "Kriv unos!";
		break;
	}

	//Za test 

	file = "./data/74402.txt";

	return file;

}


int createNewFilter(int bitNumber, int WithKM, int hashesChosen) {
	
	int data = 0;
	string file;
	
	if (WithKM) {

		BloomFilter filterKm(bitNumber, WithKM);
		file = chooseFile();
		chooseDifferentHashes(hashesChosen, file, filterKm);
		checkForFalsePositive("./data/10000.txt", filterKm);
		filterKm.getStatsF();


	}
	else
	{
		BloomFilter filter(bitNumber);
		file = chooseFile();
		chooseDifferentHashes(hashesChosen, file, filter);
		filter.getStatsF();
		checkForFalsePositive("./data/10000.txt", filter);

	}


	return 0;
}

int main()
{

	int count, numOfKM = 0 , hashesChosen, bitNumber;
	string includesKM;


		cout << "Koliko filtera zelite stvoriti: " ;
		cin >> count;
		cout << endl;

		for (int i = 0; i < count; i++) {

			cout << "Koliko bitovaa treba Bloomov filter imati: ";
			cin >> bitNumber;
			cout << endl;

			cout << "Zelite li da filter ima KM optimizaciju: ";
			cin >> includesKM;
			cout << endl;
			if (includesKM == "da" || includesKM == "Da") {
				
				cout << "Koliko zelite simulirati hash funkcija pomocu KM optimizacije: ";
				cin >> numOfKM;
				cout << endl;
			}
			cout << "Zelite li koristiti brze ili sporije hash funkcije, odaberite 1 za brze, 2 za sporije " << endl;
			cin >> hashesChosen;
			cout << endl;

			createNewFilter(bitNumber, numOfKM, hashesChosen );

			numOfKM = 0;

		}
	
	system("pause");
	return 0;

}
