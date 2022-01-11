#include "sarpSolution.h"


void sarpSolution::addRoute(sarpRoute *route){
    this->routes.push_back(*route);
    this->updateCost();
}

void sarpSolution::removeRoute(){
    this->routes.pop_back();
    this->updateCost();
    this->usedK--;
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
        cout << " Route Start Time: " << sroute.startTime() << " - " << endl;
        sroute.printTotalTime();
        
        cout << endl;
    }
    cout << endl;

    cout << "Final number of vehicles: " << usedK << endl;
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
    // // getchar();
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

    // cout << "Routes: " << rid1 << " and " << rid2;

    for (int i = 1; i < rsize1-1; i++){
        candidate = r1.getReq(i);
        compareCost = rval1 + rval2;
        // cout << "\ncandidate: " << candidate << endl;

        // cout << "Initial cost of both routes: " << compareCost << endl;

        if(nodeVec[candidate].load < 0){
            // cout << "\n Delivery node (skip)";
            // // getchar();
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
        // // getchar();

        if (candidate < inst->n){
            cheapestpair = r2.cheapestInsertion(inst, nodeVec, Mdist, candidate, inspositions);
            // cout << "end of cheapest insertion." << endl;
            // // getchar();
            addVal = cheapestpair.second;

            // cout << "Passenger add val: " << addVal << endl;

            if (addVal > 0){
                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 0);
                // cout << "Calculate rmvval: " << rmvVal << endl;
                compareCost += addVal + rmvVal;           
            }
            else{
                continue;
            }                
            
            // cout << "Passenger added at position: " << cheapestpair.first << endl;

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
                // cout << "New add value: " << addVal << endl;
                // // getchar();

                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 1);

                // cout << "New rmv value: " << rmvVal << endl;
                // // getchar();
                
                // cout << "Adding at position: " << cheapestMove[0].first << " and " << cheapestMove[1].first << endl;
                // // getchar();

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
            // // getchar();
            // inCurrent = i;
            currCand = i;
            // currCand = &inCurrent;
            // cout << "Current candidate in solutions function: " << currCand << endl;
            // // getchar();
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

    // cout << "Best cost: " << best_cost << endl;
    // cout << "cost (previous cost): " << iniCost << endl;
    // cout << "delta: " << delta << endl;
    // // getchar();
    // currCand = &inCurrent;

    // cout << "Current candidate passing out of function: " << currCand << endl;
    // // getchar();

    return delta;
}

