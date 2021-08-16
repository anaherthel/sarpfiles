#include "sarpILS.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>


void sarpILS::ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    
    iterILS = 1;
    maxIterILS = 1;

    sarpRoute sroute(inst, 0);

    double bestCost;
    double currentCost;
    sarpConstruction sCon(inst, nodeVec);

    sCon.ConstrProc(inst, nodeVec, Mdist, problem, &solution);

    currentCost = solution.getCost();
    bestCost = currentCost;
    bestSol = solution;

    int sol_size;
    sol_size = solution.getRoutesSize();

    //Messing solution to test neighborhoods
    sroute = solution.getRoute(0);
    double profit;

    profit = sroute.getProfit(nodeVec, 1);
    sroute.erase(inst, Mdist, 1, profit);
    sroute.insert(inst, Mdist, 11, 2, profit);

    sroute.updateAll(inst, nodeVec, Mdist);
        
    solution.updateRoutes(&sroute, 0);
    solution.updateCost();

    solution.printSol(inst);
    // profit = sroute.getProfit(nodeVec, 1);
    // sroute.erase(inst, Mdist, 1, profit);
    // sroute.insert(inst, Mdist, 11, 2, profit);

    //Initial solution
    while (iterILS <= maxIterILS) {


        RVNDInter(inst, nodeVec, Mdist, problem);
        
        currentCost = solution.getCost();

        if (currentCost > bestCost) {
            bestSol = solution;
            bestCost = currentCost;
    
            //cout << endl << "\nBetter Solution Cost: " << solutionCost << endl << endl;
    
            // iterILS = 0;
        }

	// 	perturbation(distMatrix, solutionBest, solution, solutionCost);
		iterILS++;
	}

    cout << "\n-----x-----" << "\nBest Solution: ";
    bestSol.printSol(inst);

	cout <<"\nSolution Best Cost: ";
    
    bestSol.printCosts();
    cout << "\n-----x-----" << endl;
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

    cout << "Neighborhood list (before): " << endl;
    for (it = nbrList.begin(); it != nbrList.end(); it++) { 
        cout << *it;
    }
    getchar();

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
                cout << "\nCalling Swap All" << endl;
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

        cout << "\nAfter Swap: " << endl;
        solution.printSol(inst);
        solution.printCosts();
        getchar();
        
		if (newCost < bestCost) {
			bestCost = newCost;
			if (nbrList.size() < 1) {
				nbrList.merge(usedNbr);
				usedNbr.clear();
			}
		}

		else {
			nbrList.remove(neighbor);
			usedNbr.push_back(neighbor);
		}

        cout << "Neighborhood list (after): " << endl;
        for (it = nbrList.begin(); it != nbrList.end(); it++) { 
            cout << *it;
        }
        getchar();
	}
    
    
}

void sarpILS::SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta;
    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution.getvehicle();

    sarpRoute sroute(inst, 0);

    for (int rid = 0; rid < solSize; rid++) {
        delta = 0;
        cout << "\nCalling SWAP for route " << rid << endl;
        sroute = solution.getRoute(rid);
        delta = sroute.Swap(inst, Mdist, nodeVec, problem);
        
        totaldelta += delta;
        if (delta < 0){
            solution.updateRoutes(&sroute, rid);
        }
        cout << "In SWAP ALL: There was an improvement: " << 
        "route: " << rid << " - " << delta << endl;

    }

    solution.updateCost();
}

