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

//izbacit nepotrebno
class BloomFilter
{
public:
	BloomFilter(int);
	vector<bool> filter;

	int getSize() {
		return velfiltera;
	}
	int set(int index) {
		filter[index] = 1;
	}
	int check(int index) {
		if (filter[index]) {
			return 1;
		}
		else {
			return 0;
		}
	}
	int getStats(int);
	int countTrue();
	int setFirstTime(int, int& , int& );
protected:
	int velfiltera;
};


//NOVO DODANO od ovde

class Times {
public:
	int getTimeFull() {
		return full;
	}
	int getTimeMin() {
		return min;
	}
	int getTimeMax(){
		return max;
	}
	int setTimeFull(int s_full) {
		full += s_full;
	}
	int setTimeMin(int s_min) {
		min = s_min;
	}
	int setTimeMax(int s_max) {
		max = s_max;
	}
protected:
	int full;
	int min;
	int max;

};

class operations {
public:
	operations(int);

	int insert(string data, BloomFilter filter) {
		int i, size = filter.getSize();

		if (!KMfunctions) {
			for (i = startNum; i < startNum + 2; i++) {

				filter.set(hashes(i, data) % size);
			
			}
		}
		else {
			for (i = 0; i < KMfunctions; i++) {

				filter.set((hashes(startNum, data) + i * hashes(++startNum, data) + i * i) % size);
			}
		}

	}
	int check(string data, BloomFilter filter) {
		int size = filter.getSize(), shouldBeConstant = 0;

		if (!KMfunctions) {

		
			if (filter.check(hashes(startNum, data) % size) && filter.check(hashes(startNum + 1, data) % size)) {
				return 1;
			}
			else {
				return 0;
			}
		
		}
		else {

			for (int i = 0; i < KMfunctions; i++) {
			
				if (!filter.check((hashes(startNum, data) + i * hashes(++startNum, data) + i * i) % size)) {
					shouldBeConstant = 1;
					break;
				}
			
			
			}
			if (!shouldBeConstant) {
				return 1;
			}
			else {
				return 0;
			}

		
		}
	
	
	}
protected:
	int startNum;
	int KMfunctions;
};


int hashes(int num, string data) {
	int result;
	if (num == 0) {
		//!! Ovde se mire vrimena, moze times bit klasa koje ce u operations klasi bit niz od dva, pa po indeksima se slat i ovde primat ka argument
		result  = MuRMuR3(data, 123);
	} 
	else if (num == 1) {
		result = FNV1a(data);
	}
	else if (num == 2) {
		result = sha1(data);
	}
	else if (num == 3) {
		result = md5(data);
	}

	return result;
}

operations::operations(int num) {
	if (num / 1000 == 5) {
		startNum = 0;
		KMfunctions = num % 1000;
	}
	else if(num/1000 == 7)
	{
		startNum = 2;
		KMfunctions = num % 1000;
	}

}

//DO OVDE


//!! statove dobivat getanjem vrimena iz klase opeations !!
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
int checkMinMax(int duration, Times time) {

	if (duration > time.getTimeMax()) {
		time.setTimeMax(duration);
	}
	else if (duration < time.getTimeMin()) {
		time.setTimeMin(duration);
	}

	return 0;
}


//Za postavljanje min i max vrijednosti na prvi element 
int BloomFilter::setFirstTime(int duration, int& hashMin, int& hashMax) {
	hashMin = duration;
	hashMax = duration;

	return 0;
}



//duration.count() vraca vrime u nanosekundama
int BloomFilter::insertPlainF(string data) {

	auto startMurmur3 = chrono::high_resolution_clock::now();
	filter[MuRMuR3(data, murmurSeed) % velfiltera] = 1;
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


	return 0;
}




BloomFilter::BloomFilter(int velicina)
{
	velfiltera = velicina;

	filter.resize(velfiltera, 0);
}




ostream& operator<<(ostream &out, BloomFilter &bloom)
{
	for (bool j : bloom.filter) out << " " << j;
	return out;
}

//Provjerava koliko filter daje false positiva za datoteku s rijecima koje nisu u filteru, triba pripravit
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

//Ovo dodaje el iz datoteke, triba pripravvit
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

//triba pripravit ovo s numon
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
//triba pripravit
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
//triba pripravit
int createNewFilter(const int bitNumber, const int WithKM, const int hashesChosen) {

	int data = 0, itemNum = 0;
	string file;
	BloomFilter filter(bitNumber);
	operations op(hashesChosen + WithKM);


	BloomFilter filterKm(bitNumber);
	cout << "Odaberite datoteku za dodati u filter: " << endl;
	file = chooseFile(itemNum);
	chooseDifferentHashes(hashesChosen, file, filterKm);

	filterKm.getStats(itemNum);
	compareSearchFileFilter(file, filterKm, hashesChosen);
	cout << endl;
	cout << "Odaberite datoteku za provjeru u filter: " << endl;
	checkForFalsePositive(chooseFile(itemNum), filterKm, hashesChoosen);

	BloomFilter filter(bitNumber);
	cout << "Odaberite datoteku za dodati u filter: " << endl;
	file = chooseFile(itemNum);
	cout << endl << endl;
	chooseDifferentHashes(hashesChosen, file, filter);
	filter.getStats(itemNum);
	compareSearchFileFilter(file, filter, hashesChosen);
	cout << endl << endl;
	cout << "Odaberite datoteku za provjeru u filter: " << endl;

	checkForFalsePositive(chooseFile(itemNum), filter, hashesChosen);




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

			if (hashesChosen == 1) {
				hashesChosen == 5000;
			}
			else if (hashesChosen == 2) {
				hashesChosen = 7000;
			}
			cout << endl<<endl;
			createNewFilter(bitNumber, numOfKM, hashesChosen );

			numOfKM = 0;

		}
	
	system("pause");
	return 0;

}
