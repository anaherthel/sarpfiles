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

    profit = sroute.getProfit(nodeVec, 3);
    sroute.erase(inst, Mdist, 3, profit);
    sroute.updateAll(inst, nodeVec, Mdist);
    sroute.insert(inst, Mdist, 3, 2, profit);
    sroute.updateAll(inst, nodeVec, Mdist);

    sroute.calcCost(inst, nodeVec, Mdist);
        
    solution.updateRoutes(&sroute, 0);
    solution.updateCost();

    cout << "Worse solution: " << endl;

    solution.printSol(inst);
    solution.printCosts();
    getchar();
    // profit = sroute.getProfit(nodeVec, 1);
    // sroute.erase(inst, Mdist, 1, profit);
    // sroute.insert(inst, Mdist, 11, 2, profit);

    //Initial solution
    while (iterILS <= maxIterILS) {


        RVNDInter(inst, nodeVec, Mdist, problem);

        RVNDIntra(inst, nodeVec, Mdist, problem);

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
	}
    
    
}

void sarpILS::SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta;
    // int vehicle;
    // vehicle = solution.getvehicle();

    sarpRoute sroute(inst, 0);

    for (int rid = 0; rid < solSize; rid++) {
        delta = 0;
        sroute = solution.getRoute(rid);
        delta = sroute.Swap(inst, Mdist, nodeVec, problem);

        
        if (delta > 0){

            // cout << "Route with swap: " << endl;
            // for (int a = 0; a < sroute.getNodesSize(); a++){
            //     cout << sroute.getReq(a) << " - ";
            // }
            // cout << endl;

            // cout << "New route cost after 1 swap: " << sroute.cost() << endl;
            // getchar();

            sroute.updateAll(inst, nodeVec, Mdist);
            solution.updateRoutes(&sroute, rid);
        }
        // cout << "In SWAP ALL: There was an improvement: " << 
        // "route: " << rid << " - " << delta << endl;

    }

    solution.updateCost();

}

void sarpILS::RVNDIntra(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
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
                cout << "\nCalling Relocate Intra" << endl;
				relocate (inst, nodeVec, Mdist, problem);
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

        cout << "\nAfter Relocate: " << endl;
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

void sarpILS::relocate(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta, bestDelta;
    bestDelta = 1000000;
    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution.getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    int bestCand, currCand;

    pair <int, int> bestPairPos, currPairPos, bestRoutePair; //position and cost
    
    bool improve;

    for (int rid1 = 0; rid1 < solSize; rid1++){
        improve = 0;
        for (int rid2 = 0; rid2 < solSize; rid2++){
            delta = 0;
            if (rid1 != rid2){
                //this is the delta in solution value, so it is just a matter of cost.
                delta = solution.relocate(inst, nodeVec, Mdist, problem,
                                        rid1, rid2, currCand, currPairPos);
                cout << "relocating candidates from route " << rid1  
                << " to route "  << rid2 << endl;
                cout << "Delta obtained: " << delta;
                if (delta < 0){
                    if (delta < bestDelta){
                        improve = 1;
                        bestCand = currCand;
                        bestRoutePair.first = rid1;
                        bestRoutePair.second = rid2;
                        bestPairPos = currPairPos;

                    }
                }
            }
        }
    }

    if (improve){
        sroute1 = solution.getRoute(bestRoutePair.first);
        sroute2 = solution.getRoute(bestRoutePair.second);
        double profit = sroute1.getProfit(nodeVec, bestCand);
        int candidate = sroute1.getReq(bestCand);
        

        if(bestPairPos.second < 0){ //passenger
            cout << "Relocate candidate passenger: " << candidate << endl;
            sroute2.insert(inst, Mdist, candidate, bestPairPos.first, profit);
            sroute1.erase(inst, Mdist, bestCand, profit);
        }
        else{
            int dlpos, dl;
            dlpos = sroute1.getDL(candidate-inst->n);
            dl = candidate + inst->m;
            sroute2.insert(inst, Mdist, candidate, bestPairPos.first, profit);
            sroute2.insert(inst, Mdist, dl, bestPairPos.second, profit);

            sroute1.erase(inst, Mdist, bestCand, profit);
            sroute1.erase(inst, Mdist, dlpos, profit);
        }
        sroute1.updateAll(inst, nodeVec, Mdist);
        sroute2.updateAll(inst, nodeVec, Mdist);
        
        solution.updateRoutes(&sroute1, bestRoutePair.first);
        solution.updateRoutes(&sroute2, bestRoutePair.second);
        solution.updateCost();
    }    
}

