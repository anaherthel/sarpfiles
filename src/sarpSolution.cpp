#include "sarpSolution.h"


void sarpSolution::addRoute(sarpRoute *route){
    this->routes.push_back(*route);
    this->updateCost();
}

void sarpSolution::updateCost() {
    this->cost = 0;
    for (int i = 0; i < routes.size(); ++i) {
        this->cost += routes[i].cost();
    }
}

void sarpSolution::updateRoutes(sarpRoute *route, int idr) {
    this->routes[idr] = *route;
}

void sarpSolution::updateVehicles(){
    this->usedK++;
}

void sarpSolution::printSol(instanceStat *inst) {

    sarpRoute sroute(inst, usedK);

    cout << "SARP Solution: " << endl;
    for (int i = 0; i < routes.size(); i++){
        sroute = routes[i];
        for(auto j: sroute){
            cout << j << " - ";
        }
        sroute.printTotalTime();
        cout << endl;
    }
    cout << endl;
}
void sarpSolution::printLoadSol(instanceStat *inst) {

    sarpRoute sroute(inst, usedK);

    cout << "Solution Load: " << endl;
    for (int i = 0; i < routes.size(); i++){
        sroute = routes[i];
        cout << endl << "**********Load************" << endl;
        cout << "Route " << i << ": " << endl;
        sroute.printLoad();
        cout << endl << "**********end Load********" << endl;
    }
    cout << endl;
}
void sarpSolution::addtounserved(int candidate) {
    unserved.push_back(candidate);
}   


void sarpSolution::printCosts(){
    cout << "Cost of routes: " << endl;

    for (int i = 0; i < this->routes.size(); i++){
        cout << "Route " << i << ": " << routes[i].cost() << endl;
    }

    cout << "\nCost of routes: " << this->cost << endl;
}

