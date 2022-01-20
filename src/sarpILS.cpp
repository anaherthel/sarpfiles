#include "sarpILS.h"
// #include "readdata.h"


#include <cstdlib>
#include <stdio.h>


void sarpILS::ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    
    // maxIterILS = 10*inst->n+inst->m;
    
    maxIterILS = 15;

    iterMS = 0;
    maxiterMS = 2;

    sarpRoute sroute(inst, 0);

    double globalBestCost;
    double bestCost;
    double currentCost;
    double profit;

    globalBestSol = new sarpSolution();
    globalBestCost = 0;

    double testcost = 0;
    double globaltestcost = 0;
    while (iterMS <= maxiterMS){
        iterILS = 0;
        cout << "Multistart count: " << iterMS << endl;
        sarpConstruction sCon(inst, nodeVec);
        solution = new sarpSolution();
        
        solution->printUnserved();
        sCon.ConstrProc(inst, nodeVec, Mdist, problem, solution);

        currentCost = solution->getCost();
        bestCost = currentCost;
        bestSol = new sarpSolution(*solution);
        // testcost = currentCost;

        int sol_size;
        sol_size = solution->getRoutesSize();

        while (iterILS <= maxIterILS) {
            cout << "***********************" << endl;
            cout << "ITERATION: " << iterILS << endl;
            cout << "***********************" << endl;

            
            RVNDIntra(inst, nodeVec, Mdist, problem);
    
            RVNDInter(inst, nodeVec, Mdist, problem);

            // RVNDAll(inst, nodeVec, Mdist, problem);
            currentCost = solution->getCost();
            int currentK = solution->getvehicle();


            if (currentCost > bestCost) {
                delete bestSol;
                bestSol = new sarpSolution(*solution);
                bestCost = currentCost;
        
                cout << endl << "\nBetter Solution Cost: " << currentCost << endl << endl;
        
                iterILS = 0;

                cout << "\n-----x-----" << "\nBest Solution so far: ";
                bestSol->printSol(inst);

                cout <<"\nSolution Best Cost: " << endl;
                
                bestSol->printCosts();
                cout << "\n-----x-----" << endl;
            }

            cout << "\n-----x-----" << "\nSolution before perturbation (ILS): ";
            solution->printSol(inst);        
            solution->printCosts();
            cout << "\n-----x-----" << endl;
            

            // bestSol.addunserved(inst, nodeVec, Mdist, problem);

            // solution = bestSol;
            solution->PurgeRoutes(inst, nodeVec, Mdist, problem);

            cout << "Calling Perturbation: " << endl;

            Perturbation(inst, nodeVec, Mdist, problem);

            cout << "\n-----x-----" << "\nSolution with perturbation: ";
            solution->printSol(inst);        
            solution->printCosts();
            cout << "\n-----x-----" << endl;

            iterILS++;
        }

        if (bestCost > globalBestCost){
            delete globalBestSol;
            globalBestSol = new sarpSolution(*bestSol);
            globalBestCost = bestCost;
            // getchar();
        }


        delete solution;
        delete bestSol;

        iterMS++;

    }

    cout << "\n-----x-----" << "\nBest Global Solution: ";
    globalBestSol->printSol(inst);

    cout <<"\nSolution Best Global Cost: " << endl;
    
    globalBestSol->printCosts();
    cout << "\n-----x-----" << endl;
    
    delete globalBestSol;
    
}

void sarpILS::RVNDIntra(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int neighbor = 0;

	double bestCost = solution->getCost();
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
                cout << "\nAfter Swap Intra: " << endl;
                solution->printSol(inst);
                solution->printCosts();
				break;

			case 1:
                cout << "Calling relocate intra: " << endl;
				RelocateAll (inst, nodeVec, Mdist, problem);
                cout << "\nAfter Relocate Intra: " << endl;
                solution->printSol(inst);
                solution->printCosts();                      
				break;

            // case 2:
			// 	cout << "Calling 3-opt: " << endl;
            //     ThreeOptAll(inst, nodeVec, Mdist, problem);

			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution->getCost();
        
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

    cout << "\nAfter All Intra: " << endl;
    solution->printSol(inst);
    solution->printCosts();
    // // getchar();
}

// void sarpILS::RVNDAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
//     int neighbor = 0;

// 	double bestCost = solution->getCost();
// 	double newCost = 0;
//     double delta = 0;
	
// 	list<int> nbrList;
// 	list<int> usedNbr;


// 	list<int>::iterator it;

// 	for (int i = 0; i < 4; i++) {
// 		nbrList.push_back(i);
// 	}

// 	while (!nbrList.empty()) {
		
// 		int neighbor = rand() % nbrList.size();
//         // int neighbor = 1;
// 		int counter = 0;

