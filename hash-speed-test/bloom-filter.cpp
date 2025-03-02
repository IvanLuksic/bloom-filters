##define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<fstream>
#include<ctime>
#include<chrono>
#include<list>
#include<sstream>
#include<thread>
#include"./Hash-functions/md5.h"
#include"./Hash-functions/murmur3_fnv.h"
#include "./Hash-functions/sha1.h"
#include "header.h"
using namespace std;

//------------Opisi funkcija se nalaze u header.h--------------

//Konstruktor klase times 
Times::Times()
{
	full = 0;
	min = 0;
	max = 0;
}

//Konstruktor Bloomovog filtera
BloomFilter::BloomFilter(const int numOfBits)
{
	velfiltera = numOfBits;
	filter.resize(velfiltera, 0);
}

//Konstruktor operations klase
operations::operations(bool fastOrSlow,const int num) {
	if (fastOrSlow)indexOfHash = 0;//koristit ce 0 i 1 -brze murmur i fnv
	else indexOfHash = 2;//koristit ce hasheve 2 i 3 tj spore md5 i sha1
	KMfunctions = num;
}

//Dodaje element u filter
int operations::insert(string data, BloomFilter& filter)
{
	int size = filter.getSize();
	if (!KMfunctions)
	{
		int j = 0;
		for (int i = indexOfHash; i < indexOfHash + 2; i++)
		{
			filter.set(hashes(i, data, timeInsert[j]) % size);
			j++;
		}
	}
	else {
		uint32_t h1, h2;
		h1 = hashes(indexOfHash, data, timeInsert[0]);
		h2 = hashes((indexOfHash + 1), data, timeInsert[1]);
		for (int i = 0; i < KMfunctions; i++) filter.set((h1 + i * h2 + i * i) % size);
	}

	return 0;
}

//Provjerava postoji li element u filteru
int operations::check(string data, BloomFilter& filter) {
	int size = filter.getSize(), counterCheck = 0;

	if (!KMfunctions)
	{
		if (filter.check(hashes(indexOfHash, data, timeCheck) % size) && filter.check(hashes((indexOfHash + 1), data, timeCheck) % size)) return 1;
		else return 0;
	}
	else
	{
		uint32_t h1 = hashes(indexOfHash, data, timeCheck);
		uint32_t h2 = hashes((indexOfHash + 1), data, timeCheck);
		for (int i = 0; i < KMfunctions; i++)
		{
			if (filter.check((h1 + i * h2 + i * i) % size)) counterCheck++;
		}
		if (counterCheck == KMfunctions) return 1;
		else return 0;
	}
}

int checkMinMax(const int duration, Times& time)
{
	if (duration > time.getTimeMax()) {
		time.setTimeMax(duration);
	}
	else if (duration < time.getTimeMin()) {
		time.setTimeMin(duration);
	}

	return 0;
}

int setFirstTime(const int duration, Times& t)
{
	t.setTimeMax(duration);
	t.setTimeMin(duration);

	return 0;
}