pair <double, double> sarpSolution::TwoOpt(instanceStat *inst, double **Mdist,
                    vector<nodeStat> &nodeVec, int rid1, int rid2, 
                    pair <int, int> &currPairPos, probStat* problem,
                    sarpBlock &reqBlock1, sarpBlock &reqBlock2){

    pair<double, double> delta;

    delta.first = 0;
    delta.second = 0;

	pair<double, double> bestDelta;

	double newCost = 0;
    
    pair<int, int> interpos1, interpos2;

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);
	
    sroute1 = getRoute(rid1);
    sroute2 = getRoute(rid2);

    bestDelta.first = sroute1.cost();
    bestDelta.second = sroute2.cost();

    int pos1 = 0;
	int pos2 = 0;

    int jstart, jend;

    vector<int> zeroPos1, zeroPos2, nodes1, nodes2;

    nodes1 = sroute1.getNodes();
    nodes2 = sroute2.getNodes();
    
    sarpBlock currBlock1;
    sarpBlock currBlock2;  

    bool feasible, improve;

    improve = 0;
    // cout << "Current cost: " << cost_ << endl;
    // // getchar();
    int loadSize1, loadSize2;
    double newEndTime1, newEndTime2, bestp1, bestp2;
    
    loadSize1 = sroute1.getLoadSize();
    loadSize2 = sroute2.getLoadSize();

    //loadSize1 - 1 to be able to exchange blocks of one request (different from relocate 2x)
    for (int i = 1; i < loadSize1 - 1; i++){
        int currLoad, request;
        currLoad = sroute1.getReqLoad(i);
        request = sroute1.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                zeroPos1.push_back(i);
            }
        }
        else if (currLoad == 1){ 
            if (i == loadSize1 - 3){ //remove possibility of parcel pickup and delivery pair as block
                int nextreq = sroute1.getReq(loadSize1 - 2);
                if (nodeVec[nextreq].load < 0){
                    continue;
                }
            }            
            else if (nodeVec[request].load > 0 ){
                zeroPos1.push_back(i);
            }
        }
    }

    for (int i = 1; i < loadSize2 - 1; i++){
        int currLoad, request;
        currLoad = sroute2.getReqLoad(i);
        request = sroute2.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                zeroPos2.push_back(i);
            }
        }
        else if (currLoad == 1){
            if (i == loadSize2 - 3){//remove possibility of parcel pickup and delivery pair as block
                int nextreq = sroute2.getReq(loadSize2 - 2);
                if (nodeVec[nextreq].load < 0){
                    continue;
                }
            }
            else if (nodeVec[request].load > 0 ){
                zeroPos2.push_back(i);
            }
        }
    }

    // cout << "Zero Pos vec (1): " << endl;

    // for (int j = 0; j < zeroPos1.size(); j++){
    //     cout << zeroPos1[j] << " ";
    // }
    // cout << endl;
    // // getchar();

    // cout << "Zero Pos vec (2): " << endl;

    // for (int j = 0; j < zeroPos2.size(); j++){
    //     cout << zeroPos2[j] << " ";
    // }
    // cout << endl;
    // // getchar();

    for (int i = 0; i < zeroPos1.size(); i++){
        pos1 = zeroPos1[i];
        currBlock1.clearBlock();
        //making zero sum vec (new block) from r1

        currBlock1.makeBlock(nodes1, pos1, loadSize1 - 1);
        currBlock1.calcBlockTimes(inst, nodeVec, Mdist);

        int iniPos1 = currBlock1.getiniPos();
        int endPos1 = currBlock1.getendPos();

        // cout << "r1: inipos: " << iniPos1 << " - endPos: " << endPos1 << endl;
        // // getchar();
        //determining the new end time for the requests that are not part of the 2-opt block.
        if (currBlock1.getiniPos() > 1){
            newEndTime1 = sroute1.blockrmvTime(inst, nodeVec, Mdist, iniPos1);
        }
        else{
            newEndTime1 = 0;
        }

        // cout << "New End time: " << newEndTime1 << endl;
        // // getchar();

        for (int j = 0; j < zeroPos2.size(); j++){
            pos2 = zeroPos2[j];
            currBlock2.clearBlock();
           
            //making zero sum vec (new block) from r2
            currBlock2.makeBlock(nodes2, pos2, loadSize2 - 1);
            currBlock2.calcBlockTimes(inst, nodeVec, Mdist);

            int iniPos2 = currBlock2.getiniPos();
            int endPos2 = currBlock2.getendPos();

            // cout << "r2: inipos: " << iniPos2 << " - endPos: " << endPos2 << endl;
            // // getchar();

            if (currBlock2.getiniPos() > 1){
                newEndTime2 = sroute2.blockrmvTime(inst, nodeVec, Mdist, iniPos2);
            }
            else{
                newEndTime2 = 0;
            }

            // cout << "New End time (BLOCK 2): " << newEndTime2 << endl;

            // cout << "Block 1: " << endl;
            // for (int k = 0; k < currBlock1.getBlockSize(); k++){
            //     cout << currBlock1.getBlockReq(k) << " ";
            // }
            // cout << endl;

            // cout << "Block 2: " << endl;
            // for (int k = 0; k < currBlock2.getBlockSize(); k++){
            //     cout << currBlock2.getBlockReq(k) << " ";
            // }
            // cout << endl;
            
            // cout << "Times for block 1: " << currBlock1.getStart() << " - " << currBlock1.getEnd() << endl;
            // // getchar();

            // cout << "Times for block 2: " << currBlock2.getStart() << " - " << currBlock2.getEnd() << endl;
            // // getchar();

            // newEndTime2 = 0;

            //testing piece of route 2 into route 1
            // cout << "Testing feasibility r1" << endl;
            feasible = sroute1.testBlockIns(inst, nodeVec, 
                                            Mdist, newEndTime1, iniPos1, 
                                            endPos1, currBlock2);

            // cout << "Feasible insertion r1: " << feasible << endl;
            // // getchar();


            if (feasible){
                //testing piece of route 1 into route 2
                feasible = 0;
                // cout << "Testing feasibility r2" << endl;
                feasible = sroute2.testBlockIns(inst, nodeVec, 
                                                Mdist, newEndTime2, iniPos2,
                                                endPos2, currBlock1);

                // cout << "Feasible insertion r2: " << feasible << endl;
                // // getchar();
            }
            else{
                continue;
            }

            if (feasible){
                // cout << "Profits (before)" << endl;
                currBlock1.blockProfit(inst, nodeVec, Mdist);
                currBlock2.blockProfit(inst, nodeVec, Mdist);

                double p1, p2;

                p1 = currBlock1.profit();
                p2 = currBlock2.profit();

                // cout << "Profits: " << p1 << " - " << p2 << endl;

                interpos1.first = sroute1.getReq(iniPos1 - 1);
                interpos1.second = sroute1.getReq(endPos1);

                interpos2.first = sroute2.getReq(iniPos2 - 1);
                interpos2.second = sroute2.getReq(endPos2);

                //delta is the whole new cost, not only the difference

                // cout << "******************************************" << endl;
                // cout << "Calculating deltas: " << endl;
                // cout << "Delta First: " << endl;
                // cout << "cost of route: " << sroute1.cost() << endl;
                // cout << "profit block1: " << p1 << endl;
                // cout << "profit block2: " << p2 << endl;
                // cout << "removed arcs: " << Mdist[interpos1.first][currBlock1.getBlockReq(0)]  << " " << Mdist[currBlock1.getLastReq()][interpos1.second] << endl;
                // cout << "added arcs: " << Mdist[interpos1.first][currBlock2.getBlockReq(0)] << " " << Mdist[currBlock2.getLastReq()][interpos1.second] << endl;
                // cout << "******************************************" << endl;
                // cout << "******************************************" << endl;
                // cout << "Calculating deltas: " << endl;
                // cout << "Delta Second: " << endl;
                // cout << "cost of route: " << sroute2.cost() << endl;
                // cout << "profit block1: " << p1 << endl;
                // cout << "profit block2: " << p2 << endl;
                // cout << "removed arcs: " << Mdist[interpos2.first][currBlock2.getBlockReq(0)]  << " " << Mdist[currBlock2.getLastReq()][interpos2.second] << endl;
                // cout << "added arcs: " << Mdist[interpos2.first][currBlock1.getBlockReq(0)] << " " << Mdist[currBlock1.getLastReq()][interpos2.second] << endl;
                // cout << "******************************************" << endl;

                // // getchar();

                delta.first = sroute1.cost() - p1 + p2 - (inst->costkm*(Mdist[interpos1.first][currBlock2.getBlockReq(0)] 
                            - Mdist[currBlock1.getLastReq()][interpos1.second]
                            - Mdist[interpos1.first][currBlock1.getBlockReq(0)] 
                            + Mdist[currBlock2.getLastReq()][interpos1.second]));

                delta.second = sroute2.cost() - p2 + p1 - (inst->costkm*(Mdist[interpos2.first][currBlock1.getBlockReq(0)] 
                            - Mdist[currBlock2.getLastReq()][interpos2.second]
                            - Mdist[interpos2.first][currBlock2.getBlockReq(0)] 
                            + Mdist[currBlock1.getLastReq()][interpos2.second]));


                // cout << "Deltas: " << delta.first << " " << delta.second << endl;

                if ((delta.first + delta.second) > (bestDelta.first + bestDelta.second)){
                    bestDelta.first = delta.first;
                    bestDelta.second =  delta.second;

                    currPairPos.first = iniPos1;
                    currPairPos.second = iniPos2;

                    // cout << "Current pair positions: " <<  currPairPos.first << " " << currPairPos.second << endl;
                    // // getchar();
                    // currBlock1.clearBlock();
                    // currBlock2.clearBlock();

                    reqBlock1 = currBlock1;
                    reqBlock2 = currBlock2;

                    improve = 1;

                    vector<int> movingVec1;
                    vector<int> movingVec2;

                    movingVec1 = reqBlock1.getBlock();
                    movingVec2 = reqBlock2.getBlock();

                    // cout << "IN SOLUTIONS" << endl;
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

                }
            }
        }
    }

    if (!improve){
        bestDelta.first = -999999;
        bestDelta.second = -999999;
    }

    return bestDelta;
}

