#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include<chrono>
#include<list>
#include<sstream>
#include"./Hash-functions/md5.h"
#include"murmur3_fnv.h"
#include "sha1.h"
using namespace std;

class BloomFilter
{
public:
	BloomFilter(int,int);
	BloomFilter(int);
	vector<bool> filter;
	int check(string ,int);
	//int insert(string);
	int insertKMF(string);
	int insertKMS(string);
	int insertPlainF(string);
	int insertPlainS(string);
	int getStats(int);
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
	int durationSlowerHashes;

	//Indeksi 0 - ukupno, 1 - minimum, 2 - max
	int murmurTimes[3];
	int fnvTimes[3];
	int md5Times[3];
	int sha1Times[3];
};

//Racunanje prosjecnih i potpunih vremena dodavanja
int BloomFilter::getStats(const int itemNum) {

	cout << endl << endl;
	if (murmurTimes[0] != 0) {
		
		cout << "Vrijeme izvrsavanja (u nanosekundama)- murmur3:" << endl << " 1) Prosjek: " << murmurTimes[0] / itemNum << " 2) Najkrace izvrsavanje: " << murmurTimes[1] << " 3) Najdulje izvrsavanje: " << murmurTimes[2]<<endl<<endl;
		cout << "Vrijeme izvrsavanja (u nanosekundama)- fnv:" << endl << " 1) Prosjek: " << fnvTimes[0] / itemNum << " 2) Najkrace izvrsavanje: " << fnvTimes[1] << " 3) Najdulje izvrsavanje: " << fnvTimes[2] << endl<<endl;
		cout << "Broj dodanih elemenata: " << itemNum << " Sveukupno vrijeme izvrsavanja (u milisekundama): " << durationFasterHashes/1000000 << endl << endl;
	}
	else if(md5Times[0] !=0) {
		cout << "Vrijeme izvrsavanja (u nanosekundama) - md5:" << endl << " 1) Prosjek: " << md5Times[0] / itemNum << " 2) Najkrace izvrsavanje: " << md5Times[1] << " 3) Najdulje izvrsavanje: " << md5Times[2] << endl << endl;
		cout << "Vrijeme izvrsavanja (u nanosekundama) - sha1:" << endl << " 1) Prosjek: " << sha1Times[0] / itemNum << " 2) Najkrace izvrsavanje: " << sha1Times[1] << " 3) Najdulje izvrsavanje: " << sha1Times[2] << endl << endl;
		cout << "Broj dodanih elemenata: " << itemNum << " Sveukupno vrijeme izvrsavanja (u milisekundama): " << durationSlowerHashes/1000000 << endl << endl;;
	}
	
	cout << endl << endl;

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
int BloomFilter::insertKMF(string data) {

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


int BloomFilter::insertKMS(string data) {

	auto startMd5 = chrono::high_resolution_clock::now();
	uint32_t md = md5(data);
	auto endMd5 = chrono::high_resolution_clock::now();



	auto startSha1 = chrono::high_resolution_clock::now();
	uint32_t sha = FNV1a(data);
	auto endSha1 = chrono::high_resolution_clock::now();


	for (int i = 0; i < numOfSimulatedHashFunctions; i++)
	{
		filter[(md + i * sha + i * i) % velfiltera] = 1;
	}

	auto endSha1Km = chrono::high_resolution_clock::now();

	auto durationMd5 = chrono::duration_cast<chrono::nanoseconds>(endMd5 - startMd5);
	auto durationSha1 = chrono::duration_cast<chrono::nanoseconds>(endSha1Km - startSha1);

	if (md5Times[0] == 0 && sha1Times[0] == 0) {

		setFirstTime(durationMd5.count(), md5Times[1], md5Times[2]);
		setFirstTime(durationSha1.count(), sha1Times[1], sha1Times[2]);
	}

	checkMinMax(durationMd5.count(), md5Times[1], md5Times[2]);
	checkMinMax(durationSha1.count(), sha1Times[1], sha1Times[2]);

	md5Times[0] += durationMd5.count();
	sha1Times[0] += durationSha1.count();

	durationSlowerHashes += durationMd5.count() + durationSha1.count();

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
	filter[md5(data) % velfiltera] = 1;
	auto endMd5 = chrono::high_resolution_clock::now();



	auto startSha1 = chrono::high_resolution_clock::now();
	filter[sha1(data) % velfiltera] = 1;
	auto endSha1 = chrono::high_resolution_clock::now();

	auto durationMd5 = chrono::duration_cast<chrono::nanoseconds>(endMd5 - startMd5);
	auto durationSha1 = chrono::duration_cast<chrono::nanoseconds>(endSha1 - startSha1);

	if (md5Times[0] == 0 && fnvTimes[0] == 0) {

		setFirstTime(durationMd5.count(), md5Times[1], md5Times[2]);
		setFirstTime(durationSha1.count(), sha1Times[1], sha1Times[2]);
	}

	checkMinMax(durationMd5.count(), md5Times[1], md5Times[2]);
	checkMinMax(durationSha1.count(), sha1Times[1], sha1Times[2]);

	md5Times[0] += durationMd5.count();
	sha1Times[0] += durationSha1.count();

	durationSlowerHashes += durationMd5.count() + durationSha1.count();



	return 0;
}

//Provjera postoji li element u filteru
int BloomFilter::check(string data, int hashes)
{
	if (WithKirschMitzenmacher == 0 && hashes ==1)
	{
		if (filter[FNV1a(data) % velfiltera] == 1 && filter[MuRMuR3(data, seed) % velfiltera] == 1) return 1;
		else return 0;
	}
	else if (WithKirschMitzenmacher == 0 && hashes == 2) {

		if (filter[sha1(data) % velfiltera] == 1 && filter[md5(data) % velfiltera] == 1) return 1;
		else return 0;

	}
	else if(WithKirschMitzenmacher == 1 && hashes == 1)//km optimizacija
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
	else if (WithKirschMitzenmacher == 1 && hashes == 2) {
		uint32_t mda = md5(data);
		uint32_t sha = sha1(data);
		int ind = 0;
		for (int i = 0; i < numOfSimulatedHashFunctions; i++)
		{
			if (filter[(mda + i * sha + i * i) % velfiltera] == 1) ind++;
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
int checkForFalsePositive(const string file , BloomFilter& filter, const int numOfHash) {

	ifstream checkFile(file);
	string line;
	int falsePositive = 0;

	if (!checkFile) {
		cout << "Kriv unos!" << endl;
	}



	while (checkFile >> line) if (filter.check(line, numOfHash) == 1) falsePositive++;

	
	cout << endl << "Filter vraca " << falsePositive << " netocnih" << endl;

	
	checkFile.clear();
	checkFile.seekg(0, ios::beg);

	return 0;
}


int chooseDifferentHashes(const int hashChoice, const string file, BloomFilter& filter){
	
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
				filter.insertKMF(line);
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
				filter.insertKMS(line);
			}

		}



	}

	fileChoosen.clear();
	fileChoosen.seekg(0, ios::beg);


	return 0;
}

string chooseFile(int& num) {

	int data;
	string file;
	cout << "1) 1k 2) 5k 3) 20k 4) 50K 5) 100K 6) 800K " << endl;
	cin >> data;

	switch (data)
	{
	case 1:
		file = "./data/1k.txt";
		num = 1000;
		break;
	case 2:
		file = "./data/5k.txt";
		num = 5000;
		break;
	case 3:
		file = "./data/20k.txt";
		num = 20000;
		break;
	case 4:
		file = "./data/50k.txt";
		num = 50000;
		break;
	case 5:
		file = "./data/100k.txt";
		num = 100000;
		break;
	case 6:
		file = "./data/800k.txt";
		num = 800000;
		break;
	default:
		cout << "Kriv unos!";
		break;
	}


	return file;

}

int compareSearchFileFilter(string file, BloomFilter filter, const int hashNum) {
	int responeFilter, responeFile = 0;
	string searchString,line;
	ifstream fileChoosen(file);

	cout << "Unesite rijec koju zelite traziti: ";
	cin >> searchString;

	auto startFilter= chrono::high_resolution_clock::now();
	responeFilter = filter.check(searchString,hashNum);
	auto endFilter = chrono::high_resolution_clock::now();

	auto startFile = chrono::high_resolution_clock::now();
	
	while(fileChoosen >> line) {


		if (line == searchString) {
			responeFile = 1;
			break;
		}

	}
	auto endFile = chrono::high_resolution_clock::now();

	auto durationFilter = chrono::duration_cast<chrono::nanoseconds>(endFilter - startFilter);
	auto durationFile = chrono::duration_cast<chrono::nanoseconds>(endFile - startFile);

	cout << "Za napraviti pretragu Bloom filteru je trebalo: " << durationFilter.count() << " dok je za traziti to u datoteci bilo potrebno: " << durationFile.count() << endl << endl;

	return 0;
}

int createNewFilter(const int bitNumber, const int WithKM, const int hashesChosen) {
	
	int data = 0, itemNum =0;
	string file;
	
	if (WithKM) {

		BloomFilter filterKm(bitNumber, WithKM);
		cout << "Odaberite datoteku za dodati u filter: " << endl;
		file = chooseFile(itemNum);
		chooseDifferentHashes(hashesChosen, file, filterKm);

		filterKm.getStats(itemNum);
		compareSearchFileFilter(file, filterKm, hashesChosen);
		cout << endl;
		cout << "Odaberite datoteku za provjeru u filter: " << endl;
		checkForFalsePositive(chooseFile(itemNum), filterKm,hashesChosen);


	}
	else
	{
		BloomFilter filter(bitNumber);
		cout << "Odaberite datoteku za dodati u filter: " << endl;
		file = chooseFile(itemNum);
		cout << endl << endl;
		chooseDifferentHashes(hashesChosen, file, filter);
		filter.getStats(itemNum);
		compareSearchFileFilter(file, filter, hashesChosen);
		cout << endl<<endl;
		cout << "Odaberite datoteku za provjeru u filter: " << endl;

		checkForFalsePositive(chooseFile(itemNum), filter,hashesChosen);

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

			cout << "Koliko bitova treba Bloomov filter imati: ";
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
			cout << endl<<endl;
			createNewFilter(bitNumber, numOfKM, hashesChosen );

			numOfKM = 0;

		}
	
	system("pause");
	return 0;

}
