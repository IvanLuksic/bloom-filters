#define _CRT_SECURE_NO_WARNINGS

#include <iomanip>
#include <iostream>
#include <stdio.h>
#include<vector>
#include<cmath>
#include<string>
#include <fstream>
#include "murmur3_fnv.h"

using namespace std;

class BloomFilter {
private:
	vector<bool> filter;
	int velicina;
	int seed;
public:
	int getVelicina();
	BloomFilter(int);
	int ispis();
	int unesi(int, int);
	int Provjera(string);
	double falsePositive();
	double getCount();

};

BloomFilter::BloomFilter(int velicina) {
	this->velicina = velicina;
	filter.resize(velicina, 0);
}

int BloomFilter::getVelicina() {
	return velicina;
}

int BloomFilter::ispis() {
	
	for (int i = 0; i < filter.size(); i++) {
		cout << "[" << filter[i] << "] ";
	}
	cout << "\n";
	for (int i = 0; i < filter.size()-5; i++) {
		cout << " "<<i<<"  ";
	}
	for (int i = filter.size() - 5; i < filter.size(); i++) {
		cout << " " << i << " ";
	}
	cout << "\n";
	return 0;
}

int BloomFilter::unesi(int fnv, int murmur) {

	filter[fnv] = 1;
	filter[murmur] = 1;
	ispis();
	return 0;
}

int BloomFilter::Provjera(string provjera) {
	if (filter[FNV1a(provjera) % velicina] == 1 && filter[MuRMuR3(provjera, 2) % velicina] == 1)
		return 1;
	else
		return 0;
}

double BloomFilter::getCount() {
	double count = 0;
	for (int i = 0; i < filter.size(); i++) {
		if (filter[i] == 1)
			count++;
	}
	return count;
}

double BloomFilter::falsePositive() {
	double fp = double(100 * pow(1-(exp(-2*getCount()/15)), 2));
	return fp;
}

int ispisRijeci(vector<string> set) {
	for (int i = 0; i < set.size(); i++) {
		cout << set[i] << ", ";
	}
	return 0;
}

int main() 
{

	BloomFilter bf(15);

	vector<string> uneseneRijeci;

	int a = 0;
	int n;
	string rijec;

	cout << "Bloom Filter: "<< endl;
	bf.ispis();

	while (a == 0) 
	{
		cout << "\nUnesite broj za izvrsavanje one funkcije koju zelite.\n1. Unesi u Bloom.\n2. Provjeri nalazi li se element u Bloom Filteru.\n3. Ispis filtera." << endl;
		cin >> n;
		switch (n)
		{
		case 1:
			cout << "\nUnesite string: ";
			cin.ignore();
			getline(cin, rijec);
			uneseneRijeci.push_back(rijec);
			cout << "fnv: " << FNV1a(rijec)%bf.getVelicina() <<endl <<"murmur: " << MuRMuR3(rijec, 2)%bf.getVelicina() << endl;
			bf.unesi(FNV1a(rijec) % bf.getVelicina(), MuRMuR3(rijec, 2) % bf.getVelicina());
			cout << "\nFalse positive: " << setprecision(2) << fixed << bf.falsePositive() << "%"<<endl;

			/*Provjera jeli veæ unesena rijec ista koja u biti ne minja false positive.
			for (int i = 0; i < uneseneRijeci.size(); i++) {
				if (rijec == uneseneRijeci[i])
					break;
				else
					brojUnesenih++;
			}*/

			cout << "Vas set = [";
			ispisRijeci(uneseneRijeci);
			cout << "]" << endl;
			break;
		case 2:
			cout << "\nUnesite string: ";
			// NE ÈITA RAZMAK
			cin >> rijec;

			bf.ispis();
			cout << "fnv: " << FNV1a(rijec) % bf.getVelicina() << endl << "murmur: " << MuRMuR3(rijec, 2) % bf.getVelicina() << endl;
			if (bf.Provjera(rijec) == 1)
				cout << "Vjerojatno se nalazi u setu!";
			else
				cout << "Sigurno se ne nalazi u setu!";
			cout <<"\nFalse positive: " <<setprecision(2) << fixed << bf.falsePositive() << "%" << endl;

			break;
		case 3:
			bf.ispis();
			break;
		default:
			cout << "Kriv unos.";
			break;
		}
	}

}