double sarpSolution::relocate (instanceStat *inst, vector<nodeStat> &nodeVec, 
                                double **Mdist, probStat* problem,
                                 int rid1, int rid2, int &currCand, pair <int, int> &currPairPos){

    sarpRoute r1(inst, 0);
    sarpRoute r2(inst, 0);
    int rsize1, rsize2;

    r1 = getRoute(rid1);
    r2 = getRoute(rid2);

    rsize1 = r1.getNodesSize();

    // cout << "Size of route 1: " << rsize1 << endl;
    // getchar();
    rsize2 = r2.getNodesSize(); 

    bool feasible = 0;

    int candidate, candidate2;
    int bestpos1, bestpos2;
    int inCurrent;

    vector<int> inspositions; 
    vector<int> inspositions2;

    pair <int, double> cheapestpair; //position and delta
    
    cheapestpair.first = -1;
    cheapestpair.second = -100000;
    
    vector< pair <int, double> > cheapestMove;
    
    cheapestMove.push_back(cheapestpair);
    cheapestMove.push_back(cheapestpair); //chapest move has 2 cheapest pairs. Only 1 is used for passengers.
    
    double best_cost, compareCost, iniCost, rval1, rval2, rmvVal, addVal;

    rval1 = r1.cost();
    rval2 = r2.cost();

    best_cost = rval1 + rval2;
    iniCost = rval1 + rval2;

    currCand = -1;

    cout << "Routes: " << rid1 << " and " << rid2;

    for (int i = 1; i < rsize1-1; i++){
        candidate = r1.getReq(i);
        compareCost = rval1 + rval2;
        cout << "\ncandidate: " << candidate << endl;

        cout << "Initial cost of both routes: " << compareCost << endl;

        if(nodeVec[candidate].load < 0){
            // cout << "\n Delivery node (skip)";
            // getchar();
            continue;
        }

        inspositions.clear();
        r2.availablePos(inst, nodeVec, candidate, problem, inspositions);

        if (inspositions.size() < 1){
            continue;
        }
        
        // cout << "Available positions for insertion: " << endl;
        // for (int p = 0; p < inspositions.size(); p++){
        //     cout << inspositions[p] << " - ";
        // }
        // cout << endl;
        // getchar();

        if (candidate < inst->n){
            cheapestpair = r2.cheapestInsertion(inst, nodeVec, Mdist, candidate, inspositions);
            // cout << "end of cheapest insertion." << endl;
            // getchar();
            addVal = cheapestpair.second;

            cout << "Passenger add val: " << addVal << endl;

            if (addVal > 0){
                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 0);
                cout << "Calculate rmvval: " << rmvVal << endl;
                compareCost += addVal + rmvVal;           
            }
            else{
                continue;
            }                
            
            cout << "Passenger added at position: " << cheapestpair.first << endl;

            // cout << "Compare Cost: " << compareCost;
        }
        else {
            cheapestMove[0].first = -1;
            cheapestMove[0].second = -100000;
            cheapestMove[1].first = -1;
            cheapestMove[1].second = -100000;
            // cout << "cheapest move 2 BEFORE: " <<  cheapestMove[1].second << endl;
            int node2 = candidate + inst->m;
            inspositions2.clear();
            r2.cheapestInsertionParcel(inst, nodeVec, Mdist, candidate, node2, inspositions, inspositions2, cheapestMove, problem);
            
            // if (inspositions2.size() < 1){
            //     continue;
            // }

            if(cheapestMove[0].first != cheapestMove[1].first){
                addVal = cheapestMove[0].second + cheapestMove[1].second;
            }
            else{
                addVal = nodeVec[candidate].profit + cheapestMove[1].second;
                // cout << "cheapest move 2: " <<  cheapestMove[1].second << endl;
            }  
 
            if (addVal > 0){
                cout << "New add value: " << addVal << endl;
                // getchar();

                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 1);

                cout << "New rmv value: " << rmvVal << endl;
                // getchar();
                
                cout << "Adding at position: " << cheapestMove[0].first << " and " << cheapestMove[1].first << endl;
                // getchar();

                compareCost += addVal + rmvVal;  
            }
            else{
                continue;
            }
            // cout << "Compare Cost: " << compareCost;
        }

        if (compareCost > best_cost){
            // cout << "\nThere was an improvement" << endl;
            // cout << "\nCandidate: " << candidate << endl;
            // getchar();
            // inCurrent = i;
            currCand = i;
            // currCand = &inCurrent;
            cout << "Current candidate in solutions function: " << currCand << endl;
            // getchar();
            if (candidate < inst->n){
                best_cost = compareCost;
                currPairPos.first = cheapestpair.first;
                currPairPos.second = -1;

                // cout << "insert in position: "<< currPairPos.first << endl; 
                // cout << "with an improvement of " << best_cost << endl;
            }
            else{
                best_cost = compareCost;
                currPairPos.first = cheapestMove[0].first;
                currPairPos.second = cheapestMove[1].first;

                // cout << "insert in position: "<< currPairPos.first << endl; 
                // cout << "with an improvement of " << best_cost << endl;
            }

        }
        // else{
        //     currCand = -1;
        // }
    }

    double delta;

    delta = iniCost - best_cost;

    cout << "Best cost: " << best_cost << endl;
    cout << "cost (previous cost): " << iniCost << endl;
    cout << "delta: " << delta << endl;
    // getchar();
    // currCand = &inCurrent;

    cout << "Current candidate passing out of function: " << currCand << endl;
    // getchar();

    return delta;
}

    pair <double, double> sarpSolution::TwoOpt(instanceStat *inst, double **Mdist,
                    vector<nodeStat> &nodeVec, 
                    int rid1, int rid2, 
                    pair <int, int> &currPairPos,
                    probStat* problem){

    pair<double, double> delta;

    delta.first = 0;
    delta.second = 0;

	pair<double, double> bestDelta;

    bestDelta.first = 0;
    bestDelta.second = 0;

	double newCost = 0;

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);
	
    sroute1 = getRoute(rid1);
    sroute2 = getRoute(rid2);

    int pos1 = 0;
	int pos2 = 0;

    int jstart, jend;

    pair <int, int> inter1, inter2, tempinter;
    
    vector<int> tempVector, zeroVec;
    
    int temp;

    bool feasible, feasPos, improve;

    improve = 0;
    // cout << "Current cost: " << cost_ << endl;
    // getchar();
    int loadSize1, loadSize2;

    loadSize1 = sroute1.getLoadSize();
    loadSize2 = sroute2.getLoadSize();

    for (int i = 0; i < loadSize1; i++){
        int currLoad, request;
        currLoad = sroute1.getReqLoad(i);
        request = sroute1.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                zeroVec.push_back(i);
            }

        }
        else if (currLoad == 1){
            if (nodeVec[request].load > 0 ){
                zeroVec.push_back(i);
            }
        }
    }

    cout << "Zero Sum vec: " << endl;

    for (int j = 0; j < zeroVec.size(); j++){
        cout << zeroVec[j] << " ";
    }
    cout << endl;
    getchar();

	// for (int i = 1; i < nodes_.size() - 3; i++) {
	// 	delta = -Mdist[nodes_[i - 1]][nodes_[i]];
    //     // inter1 = getInterval(i);

    //     // cout << "Interval of i: " << inter1.first <<  " - " << inter1.second << endl; 

    //     if(nodes_[i] < inst->n){
    //         jend = inter1.second;            
    //     }
    //     else if (nodes_[i] < inst->n+inst->m){
    //         temp = getDL(nodes_[i]-inst->n);
    //         // cout << "temp: " << temp << endl;
    //         if (problem->dParcel > 0){
    //             if (temp == i + 1){
    //                 // getchar();
    //                 continue;
    //             }
    //             jend = temp;
    //         }
    //         else{
    //             jend = getPrevPass(temp);
    //         }
    //     }
    //     else{
    //         jend = nodes_.size() - 1;
    //     }
	// 	for (int j = i + 2; j < jend; j++) {
    //         feasPos = 0;
    //         feasible = 0;

    //         if (nodes_[i] < inst->n && nodes_[j] < inst->n){//cannot interexchange passengers (TW)
    //             continue;
    //         }
    //         else{
    //             inter2 = getInterval(j);
    //             if (nodes_[i] < inst->n || nodes_[j] < inst->n){
    //                 feasPos = checkInterval(inst, i, j, inter1, inter2);
    //             }
    //             else{
    //                 feasPos = 1;
    //             }
    //             if(nodeVec[nodes_[j]].load < 0){
    //                 cout << "Check delivery" << endl;
    //                 feasPos = checkDelivery(inst, i, j, problem);
    //             }

    //             if (feasPos){
    //                 feasible = testSwap(inst, nodeVec, Mdist, i, j, inter1, inter2);

    //                 if (feasible){

    //                     if (j - i == 1) {
    //                         delta += - Mdist[nodes_[i]][nodes_[j]] 
    //                         + Mdist[nodes_[j]][nodes_[i]]                            
    //                         - Mdist[nodes_[j]][nodes_[j + 1]] 
    //                         + Mdist[nodes_[i - 1]][nodes_[j]]
    //                         + Mdist[nodes_[i]][nodes_[j + 1]];
    //                     }
    //                     else {
    //                         delta += - Mdist[nodes_[i]][nodes_[i + 1]] 
    //                         - Mdist[nodes_[j - 1]][nodes_[j]]
    //                         - Mdist[nodes_[j]][nodes_[j + 1]] 
    //                         + Mdist[nodes_[i - 1]][nodes_[j]] 
    //                         + Mdist[nodes_[j]][nodes_[i + 1]] 
    //                         + Mdist[nodes_[j - 1]][nodes_[i]] 
    //                         + Mdist[nodes_[i]][nodes_[j + 1]];
    //                     }

    //                     if (delta < 0) {
    //                         if (delta < bestDelta) {
    //                             // cout << "\nThere was an improvement with delta as " << delta << endl;
    //                             // getchar();
    //                             bestDelta = delta;
    //                             pos1 = i;
    //                             pos2 = j;
    //                             swap = 1;
    //                         }
    //                     }
    //                 }
    //             }          
    //         }			
	// 	}
	// }

    // if (improve){

    //     cout << "pos1: " << pos1 << endl;
    //     cout << "pos2: " << pos2 << endl;
    //     cout << "element1: " << nodes_[pos1] << endl;
    //     cout << "element2: " << nodes_[pos2] << endl;

    //     tempElement = nodes_[pos1];
    //     nodes_[pos1] = nodes_[pos2];
    //     nodes_[pos2] = tempElement;

    //     updateAll(inst,nodeVec, Mdist);
    //     bestDelta = -(bestDelta*inst->costkm);
    //     // cout << "Best Delta: " << bestDelta << endl;

    //     updateCost(bestDelta);

    //     // cout << "New cost: " << cost_ << endl;

    //     calcCost(inst, nodeVec, Mdist);
    //     // cout << "Calculated cost: " << cost_ << endl;
    //     // getchar();

    //     // cout << "Route with swap: " << endl;
    //     // for (int a = 0; a < nodes_.size(); a++){
    //     //     cout << nodes_[a] << " - ";
    //     // }
    //     // cout << endl;
    //     // getchar();
    // }
    // else{
    //     delta = 0;
    // }
    return bestDelta;
}

