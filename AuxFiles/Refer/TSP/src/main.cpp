#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <limits>
#include <float.h>
#include "ReadData.h"
#include "Construction.h"
#include "Utils.h"
#include "LocalSearch.h"
#include "GA.h"
#include "hungarian.h"
#include "BranchAndBound.h"

#define BIGNUMBER 10000

using namespace std;

//ótimo de Teste1.tsp = 2378
//Teste3 = 29 - 0 4 3 1 2 0
//1 5 4 2 3 1
//Teste4: 30 - 1 6 3 7 2 4 5 1 - 0 5 2 6 1 3 4 0

int main (int argc, char *argv[])
{

	srand (time(0));

	int n;
	int iterILS = 0;
	int maxIterILS = 200;
	
	int iniPopSize = 100;
	int iterGA = 0;
	int maxIterGA = 50;
	int parent1 = 0;
	
	double **distMatrix;
	double newCost = 0;
	double solutionCost = 0;
	double solutionBestCost = 9999999;

	double solutionBetterCost = 0;

	vector<int> solution;
	vector<int> solution2;
	vector<int> solutionBest;

	vector<individual_t> population;
	vector<individual_t> newPopulation;

	ReadData(argc, argv, &n, &distMatrix);

//ILS

/*	initialSolutionCI(n, distMatrix, solution, solutionCost);
	solutionBest = solution;
	solutionBestCost = solutionCost;

	cout << "\nInitial Solution: ";
	printSolution(solutionBest);
	cout << "\nInitial Solution Cost: " << solutionBestCost << endl;


	while (iterILS <= maxIterILS) {

		RVND(distMatrix, solution, solutionCost);

		if (solutionCost < solutionBestCost) {
			solutionBest = solution;
			solutionBestCost = solutionCost;
	
			//cout << endl << "\nBetter Solution Cost: " << solutionCost << endl << endl;
	
			iterILS = 0;
		}

		perturbation(distMatrix, solutionBest, solution, solutionCost);
		iterILS++;
	}

	cout << "\n-----x-----" << "\nBest Solution: ";
	printSolution(solutionBest);
	cout <<"\nSolution Best Cost: " << solutionBestCost << endl;*/


//Genetic Algorithm

/*	cout << endl;

	for (int i = 0; i < iniPopSize; i++) {
		initialSolutionCI(n, distMatrix, solution, solutionCost);
		//cout << endl << "Solution " << i + 1 << ": ";
		//printSolution(solution);
		//cout << endl << "Solution Cost: " << solutionCost << endl;
		
		individual_t ind;
		ind.solution = solution;
		ind.cost = solutionCost;

		population.push_back(ind);
		solution.clear();
	}

	sort(population.begin(), population.end(), compareCosts);


	while (iterGA <= maxIterGA) {
		for (int k = 0; k < iniPopSize - 2; k++){
			CrossoverOX(distMatrix, population, n, parent1);
			Mutation(distMatrix, population, n);
			parent1 = parent1 + 2;
		}
		sort(population.begin(), population.end(), compareCosts);
	
		solutionCost = population[0].cost;

		if (solutionCost < solutionBestCost) {
			solutionBest = solution;
			solutionBestCost = solutionCost;
	
			iterGA = 0;
		}

		parent1 = 0;
		iterGA++;
		cout << "Best Solution Cost: " << solutionBestCost << endl << endl;
	}*/

//Alg. Húngaro

/*	//=========================================SOLVE PROBLEM ================================================================//    
	hungarian_problem_t p;
	int mode = HUNGARIAN_MODE_MINIMIZE_COST;
	
	hungarian_init(&p, costMatrix, tamanho, tamanho, mode);
	raiz->solucao = hungarian_solve(&p);
	getVectorSolution(tamanho, raiz->rota, p.assignment);
	verifCiclo(&tamanho, raiz->rota, vCiclo, raiz->arcos); 
	hungarian_free(&p);
	//=======================================================================================================================//*/

	vector<int> sCicle;



	//bbNode_t bbNode;
	
	bool cicle;
	//int c = 0;



	matrixInfinity(distMatrix, n);

	trMatrix(distMatrix, n);

	//BBDFS(sCicle, cicle, distMatrix, n);

	//hungarian_init(&hp, distMatrix, n, n, mode);
	
	BBBFS(sCicle, cicle, distMatrix, n);
	
/*	c = hungarian_solve(&hp);

	cout << "\nCusto da resolução: " << c << endl;
	
	getVectorSolution(n, &bbNode, &hp);
	
	hungarian_print_status(&hp);
*/
//	printMatrix(distMatrix, n);


	for ( int i = 0; i < n+1; i++ ) {
		delete[] distMatrix[i];
	}

	delete[] distMatrix;

return 0;
}

