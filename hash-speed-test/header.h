#pragma once
//Deklaracije klasa

//Deklaracija klase koja sadrzi vremena i metode za postavljanje i vracanje vrijednosti
class Times
{
public:
	Times();
	int64_t getTimeFull() {
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
	int64_t full;
	int min;
	int max;
};

//Deklaracija Bloomovof filtera i njegovih get i set metoda
class BloomFilter
{
public:
	BloomFilter(int);
	vector<bool> filter;
	int getSize() { return velfiltera; }
	int set(int index)
	{
		if (index < velfiltera)
		{
			filter[index] = true;
			return 0;
		}
		else return -1;
	}
	int check(int index)
	{
		if (index < velfiltera)
		{
			if (filter[index]) return 1;
			else return 0;
		}
		else return -1;
	}

protected:

	int velfiltera;
};

//Deklaracija klase operations, koja za dani Bloomov filter sadrzi odabir hash funkcija i KM optimizaciju
class operations
{
public:
	operations(bool, int);//bool true za brze false za spore
	int insert(string, BloomFilter&);
	int check(string, BloomFilter&);
	int getStartNum()
	{
		return indexOfHash;
	}

	int getKMfunctions()
	{
		return KMfunctions;
	}

	Times getTimes(int index)
	{
		return timeInsert[index];
	}
protected:
	int indexOfHash;
	int KMfunctions;
	Times timeInsert[2], timeCheck;
};

//------------------Prototipi funkcija----------------------------

//Funkcija koja provjerava je li neko vrijeme minimum ili maksimum i postavlja ako je
int checkMinMax(const int, Times&);

//Fukncija koja postavlja vremena vremena prvi put kada se vremena mjere
int setFirstTime(const int, Times&);

//Mjeri vremena, odabire koja se hash funkcija koristi i vraca vrijednost hasha
uint32_t hashes(const int, string, Times&);

//Ispisuje statistiku
int getStats(const int, operations);

//Poziva vec napravljen, ali popriliicno velik filter za poboljsan user experience
int premadeFilter(const bool, const bool);