uint32_t hashes(const int num, string data, Times& time) {
	uint32_t result;
	chrono::high_resolution_clock::time_point start, end;

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

int getStats(const int itemNum, operations op) {

	cout << endl << endl;
	if (op.getStartNum() == 0) {
		cout << "\t\t\tMurmur3\t\tfnv" << endl;
	}
	else if (op.getStartNum() == 2) {

		cout << "\t\t\tMd5\t\tSha1" << endl;
	}

	cout << "\t\t\t____________\t_____________\t\t\t (Sva su vremena u nanosekundama)" << endl;
	cout << "Prosjek: \t\t" << op.getTimes(0).getTimeFull()/itemNum << "\t\t" << op.getTimes(1).getTimeFull()/itemNum << endl;
	cout << "Minimum: \t\t" << op.getTimes(0).getTimeMin() << "\t\t" << op.getTimes(1).getTimeMin()  << endl;
	cout << "Maksimum: \t\t" << op.getTimes(0).getTimeMax() << "\t\t" << op.getTimes(1).getTimeMax()  << endl;
	cout << endl;

	return 0;
}

int premadeFilter(const bool WithKM, const bool fastHashes)
{
	int bitNumber = 0, numOfFalsePositives = 0, KMfunctions;
	string line, varijanta;
	ifstream input("./data/100k.txt");
	ifstream checkFile("./data/50k.txt");

	if (fastHashes && WithKM)
	{
		bitNumber = 2000000;
		KMfunctions = 11;
		varijanta = ("Filter sa nekriptografskim (MurMur3 i Fnv1a) hashevima, uz KM optimizaciju s" + to_string(KMfunctions) + "simuliranih funkcija - 2 milijuna bitova");
	}
	else if (fastHashes && !WithKM)
	{
		varijanta = ("Filter sa nekriptografskim (MurMur3 i Fnv1a) hashevima, bez KM optimizacije - 10 milijuna bitova");
		bitNumber = 10000000;
		KMfunctions = 0;
	}
	else if (!fastHashes && WithKM)
	{
		bitNumber = 2000000;
		KMfunctions = 11;
		varijanta = ("Filter sa kriptografskim (SHA1 i md5) hashevima, uz KM optimizaciju s " + to_string(KMfunctions) + " simuliranih funkcija - 2 milijuna bitova");
	}
	else
	{
		varijanta = ("Filter sa kriptografskim (SHA1 i md5) hashevima, bez KM optimizacije - 10 milijuna bitova");
		bitNumber = 10000000;
		KMfunctions = 0;
	}

	BloomFilter filter(bitNumber);

	operations postavke(fastHashes, KMfunctions);

	//Unos podataka u Bloomov filter iz datoteke 100k.txt
	while (input >> line) postavke.insert(line, filter);
	input.clear();
	input.seekg(0, ios::beg);

	//Provjera postojanja riječi iz 50k.txt u 100k.txt - pokušaj izazivanja false positive-a 
	while (checkFile >> line){
		if (postavke.check(line, filter)) numOfFalsePositives++;
	}
	checkFile.clear();
	checkFile.seekg(0, ios::beg);

	//mjerenje vremena potrebnog za provjeru postojanja "GitGud" pomoću Bloomova filtera i prolaska kroz tekst
	bool gitgud = 0;
	chrono::high_resolution_clock::time_point start1,start2,end1, end2;

	start1 = chrono::high_resolution_clock::now();
	gitgud=postavke.check("GitGud", filter);
	end1 = chrono::high_resolution_clock::now();
	auto duration1 = chrono::duration_cast<chrono::nanoseconds>(end1 - start1);

	start2 = chrono::high_resolution_clock::now();
	while(input >> line) { if (line == "gitgud") break;}
	end2 = chrono::high_resolution_clock::now();
	auto duration2 = chrono::duration_cast<chrono::nanoseconds>(end2 - start2);

	//ispis mjerenja
	cout << "___________________________________________________________________________________________________________________________________"<<endl<<endl;
	cout << varijanta << ":";
	getStats(100000, postavke);
	cout << "Broj izazvanih false positivea na 50 tisuca pokusaja:\t" << numOfFalsePositives << endl ;
	cout << "Vrijeme provjere postojanja rijeci 'GitGud' u obradenom tekstu:" <<endl<<"  -Bloomov filter:  "<<duration1.count()<<" ns"<<endl<<"  -prolazak kroz .txt file:  "<< duration2.count() <<" ns"<< endl << endl<<endl;

	return 0;
}


int main()
{
	thread t[4];
	t[0] = thread(premadeFilter, 0, 1);
	t[1] = thread(premadeFilter, 1, 1);
	t[2] = thread(premadeFilter, 0, 0);
	t[3] = thread(premadeFilter, 1, 0);

	for (int i = 0; i < 4; i++)
	{
		t[i].join();
	}
	system("pause");
	return 0;

	system("pause");
	return 0;

}