// 		for (it = nbrList.begin(); it != nbrList.end(); it++) { 
// 			if (counter == neighbor) {
// 				neighbor = *it;
// 				break;
// 			}
// 			counter++;
// 		}

// 		switch (neighbor) {
// 			case 0:
//                 cout << "Calling swap: " << endl;
// 				SwapAll (inst, nodeVec, Mdist, problem);

// 				break;

// 			case 1:
//                 cout << "Calling relocate intra: " << endl;
// 				RelocateAll (inst, nodeVec, Mdist, problem);
                      
// 				break;
// 			case 2:
//                 cout << "Calling relocate inter: " << endl;
// 				relocate (inst, nodeVec, Mdist, problem);
//                 // cout << "\n++++++++After RelocateInter:++++++++++ " << endl;
//                 // solution->printSol(inst);
//                 // solution->printCosts();
//                 // cout << "+++++++++++++++" << endl;
// 				break;
//             case 3:
//                 // cout << "Calling add unserved: " << endl;
//                 if (solution->unservedsize() < 1){
//                     break;
//                 }
//                 else{
//                     // solution->addunserved(inst, nodeVec, Mdist, problem);
//                 }
// 				break;
// 			default:
// 				cout << "Out of range" << endl;
// 				break;
// 		}

// 		newCost = solution->getCost();

//         cout << "\nAfter Intra: " << endl;
//         solution->printSol(inst);
//         solution->printCosts();
//         // // getchar();
        
// 		if (newCost > bestCost) {
// 			bestCost = newCost;
// 			if (nbrList.size() < 2) {
// 				nbrList.merge(usedNbr);
// 				usedNbr.clear();
// 			}
// 		}

// 		else {
// 			nbrList.remove(neighbor);
// 			usedNbr.push_back(neighbor);
// 		}
// 	}
// }

void sarpILS::SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution->getRoutesSize();

    double delta;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute(inst, 0);

    sroute.stats.setStart();

    for (int rid = 0; rid < solSize; rid++) {
        delta = 0;
        sroute = solution->getRoute(rid);

        // cout << "Current route: " << rid << endl;
        delta = sroute.Swap(inst, Mdist, nodeVec, problem);


        if (delta > 0){

            // cout << "Route with swap: " << endl;
            // for (int a = 0; a < sroute.getNodesSize(); a++){
            //     cout << sroute.getReq(a) << " - ";
            // }
            // cout << endl;

            // cout << "New route cost after 1 swap: " << sroute.cost() << endl;
            // // getchar();

            sroute.updateAll(inst, nodeVec, Mdist);
            solution->updateRoutes(&sroute, rid);
        }
        // cout << "In SWAP ALL: There was an improvement: " << 
        // "route: " << rid << " - " << delta << endl;
    }
    sroute.stats.setEnd();

    solution->updateCost();

    // cout << "After Swap all" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // getchar();

    cout << "\nSwap Time: " << std::setprecision(8) << sroute.stats.printTime() << endl;
}

void sarpILS::RelocateAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    int solSize = solution->getRoutesSize();

    double delta;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute(inst, 0);

    sroute.stats.setStart();

    for (int rid = 0; rid < solSize; rid++) {
        delta = 0;
        sroute = solution->getRoute(rid);
        // cout << "rid: " << rid << endl;
        delta = sroute.relocateK(inst, Mdist, nodeVec, problem, 1);
       
        if (delta > 0){
            // cout << "delta: " << delta << endl;
            // cout << "Route with relocate K: " << endl;
            // for (int a = 0; a < sroute.getNodesSize(); a++){
            //     cout << sroute.getReq(a) << " - ";
            // }
            // cout << endl;

            // cout << "New route cost after 1 relocateK: " << sroute.cost() << endl;
            // // getchar();

            sroute.updateAll(inst, nodeVec, Mdist);
            solution->updateRoutes(&sroute, rid);
        }
        // cout << "In SWAP ALL: There was an improvement: " << 
        // "route: " << rid << " - " << delta << endl;

    }

    sroute.stats.setEnd();

    solution->updateCost();

    // cout << "After Relocate all" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // getchar();
    
    cout << "\nRelocateK Time: " << std::setprecision(8) << sroute.stats.printTime() << endl;

}

