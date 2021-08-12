#include <cstdlib>
#include "LocalSearch.h"
#include "Utils.h"
#include <stdio.h>

using namespace std;

void swap (double **distM, vector<int> &sol)
{
	double imp = 0;
	double preImp = 0;
	double bestImp = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;

	for (int i = 1; i < sol.size() - 2; i++) {
		preImp = - distM[sol[i - 1]][sol[i]];
		for (int j = i + 1; j < sol.size() - 1; j++) {
			if (j - i == 1) {
				imp = preImp - distM[sol[j]][sol[j + 1]] + distM[sol[i - 1]][sol[j]] + distM[sol[i]][sol[j + 1]];	
			}
			else {
				imp = preImp - distM[sol[i]][sol[i + 1]] - distM[sol[j]][sol[j - 1]] - distM[sol[j]][sol[j + 1]] + distM[sol[i - 1]][sol[j]] + distM[sol[j]][sol[i + 1]] + distM[sol[j - 1]][sol[i]] + distM[sol[i]][sol[j + 1]];
			}
			if (imp < 0) {
				if (imp < bestImp) {
					bestImp = imp;
					pos1 = i;
					pos2 = j;
				}
			}
		}
	}

	tempElement = sol[pos1];
	sol[pos1] = sol[pos2];
	sol[pos2] = tempElement;
}

void relocate1 (double **distM, vector<int> &sol)
{
	double imp = 0;
	double preImp = 0; 
	double bestImp = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;
	int element1 = 0;

	for (int i = 1; i < sol.size() - 1; i++) {
		preImp = - distM[sol[i - 1]][sol[i]] - distM[sol[i]][sol[i + 1]] + distM[sol[i - 1]][sol[i + 1]];
		for (int j = 1; j < sol.size() - 1; j++) {
			if (i != j) {
				if (i < j) {
					imp = preImp - distM[sol[j]][sol[j + 1]] + distM[sol[j]][sol[i]] + distM[sol[i]][sol[j + 1]];
				}
				else {
					imp = preImp - distM[sol[j - 1]][sol[j]] + distM[sol[j - 1]][sol[i]] + distM[sol[i]][sol[j]];
				}

				if (imp < 0) {
					if (imp < bestImp) {
						bestImp = imp;
						pos1 = i;
						pos2 = j;
						element1 = sol[i];
					}
				}
			}
		}
	}

	if (bestImp < 0) {
		sol.erase(sol.begin() + pos1);
		sol.insert(sol.begin() + pos2, element1);
	}
}

void relocate2 (double **distM, vector<int> &sol)
{
	double imp = 0;
	double preImp = 0; 
	double bestImp = -1;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;
	int pos3 = 0;

	vector<int> tempVector;

	while (bestImp < 0) {
		bestImp = 0;

		for (int i = 1; i < sol.size() - 2; i++) {
			preImp = - distM[sol[i - 1]][sol[i]] - distM[sol[i + 1]][sol[i + 2]] + distM[sol[i - 1]][sol[i + 2]];
			for (int j = 1; j < sol.size() - 1; j++) {
				if (i > j) {
					imp = preImp - distM[sol[j - 1]][sol[j]] + distM[sol[j - 1]][sol[i]] + distM[sol[i + 1]][sol[j]];
				}
				else if (i + 1 < j) {
					imp = preImp - distM[sol[j]][sol[j + 1]] + distM[sol[j]][sol[i]] + distM[sol[i + 1]][sol[j + 1]];
				}
				else {
					imp = 0;
				}
				if (imp < 0) {
					if (imp < bestImp) {
						bestImp = imp;
						pos1 = i;
						pos2 = i + 2;
						pos3 = j;
					}
				}
			}
		}


		if (bestImp < 0) {
			if (pos2 - 1 < pos3) {
				pos3 = pos3 - 1;
			}

			tempVector.insert(tempVector.begin(), sol.begin() + pos1, sol.begin() + pos2);
			sol.erase(sol.begin() + pos1, sol.begin() + pos2);
			sol.insert(sol.begin() + pos3, tempVector.begin(), tempVector.end());
		}

		tempVector.clear();
	}

/*	cout << endl << "Solution using Relocate2: ";	
	printSolution(sol);
	newCost = costCalc(distM, sol);
	cout << endl << "Solution cost using Relocate2: " << newCost << endl << "-----x-----" << endl;*/

}

