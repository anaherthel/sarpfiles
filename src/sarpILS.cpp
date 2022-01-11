#include "sarpILS.h"
// #include "readdata.h"


#include <cstdlib>
#include <stdio.h>


void sarpILS::ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
    
    iterILS = 0;
    // maxIterILS = 10*inst->n+inst->m;
    
    maxIterILS = 300;

    iterMS = 0;
    maxiterMS = 30;

    sarpRoute sroute(inst, 0);

    double globalBestCost;
    double bestCost;
    double currentCost;
    double profit;

    globalBestSol = new sarpSolution();
    while (iterMS <= maxiterMS){

        sarpConstruction sCon(inst, nodeVec);
        solution = new sarpSolution();
        
        sCon.ConstrProc(inst, nodeVec, Mdist, problem, solution);

        currentCost = solution->getCost();
        bestCost = currentCost;
        bestSol = solution;
        globalBestCost = currentCost;
        globalBestSol = solution;
        // minK = solution->getvehicle();

        int sol_size;
        sol_size = solution->getRoutesSize();

        //Messing solution to test neighborhoods
        // sroute = solution->getRoute(0);
        
        // profit = sroute.getProfit(nodeVec, 1);
        // sroute.erase(inst, Mdist, 7, profit);
        // sroute.updateAll(inst, nodeVec, Mdist);
        // sroute.insert(inst, Mdist, 1, 5, profit);
        // sroute.updateAll(inst, nodeVec, Mdist);

        // sroute.calcCost(inst, nodeVec, Mdist);
            
        // solution->updateRoutes(&sroute, 0);
        // solution->updateCost();

        // cout << "Worse solution: " << endl;

        // solution->printSol(inst);
        // solution->printCosts();
        // // getchar();
        // profit = sroute.getProfit(nodeVec, 1);
        // sroute.erase(inst, Mdist, 1, profit);
        // sroute.insert(inst, Mdist, 11, 2, profit);

        //Initial solution
        while (iterILS <= maxIterILS) {
            cout << "***********************" << endl;
            cout << "ITERATION: " << iterILS << endl;
            cout << "***********************" << endl;

            
            RVNDIntra(inst, nodeVec, Mdist, problem);
    
            RVNDInter(inst, nodeVec, Mdist, problem);


            // RVNDAll(inst, nodeVec, Mdist, problem);
            currentCost = solution->getCost();
            int currentK = solution->getvehicle();
            if (bestCost > currentCost) {
                bestSol = solution;
                bestCost = currentCost;
        
                //cout << endl << "\nBetter Solution Cost: " << solutionCost << endl << endl;
        
                iterILS = 0;
            }

            // cout << "\n-----x-----" << "\nBest Solution so far: ";
            // bestSol.printSol(inst);

            // cout <<"\nSolution Best Cost: " << endl;
            
            // bestSol.printCosts();
            // cout << "\n-----x-----" << endl;

            // bestSol.addunserved(inst, nodeVec, Mdist, problem);
            solution = bestSol;
            cout << "Calling Perturbation: " << endl;

            Perturbation(inst, nodeVec, Mdist, problem);



            cout << "\n-----x-----" << "\nSolution with perturbation: ";
            solution->printSol(inst);        
            solution->printCosts();
            cout << "\n-----x-----" << endl;
            getchar();

            // solution->addunserved(inst, nodeVec, Mdist, problem);

            iterILS++;
        }

        cout << "\n-----x-----" << "\nBest Solution: ";
        bestSol->printSol(inst);

        cout <<"\nSolution Best Cost: " << endl;
        
        bestSol->printCosts();
        cout << "\n-----x-----" << endl;

        if (bestCost > globalBestCost){
            globalBestSol = bestSol;
            globalBestCost = bestCost;
        }
        // cout << "\n-----x-----" << "\nBest Solution: ";
        // printSolution(solutionBest);
        // cout <<"\nSolution Best Cost: " << solutionBestCost << endl;
        iterMS++;

    }

    cout << "\n-----x-----" << "\nBest Global Solution: ";
    globalBestSol->printSol(inst);

    cout <<"\nSolution Best Global Cost: " << endl;
    
    globalBestSol->printCosts();
    cout << "\n-----x-----" << endl;
    
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

				break;

			case 1:
                cout << "Calling relocate intra: " << endl;
				RelocateAll (inst, nodeVec, Mdist, problem);
                      
				break;

            // case 2:
			// 	cout << "Calling 3-opt: " << endl;
            //     ThreeOptAll(inst, nodeVec, Mdist, problem);

			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution->getCost();

        cout << "\nAfter Intra: " << endl;
        solution->printSol(inst);
        solution->printCosts();
        // // getchar();
        
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

        cout << "Current route: " << rid << endl;
        delta = sroute.Swap(inst, Mdist, nodeVec, problem);


        if (delta > 0){

            cout << "Route with swap: " << endl;
            for (int a = 0; a < sroute.getNodesSize(); a++){
                cout << sroute.getReq(a) << " - ";
            }
            cout << endl;

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

// void sarpILS::ThreeOptAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem){
//    int solSize = solution->getRoutesSize();

//     double delta;
//     // int vehicle;
//     // vehicle = solution->getvehicle();

//     sarpRoute sroute(inst, 0);

//     sroute.stats.setStart();

//     for (int rid = 0; rid < solSize; rid++) {
//         delta = 0;
//         sroute = solution->getRoute(rid);

//         cout << "Current route: " << rid << endl;
//         delta = sroute.ThreeOpt(inst, Mdist, nodeVec, problem);

//         if (delta > 0){

//             cout << "Route with 3opt: " << endl;
//             for (int a = 0; a < sroute.getNodesSize(); a++){
//                 cout << sroute.getReq(a) << " - ";
//             }
//             cout << endl;

//             // cout << "New route cost after 1 swap: " << sroute.cost() << endl;
//             // // getchar();

//             sroute.updateAll(inst, nodeVec, Mdist);
//             solution->updateRoutes(&sroute, rid);
//         }
//         // cout << "In SWAP ALL: There was an improvement: " << 
//         // "route: " << rid << " - " << delta << endl;
//     }
//     sroute.stats.setEnd();

//     solution->updateCost();

//     // cout << "After Swap all" << endl;
//     // solution->printSol(inst);
//     // solution->printCosts();
//     // // getchar();

//     cout << "\n3opt Time: " << std::setprecision(8) << sroute.stats.printTime() << endl;
// }



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
        cout << "iteration rvnd: " << iterator << endl;
		
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
                // cout << "\n++++++++After RelocateInter:++++++++++ " << endl;
                // solution->printSol(inst);
                // solution->printCosts();
                // cout << "+++++++++++++++" << endl;
				break;

            case 1:
                if (solution->unservedsize() < 1){
                    break;
                }
                else{
                    cout << "Calling add unserved: " << endl;
                    solution->addunserved(inst, nodeVec, Mdist, problem);
                }
				break;
            case 2:
                cout << "Calling 2opt inter: " << endl;
				// TwoOptAll (inst, nodeVec, Mdist, problem);

				break;

            case 3:
                cout << "Calling relocate block: " << endl;
				// relocateBlockAll (inst, nodeVec, Mdist, problem);

				break;

            case 4:
                cout << "Calling exchange blocks: " << endl;
				// exchangeBlocksAll (inst, nodeVec, Mdist, problem);

				break;
                
			default:
				cout << "Out of range" << endl;
				break;
		}

		newCost = solution->getCost();

        // cout << "\nAfter Inter: " << endl;
        // solution->printSol(inst);
        // solution->printCosts();

        int solSize = solution->getRoutesSize();
        sarpRoute sroute(inst, 0);
        pair <int, int> temppair;
        pair <nodeStat, int> temppass;

        // for (int rid = 0; rid < solSize; rid++){
        //     sroute = solution->getRoute(rid);
        //     cout << "Route: " << rid << endl << endl;
        //     cout << "First passenger: " << sroute.fPass() << endl;
        //     cout << "Last passenger: " << sroute.lPass() << endl;
        //     cout << "Start time: " << sroute.startTime() << endl;
        //     cout << "End time: " << sroute.endTime() << endl;
        //     cout << "Passengers: " << endl;

        //     for (int nid = 0; nid < sroute.getPassPosSize(); nid++){
        //         temppass = sroute.getPassReq(nid);
        //         cout << nid << ": " << temppass.first.index << " - " << temppass.second << endl; 
        //     }
        //     cout << "Parcels: " << endl;

        //     for (int nid = 0; nid < sroute.getPaPdvecSize(); nid++){
        //         temppair = sroute.getPDReq(nid);
        //         cout << nid << ": " << temppair.first << " - " << temppair.second << endl; 
        //     }
        //     cout << endl;           
        // }
        // // getchar();
        
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

    cout << "(((((Solution so far ))))))" << endl;
    solution->printSol(inst);
    solution->printCosts();
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

    cout << "After Relocate Inter" << endl;
    solution->printSol(inst);
    solution->printCosts();

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

    cout << "After 2opt" << endl;
    solution->printSol(inst);
    solution->printCosts();

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

            cout << "r1: " << rid1 << " - r2: " << rid2 << endl;
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

    cout << "(((((Solution so far ))))))" << endl;
    solution->printSol(inst);
    solution->printCosts();
    // // getchar();

    for (int rid1 = 0; rid1 < solSize; rid1++){
        for (int rid2 = 0; rid2 < solSize; rid2++){
            delta.first = 0;
            delta.second = 0;
            if (rid1 != rid2){
                cout << "Route 2: " << rid2 << endl;
                //rid1: From where the block will come
                //rid2: Where the block will go
                //this is the delta in solution value, so it is just a matter of cost.

                delta = solution->relocateBlock(inst, Mdist, nodeVec, 
                                                rid1, rid2, 
                                                insPos, 
                                                problem, currBlock);                
                
                
                cout << "Current inipos of r2 in ILS function: " << endl;
                cout << insPos << endl;

                // // getchar();
                // cout << "After relocate" << endl;
                // // getchar();
                totaldelta = delta.first + delta.second;

                if (totaldelta > 0){
                    if (totaldelta > bestDelta){
                        
                        cout << "-x----x----x-----x----x----x-" << endl;
                        sroute1 = solution->getRoute(rid1);
                        sroute2 = solution->getRoute(rid2);
                        cout << "insertion position: " << insPos << endl; 
                        cout << "relocating block from route " << rid1;
                        cout << " to route "  << rid2 << endl;
                        cout << "Delta obtained (first, second, total): " << delta.first;
                        cout << " " << delta.second << " " << totaldelta << endl;
                        cout << "-x----x----x-----x----x----x-" << endl;
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

        cout << "Moving vec: " << endl;
        for (int i = 0; i < movingVec.size(); i++){
            cout << movingVec[i] << " ";
        }
        cout << endl;

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

    bool found;
    int candidate1, candidate2, candpar1, candpar2;
    int pos1, pos2;

    bool feasible;

    vector <int> inspositions1, inspositions2, candidates;

    vector < pair <int, double> > unservPass;
    pair <int, double> PassProf;

    //selecting routes for perturbation
    rid1 = rand() % solSize;
    rid2 = rid1;
    sroute2 = solution->getRoute(rid2);

    while (rid2 == rid1 || sroute2.getPassPosSize() < 1){
        rid2 = rand() % solSize;
        sroute2 = solution->getRoute(rid2);
    }

    cout << "Routes for shaking: " << rid1 << " - " << rid2 << endl;


    sroute1 = solution->getRoute(rid1);
    sroute2 = solution->getRoute(rid2);
    
    //finding passengers and parcels in the selected routes; storing item labels
    for (int i = 0; i < sroute1.getNodesSize(); i++){
        req = sroute1.getReq(i);
        if (req < inst->n + inst->m){
            avlParc1.push_back(req);
        }
    }

    for (int i = 0; i < sroute2.getNodesSize(); i++){
        req = sroute2.getReq(i);
        if (req < inst->n + inst->m){
            avlParc2.push_back(req);
        }
    }
 
    //selecting parcels to be readded
    par1 = rand() % avlParc1.size();
    par2 = rand() % avlParc2.size();

    // candpar1 = sroute1.getReq(avlParc1[par1]);
    // candpar2 = sroute2.getReq(avlParc2[par2]);

    cout << "Parcels for exchanging: " << endl;
    cout << "route1: " << par1 << " - route2: " << par2 << endl;

    // par11 = sroute1.getDL(candpar1 - inst->m);
    // par22 = sroute2.getDL(candpar2 - inst->m);

    //storing passengers (storing the item label)
    if (sroute1.fPass() > -1){
        for (int i = 0; i < sroute1.getNodesSize(); i++){
            req = sroute1.getReq(i);
            if (req < inst->n){
                avlPas1.push_back(req);
            }
            
        }
    }

    if (sroute2.fPass() > -1){
        for (int i = 0; i < sroute2.getNodesSize(); i++){
            req = sroute2.getReq(i);
            if (req < inst->n){
                avlPas2.push_back(req);
            }
        }
    }

    //creating passenger blocks

    blockpas1.makeBlock(avlPas1, 0, avlPas1.size()-1);
    blockpas2.makeBlock(avlPas2, 0, avlPas2.size()-1);
    blockpas1.blockProfit(inst, nodeVec, Mdist);
    blockpas2.blockProfit(inst, nodeVec, Mdist);
    blockpas1.calcBlockTimes(inst, nodeVec, Mdist);
    blockpas2.calcBlockTimes(inst, nodeVec, Mdist);
    profc1 = blockpas1.profit();
    profc2 = blockpas2.profit();
    //clearing both routes

    sroute1.clearRoute();
    sroute2.clearRoute();

    solution->updateRoutes(&sroute1, rid1);
    solution->updateRoutes(&sroute2, rid2);
    solution->updateCost();     

    cout << "After removal of everything: " << endl;

    solution->printSol(inst);
    solution->printCosts();

    //Exchanging blocks of passengers

    sroute1.insertBlock(inst, Mdist, avlPas2, 1, profc2);
    sroute2.insertBlock(inst, Mdist, avlPas1, 1, profc1);
    sroute1.updateAll(inst, nodeVec, Mdist);
    sroute2.updateAll(inst, nodeVec, Mdist);

    
    solution->updateRoutes(&sroute1, rid1);
    solution->updateRoutes(&sroute2, rid2);
    solution->updateCost();   

    cout << "After reinsertion of passengers: " << endl;

    solution->printSol(inst);
    solution->printCosts();

    getchar();



    

}