void sarpILS::RVNDInter(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){

	double bestCost = solution->getCost();
	double newCost = 0;
    double delta = 0;

	list<int> nbrList;
	list<int> usedNbr;


	list<int>::iterator it;
    double begin;
	double end;

	double elapTm;
	for (int i = 0; i < 4; i++) {
		nbrList.push_back(i);
	}

    // cout << "Neighborhood list (before): " << endl;
    // for (it = nbrList.begin(); it != nbrList.end(); it++) { 
    //     cout << *it;
    // }
    // // getchar();
    int iterator = 0;

	while (!nbrList.empty()) {
    // while (iterator < 2){
        // cout << "iteration rvnd: " << iterator << endl;
		
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

        // neighbor = 4;

		switch (neighbor) {
			case 0:
                cout << "Calling relocate inter: " << endl;
				relocate (inst, nodeVec, Mdist, problem);
                cout << "\n++++++++After RelocateInter:++++++++++ " << endl;
                solution->printSol(inst);
                solution->printCosts();
                cout << "+++++++++++++++" << endl;
				break;

            case 1:
                if (solution->unservedsize() < 1){
                    break;
                }
                else{
                    cout << "Calling add unserved: " << endl;
                    solution->addunserved(inst, nodeVec, Mdist, problem);
                    cout << "\nAfter Unserved: " << endl;
                    solution->printSol(inst);
                    solution->printCosts();
                }
				break;
            case 2:
                // cout << "Calling 2opt inter: " << endl;
				// TwoOptAll (inst, nodeVec, Mdist, problem);
                // cout << "\nAfter 2opt: " << endl;
                // solution->printSol(inst);
                // solution->printCosts();                

				break;

            case 3:
                // cout << "Calling relocate block: " << endl;
				// relocateBlockAll (inst, nodeVec, Mdist, problem);
                // cout << "\nAfter relocate block: " << endl;
                // solution->printSol(inst);
                // solution->printCosts();    

				break;

            case 4:
                // cout << "Calling exchange blocks: " << endl;
				// exchangeBlocksAll (inst, nodeVec, Mdist, problem);
				// relocateBlockAll (inst, nodeVec, Mdist, problem);
                // cout << "\nAfter exchange block: " << endl;
                // solution->printSol(inst);
                // solution->printCosts();    

				break;
                
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution->getCost();

        //Removing last vehicle from solution if it is empty.
        int solSize = solution->getRoutesSize();
        sarpRoute sroute(inst, 0);

        int routeid = solSize - 1;
        sroute = solution->getRoute(routeid);

        if (sroute.getNodesSize() < 3){
            solution->removeRoute();
        }

      
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
        // // getchar();
        iterator++;
	}

    cout << "\nAfter All Inter: " << endl;
    solution->printSol(inst);
    solution->printCosts();
}

void sarpILS::relocate(instanceStat *inst,
                        vector<nodeStat> &nodeVec,
                        double **Mdist, probStat* problem){
    int solSize = 0;
    
    solSize = solution->getRoutesSize();

    double delta, bestDelta;
    bestDelta = 0;
    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    int bestCand, currCand; //both are positions in which the requests to be moved are located.

    pair <int, int> bestPairPos, currPairPos, bestRoutePair; //position and cost
    
    bool improve;
    improve = 0;
    int candidate;
    double profit;

    solution->stats.setStart();

    // cout << "(((((Solution so far ))))))" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // getchar();

    for (int rid1 = 0; rid1 < solSize; rid1++){
        for (int rid2 = 0; rid2 < solSize; rid2++){
            delta = 0;
            if (rid1 != rid2){
                //this is the delta in solution value, so it is just a matter of cost.
                
                delta = solution->relocate(inst, nodeVec, Mdist, problem,
                                        rid1, rid2, currCand, currPairPos);                

                // cout << "Current candidate in ILS function: " << currCand << endl;
                // // getchar();
                // cout << "After relocate" << endl;
                // // getchar();
                if (delta < 0){
                    if (delta < bestDelta){
                        // cout << "**********************" << endl;
                        // sroute1 = solution->getRoute(rid1);
                        // candidate = sroute1.getReq(currCand);
                        // cout << "candidate: " << candidate << endl;
                        // cout << "original position: " << currCand << endl;
                        // cout << "relocating candidates from route " << rid1  
                        // << " to route "  << rid2 << endl;
                        // cout << "New position: " << currPairPos.first << " - " << currPairPos.second << endl;
                        // cout << "Delta obtained: " << delta << endl;
                        // cout << "**********************" << endl;
                        // // getchar();
                        bestDelta = delta;
                        improve = 1;

                        // cout <<  "IMPROVE!" << endl;
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
        sroute1 = solution->getRoute(bestRoutePair.first);
        sroute2 = solution->getRoute(bestRoutePair.second);
        candidate = sroute1.getReq(bestCand);

        // cout << "Best candidate: " << bestCand << endl;
        // // getchar();

        profit = sroute1.getProfit(nodeVec, bestCand);

        // cout << "\nProfit: " << profit << endl;

        if(candidate < inst->n){ //passenger
            // cout << "\nRelocate candidate passenger: " << candidate << endl;
            // // getchar();
            sroute2.insert(inst, Mdist, candidate, bestPairPos.first, profit);
            sroute1.erase(inst, Mdist, bestCand, profit);
        }
        // else if (candidate < inst->n+inst->m){ //parcel
        else{

            // cout << "Relocate candidate parcel: " << candidate << endl;
            // // getchar();
            int dlpos, dl;
            dlpos = sroute1.getDL(candidate-inst->n);
            dl = candidate + inst->m;

            // cout << "Relocate Inter: " << endl;
            // cout << "PU: " << candidate << endl;
            // cout << "DL: " << dl << endl;
            // // getchar();

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
        // // getchar();

        sroute1.updateAll(inst, nodeVec, Mdist);
        
        sroute2.updateAll(inst, nodeVec, Mdist);

        solution->updateRoutes(&sroute1, bestRoutePair.first);
        solution->updateRoutes(&sroute2, bestRoutePair.second);
        solution->updateCost();

    }

    solution->stats.setEnd();

    // cout << "After Relocate Inter" << endl;
    // solution->printSol(inst);
    // solution->printCosts();

    cout << "\nRelocate Time: " << std::setprecision(8) << solution->stats.printTime() << endl;

}

void sarpILS::TwoOptAll(instanceStat *inst, vector<nodeStat> &nodeVec,
                        double **Mdist, probStat* problem){
    int solSize = 0;
    
    solSize = solution->getRoutesSize();

    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    sarpBlock currBlock1;
    sarpBlock currBlock2; 

    sarpBlock reqBlock1;
    sarpBlock reqBlock2; 

    //currPairPos: only necessary to track the first position to shift to other route.
    pair <int, int> bestPairPos, currPairPos, bestRoutePair; //positions of insertion
    pair <double, double> delta;

    double bestDelta = 0;

    bool improve;
    improve = 0;

    double profit;
    
    solution->stats.setStart();

    // cout << "(((((Solution so far ))))))" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // // getchar();

    for (int rid1 = 0; rid1 < solSize - 1; rid1++){
        for (int rid2 = rid1+1; rid2 < solSize; rid2++){
            delta.first = 0;
            delta.second = 0;
            // if (rid1 != rid2){
                //this is the delta in solution value, so it is just a matter of cost.

            delta = solution->TwoOpt(inst, Mdist, nodeVec, 
                                    rid1, rid2, 
                                    currPairPos, 
                                    problem, currBlock1, currBlock2);                

            // cout << "Current inipos of r1 in ILS function: " << endl;
            // cout << currPairPos.first << endl;

            // cout << "Current inipos of r2 in ILS function: " << endl;
            // cout << currPairPos.second << endl;

            // // getchar();
            // cout << "After relocate" << endl;
            // // getchar();
            totaldelta = delta.first + delta.second;

            if (totaldelta > 0){
                if (totaldelta > bestDelta){
                    // cout << "**********************" << endl;
                    // // sroute1 = solution->getRoute(rid1);
                    // // sroute2 = solution->getRoute(rid2);
                    

                    // cout << "initial position of route 1: " << currPairPos.first << endl; 
                    // cout << "initial position of route 2: " << currPairPos.second << endl;                        
                    // cout << "relocating candidates from route " << rid1  
                    // << " to route "  << rid2 << endl;
                    // cout << "Delta obtained (first, second, total): " << delta.first << " " << delta.second << " " << totaldelta << endl;
                    // cout << "**********************" << endl;
                    // // getchar();
                    bestDelta = totaldelta;
                    improve = 1;

                    // cout <<  "IMPROVE!" << endl;
                    bestPairPos = currPairPos;
                    bestRoutePair.first = rid1;
                    bestRoutePair.second = rid2;

                    reqBlock1 = currBlock1;
                    reqBlock2 = currBlock2;
                }
            }
            // }
        }
    }

    if (improve){
        sroute1 = solution->getRoute(bestRoutePair.first);
        sroute2 = solution->getRoute(bestRoutePair.second);

        int srouteSize1, srouteSize2;
        srouteSize1 = sroute1.getNodesSize() - 1;
        srouteSize2 = sroute2.getNodesSize() - 1;

        double p1 = reqBlock1.profit();
        double p2 = reqBlock2.profit();

        vector<int> movingVec1;
        vector<int> movingVec2;

        movingVec1 = reqBlock1.getBlock();
        movingVec2 = reqBlock2.getBlock();

        // cout << "Moving vec 1: " << endl;
        // for (int i = 0; i < movingVec1.size(); i++){
        //     cout << movingVec1[i] << " ";
        // }
        // cout << endl;

        // cout << "Moving vec 2: " << endl;
        // for (int i = 0; i < movingVec2.size(); i++){
        //     cout << movingVec2[i] << " ";
        // }
        // cout << endl;        

        sroute1.eraseBlock(inst, Mdist, bestPairPos.first, srouteSize1, p1);
        sroute2.eraseBlock(inst, Mdist, bestPairPos.second, srouteSize2, p2);

        sroute1.insertBlock(inst, Mdist, movingVec2, bestPairPos.first, p2);
        sroute2.insertBlock(inst, Mdist, movingVec1, bestPairPos.second, p1);

        sroute1.updateAll(inst, nodeVec, Mdist);
        
        sroute2.updateAll(inst, nodeVec, Mdist);

        solution->updateRoutes(&sroute1, bestRoutePair.first);
        solution->updateRoutes(&sroute2, bestRoutePair.second);
        solution->updateCost();        
    }

    solution->stats.setEnd();

    // cout << "After 2opt" << endl;
    // solution->printSol(inst);
    // solution->printCosts();

    cout << "\n2opt Time: " << std::setprecision(8) << solution->stats.printTime() << endl;

}

void sarpILS::exchangeBlocksAll(instanceStat *inst, vector<nodeStat> &nodeVec,
                        double **Mdist, probStat* problem){
    int solSize = 0;
    
    solSize = solution->getRoutesSize();

    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    sarpBlock currBlock1;
    sarpBlock currBlock2; 

    sarpBlock reqBlock1;
    sarpBlock reqBlock2; 

    //currPairPos: only necessary to track the first position to shift to other route.
    pair <int, int> bestPairPos, currPairPos, bestRoutePair; //positions of insertion
    pair <double, double> delta;

    double bestDelta = 0;

    bool improve;
    improve = 0;

    double profit;
    
    solution->stats.setStart();

    // cout << "(((((Solution so far ))))))" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // // getchar();

    for (int rid1 = 0; rid1 < solSize - 1; rid1++){
        for (int rid2 = rid1+1; rid2 < solSize; rid2++){
            delta.first = 0;
            delta.second = 0;

            // cout << "r1: " << rid1 << " - r2: " << rid2 << endl;
            // if (rid1 != rid2){
                //this is the delta in solution value, so it is just a matter of cost.

            delta = solution->exchangeBlocks(inst, Mdist, nodeVec, 
                                    rid1, rid2, 
                                    currPairPos, 
                                    problem, currBlock1, currBlock2);                

            // cout << "Current inipos of r1 in ILS function: " << endl;
            // cout << currPairPos.first << endl;

            // cout << "Current inipos of r2 in ILS function: " << endl;
            // cout << currPairPos.second << endl;

            // // getchar();
            // cout << "After relocate" << endl;
            // // getchar();
            totaldelta = delta.first + delta.second;

            if (totaldelta > 0){
                if (totaldelta > bestDelta){
                    // cout << "**********************" << endl;
                    // // sroute1 = solution->getRoute(rid1);
                    // // sroute2 = solution->getRoute(rid2);
                    

                    // cout << "initial position of route 1: " << currPairPos.first << endl; 
                    // cout << "initial position of route 2: " << currPairPos.second << endl;                        
                    // cout << "relocating candidates from route " << rid1  
                    // << " to route "  << rid2 << endl;
                    // cout << "Delta obtained (first, second, total): " << delta.first << " " << delta.second << " " << totaldelta << endl;
                    // cout << "**********************" << endl;
                    // // getchar();
                    bestDelta = totaldelta;
                    improve = 1;

                    // cout <<  "IMPROVE!" << endl;
                    bestPairPos = currPairPos;
                    bestRoutePair.first = rid1;
                    bestRoutePair.second = rid2;

                    reqBlock1 = currBlock1;
                    reqBlock2 = currBlock2;
                }
            }
            // }
        }
    }

    if (improve){
        sroute1 = solution->getRoute(bestRoutePair.first);
        sroute2 = solution->getRoute(bestRoutePair.second);

        int srouteSize1, srouteSize2;
        srouteSize1 = sroute1.getNodesSize() - 1;
        srouteSize2 = sroute2.getNodesSize() - 1;

        double p1 = reqBlock1.profit();
        double p2 = reqBlock2.profit();

        vector<int> movingVec1;
        vector<int> movingVec2;

        movingVec1 = reqBlock1.getBlock();
        movingVec2 = reqBlock2.getBlock();

        // cout << "Moving vec 1: " << endl;
        // for (int i = 0; i < movingVec1.size(); i++){
        //     cout << movingVec1[i] << " ";
        // }
        // cout << endl;

        // cout << "Moving vec 2: " << endl;
        // for (int i = 0; i < movingVec2.size(); i++){
        //     cout << movingVec2[i] << " ";
        // }
        // cout << endl;        
        int iniPosBlock1 = reqBlock1.getiniPos();
        int endPosBlock1 = reqBlock1.getendPos();

        int iniPosBlock2 = reqBlock2.getiniPos();
        int endPosBlock2 = reqBlock2.getendPos();

        sroute1.eraseBlock(inst, Mdist, iniPosBlock1, endPosBlock1, p1);
        sroute2.eraseBlock(inst, Mdist, iniPosBlock2, endPosBlock2, p2);

        sroute1.insertBlock(inst, Mdist, movingVec2, iniPosBlock1, p2);
        sroute2.insertBlock(inst, Mdist, movingVec1, iniPosBlock2, p1);

        sroute1.updateAll(inst, nodeVec, Mdist);
        
        sroute2.updateAll(inst, nodeVec, Mdist);

        solution->updateRoutes(&sroute1, bestRoutePair.first);
        solution->updateRoutes(&sroute2, bestRoutePair.second);
        solution->updateCost();        
    }

    solution->stats.setEnd();

    cout << "After exchange blocks" << endl;
    solution->printSol(inst);
    solution->printCosts();

    cout << "\nExchange Blocks Time: " << std::setprecision(8) << solution->stats.printTime() << endl;

}

void sarpILS::relocateBlockAll(instanceStat *inst, vector<nodeStat> &nodeVec,
                        double **Mdist, probStat* problem){
    int solSize = 0;
    
    solSize = solution->getRoutesSize();

    double totaldelta = 0;
    // int vehicle;
    // vehicle = solution->getvehicle();

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    sarpBlock currBlock;

    sarpBlock reqBlock;

    //currPairPos: only necessary to track the first position to shift to other route.

    pair <int, int> bestRoutePair; 
    pair <double, double> delta;

    //the insertion position of the block
    //start and ending positions of the block are defined in the block class
    int insPos, bestInsPos;

    insPos = -1;
    bestInsPos = -1;

    double bestDelta = 0;

    bool improve;
    improve = 0;

    double profit;
    
    solution->stats.setStart();

    // cout << "(((((Solution so far ))))))" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // // getchar();

    for (int rid1 = 0; rid1 < solSize; rid1++){
        for (int rid2 = 0; rid2 < solSize; rid2++){
            delta.first = 0;
            delta.second = 0;
            if (rid1 != rid2){
                // cout << "Route 2: " << rid2 << endl;
                //rid1: From where the block will come
                //rid2: Where the block will go
                //this is the delta in solution value, so it is just a matter of cost.

                delta = solution->relocateBlock(inst, Mdist, nodeVec, 
                                                rid1, rid2, 
                                                insPos, 
                                                problem, currBlock);                
                
                
                // cout << "Current inipos of r2 in ILS function: " << endl;
                // cout << insPos << endl;

                // // getchar();
                // cout << "After relocate" << endl;
                // // getchar();
                totaldelta = delta.first + delta.second;

                if (totaldelta > 0){
                    if (totaldelta > bestDelta){
                        
                        // cout << "-x----x----x-----x----x----x-" << endl;
                        // sroute1 = solution->getRoute(rid1);
                        // sroute2 = solution->getRoute(rid2);
                        // cout << "insertion position: " << insPos << endl; 
                        // cout << "relocating block from route " << rid1;
                        // cout << " to route "  << rid2 << endl;
                        // cout << "Delta obtained (first, second, total): " << delta.first;
                        // cout << " " << delta.second << " " << totaldelta << endl;
                        // cout << "-x----x----x-----x----x----x-" << endl;
                        // getchar();

                        bestDelta = totaldelta;

                        improve = 1;

                        bestInsPos = insPos;
                        
                        bestRoutePair.first = rid1;
                        bestRoutePair.second = rid2;

                        reqBlock = currBlock;                        
                    }
                }
            }
        }
    }

    if (improve){
        sroute1 = solution->getRoute(bestRoutePair.first);
        sroute2 = solution->getRoute(bestRoutePair.second);

        int srouteSize1;
        srouteSize1 = sroute1.getNodesSize() - 1;

        double profit = reqBlock.profit();

        vector<int> movingVec;

        movingVec = reqBlock.getBlock();

        // cout << "Moving vec: " << endl;
        // for (int i = 0; i < movingVec.size(); i++){
        //     cout << movingVec[i] << " ";
        // }
        // cout << endl;

        int iniPosBlock = reqBlock.getiniPos();

        int endPosBlock = reqBlock.getendPos();

        sroute1.eraseBlock(inst, Mdist, iniPosBlock, endPosBlock, profit);

        sroute2.insertBlock(inst, Mdist, movingVec, bestInsPos, profit);

        sroute1.updateAll(inst, nodeVec, Mdist);
        sroute2.updateAll(inst, nodeVec, Mdist);

        solution->updateRoutes(&sroute1, bestRoutePair.first);
        solution->updateRoutes(&sroute2, bestRoutePair.second);
        solution->updateCost();        
    }

    solution->stats.setEnd();

    cout << "After relocate Block" << endl;
    solution->printSol(inst);
    solution->printCosts();

    cout << "\nRelocate block Time: " << std::setprecision(8) << solution->stats.printTime() << endl;

}


void sarpILS::Perturbation(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){


    //exchange routes for all passengers of the route.
    //Remove all requests from both routes; Re-add only passengers in exchanged routes (same order)
    //Re-add 1 pair of parcels in previous routes.

    sarpRoute sroute(inst, 0);
    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);

    sarpBlock blockpas1, blockpas2;

    int rid1, rid2;
    int sizeRoute;

    double profp1, profp2, profc1, profc2;

    // int outcust = ceil(0.2*inst->n);
    // int outparc = ceil(0.3*inst->m);

    // int outcust = 2;
    // int outparc = 2;

    vector <int> vecoutcust, vecoutparc;

    vector <int> avlPas1, avlPas2; //available passengers to be removed from the selected route (1 or 2)
    vector <int> avlParc1, avlParc2; //available parcels to be removed from the selected route (1 or 2)

    int solSize = solution->getRoutesSize();

    int par1, par2, par11, par22, pas1, pas2, req;
    par1 = -1;
    par2 = -1;

    bool found, feasible, onlyparcel;
    int candidate1, candidate2, candpar1, candpar2;
    int pos1, pos2;

    vector <int> inspositions1, inspositions2, candidates;

    vector < pair <int, double> > unservPass;
    pair <int, double> PassProf;

    bool shakeparcelonly = (rand() % 100) < 60; //60% chance of shaking only parcels
    // bool shakeparcelonly = 0;

    //Purging empty routes
    bool p1 = 1;
    int emptyroute = -1;

    for (int rid = 0; rid < solSize - 1; rid++){
        sroute1 = solution->getRoute(rid);
        if (shakeparcelonly){
            if (sroute1.getPassPosSize() < 1){
                for (int i = 1; i < sroute1.getNodesSize(); i++){
                    int request = sroute1.getReq(i);
                    // cout << "Request: " << request << endl;
                    // getchar();
                    if (nodeVec[request].load > 0){//if the request is a parcel pickup location
                        solution->addtounserved(request);
                    }
                }
                sroute1.clearRoute();
                onlyparcel = 1;
            }
        }
        if (sroute1.getNodesSize() < 3){
            emptyroute = rid;
            break;
        }
    }


    if (emptyroute > -1){//if there are empty routes
        rid1 = emptyroute;
        rid2 = solSize - 1;

        // cout << "Routes for purging: " << rid1 << " - " << rid2 << endl;
        if (!onlyparcel){
            sroute1 = solution->getRoute(rid1);
        }

        sroute2 = solution->getRoute(rid2);
    }
    else{
        //selecting routes for perturbation
        rid1 = rand() % solSize;
        sroute1 = solution->getRoute(rid1);

        while (sroute1.getPassPosSize() < 1){
            // cout << "found" << endl;
            // getchar();
            rid1 = rand() % solSize;
            sroute1 = solution->getRoute(rid1);
        }

        rid2 = rid1;
        sroute2 = solution->getRoute(rid2);

        while (rid2 == rid1 || sroute2.getPassPosSize() < 1){
            rid2 = rand() % solSize;
            sroute2 = solution->getRoute(rid2);
        }
        
    }
    cout << "Routes for shaking: " << rid1 << " - " << rid2 << endl;


    //finding passengers and parcels in the selected routes; storing item labels
    if (emptyroute < 0){ //no empty routes in the solution 
        for (int i = 0; i < sroute1.getNodesSize(); i++){
            req = sroute1.getReq(i);
            if (req < inst->n){
                avlPas1.push_back(req);
            }
            else if (req < inst->n + inst->m){
                avlParc1.push_back(req);
                solution->addtounserved(req);
            }
        }
    }

    for (int i = 0; i < sroute2.getNodesSize(); i++){
        req = sroute2.getReq(i);
        if (req < inst->n){
            avlPas2.push_back(req);
        }        
        else if (req < inst->n + inst->m){
            avlParc2.push_back(req);
            solution->addtounserved(req);
        }
    }

    // cout << "Parcels from route rid1: " << endl;
    // for (int a = 0; a < avlParc1.size(); a++){
    //     cout << avlParc1[a] << " ";
    // }
    // cout << endl;

    // // getchar(); 

    // cout << "Parcels from route rid2: " << endl;
    // for (int a = 0; a < avlParc2.size(); a++){
    //     cout << avlParc2[a] << " ";
    // }
    // cout << endl;

    // getchar(); 

    //creating passenger blocks

    //adding a dummy last item to passengers blocks for adapting to make block function
    avlPas2.push_back(-1);
    avlPas1.push_back(-1);

    if (emptyroute < 0){ // no empty routes in the solution
        blockpas1.makeBlock(avlPas1, 0, avlPas1.size()-1);
        blockpas1.blockProfit(inst, nodeVec, Mdist);
        blockpas1.calcBlockTimes(inst, nodeVec, Mdist);
        profc1 = blockpas1.profit();
        sroute1.clearRoute();
        solution->updateRoutes(&sroute1, rid1);
    }

    blockpas2.makeBlock(avlPas2, 0, avlPas2.size()-1);
    blockpas2.blockProfit(inst, nodeVec, Mdist);
    blockpas2.calcBlockTimes(inst, nodeVec, Mdist);
    profc2 = blockpas2.profit();

    //clearing both routes

    sroute2.clearRoute();

    solution->updateRoutes(&sroute2, rid2);
    solution->updateCost();     

    // cout << "After removal of everything: " << endl;

    // solution->printSol(inst);
    // solution->printCosts();

    //Exchanging blocks of passengers
    if (emptyroute < 0){ //no empty routes in the solution
        avlPas1.pop_back();
        sroute2.insertBlock(inst, Mdist, avlPas1, 1, profc1);
        sroute2.updateAll(inst, nodeVec, Mdist);
        solution->updateRoutes(&sroute2, rid2);
    }

    avlPas2.pop_back();
    sroute1.insertBlock(inst, Mdist, avlPas2, 1, profc2);
    sroute1.updateAll(inst, nodeVec, Mdist);

    
    solution->updateRoutes(&sroute1, rid1);
    solution->updateCost();   

    // cout << "After reinsertion of passengers: " << endl;

    // solution->printSol(inst);
    // solution->printCosts();

    //Reinserting pairs of parcels

    //selecting parcels to be readded
    if (avlParc1.size() > 0){
        par1 = rand() % avlParc1.size();
    }

    if (avlParc2.size() > 0){
        par2 = rand() % avlParc2.size();
    }

    // cout << "Parcels for exchanging: " << endl;
    // cout << "route1: " << par1 << " - route2: " << par2 << endl;

    if (emptyroute < 0){ //no empty route in the solution
        if (par1 > -1){
            candidate1 = avlParc1[par1];
            // cout << "Parcels for reinserting rid " << rid1 << ": " << candidate1 << endl;
            
            int cand11 = candidate1 + inst->m;

            sroute1.availablePos(inst, nodeVec, candidate1, problem, inspositions1);
            int randindex =  rand() % inspositions1.size();
            pos1 = inspositions1[randindex];

            inspositions2.push_back(pos1);
            sroute1.availablePos(inst, nodeVec, cand11, problem, inspositions2);

            randindex =  rand() % inspositions2.size();
            pos2 = inspositions2[randindex];

            sroute1.insert(inst, Mdist, candidate1, pos1, nodeVec[candidate1].profit);
            sroute1.insert(inst, Mdist, cand11, pos2+1, nodeVec[cand11].profit);
            
            solution->removeunserved(candidate1);

            sroute1.updateAll(inst, nodeVec, Mdist);
            solution->updateRoutes(&sroute1, rid1);
            solution->updateCost();

        }
        if (par2 > -1){    
            candidate2 = avlParc2[par2];
            
            // cout << "Parcels for reinserting rid " << rid2 << ": " << candidate2 << endl;

            int cand22 = candidate2 + inst->m;
        
            inspositions1.clear();
            
            sroute2.availablePos(inst, nodeVec, candidate2, problem, inspositions1);
            int randindex =  rand() % inspositions1.size();
            pos1 = inspositions1[randindex];
            
            inspositions2.clear();
            
            inspositions2.push_back(pos1);
            sroute2.availablePos(inst, nodeVec, cand22, problem, inspositions2);

            randindex =  rand() % inspositions2.size();
            pos2 = inspositions2[randindex];

            sroute2.insert(inst, Mdist, candidate2, pos1, nodeVec[candidate2].profit);
            sroute2.insert(inst, Mdist, cand22, pos2+1, nodeVec[cand22].profit);
            
            solution->removeunserved(candidate2);
            sroute2.updateAll(inst, nodeVec, Mdist);
            solution->updateRoutes(&sroute2, rid2);
            solution->updateCost();

        }
    }
    
    // cout << "After reinsertion of selected parcels: " << endl;

    // solution->printSol(inst);
    // solution->printCosts();

    if (emptyroute > -1){
        int routeid = solSize - 1;
        sroute = solution->getRoute(routeid);

        if (sroute.getNodesSize() < 3){
            solution->removeRoute();
        }
    }

    cout << "After purging: " << endl;

    solution->printSol(inst);
    solution->printCosts();

    // getchar();
    
    solution->printUnserved();
    
}