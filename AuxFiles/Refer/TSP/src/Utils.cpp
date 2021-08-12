#include <cstdlib>
#include "GA.h"
#include "Utils.h"
#include <stdio.h>

using namespace std;

void printSolution (vector<int> &sol)
{
	ostream_iterator<int> output( cout, " " );
	copy(sol.begin(), sol.end(), output);
	cout << endl;
}

void printList (list<int> &lst)
{
	ostream_iterator<int> output( cout, " " );
	copy(lst.begin(), lst.end(), output);
	cout << endl;
}

double costCalc (double **distM, vector<int> &sol)
{
	double totalCost = 0;
	double c = 0;

	for (int i = 0; i < sol.size() - 1; i++) {
			c = distM[sol[i]][sol[i + 1]];
			totalCost = totalCost + c;
		}

	return totalCost;
}

void generateRandomSolution (vector<int> &sol, int maxSize, list<int> &candList, list<int>::iterator &itr, double **distM)
{
	while (sol.size() < maxSize) {
		int position = 2 + rand() % candList.size();
			
		for (itr = candList.begin(); itr != candList.end(); itr++) {
			
			if (position == *itr) {		
				sol.push_back(*itr);
				candList.erase(itr);
				break;				
			}
		}
	}
}

void matrixInfinity (double **distM, int size)
{
	for (int i = 1; i < size+1; i++) {
		for (int j = 1; j < size+1; j++) {
			if (i == j){
				distM[i][j] = 99999;
			}
		}
	}
}

void printMatrix (double **distM, int size)
{
	cout << "\nn = " << size << endl;
	cout << endl;

	for (int i = 1; i < size+1; i++) {
		for (int j = 1; j < size+1; j++) {
			cout << distM[i][j] << '\t';
		}
		cout << endl;
	}
}

void trMatrix (double **distM, int size)
{
	for (int i = 1; i < size + 1; i++) {
		for (int j = 1; j < size + 1; j++) {
			distM[i - 1][j - 1] = distM[i][j];
		}
	}
}
