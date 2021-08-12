#include <cstdlib>
#include "Construction.h"
#include "Utils.h"
#include "LocalSearch.h"
#include <stdio.h>


using namespace std;

void initialSolutionNN(int n, double **distMatrix, vector <int> &sol, double &solCost)
{
	double cost = 0;
	double bestCost = 0;

	int insertionPosition = 0;
	int costumer1 = 0;
	int costumer2 = 0;
	
	list<int> CL;
	list<int>::iterator it;
	list<int>::iterator it2;
	
	for ( int i = 2; i <= n; i++ ) {
		CL.push_back(i);
	}

	sol.push_back(1);
	generateRandomSolution(sol, 4, CL, it, distMatrix);
	sol.push_back(1);

	while (!CL.empty()) {
		bestCost = 9999999;

		for ( it = CL.begin(); it != CL.end(); it++ ) {
			costumer1 = sol[sol.size()-2];
			cost = distMatrix[costumer1][*it];
			
			if (cost < bestCost) {
				bestCost = cost;
				it2 = it;
			}
		}

		sol.insert(sol.end() - 1, *it2);
		CL.erase(it2);
	}

	solCost = costCalc(distMatrix, sol);
}

void initialSolutionCI(int n, double **distMatrix, vector<int> &sol, double &solCost)
{
	double cost = 0;
	double bestCost = 0;

	int insertionPosition = 0;
	int costumer1 = 0;
	int costumer2 = 0;

	//vector<int> sol;
	
	list<int> CL;
	list<int>::iterator it;
	list<int>::iterator it2;

	for ( int i = 2; i <= n; i++ ) {
		CL.push_back(i);
	}

	sol.push_back(1);
	generateRandomSolution(sol, 3, CL, it, distMatrix);
	sol.push_back(1);

	while (!CL.empty()) {
		bestCost = 9999999;

		for ( it = CL.begin(); it != CL.end(); it++ ) {
			for ( int i = 1; i < sol.size() - 1; i++ ) {
				costumer1 = sol[i];
				costumer2 = sol[i + 1];
				cost = distMatrix[costumer1][*it] + distMatrix[*it][costumer2] - distMatrix[costumer1][costumer2];
				if (cost < bestCost) {
					bestCost = cost;
					it2 = it;
					insertionPosition = i;
				}
			} 

		}

		sol.insert(sol.begin() + insertionPosition, *it2);
		CL.erase(it2);
	}

	solCost = costCalc(distMatrix, sol);
}

