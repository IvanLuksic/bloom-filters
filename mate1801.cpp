#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include<chrono>
#include<list>
#include<sstream>
#include"md5.h"
#include"murmur3_fnv.h"
#include "sha1.h"
using namespace std;
// OVO SAN BACIA NA VRH JER SAN MORA DEKLARIRAT NA VRHU FUNKCIJU INT HASHES A ONA PRIMA TIMES PA JE UNDEFINED AKO NIJE POVIŠE NJE
class Times {
public:
	int getTimeFull() {
		return full;
	}
	int getTimeMin() {
		return min;
	}
	int getTimeMax() {
		return max;
	}
	int setTimeFull(int s_full) {
		full += s_full;
		return full;
	}
	int setTimeMin(int s_min) {
		min = s_min;
		return min;
	}
	int setTimeMax(int s_max) {
		max = s_max;
		return max;
	}
protected:
	int full;
	int min;
	int max;

};

int hashes(int, string, Times);

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

		//OVO SI DODA JER TI JE BACILO ERROR DA MORA VRATIT NEŠTO
		return 0;
	}
	int check(int index) {
		if (filter[index]) {
			return 1;
		}
		else {
			return 0;
		}
	}
	int countTrue();
	int setFirstTime(int, int&, int&);
protected:
	int velfiltera;
};


//NOVO DODANO od ovde



class operations {
public:
	operations(int);

	//OVDI SU SAD TIME[0] SPORE, A TIME[1] BRZE

	//OVO SAN DODA DA U GETSTATS MOGU VIDIT KOLKI JE START NUM DA ZNAN KOJI SU HASHEVI
	int getStartNum() {
		return startNum;
	}

	int getKMfunctions() {
		return KMfunctions;
	}

	Times getTimes() {
		return time[0];
	}

	Times getTimesTwo() {
		return time[1];
	}

