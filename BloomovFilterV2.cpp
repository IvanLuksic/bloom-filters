#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include<chrono>
#include<list>
#include<sstream>
#include"hash-functions/md5.h"
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
		return 0;
	}
	int setTimeMin(int s_min) {
		min = s_min;
		return 0;
	}
	int setTimeMax(int s_max) {
		max = s_max;
		return 0;
	}
protected:
	int full;
	int min;
	int max;

};

int hashes(int, string, Times);
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
protected:
	int velfiltera;
};




class operations {
public:
	operations(int);

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
	Times time[2];
};

int setFirstTime(int duration, Times time) {
	time.setTimeMax(duration);
	time.setTimeMin(duration);

	return 0;
}

int checkMinMax(int duration, Times time);
int hashes(int num, string data, Times time) {
	int result;
	chrono::steady_clock::time_point start, end;

	if (num == 0) {

		start = chrono::high_resolution_clock::now();
		result = MuRMuR3(data, 123);
		end = chrono::high_resolution_clock::now();

	}
	else if (num == 1) {
		start = chrono::high_resolution_clock::now();
		result = FNV1a(data);
		end = chrono::high_resolution_clock::now();




	}
	else if (num == 2) {
		start = chrono::high_resolution_clock::now();
		result = sha1(data);
		end = chrono::high_resolution_clock::now();




	}
	else if (num == 3) {
		start = chrono::high_resolution_clock::now();
		result = md5(data);
		end = chrono::high_resolution_clock::now();

	}

	auto duration = chrono::duration_cast<chrono::nanoseconds>(end - start);
	if (time.getTimeFull() == 0) {
		setFirstTime(duration.count(), time);
	}
	checkMinMax(duration.count(), time);
	time.setTimeFull(duration.count());

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



//Racunanje prosjecnih i potpunih vremena dodavanja
int getStats(const int itemNum, operations op) {

	//SAMO NISAN SIGURAN ZA OVO SVEUKUPNO VRIJEME

	cout << endl << endl;
	if (op.getStartNum() == 0) {
		cout << "\t\tMurmur3\t\t\tfnv" << endl;
	}
	else if (op.getStartNum() == 2) {

		cout << "\t\tMd5\t\t\tSha1" << endl;
	}

	cout << "Ukupno: " << op.getTimes().getTimeFull() << "\t\t" << op.getTimesTwo().getTimeFull() << endl;
	cout << "Ukupno: " << op.getTimes().getTimeMin() << "\t\t" << op.getTimesTwo().getTimeMin() << endl;
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
int checkForFalsePositive(const string file, BloomFilter filter,  operations op) {

	ifstream checkFile(file);
	string line;
	int falsePositive = 0;

	if (!checkFile) {
		cout << "Kriv unos!" << endl;
	}

	while (checkFile >> line) if (op.check(line, filter) == 1) falsePositive++;


	cout << endl << "Filter vraca " << falsePositive << " netocnih" << endl;


	checkFile.clear();
	checkFile.seekg(0, ios::beg);

	return 0;
}

//Ovo dodaje el iz datoteke, triba pripravvit
int readFileInsert(const string file, BloomFilter filter, operations op) {

	ifstream fileChoosen(file);
	string line;


			while (fileChoosen >> line)
			{
				op.insert(line, filter);
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


int compareSearchFileFilter(string file, BloomFilter filter, operations op) {
	int responeFilter, responeFile = 0;
	string searchString, line;
	ifstream fileChoosen(file);

	cout << "Unesite rijec koju zelite traziti: ";
	cin >> searchString;

	auto startFilter = chrono::high_resolution_clock::now();
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


int createNewFilter(const int bitNumber, const int WithKM, const int hashesChosen) {

	int data = 0, itemNum = 0;
	string file;
	BloomFilter filter(bitNumber);
	operations op(hashesChosen + WithKM);

	cout << "Odaberite datoteku za dodati u filter: " << endl;
	file = chooseFile(itemNum);
	readFileInsert(file, filter, op);

	getStats(itemNum, op);
	compareSearchFileFilter(file, filter, op);
	cout << endl;
	cout << "Odaberite datoteku za provjeru u filter: " << endl;
	checkForFalsePositive(chooseFile(itemNum), filter, op);

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