void sarpSolution::addunserved(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){
    sarpRoute sroute(inst, 0);
    int candidate, candidate2;

    vector<int> inspositions, inspositions2;
    pair <int, double> cheapestpair;

    cheapestpair.first = -1;
    cheapestpair.second = -100000;

    vector< pair<int, double> > cheapestMove;
    pair <int, int> bestpositions;

    double best_cost;
    int best_route;
    bool inserted;
    bool feastime = 0;
    int vehicle;

    this->stats.setStart();

    cheapestMove.push_back(cheapestpair);
    cheapestMove.push_back(cheapestpair);

    for (int cand = 0; cand < unserved.size(); cand++){
        candidate = unserved[cand];
        candidate2 = candidate + inst->m;

        best_route = -1;
        best_cost = -100000;
        inserted = 0;
        feastime = 0;
        bestpositions.first = -1;
        bestpositions.second = -1;

        for (int rid = 0; rid < routes.size(); rid++){
            cheapestMove[0].first = -1;
            cheapestMove[0].second = -100000;
            cheapestMove[1].first = -1;
            cheapestMove[1].second = -100000;

            sroute = getRoute(rid);

            inspositions.clear();
            inspositions2.clear();
            
            sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);

            sroute.cheapestInsertionParcel(inst, nodeVec, Mdist, candidate, candidate2, inspositions, inspositions2, cheapestMove, problem);
            
            if (cheapestMove[0].first == cheapestMove[1].first){
                if (cheapestMove[0].second > best_cost){
                    best_cost = cheapestMove[0].second;
                    best_route = rid;
                    bestpositions.first = cheapestMove[0].first;
                    bestpositions.second = cheapestMove[1].first;
                    inserted = 1;
                }                    
            }
            else{
                if (cheapestMove[0].second + cheapestMove[1].second > best_cost){
                    best_cost = cheapestMove[0].second + cheapestMove[1].second;
                    best_route = rid;
                    bestpositions.first = cheapestMove[0].first;
                    bestpositions.second = cheapestMove[1].first;
                    inserted = 1;
                }
            }
        }
        if (best_cost == -100000){
            if (problem->dParcel > 0){
                vehicle = getvehicle();
                sarpRoute newroute(inst, vehicle);
                feastime = newroute.fInsertionParcel(inst, nodeVec, Mdist, candidate, candidate2);
                newroute.calcCost(inst, nodeVec, Mdist);
                newroute.insert(inst, Mdist, candidate, 1, nodeVec[candidate].profit);
                newroute.insert(inst, Mdist, candidate2, 2, nodeVec[candidate2].profit);
                newroute.updateAll(inst, nodeVec, Mdist);
                addRoute(&newroute);

                for (int u = 0; u < unserved.size(); ++u) {
                    if (this->unserved[u] == candidate) {
                        this->unserved.erase(this->unserved.begin() + u);
                        break;
                    }
                }

                updateVehicles();
                inserted = 1;
                updateCost();
                break;
            }
            else{
                addtounserved(candidate);
                for (int u = 0; u < unserved.size(); ++u) {
                    if (this->unserved[u] == candidate) {
                        this->unserved.erase(this->unserved.begin() + u);
                        break;
                    }
                }
                break;
            }
        }

        else{
            sroute = getRoute(best_route);

            // cout << "::::******::::::******::::::" << endl;
            // cout << "Request1: " << candidate << " - Request2: " << candidate2 << endl;
            // cout << "Best position 1: " << bestpositions.first << "; Best position 2: " << bestpositions.second << endl;
            // cout << "::::******::::::******::::::" << endl;

            // getchar();

            sroute.insert(inst, Mdist, candidate, bestpositions.first, nodeVec[candidate].profit);
            sroute.insert(inst, Mdist, candidate2, bestpositions.second+1, nodeVec[candidate2].profit);
            sroute.updateAll(inst, nodeVec, Mdist);
            updateRoutes(&sroute, best_route);
            updateCost();

            inserted = 1;
            // cout << "*****Route with insertion: " << endl;
            // for (auto a: sroute){
            //     cout << a << " - ";
            // }
            // cout << "New route cost: " << sroute.cost() << endl;
            // cout << endl << endl;

            for (int u = 0; u < unserved.size(); ++u) {
                if (this->unserved[u] == candidate) {
                    this->unserved.erase(this->unserved.begin() + u);
                    break;
                }
            }
            break;
        }

    }

    cout << "After add unserved" << endl;
    printSol(inst);
    printCosts();

    this->stats.setEnd();
    cout << "\nAdd Unserved Time: " << std::setprecision(8) << this->stats.printTime() << endl;

}
