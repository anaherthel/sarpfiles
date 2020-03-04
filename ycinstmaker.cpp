//create instance from yellow cab data

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>	
#include <iterator>
#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <iomanip>
#include <ctime>
#include <ilcplex/ilocplex.h>
#include <stdlib.h>
#include <iostream>
#include <locale.h>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>

using namespace std;

int main (){ 

	srand (time(0));

	// std::locale loc("");
	// std::locale::global (loc);

	// wcout.imbue(loc);

	ifstream infile("yellow_cab_data.txt");

	infile1.imbue(loc);

	if (infile1.is_open()){
		while (wordList.size() < lSize){
			infile >> wordSt.sWord;
			infile >> wordSt.wordFreq;
			wordList.push_back(wordSt);
			//counter++;
		}
		infile1.close();
	}
	else cout << "Unable to open file" << endl;



}