pair <double, double> sarpSolution::exchangeBlocks(instanceStat *inst, double **Mdist,
                    vector<nodeStat> &nodeVec, int rid1, int rid2, 
                    pair <int, int> &currPairPos, probStat* problem,
                    sarpBlock &reqBlock1, sarpBlock &reqBlock2){

    pair<double, double> delta;

    delta.first = 0;
    delta.second = 0;

	pair<double, double> bestDelta;

	double newCost = 0;
    
    pair<int, int> interpos1, interpos2;

    sarpRoute sroute1(inst, 0);
    sarpRoute sroute2(inst, 0);
	
    sroute1 = getRoute(rid1);
    sroute2 = getRoute(rid2);

    bestDelta.first = sroute1.cost();
    bestDelta.second = sroute2.cost();

    int pos1 = 0;
    int pos11 = 0;
	int pos2 = 0;
    int pos22 = 0;
    
    int jstart, jend;

    vector<int> iniBlocks1, endBlocks1, iniBlocks2, endBlocks2, nodes1, nodes2;

    nodes1 = sroute1.getNodes();
    nodes2 = sroute2.getNodes();
    
    sarpBlock currBlock1;
    sarpBlock currBlock2;  

    bool feasible, improve;

    improve = 0;
    // cout << "Current cost: " << cost_ << endl;
    // // getchar();
    int loadSize1, loadSize2;
    double newEndTime1, newEndTime2, bestp1, bestp2;
    
    loadSize1 = sroute1.getLoadSize();
    loadSize2 = sroute2.getLoadSize();

    //-1 exchange last request (if passenger) with other blocks
    for (int i = 1; i < loadSize1 - 1; i++){

        int currLoad, request;
        currLoad = sroute1.getReqLoad(i);//load of position
        request = sroute1.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                if (iniBlocks1.size() > 0){
                    endBlocks1.push_back(i);
                }
                iniBlocks1.push_back(i); 
            }
        }
        else if (currLoad == 1){
            if (nodeVec[request].load > 0 ){
                if (iniBlocks1.size() > 0){
                    endBlocks1.push_back(i);
                }
                iniBlocks1.push_back(i);
            }
        }
    }
    endBlocks1.push_back(loadSize1 - 1);

    for (int i = 1; i < loadSize2 - 1; i++){
        int currLoad, request;
        currLoad = sroute2.getReqLoad(i);
        request = sroute2.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                if (iniBlocks2.size() > 0){
                    endBlocks2.push_back(i);
                }
                iniBlocks2.push_back(i);
            }
        }
        else if (currLoad == 1){
            if (nodeVec[request].load > 0 ){
                if (iniBlocks2.size() > 0){
                    endBlocks2.push_back(i);
                }                
                iniBlocks2.push_back(i);
            }
        }
    }
    endBlocks2.push_back(loadSize2 - 1);

    
    for (int i = 0; i < iniBlocks1.size(); i++){
        pos1 = iniBlocks1[i];

        for (int k = 0; k < endBlocks1.size(); k++){
            pos11 = endBlocks1[k];

            if (pos1 < pos11 && pos11-pos1 > 1){

                currBlock1.clearBlock();

                currBlock1.makeBlock(nodes1, pos1, pos11);
                currBlock1.calcBlockTimes(inst, nodeVec, Mdist); 

                int iniPos1 = currBlock1.getiniPos();
                int endPos1 = currBlock1.getendPos();     

                // cout << "r1: inipos: " << iniPos1 << " - endPos: " << endPos1 << endl;     

                //determining the new end time for the requests that are not part of the 2-opt block.
                if (currBlock1.getiniPos() > 1){
                    newEndTime1 = sroute1.blockrmvTime(inst, nodeVec, Mdist, iniPos1);
                }
                else{
                    newEndTime1 = 0;
                }

                // cout << "+++++++++++++++" << "\nBlock 1: " << endl;
                // for (int a = 0; a < currBlock1.getBlockSize(); a++){
                //     cout << currBlock1.getBlockReq(a) << " ";
                // }
                // cout << endl;
                for (int j = 0; j < iniBlocks2.size(); j++){
                    pos2 = iniBlocks2[j];
                    for (int l = 0; l < endBlocks2.size(); l++){
                        pos22 = endBlocks2[l];

                        if (pos2 < pos22 && pos22-pos2 > 1){
                            currBlock2.clearBlock();

                            currBlock2.makeBlock(nodes2, pos2, pos22);
                            currBlock2.calcBlockTimes(inst, nodeVec, Mdist);

                            int iniPos2 = currBlock2.getiniPos();
                            int endPos2 = currBlock2.getendPos();
                            // cout << "r2: inipos: " << iniPos2 << " - endPos: " << endPos2 << endl;

                            if (currBlock2.getiniPos() > 1){
                                newEndTime2 = sroute2.blockrmvTime(inst, nodeVec, Mdist, iniPos2);
                            }
                            else{
                                newEndTime2 = 0;
                            }

                            // cout << "\n\nBlock 2: " << endl;
                            // for (int breq = 0; breq < currBlock2.getBlockSize(); breq++){
                            //     cout << currBlock2.getBlockReq(breq) << " ";
                            // }
                            // cout << endl;
                            

                            feasible = sroute1.testBlockIns(inst, nodeVec, 
                                                    Mdist, newEndTime1, iniPos1, 
                                                    endPos1, currBlock2);

                            // cout << "Feasible insertion r1: " << feasible << endl;
                            // // getchar();


                            if (feasible){
                                //testing piece of route 1 into route 2
                                feasible = 0;
                                // cout << "Testing feasibility r2" << endl;
                                feasible = sroute2.testBlockIns(inst, nodeVec, 
                                                                Mdist, newEndTime2, iniPos2,
                                                                endPos2, currBlock1);

                                // cout << "Feasible insertion r2: " << feasible << endl;
                                // // getchar();
                            }
                            else{
                                continue;
                            }

                            if (feasible){
                                // cout << "Profits (before)" << endl;
                                currBlock1.blockProfit(inst, nodeVec, Mdist);
                                currBlock2.blockProfit(inst, nodeVec, Mdist);

                                double p1, p2;

                                p1 = currBlock1.profit();
                                p2 = currBlock2.profit();

                                // cout << "Profits: " << p1 << " - " << p2 << endl;

                                interpos1.first = sroute1.getReq(iniPos1 - 1);
                                interpos1.second = sroute1.getReq(endPos1);

                                interpos2.first = sroute2.getReq(iniPos2 - 1);
                                interpos2.second = sroute2.getReq(endPos2);

                                //delta is the whole new cost, not only the difference

                                // cout << "******************************************" << endl;
                                // cout << "Calculating deltas: " << endl;
                                // cout << "Delta First: " << endl;
                                // cout << "cost of route: " << sroute1.cost() << endl;
                                // cout << "profit block1: " << p1 << endl;
                                // cout << "profit block2: " << p2 << endl;
                                // cout << "removed arcs: " << Mdist[interpos1.first][currBlock1.getBlockReq(0)]  << " " << Mdist[currBlock1.getLastReq()][interpos1.second] << endl;
                                // cout << "added arcs: " << Mdist[interpos1.first][currBlock2.getBlockReq(0)] << " " << Mdist[currBlock2.getLastReq()][interpos1.second] << endl;
                                // cout << "******************************************" << endl;
                                // cout << "******************************************" << endl;
                                // cout << "Calculating deltas: " << endl;
                                // cout << "Delta Second: " << endl;
                                // cout << "cost of route: " << sroute2.cost() << endl;
                                // cout << "profit block1: " << p1 << endl;
                                // cout << "profit block2: " << p2 << endl;
                                // cout << "removed arcs: " << Mdist[interpos2.first][currBlock2.getBlockReq(0)]  << " " << Mdist[currBlock2.getLastReq()][interpos2.second] << endl;
                                // cout << "added arcs: " << Mdist[interpos2.first][currBlock1.getBlockReq(0)] << " " << Mdist[currBlock1.getLastReq()][interpos2.second] << endl;
                                // cout << "******************************************" << endl;

                                // // getchar();

                                delta.first = sroute1.cost() - p1 + p2 - (inst->costkm*(Mdist[interpos1.first][currBlock2.getBlockReq(0)] 
                                            - Mdist[currBlock1.getLastReq()][interpos1.second]
                                            - Mdist[interpos1.first][currBlock1.getBlockReq(0)] 
                                            + Mdist[currBlock2.getLastReq()][interpos1.second]));

                                delta.second = sroute2.cost() - p2 + p1 - (inst->costkm*(Mdist[interpos2.first][currBlock1.getBlockReq(0)] 
                                            - Mdist[currBlock2.getLastReq()][interpos2.second]
                                            - Mdist[interpos2.first][currBlock2.getBlockReq(0)] 
                                            + Mdist[currBlock1.getLastReq()][interpos2.second]));


                                // cout << "Deltas: " << delta.first << " " << delta.second << endl;

                                if ((delta.first + delta.second) > (bestDelta.first + bestDelta.second)){
                                    bestDelta.first = delta.first;
                                    bestDelta.second =  delta.second;

                                    currPairPos.first = iniPos1;
                                    currPairPos.second = iniPos2;

                                    // cout << "Current pair positions: " <<  currPairPos.first << " " << currPairPos.second << endl;
                                    // // getchar();
                                    // currBlock1.clearBlock();
                                    // currBlock2.clearBlock();

                                    reqBlock1 = currBlock1;
                                    reqBlock2 = currBlock2;

                                    improve = 1;

                                    vector<int> movingVec1;
                                    vector<int> movingVec2;

                                    movingVec1 = reqBlock1.getBlock();
                                    movingVec2 = reqBlock2.getBlock();

                                    // cout << "IN SOLUTIONS" << endl;
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

                                }
                            }
                        }
                    }          
                }
            }
        }

    }
    if (!improve){
        bestDelta.first = -999999;
        bestDelta.second = -999999;
    }

    return bestDelta;
}