void relocateK (double **distM, vector<int> &sol, int k)
{
	double imp = 0;
	double preImp = 0; 
	double bestImp = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;
	int pos3 = 0;
	int l = k - 1;

	vector<int> tempVector;
	
	for (int i = 1; i < sol.size() - k; i++) {
		preImp = - distM[sol[i - 1]][sol[i]] - distM[sol[i + l]][sol[i + k]] + distM[sol[i - 1]][sol[i + k]];
		for (int j = 1; j < sol.size() - 1; j++) {
			if (i + l < j) {
				imp = preImp - distM[sol[j]][sol[j + 1]] + distM[sol[j]][sol[i]] + distM[sol[i + l]][sol[j + 1]];
			}
			else if (i > j) {
				imp = preImp - distM[sol[j - 1]][sol[j]] + distM[sol[j - 1]][sol[i]] + distM[sol[i + l]][sol[j]];
			}

			else {
				imp = 0;
			}

			if (imp < 0) {
				if (imp < bestImp) {
					bestImp = imp;
					pos1 = i;
					pos2 = i + k;
					pos3 = j;
				}
			}
		}
	}


	if (pos2 - 1 < pos3) {
		pos3 = pos3 - l;
	}

	tempVector.insert(tempVector.begin(), sol.begin() + pos1, sol.begin() + pos2);
	sol.erase(sol.begin() + pos1, sol.begin() + pos2);
	sol.insert(sol.begin() + pos3, tempVector.begin(), tempVector.end());
	tempVector.clear();
}

void twoOpt (double **distM, vector<int> &sol)
{
	double imp = 0;
	double preImp = 0; 
	double bestImp = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;

	vector<int> tempVector;
						
	for (int i = 1; i < sol.size() - 3; i++) {
		preImp = - distM[sol[i - 1]][sol[i]];
		
		for (int j = i + 2; j < sol.size() - 1; j++) {
			imp = preImp - distM[sol[j]][sol[j + 1]] + distM[sol[i - 1]][sol[j]] + distM[sol[i]][sol[j + 1]];

			if (imp < 0) {
				if (imp < bestImp) {
					bestImp = imp;
					pos1 = i;
					pos2 = j + 1;
				}
			}
		}
	}

	tempVector.insert(tempVector.begin(), sol.begin() + pos1, sol.begin() + pos2);
	reverse(tempVector.begin(), tempVector.end());
	sol.erase(sol.begin() + pos1, sol.begin() + pos2);
	sol.insert(sol.begin() + pos1, tempVector.begin(), tempVector.end());
	tempVector.clear();
}

void RVND (double **distM, vector<int> &sol, double &solCost)
{
	int neighbor = 0;
	int pos = 0;

	double bestCost = solCost;
	double newCost = 0;

	list<int> nbrList;
	list<int> usedNbr;

	list<int>::iterator it;

	for ( int i = 1; i <= 5; i++ ) {
		nbrList.push_back(i);
	}
	while (!nbrList.empty()) {
		
		int neighbor = 1 + rand() % nbrList.size();
		int counter = 1;

		for (it = nbrList.begin(); it != nbrList.end(); it++) {
			if (counter == neighbor) {
				neighbor = *it;
				break;
			}
			counter++;
		}

		switch (neighbor) {
			case 1:
				swap (distM, sol);
				newCost = costCalc(distM, sol);
				break;

			case 2:
				relocateK (distM, sol, 1);
				newCost = costCalc(distM, sol);
				break;

			case 3:
				relocateK (distM, sol, 2);
				newCost = costCalc(distM, sol);
				break;

			case 4:
				relocateK (distM, sol, 3);
				newCost = costCalc(distM, sol);
				break;

			case 5:
				twoOpt (distM, sol);
				newCost = costCalc(distM, sol);
				break;
				
			default:
				cout << "Out of range" << endl;
				break;
		}
		
		if (newCost < bestCost) {
			bestCost = newCost;
			if (nbrList.size() < 5) {
				nbrList.merge(usedNbr);
				usedNbr.clear();
			}
		}

		else {
			nbrList.remove(neighbor);
			usedNbr.push_back(neighbor);
		}
	}

	newCost = costCalc(distM, sol);
	solCost = newCost;
}

void perturbation(double **distM, vector<int> &bestSol, vector<int> &sol, double &solCost)
{
	int fstPos1;
	int fstPos2;
	int limit;

	vector<int> v1;
	vector<int> v2;

	vector<int>::iterator it;

	sol.clear();

	copy(bestSol.begin(), bestSol.end(), back_inserter(sol));

/*	cout << endl << "Manipulated Solution: ";
	printSolution(sol);
	cout << endl;*/

	limit = floor(((sol.size() / 2) - 2));

	fstPos1 = 1 + rand() % limit;

	for (int i = fstPos1; i < (sol.size() / 2); i++) {
		v1.push_back(sol[i]);
	}

	fstPos2 = (sol.size() / 2) + 1 + rand() % limit;

	for (int j = fstPos2; j < sol.size() - 1; j++){
		v2.push_back(sol[j]);
	}
/*	cout << "v1: ";
	printSolution(v1);
	cout << endl;

	cout << "v2: ";
	printSolution(v2);
	cout << endl;*/
	
	sol.erase(sol.begin() + fstPos1, sol.begin() + (sol.size() / 2));
	sol.erase(sol.end() - v2.size() - 1, sol.end() - 1);
	sol.insert(sol.begin() + fstPos1, v2.begin(), v2.end());
	sol.insert(sol.end() - 1, v1.begin(), v1.end());

	v1.clear();
	v2.clear();

	solCost = costCalc(distM, sol);

}