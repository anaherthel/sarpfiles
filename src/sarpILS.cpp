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
    sroute.erase(inst, Mdist, 7, profit);
    sroute.updateAll(inst, nodeVec, Mdist);
    sroute.insert(inst, Mdist, 1, 5, profit);
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

        RVNDIntra(inst, nodeVec, Mdist, problem);

        // RVNDInter(inst, nodeVec, Mdist, problem);

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

	cout <<"\nSolution Best Cost: " << endl;
    
    bestSol.printCosts();
    cout << "\n-----x-----" << endl;
	// cout << "\n-----x-----" << "\nBest Solution: ";
	// printSolution(solutionBest);
	// cout <<"\nSolution Best Cost: " << solutionBestCost << endl;
}

void sarpILS::RVNDIntra(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int neighbor = 0;

	double bestCost = solution.getCost();
	double newCost = 0;
    double delta = 0;

	list<int> nbrList;
	list<int> usedNbr;


	list<int>::iterator it;

	for (int i = 1; i < 2; i++) {
		nbrList.push_back(i);
	}

	while (!nbrList.empty()) {
		
		// int neighbor = 1 + rand() % nbrList.size();
        int neighbor = 1;
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

			case 1:
				RelocateAll (inst, nodeVec, Mdist, problem);            
				break;
				
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution.getCost();

        cout << "\nAfter Inter RVND: " << endl;
        solution.printSol(inst);
        solution.printCosts();
        getchar();
        
		if (newCost > bestCost) {
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

void sarpILS::RelocateAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta;
    // int vehicle;
    // vehicle = solution.getvehicle();

    sarpRoute sroute(inst, 0);

    for (int rid = 0; rid < solSize; rid++) {
        delta = 0;
        sroute = solution.getRoute(rid);
        delta = sroute.relocateK(inst, Mdist, nodeVec, problem, 1);
       
        if (delta > 0){
            // cout << "delta: " << delta << endl;
            // cout << "Route with relocate K: " << endl;
            // for (int a = 0; a < sroute.getNodesSize(); a++){
            //     cout << sroute.getReq(a) << " - ";
            // }
            // cout << endl;

            // cout << "New route cost after 1 relocateK: " << sroute.cost() << endl;
            // getchar();

            sroute.updateAll(inst, nodeVec, Mdist);
            solution.updateRoutes(&sroute, rid);
        }
        // cout << "In SWAP ALL: There was an improvement: " << 
        // "route: " << rid << " - " << delta << endl;

    }

    solution.updateCost();

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

    // cout << "Neighborhood list (before): " << endl;
    // for (it = nbrList.begin(); it != nbrList.end(); it++) { 
    //     cout << *it;
    // }
    // getchar();

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
				relocate (inst, nodeVec, Mdist, problem);
				break;
				
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution.getCost();

        // cout << "\nAfter Relocate: " << endl;
        // solution.printSol(inst);
        // solution.printCosts();
        // getchar();
        
		if (newCost > bestCost) {
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

        // cout << "Neighborhood list (after): " << endl;
        // for (it = nbrList.begin(); it != nbrList.end(); it++) { 
        //     cout << *it;
        // }
        // getchar();
	}
}

void sarpILS::relocate(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution.getRoutesSize();

    double delta, bestDelta;
    bestDelta = 0;
    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution.getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    int bestCand, currCand;

    pair <int, int> bestPairPos, currPairPos, bestRoutePair; //position and cost
    
    bool improve;
    improve = 0;
    int candidate;

    for (int rid1 = 0; rid1 < solSize; rid1++){
        for (int rid2 = 0; rid2 < solSize; rid2++){
            delta = 0;
            if (rid1 != rid2){
                //this is the delta in solution value, so it is just a matter of cost.
                delta = solution.relocate(inst, nodeVec, Mdist, problem,
                                        rid1, rid2, currCand, currPairPos);                

                if (delta < 0){
                    if (delta < bestDelta){
                        // cout << "**********************" << endl;
                        // sroute1 = solution.getRoute(rid1);
                        // candidate = sroute1.getReq(currCand);
                        // cout << "candidate: " << candidate << endl;
                        // cout << "relocating candidates from route " << rid1  
                        // << " to route "  << rid2 << endl;
                        // cout << "New position: " << currPairPos.first << " - " << currPairPos.second << endl;
                        // cout << "Delta obtained: " << delta << endl;
                        // cout << "**********************" << endl;
                        bestDelta = delta;
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
        candidate = sroute1.getReq(bestCand);

        double profit = sroute1.getProfit(nodeVec, bestCand);

        // cout << "\nProfit: " << profit << endl;

        if(candidate < inst->n){ //passenger
            // cout << "\nRelocate candidate passenger: " << candidate << endl;
            // getchar();
            sroute2.insert(inst, Mdist, candidate, bestPairPos.first, profit);
            sroute1.erase(inst, Mdist, bestCand, profit);
        }
        else{

            // cout << "Relocate candidate parcel: " << candidate << endl;
            // getchar();
            int dlpos, dl;
            dlpos = sroute1.getDL(candidate-inst->n);
            dl = candidate + inst->m;

            profit = 0;

            sroute2.insert(inst, Mdist, dl, bestPairPos.second, profit);
            sroute1.erase(inst, Mdist, dlpos, profit);

            profit = sroute1.getProfit(nodeVec, bestCand);
            sroute2.insert(inst, Mdist, candidate, bestPairPos.first, profit);            
            sroute1.erase(inst, Mdist, bestCand, profit);
        }

        // cout << "First changed route: " << endl;
        // for (int a = 0; a < sroute1.getNodesSize(); a++){
        //     cout << sroute1.getReq(a) << " - ";
        // }
        // cout << endl;

        // cout << "Second changed route: " << endl;
        // for (int a = 0; a < sroute2.getNodesSize(); a++){
        //     cout << sroute2.getReq(a) << " - ";
        // }
        // cout << endl;

        // cout << "From route: " << bestRoutePair.first << " to " << bestRoutePair.second << endl;
        // cout << "With delta: " << bestDelta << endl;
        // getchar();
        sroute1.updateAll(inst, nodeVec, Mdist);
        sroute2.updateAll(inst, nodeVec, Mdist);
        
        solution.updateRoutes(&sroute1, bestRoutePair.first);
        solution.updateRoutes(&sroute2, bestRoutePair.second);
        solution.updateCost();
    }    
}

void sarpILS::Perturbation(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){

    //remove 20% of passengers
    //remove 30% of parcels


    int outcust = ceil(0.2*inst->n);
    int outparc = ceil(0.3*inst->m);

    vector <int> vecoutcust, vecoutparc;

    int req;

    int random_position = rand() % this->CLpass.size();
    int random_request  = this->CLpass[random_position];

    for (int i = 0; i < outcust; i++){
        req = rand() % inst->n;
        vecoutcust.push_back(req);
    }

    for (int i = 0; i < outcust; i++){
        req = rand() % inst->m;
        req += inst->n;
        vecoutcust.push_back(req);
    }

    //search and remove customers and parcels
    //add customers back in random locations (check feasibility but not cost)
    //do not add parcels.
    //call ILS again for optimizing what is in the routes. 
    //call add unserved at the end.

    
}