	int insert(string data, BloomFilter filter) {
		int i, size = filter.getSize();

		if (!KMfunctions) {
			for (i = startNum; i < startNum + 2; i++) {

				filter.set(hashes(i, data, time[0]) % size);

			}
		}
		else {
			for (i = 0; i < KMfunctions; i++) {

				filter.set((hashes(startNum, data, time[1]) + i * hashes(++startNum, data, time[1]) + i * i) % size);
			}
		}
		//OVO SI DODA JER TI JE BACALO ERROR DA MORA VRAĆAT NEŠTO
		return 0;
	}
	int check(string data, BloomFilter filter) {
		int size = filter.getSize(), shouldBeConstant = 0;

		if (!KMfunctions) {


			if (filter.check(hashes(startNum, data, time[0]) % size) && filter.check(hashes(startNum + 1, data, time[0]) % size)) {
				return 1;
			}
			else {
				return 0;
			}

		}
		else {

			for (int i = 0; i < KMfunctions; i++) {

				if (!filter.check((hashes(startNum, data, time[1]) + i * hashes(++startNum, data, time[1]) + i * i) % size)) {
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
	//DODA SAN OVO ŠTA SI NAPISA U int hashes
	Times time[2];
};


int hashes(int num, string data, Times time) {
	int result;
	if (num == 0) {
		//!! Ovde se mire vrimena, moze times bit klasa koje ce u operations klasi bit niz od dva, pa po indeksima se slat i ovde primat ka argument
			// PROMINIA SAN DA SE SVAKOME MIRI VRIME, TO VRIME DODAJE U FULL TIME U OBJEKT KOJI JE POSLAN.
		auto startMurmur3 = chrono::high_resolution_clock::now();
		result = MuRMuR3(data, 123);
		auto endMurmur3 = chrono::high_resolution_clock::now();

		auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);

		time.setTimeFull(durationMM3.count());
	}
	else if (num == 1) {
		auto startMurmur3 = chrono::high_resolution_clock::now();
		result = FNV1a(data);
		auto endMurmur3 = chrono::high_resolution_clock::now();

		auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);

		time.setTimeFull(durationMM3.count());
	}
	else if (num == 2) {
		auto startMurmur3 = chrono::high_resolution_clock::now();
		result = sha1(data);
		auto endMurmur3 = chrono::high_resolution_clock::now();

		auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);

		time.setTimeFull(durationMM3.count());
	}
	else if (num == 3) {
		auto startMurmur3 = chrono::high_resolution_clock::now();
		result = md5(data);
		auto endMurmur3 = chrono::high_resolution_clock::now();

		auto durationMM3 = chrono::duration_cast<chrono::nanoseconds>(endMurmur3 - startMurmur3);

		time.setTimeFull(durationMM3.count());
	}

	return result;
}

operations::operations(int num) {
	if (num / 1000 == 5) {
		startNum = 0;
		KMfunctions = num % 1000;
	}
	else if (num / 1000 == 7)
	{
		startNum = 2;
		KMfunctions = num % 1000;
	}

}

//DO OVDE


//!! statove dobivat getanjem vrimena iz klase operations !!
//Racunanje prosjecnih i potpunih vremena dodavanja
int getStats(const int itemNum, operations op) {

	//SAMO NISAN SIGURAN ZA OVO SVEUKUPNO VRIJEME

	cout << endl << endl;
	if (op.getStartNum()==0) {

		cout << "Vrijeme izvrsavanja (u nanosekundama)- murmur3:" << endl << " 1) Prosjek: " << op.getTimes().getTimeFull() / itemNum << " 2) Najkrace izvrsavanje: " << op.getTimes().getTimeMin() << " 3) Najdulje izvrsavanje: " << op.getTimes().getTimeMax() << endl << endl;
		cout << "Vrijeme izvrsavanja (u nanosekundama)- fnv:" << endl << " 1) Prosjek: " << op.getTimesTwo().getTimeFull()/ itemNum << " 2) Najkrace izvrsavanje: " << op.getTimesTwo().getTimeMin() << " 3) Najdulje izvrsavanje: " << op.getTimesTwo().getTimeMax() << endl << endl;
		cout << "Broj dodanih elemenata: " << itemNum << " Sveukupno vrijeme izvrsavanja (u milisekundama): " << op.getTimes().getTimeFull() / 1000000 << endl << endl;
	}
	else if (op.getStartNum() == 2) {
		cout << "Vrijeme izvrsavanja (u nanosekundama) - md5:" << endl << " 1) Prosjek: " << op.getTimes().getTimeFull() / itemNum << " 2) Najkrace izvrsavanje: " << op.getTimes().getTimeMin() << " 3) Najdulje izvrsavanje: " << op.getTimes().getTimeMax() << endl << endl;
		cout << "Vrijeme izvrsavanja (u nanosekundama) - sha1:" << endl << " 1) Prosjek: " << op.getTimesTwo().getTimeFull() / itemNum << " 2) Najkrace izvrsavanje: " << op.getTimes().getTimeMin() << " 3) Najdulje izvrsavanje: " << op.getTimesTwo().getTimeMax() << endl << endl;
		cout << "Broj dodanih elemenata: " << itemNum << " Sveukupno vrijeme izvrsavanja (u milisekundama): " << op.getTimes().getTimeFull() / 1000000 << endl << endl;;
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

/*
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
*/



BloomFilter::BloomFilter(int velicina)
{
	velfiltera = velicina;

	filter.resize(velfiltera, 0);
}




ostream& operator<<(ostream& out, BloomFilter& bloom)
{
	for (bool j : bloom.filter) out << " " << j;
	return out;
}

//Provjerava koliko filter daje false positiva za datoteku s rijecima koje nisu u filteru, triba pripravit
int checkForFalsePositive(const string file, BloomFilter& filter, const int numOfHash, operations op) {

	ifstream checkFile(file);
	string line;
	int falsePositive = 0;

	if (!checkFile) {
		cout << "Kriv unos!" << endl;
	}


	// POZIVA OBJEKT OPERATIONS I TU CHEKA, ZATO VALJDA I SLUŽI TA FUNKCIJA U KLASI OPERATIONS
	while (checkFile >> line) if (op.check(line, filter) == 1) falsePositive++;


	cout << endl << "Filter vraca " << falsePositive << " netocnih" << endl;


	checkFile.clear();
	checkFile.seekg(0, ios::beg);

	return 0;
}

//Ovo dodaje el iz datoteke, triba pripravvit
int chooseDifferentHashes(const int hashChoice, const string file, BloomFilter& filter, operations op) {

	ifstream fileChoosen(file);
	string line;

	//NE SVAĆAN BAŠ OVAJ CONST INT HASH CHOICE
	if (hashChoice == 1) {

		//SAD VUČE ONU VARIJBLU KM FUNCTIONS IZ OPERATIONS DA VIDI JEL JE KM OPTIMIZACIJA IL NIJE
		if (!op.getKMfunctions()) {
			while (fileChoosen >> line)
			{
				op.insert(line, filter);
			}
		}
		else {
			while (fileChoosen >> line)
			{
				op.insert(line, filter);
			}

		}
	}
	else if (hashChoice == 2) {

		if (!!op.getKMfunctions()) {
			while (fileChoosen >> line)
			{
				op.insert(line, filter);
			}
		}
		else {
			while (fileChoosen >> line)
			{
				op.insert(line, filter);
			}

		}



	}

	fileChoosen.clear();
	fileChoosen.seekg(0, ios::beg);


	return 0;
}

//triba pripravit ovo s numon
//OVO NISAN DIRA
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
//PROMINIASAN SAMO DA NE IDE FILTER.CHECK NEGO OP.CHECK I U SVAKU FUNKCIJA DI TRIBA OBJEKT OPERATIONS
int compareSearchFileFilter(string file, BloomFilter filter, const int hashNum, operations op) {
	int responeFilter, responeFile = 0;
	string searchString, line;
	ifstream fileChoosen(file);

	cout << "Unesite rijec koju zelite traziti: ";
	cin >> searchString;

	auto startFilter = chrono::high_resolution_clock::now();
	//responeFilter = filter.check(searchString, hashNum);
	responeFilter = op.check(searchString, filter);
	auto endFilter = chrono::high_resolution_clock::now();

	auto startFile = chrono::high_resolution_clock::now();

	while (fileChoosen >> line) {


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
//ODI MI NEŽELI POKRENIT JER SU DVA BLOOM FILTERA DEKLARIRANA, OZNAČIA SAN IH I PROMINIA IME OVOM DRUGOM U FILTER2
int createNewFilter(const int bitNumber, const int WithKM, const int hashesChosen) {

	int data = 0, itemNum = 0;
	string file;
	BloomFilter filter(bitNumber);
	operations op(hashesChosen + WithKM);

	//PRVI
	BloomFilter filterKm(bitNumber);
	cout << "Odaberite datoteku za dodati u filter: " << endl;
	file = chooseFile(itemNum);
	chooseDifferentHashes(hashesChosen, file, filterKm, op);

	getStats(itemNum, op);
	compareSearchFileFilter(file, filterKm, hashesChosen, op);
	cout << endl;
	cout << "Odaberite datoteku za provjeru u filter: " << endl;
	checkForFalsePositive(chooseFile(itemNum), filterKm, hashesChosen, op);

	//DRUGI
	BloomFilter filter2(bitNumber);
	cout << "Odaberite datoteku za dodati u filter: " << endl;
	file = chooseFile(itemNum);
	cout << endl << endl;
	chooseDifferentHashes(hashesChosen, file, filter2, op);
	getStats(itemNum, op);
	compareSearchFileFilter(file, filter2, hashesChosen, op);
	cout << endl << endl;
	cout << "Odaberite datoteku za provjeru u filter: " << endl;

	checkForFalsePositive(chooseFile(itemNum), filter2, hashesChosen, op);




	return 0;
}

int main()
{

	int count, numOfKM = 0, hashesChosen, bitNumber;

	string includesKM;


	cout << "Koliko filtera zelite stvoriti: ";
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
		cout << endl << endl;
		createNewFilter(bitNumber, numOfKM, hashesChosen);

		numOfKM = 0;

	}

	system("pause");
	return 0;

}
