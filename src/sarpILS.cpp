#include "sarpILS.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>


void sarpILS::ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    
    iterILS = 1;
    maxIterILS = 1;
    
    double bestCost;
    double currentCost;
    sarpConstruction sCon(inst, nodeVec);

    sCon.ConstrProc(inst, nodeVec, Mdist, problem, &solution);

    currentCost = solution.getCost();
    bestCost = currentCost;
    bestSol = solution;

    int sol_size = solution->getRoutesSize();
    //Initial solution


    while (iterILS <= maxIterILS) {

        for(int rid = 0; rid < sol_size; rid++){

            RVNDInter(inst, nodeVec, Mdist, problem, );
            
            currentCost = solution.getCost();

            if (currentCost > bestCost) {
                bestSol = solution;
                bestCost = currentCost;
        
                //cout << endl << "\nBetter Solution Cost: " << solutionCost << endl << endl;
        
                // iterILS = 0;
            }
        }

	// 	perturbation(distMatrix, solutionBest, solution, solutionCost);
		iterILS++;
	}

	// cout << "\n-----x-----" << "\nBest Solution: ";
	// printSolution(solutionBest);
	// cout <<"\nSolution Best Cost: " << solutionBestCost << endl;



}

void sarpILS::RVNDInter(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int neighbor = 0;

	double bestCost = solution.getCost();
	double newCost = 0;
    double delta = 0;

	list<int> nbrList;
	list<int> usedNbr;


	list<int>::iterator it;

	for (int i = 0; i < 1; i++) {
		nbrList.push_back(i);
	}

	while (!nbrList.empty()) {
		
		// int neighbor = 1 + rand() % nbrList.size();
        int neighbor = 0;
		int counter = 1;

		// for (it = nbrList.begin(); it != nbrList.end(); it++) { 
		// 	if (counter == neighbor) {
		// 		neighbor = *it;
		// 		break;
		// 	}
		// 	counter++;
		// }

		switch (neighbor) {
			case 0:
				SwapAll (inst, nodeVec, Mdist, problem);
				break;

			// case 2:
			// 	relocateK (distM, sol, 1);
			// 	newCost = costCalc(distM, sol);
			// 	break;

			// case 3:
			// 	relocateK (distM, sol, 2);
			// 	newCost = costCalc(distM, sol);
			// 	break;

			// case 4:
			// 	relocateK (distM, sol, 3);
			// 	newCost = costCalc(distM, sol);
			// 	break;

			// case 5:
			// 	twoOpt (distM, sol);
			// 	newCost = costCalc(distM, sol);
			// 	break;
				
			default:
				cout << "Out of range" << endl;
				break;
		}
		
        newCost = solution.getCost();

        solution.printSol();
        solution.printCosts();
        getchar();
        
		// if (newCost < bestCost) {
		// 	bestCost = newCost;
		// 	if (nbrList.size() < 1) {
		// 		nbrList.merge(usedNbr);
		// 		usedNbr.clear();
		// 	}
		// }

		// else {
		// 	nbrList.remove(neighbor);
		// 	usedNbr.push_back(neighbor);
		// }
	}

	// newCost = costCalc(distM, sol);
	// solCost = newCost;
}

void sarpILS::SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta;
    int vehicle;
    vehicle = solution.getvehicle();

    sarpRoute sroute(inst, vehicle);

    for (int rid = 0; rid < solSize; rid++) {
        sroute = solution.getRoute(rid);
        delta += sroute.Swap(inst, nodeVec, Mdist, problem);
    }

    solution.addCost(delta);
}

