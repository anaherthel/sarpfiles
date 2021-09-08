#include "sarpILS.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>


void sarpILS::ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    
    iterILS = 1;
    maxIterILS = 2;

    sarpRoute sroute(inst, 0);

    double bestCost;
    double currentCost;
    double profit;
    sarpConstruction sCon(inst, nodeVec);

    sCon.ConstrProc(inst, nodeVec, Mdist, problem, &solution);

    currentCost = solution.getCost();
    bestCost = currentCost;
    bestSol = solution;

    int sol_size;
    sol_size = solution.getRoutesSize();

    //Messing solution to test neighborhoods
    // sroute = solution.getRoute(0);
    
    // profit = sroute.getProfit(nodeVec, 1);
    // sroute.erase(inst, Mdist, 7, profit);
    // sroute.updateAll(inst, nodeVec, Mdist);
    // sroute.insert(inst, Mdist, 1, 5, profit);
    // sroute.updateAll(inst, nodeVec, Mdist);

    // sroute.calcCost(inst, nodeVec, Mdist);
        
    // solution.updateRoutes(&sroute, 0);
    // solution.updateCost();

    // cout << "Worse solution: " << endl;

    // solution.printSol(inst);
    // solution.printCosts();
    // getchar();
    // profit = sroute.getProfit(nodeVec, 1);
    // sroute.erase(inst, Mdist, 1, profit);
    // sroute.insert(inst, Mdist, 11, 2, profit);

    //Initial solution
    while (iterILS <= maxIterILS) {

        RVNDIntra(inst, nodeVec, Mdist, problem);

        RVNDInter(inst, nodeVec, Mdist, problem);

        currentCost = solution.getCost();

        if (currentCost > bestCost) {
            bestSol = solution;
            bestCost = currentCost;
    
            //cout << endl << "\nBetter Solution Cost: " << solutionCost << endl << endl;
    
            // iterILS = 0;
        }

        cout << "\n-----x-----" << "\nBest Solution so far: ";
        bestSol.printSol(inst);

        cout <<"\nSolution Best Cost: " << endl;
        
        bestSol.printCosts();
        cout << "\n-----x-----" << endl;

        // bestSol.addunserved(inst, nodeVec, Mdist, problem);
        solution = bestSol;
        Perturbation(inst, nodeVec, Mdist, problem);
        cout << "\n-----x-----" << "\nSolution with perturbation: ";
        solution.printSol(inst);

        cout <<"\nSolution perturbation Cost: " << endl;
        
        solution.printCosts();
        cout << "\n-----x-----" << endl;

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

	for (int i = 0; i < 2; i++) {
		nbrList.push_back(i);
	}

	while (!nbrList.empty()) {
		
		int neighbor = rand() % nbrList.size();
        // int neighbor = 1;
		int counter = 0;

		for (it = nbrList.begin(); it != nbrList.end(); it++) { 
			if (counter == neighbor) {
				neighbor = *it;
				break;
			}
			counter++;
		}

		switch (neighbor) {
			case 0:
                cout << "Calling swap: " << endl;
				SwapAll (inst, nodeVec, Mdist, problem);
				break;

			case 1:
                cout << "Calling relocate intra: " << endl;
				RelocateAll (inst, nodeVec, Mdist, problem);            
				break;
				
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution.getCost();

        cout << "\nAfter Intra: " << endl;
        solution.printSol(inst);
        solution.printCosts();
        getchar();
        
		if (newCost > bestCost) {
			bestCost = newCost;
			if (nbrList.size() < 2) {
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

	for (int i = 0; i < 2; i++) {
		nbrList.push_back(i);
	}

    // cout << "Neighborhood list (before): " << endl;
    // for (it = nbrList.begin(); it != nbrList.end(); it++) { 
    //     cout << *it;
    // }
    // getchar();

	while (!nbrList.empty()) {
		
		int neighbor = rand() % nbrList.size();
        // int neighbor = 0;
		int counter = 0;

		for (it = nbrList.begin(); it != nbrList.end(); it++) { 
			if (counter == neighbor) {
				neighbor = *it;
				break;
			}
			counter++;
		}

		switch (neighbor) {
			case 0:
                cout << "Calling relocate inter: " << endl;
				relocate (inst, nodeVec, Mdist, problem);
				break;

            case 1:
                cout << "Calling add unserved: " << endl;
				solution.addunserved(inst, nodeVec, Mdist, problem);
				break;	
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution.getCost();

        cout << "\nAfter Inter: " << endl;
        solution.printSol(inst);
        solution.printCosts();
        getchar();
        
		if (newCost > bestCost) {
			bestCost = newCost;
			if (nbrList.size() < 2) {
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

    sarpRoute sroute(inst, 0);
    int sizeRoute;

    double profit;
    int outcust = ceil(0.2*inst->n);
    int outparc = ceil(0.3*inst->m);

    vector <int> vecoutcust, vecoutparc;

    vector <int> avlPas; //available passengers to be removed from the solution
    vector <int> avlParc; //available parcels to be removed from the solution

    int solSize = solution.getRoutesSize();

    int req;

    for (int i = 0; i < inst->n; i++){
        avlPas.push_back(i);
    }

    for (int i = inst->n; i < inst->n+inst->m; i++){
        avlParc.push_back(i);
    }

    for (int i = 0; i < outcust; i++){
        req = rand() % avlPas.size();
        vecoutcust.push_back(avlPas[req]);
        avlPas.erase(avlPas.begin()+req);
    }

    cout << "Removing passengers: " << endl;

    for (int i = 0; i < vecoutcust.size(); i++){
        cout << vecoutcust[i] << " ";
    }
    cout << endl;
    getchar();

    for (int i = 0; i < outparc; i++){
        req = rand() % avlParc.size();
        vecoutparc.push_back(avlParc[req]);
        solution.addtounserved(avlParc[req]);
        avlParc.erase(avlParc.begin()+req);
    }

    cout << "\nRemoving parcels: " << endl;

    for (int i = 0; i < vecoutparc.size(); i++){
        cout << vecoutparc[i] << " ";
    }
    cout << endl;
    getchar();

    bool found;
    for (int a = 0; a < vecoutcust.size(); a++){
        found = 0;
        for (int rid = 0; rid < solSize; rid++){
            sroute = solution.getRoute(rid);
            sizeRoute = sroute.getNodesSize();

            for(int c = 0; c < sizeRoute; c++){
                int pass = sroute.getReq(c);

                if (vecoutcust[a] == pass){
                    profit = sroute.getProfit(nodeVec, c);
                    sroute.erase(inst, Mdist, c, profit);
                    found = 1;
                    break;
                }
            }
            if (found){
                sroute.updateAll(inst, nodeVec, Mdist);
                sroute.calcCost(inst, nodeVec, Mdist);
                solution.updateRoutes(&sroute, rid);
                solution.updateCost();
                break;
            }
        }
    }

    cout << "After removal: " << endl;

    solution.printSol(inst);
    solution.printCosts();
    getchar();
    // adding back customers

    int pos1, pos2;
    for (int a = 0; a < vecoutparc.size(); a++){
        found = 0;
        for (int rid = 0; rid < solSize; rid++){
            sroute = solution.getRoute(rid);
            sizeRoute = sroute.getNodesSize();
            for(int c = 0; c < sizeRoute; c++){
                int parc = sroute.getReq(c);
                int parc2 = parc + inst->m;
                if (vecoutparc[a] == parc){
                    // cout << "Pickup: " << parc << endl;
                    // getchar();
                    pos1 = c;
                    pos2 = sroute.getDL(vecoutparc[a]-inst->n);

                    // cout << "Delivery: " << parc2 << " at " << pos2 << endl;
                    // getchar();
                    profit = 0;
                    sroute.erase(inst, Mdist, pos2, profit);
                    profit = sroute.getProfit(nodeVec, pos1);
                    sroute.erase(inst, Mdist, pos1, profit);
                    found = 1;
                    break;
                }

            }
            if (found){
                sroute.updateAll(inst, nodeVec, Mdist);
                sroute.calcCost(inst, nodeVec, Mdist);
                solution.updateRoutes(&sroute, rid);
                solution.updateCost();

                // cout << "After 1 removal before: " << endl;
                // solution.printSol(inst);
                // solution.printCosts();
                // getchar();

                break;
            }
        }
    }

    cout << "After removal: " << endl;

    solution.printSol(inst);
    solution.printCosts();
    getchar();
    // adding back customers

    int randRoute;
    bool inserted;
    bool emptyAdd;
    int randPos;
    int request;
    solSize = solution.getRoutesSize();

    for (int a = 0; a < vecoutcust.size(); a++){
        request = vecoutcust[a];
      
        // cout << "Request: " << request << endl;
        // getchar();
        inserted = 0;
        emptyAdd = 0;
        for (int rid = 0; rid < solSize; rid++){
            sroute = solution.getRoute(rid);
            if(sroute.getNodesSize() < 3){
                sroute.fInsertion(inst, nodeVec, Mdist, request);
                inserted = 1;
                randRoute = rid;
                break;
            }
            else if (sroute.getPassPosSize() <= 0){
                randPos = 1;
                inserted = 1;
                randRoute = rid;
                break;
            }
        }
        while(!inserted){
            randRoute = rand() % solSize;
            sroute = solution.getRoute(randRoute);
            sizeRoute = sroute.getNodesSize();

            randPos =  1 + rand() % (sizeRoute-2);

            cout << "\nTesting insertion of: " << request << " at route " << randRoute
            << " in position " << randPos << endl;
            getchar();
            inserted = sroute.testInsertion(inst, nodeVec, Mdist, randPos, request);
            // cout << "Inserted: " << inserted << endl;
            // getchar();
        }
        profit = nodeVec[request].profit;

        sroute.insert(inst, Mdist,request, randPos, profit);
        sroute.updateAll(inst, nodeVec, Mdist);
        sroute.calcCost(inst, nodeVec, Mdist);

        solution.updateRoutes(&sroute, randRoute);
        solution.updateCost();

        cout << "New addition: " << endl;

        solution.printSol(inst);
        solution.printCosts();
    }    
}