pair <double, double> sarpSolution::relocateBlock(instanceStat *inst, double **Mdist, 
                    vector<nodeStat> &nodeVec, 
                    int rid1, int rid2, int &insPos,
                    probStat* problem, sarpBlock &reqBlock){

    pair<double, double> delta;

    delta.first = 0;
    delta.second = 0;

	pair<double, double> bestDelta;

	double newCost = 0;
    
    pair<int, int> interpos1;

    sarpRoute sroute1(inst, 0);

    sarpRoute sroute2(inst, 0);
    	
    sroute1 = getRoute(rid1);
    sroute2 = getRoute(rid2);

    bestDelta.first = sroute1.cost();
    bestDelta.second = sroute2.cost();

    int pos1 = 0;
    int pos11 = 0;
	int pos2 = 0;

    int jstart, jend;

    vector<int> iniBlocks1, endBlocks1, nodes1;

    nodes1 = sroute1.getNodes();
    
    sarpBlock currBlock;

    bool feasible, improve;

    improve = 0;
    // cout << "Current cost: " << cost_ << endl;
    // // getchar();
    int loadSize1, routesize2;
    double newEndTime1, newEndTime2, bestp1;
    
    loadSize1 = sroute1.getLoadSize();
    routesize2 = sroute2.getNodesSize();

    //-2 because relocate block with a single request is already included into 'relocate inter'
    for (int i = 1; i < loadSize1 - 2; i++){
        int currLoad, request;
        currLoad = sroute1.getReqLoad(i);
        request = sroute1.getReq(i);
        if (currLoad == 0){
            if (nodeVec[request].load == 0){
                iniBlocks1.push_back(i);
            }
        }
        else if (currLoad == 1){
            if (nodeVec[request].load > 0 ){
                iniBlocks1.push_back(i);
            }
        }
    }
    //building ending positions for blocks
    for (int j = 1; j < iniBlocks1.size();j++){
        endBlocks1.push_back(iniBlocks1[j]);
    }

    if (nodeVec[loadSize1 - 2].load == 0){
        endBlocks1.push_back(loadSize1 - 2);
    }

    endBlocks1.push_back(loadSize1 - 1);

    // cout << "Initial positions for blocks (1): " << endl;

    // for (int j = 0; j < iniBlocks1.size(); j++){
    //     cout << iniBlocks1[j] << " ";
    // }
    // cout << endl;

    // cout << "End positions for blocks (1): " << endl;

    // for (int j = 0; j < endBlocks1.size(); j++){
    //     cout << endBlocks1[j] << " ";
    // }
    // cout << endl;

    // // getchar();

    for (int i = 0; i < iniBlocks1.size(); i++){
        pos1 = iniBlocks1[i];

        for (int k = 0; k < endBlocks1.size(); k++){
            pos11 = endBlocks1[k];

            int req1, req11;
            req1 = sroute1.getReq(pos1);
            req11 = sroute1.getReq(pos11 - 1);//the endpos of the block is the next from the last element in the block

            if (pos1 < pos11 && pos11-pos1 > 1 && req11 != req1 + inst->m){
                currBlock.clearBlock();
                //making zero sum vec (new block) from r1

                currBlock.makeBlock(nodes1, pos1, pos11);
                currBlock.calcBlockTimes(inst, nodeVec, Mdist);

                int iniPos1 = currBlock.getiniPos();
                int endPos1 = currBlock.getendPos();

                // cout << "\n\nr1: inipos: " << iniPos1 << " - endPos: " << endPos1 << endl;
                // // getchar();
                //determining the new end time for the requests that are not part of the 2-opt block.
                if (currBlock.getiniPos() > 1){
                    newEndTime1 = sroute1.blockrmvTime(inst, nodeVec, Mdist, iniPos1);
                }
                else{
                    newEndTime1 = 0;
                }

                // cout << "+++++++++++++++" << "\nBlock 1: " << endl;
                // for (int k = 0; k < currBlock.getBlockSize(); k++){
                //     cout << currBlock.getBlockReq(k) << " ";
                // }
                // cout << endl;
                // cout << "New End time: " << newEndTime1 << endl;
                // // getchar();

                for (int j = 1; j < routesize2; j++){
                    newEndTime2 = sroute2.blockrmvTime(inst, nodeVec, Mdist, j);
                    // cout << "New time of route 2: " << newEndTime2 << endl;
                    // cout << "current trying insertion position " << j << endl;
                    // // getchar();


                    
                    // cout << "Times for block 1: " << currBlock1.getStart() << " - " << currBlock1.getEnd() << endl;
                    // // getchar();

                    // cout << "Times for block 2: " << currBlock2.getStart() << " - " << currBlock2.getEnd() << endl;
                    // // getchar();

                    // newEndTime2 = 0;

                    //testing piece of route 1 into route 2
                    feasible = 0;

                    double totalBlock, totalRoute;
                    bool feasible, withpass;

                    totalBlock = currBlock.getEnd() - currBlock.getStart();
                    totalRoute = sroute2.endTime() - sroute2.startTime();
                    // cout << "Testing feasibility r2" << endl;
                    if (totalRoute + totalBlock > inst->maxTime){
                        feasible = 0;
                    }
                    else{
                        feasible = sroute2.testBlockIns(inst, nodeVec, 
                                Mdist, newEndTime2, j,
                                j, currBlock);
                    }
                    
                    // cout << "Feasible insertion r2: " << feasible << endl;
                    // // getchar();

                    if (feasible){
                        // cout << "Profits (before)" << endl;
                        currBlock.blockProfit(inst, nodeVec, Mdist);
                        
                        double p1;

                        p1 = currBlock.profit();                

                        // cout << "Profits: " << p1 << " - " << p2 << endl;

                        interpos1.first = sroute1.getReq(iniPos1 - 1);
                        interpos1.second = sroute1.getReq(endPos1);

                        //delta is the whole new cost, not only the difference

                        // cout << "******************************************" << endl;
                        // cout << "Calculating deltas: " << endl;
                        // cout << " Delta First: " << endl;
                        // cout << " cost of route: " << sroute1.cost() << endl;
                        // cout << " profit block1: " << p1 << endl;
                        // cout << "  a) removed arcs in r1: " << interpos1.first << "-" << currBlock.getBlockReq(0) << " // " << currBlock.getLastReq() << "-" << interpos1.second << endl;
                        // cout << "  b) Value of removed arcs in r1: " << Mdist[interpos1.first][currBlock.getBlockReq(0)]  << " " << Mdist[currBlock.getLastReq()][interpos1.second] << endl;
                        // cout << "  c) added arcs in r1: " << interpos1.first << " - " << interpos1.second << ": " << Mdist[interpos1.first][interpos1.second]  << endl;
                        // cout << "  d) removed arcs in r2: " << sroute2.getReq(j-1) << " - " << sroute2.getReq(j) << ": " << Mdist[sroute2.getReq(j-1)][sroute2.getReq(j)] << endl;
                        // cout << "  e) added arcs in r2: " << sroute2.getReq(j-1) << "-" << currBlock.getBlockReq(0) << " // " << currBlock.getLastReq() << "-" << sroute2.getReq(j) << endl;
                        // cout << "  f) Value of added arcs in r2: " << Mdist[sroute2.getReq(j-1)][currBlock.getBlockReq(0)] << " " << Mdist[currBlock.getLastReq()][sroute2.getReq(j)] << endl;
                        // cout << "******************************************" << endl;

                        // // getchar();

                        delta.first = sroute1.cost() - p1 - (inst->costkm*(Mdist[interpos1.first][interpos1.second] 
                                    - Mdist[interpos1.first][currBlock.getBlockReq(0)]
                                    - Mdist[currBlock.getLastReq()][interpos1.second]));

                        delta.second = sroute2.cost() + p1 - (inst->costkm*(-Mdist[sroute2.getReq(j-1)][sroute2.getReq(j)] 
                                    + Mdist[sroute2.getReq(j-1)][currBlock.getBlockReq(0)]
                                    + Mdist[currBlock.getLastReq()][sroute2.getReq(j)]));


                        // cout << "\nDeltas: " << delta.first << " " << delta.second << endl;

                        if ((delta.first + delta.second) > (bestDelta.first + bestDelta.second)){
                            bestDelta.first = delta.first;
                            bestDelta.second =  delta.second;

                            insPos = j;

                            // cout << "Current best insertion position: " <<  insPos << endl;
                            // // getchar();
                            // currBlock1.clearBlock();
                            // currBlock2.clearBlock();

                            reqBlock = currBlock;

                            improve = 1;

                            vector<int> movingVec;

                            movingVec = reqBlock.getBlock();

                            // cout << "IN SOLUTIONS" << endl;
                            // cout << "Relocating block: " << endl;
                            // for (int i = 0; i < movingVec.size(); i++){
                            //     cout << movingVec[i] << " ";
                            // }
                            // cout << endl;

                        }
                    }
                }
            }
        }
    }

    if (!improve){
        bestDelta.first = -999999;
        bestDelta.second = -999999;
    }

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
        if (best_cost < 0){
            if (problem->dParcel > 0){
                if (this->usedK < this->maxK){
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

            // // getchar();

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
