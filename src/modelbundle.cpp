#include "modelbundle.h"
#include <cstdlib>
#include <stdio.h>

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<int> &clusters, vector< vector<int> > &clusterVec, vector< vector<bParcelStruct> > &clsParcel, probStat* problem){
//*******
//For 1A:
//*******    
    if (problem->scen == "1A"){
        int counter = 0;
        for (int i = 0; i < inst->n; i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            for (int j = 0; j < clsParcel[i].size(); j++){
                bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                bStat->bundle.push_back(i);
                bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);
                bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j].parcelreq + inst->m].delta);
                bStat->bundleVec.push_back(bStat->bundle);
                // clusters.push_back(bStat->bundle);
                clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();
            }
            clusterVec.push_back(clusters);
            clusters.clear();
        }

        for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            
            clusterVec.push_back(clusters);
            clusters.clear();
        }
        inst->endCluster = 1;
        inst->startCluster = 1;
    }

}

void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){
    double cost;
    double service;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        cost = 0;
        service = 0;
        if (bStat->bundleVec[i].size() <= 1){
            cost = nodeVec[bStat->bundleVec[i][0]].profit;
            bStat->bundleProfVec.push_back(cost);
            service = nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleServVec.push_back(service);
        }
        else{
            if (bStat->bundleVec[i].back() == nodeVec.size() - 1){
                cost = nodeVec[bStat->bundleVec[i][0]].profit;
                bStat->bundleProfVec.push_back(cost);
                service = nodeVec[bStat->bundleVec[i][0]].delta;
                bStat->bundleServVec.push_back(service); 
            }
            else{
                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    if (bStat->bundleVec[i][j] > inst->n - 1 && bStat->bundleVec[i][j] < inst->n + inst->m){ //parcel pickup
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
                        cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                        service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                        continue;
                    }
                    else if (bStat->bundleVec[i][j] < inst->n){//passenger request
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
                        cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                        service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                        continue;
                    } 
                    else if (bStat->bundleVec[i][j] > inst->n + inst->m - 1){//parcel delivery
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
                        cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                        service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                        continue;
                    }           
                }
                cost += nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].profit;
                service += nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].delta;
                // cout << bStat->bundleVec[i][bStat->bundleVec[i].size() - 1] << " - delta: " << nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].delta;
                // getchar();
                // cout << "bundle: " << i << " - service(final): " << service << endl;
                // getchar(); 
                bStat->bundleProfVec.push_back(cost);
                bStat->bundleServVec.push_back(service);                
            }
        }
    } 
}

void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, int p, probStat* problem){
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;
    int currentCluster = 0;
    int ref;

    if (problem->scen == "1A"){
        if (p < 0){
            ref = inst->m;
        }
        else{
            ref = p;
        }

        for(int i = 0; i < bStat->bundleVec.size() - 1; i++){
            if (i > currentCluster*(ref + 1) + ref){
                currentCluster++;
            }
            if(i < setN){
                for (int j = 0; j < setN; j++){
                    if (i != j){
                        if (j > currentCluster*(ref + 1) + ref || j < currentCluster*(ref + 1)){
                            if (bStat->bundleEnd[j] <= inst->T){
                                if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                    bStat->bArcs[i][j] = true;
                                }                                
                            }
                        }
                    } 
                }
                bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
            }
            else if (i >= setN){
                for (int j = 0; j < setN; j++){
                    if (bStat->bundleEnd[j] <= inst->T){
                        if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                            bStat->bArcs[i][j] = true;
                        }
                    }
                }
            }
        }

        //remove arcs to bundles with same parcel requests
        for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
            for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
                for (int k = 0; k < bStat->parcelBundleVec[i].size(); k++){
                    if (bStat->parcelBundleVec[i][j] != bStat->parcelBundleVec[i][k]){
                        bStat->bArcs[bStat->parcelBundleVec[i][j]][bStat->parcelBundleVec[i][k]] = false;
                    }
                }
            }
        }

        //remove arcs from/to bundles with negative start times
        for (int i = 0; i < bStat->bundleStart.size() - 1; i++){
            if (bStat->bundleStart[i] < 0){
                for (int j = 0; j < bStat->bundleVec.size() - 1; j++){
                    bStat->bArcs[j][i] = false;
                    bStat->bArcs[i][j] = false;
                }
            }
        }

        for (int i = 0; i < bStat->bundleVec.size(); i++){
            for (int j = 0; j < bStat->bundleVec.size(); j++){
                if (bStat->bArcs[i][j]){
                    bStat->bFArc.first = i;
                    bStat->bFArc.second = j;
                    bStat->bArcMinus[j].push_back(bStat->bFArc);
                    bStat->bArcPlus[i].push_back(bStat->bFArc); 
                }
            }
        }  
    }


}

void feasibleClusterArcs (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector< vector<int> > &clusterVec, pair<int, int> &cFArc, vector< vector<bool> > &cArcs, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, int p, probStat* problem){
    
    int reqClusters = clusterVec.size() - inst->K - 1;
    int clusterA = 0;
    int clusterB;
    int setN;
    int setS;
    // int setf;
    int ref;
    if (p < 0){
        if(problem->scen == "1A"){
            ref = inst->m;
            setN = bStat->bundleVec.size() - inst->K - 1;
        }
    }

    else{
        ref = p;
    }

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        if (i < setN){//i
            if (i > clusterA*(ref + 1) + ref){
                clusterA++;
            }           
        }
        else{
            if(problem->scen == "1A"){
                clusterA++;
            }
        }

        clusterB = 0;
        for (int j = 0; j < bStat->bundleVec.size(); j++){
            if (j < setN){
                if (j > clusterB*(ref + 1) + ref){
                    clusterB++;
                }                
            }
            else{
                if(problem->scen == "1A"){
                    clusterB++;
                }
            }
            if (clusterA == clusterB){
                continue;
            }
            else{
                if (bStat->bArcs[i][j]){
                    // cout << i << " " << j << endl;
                    // cout << clusterA << " " << clusterB << endl;
                    // getchar();
                    cArcs[clusterA][clusterB] = true;
                    cFArc.first = i;
                    cFArc.second = j;
                    // cout << "something" << endl;
                    // getchar();
                    cArcMinus[clusterB].push_back(cFArc);
                    cArcPlus[clusterA].push_back(cFArc);
                    // cout << "other thing" << endl;
                    // getchar();
                }
            }
        }
    }
}

void makeParcelBundles(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){
    int parcelReq;
    if (problem->scen == "1A"){
        for (int i = 0; i < bStat->bundleVec.size(); i++){
            for (int j = 0; j < bStat->bundleVec[i].size(); j++){
                if (bStat->bundleVec[i][j] < inst->n){
                    break;
                }
                else if (bStat->bundleVec[i][j] > inst->n + inst->m - 1){
                    break;
                }
                else{
                    parcelReq = bStat->bundleVec[i][j];
                    bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                }
            }
        } 
    }
}

void makeStartTimes (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){

    double parcelTime;
    double bundleTime;
    double endTime;
    bool firstPassenger;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        parcelTime = 0;
        bundleTime = 0;
        endTime = 0;
        firstPassenger = false;
        if (bStat->bundleVec[i].size() > 1){
            if (bStat->bundleVec[i][0] < inst->n){//if the 1st element of the bundle is a passenger node
                firstPassenger = true;
                bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
                bStat->bundleStart.push_back(bundleTime);
                continue;
            }
            for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                
                if (bStat->bundleVec[i][j] >= inst->n){//if the element of the bundle is not a passenger request
                    parcelTime += ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed) + nodeVec[bStat->bundleVec[i][j]].delta;

                    if (bStat->bundleVec[i][j + 1] < inst->n){//if the next element is a passenger
                        bundleTime = nodeVec[bStat->bundleVec[i][j + 1]].e - parcelTime;
                        endTime += bundleTime + parcelTime;
                        firstPassenger = true;
                        // break;
                    }

                    else{
                        endTime += nodeVec[bStat->bundleVec[i][j]].delta + ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                    }
                }
                else{
                    endTime += nodeVec[bStat->bundleVec[i][j]].delta + ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                }
            }

            endTime += nodeVec[bStat->bundleVec[i].back()].delta;
            bStat->bundleEnd.push_back(endTime);

            if (firstPassenger){
                bStat->bundleStart.push_back(bundleTime);
            }
        }

        else{
            firstPassenger = true;
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
            bStat->bundleStart.push_back(bundleTime);
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e + nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleEnd.push_back(bundleTime);
            continue;
        }
    }

}

void makeBundleReference(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){

    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->lastElement.push_back(bStat->bundleVec[i][bStat->bundleVec[i].size()-1]);

    }
    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->firstElement.push_back(bStat->bundleVec[i][0]);

    }
}

bool compareCosts(const bParcelStruct &a, const bParcelStruct &b){
    return a.cost < b.cost;
}

void makeSmallerProblem(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<bParcelStruct> > &clsParcel, probStat* problem, int Q){

    if (problem->scen == "1A"){
        vector<bParcelStruct> vecOfDist;
        double singleCost;
        int counter;
        bParcelStruct bps;

        if (p > -1 && p != 0){
            for (int i = 0; i < inst->n; i++){

                for (int j = inst->n; j < inst->n + inst->m; j++){
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j;
                    vecOfDist.push_back(bps);
                }
                counter = 0;
                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){

                    vecOfDist[counter].cost += mdist[i][j];
                    counter++;
                }

                sort(vecOfDist.begin(), vecOfDist.end(), compareCosts);

                for (int j = 0; j < p; j++){
                    // clsParcel[i].push_back(vecOfDist[j].parcelreq);
                    clsParcel[i].push_back(vecOfDist[j]);
                }
                vecOfDist.clear();
            }
        }

        else if (p == 0){
            
        }

        else{
            for (int i = 0; i < inst->n; i++){
                for (int j = inst->n; j < inst->n + inst->m; j++){
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j;
                    vecOfDist.push_back(bps);
                }
                counter = 0;
                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
                    vecOfDist[counter].cost += mdist[i][j];
                    counter++;
                }
                for (int j = 0; j < inst->m; j++){
                    // clsParcel[i].push_back(vecOfDist[j].parcelreq);
                    clsParcel[i].push_back(vecOfDist[j]);
                }
                vecOfDist.clear();
            }
        }
    }
}

void nodeSolution (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int setN = bStat->bundleVec.size() - inst->K - inst->endCluster;
    int currSP;
    vector<int> orderVec;

    for (int k = 0; k < inst->K; k++){
        currSP = setN + k;
        for (int i = 0; i < sStat->solvec[k].size(); i++){
            auxPair.first = sStat->solvec[k][i].first;
            auxPair.second = sStat->solvec[k][i].second;            
            auxVec.push_back(auxPair);
        }
        // cout<< "here1";
        // getchar();
        while(!auxVec.empty()){
            // if (sStat->solOrder[k].empty()){
            if (orderVec.empty()){
                for (int i = 0; i < auxVec.size(); i++){
                    if (auxVec[i].first == currSP){
                        // sStat->solOrder[k].push_back(auxVec[i].first);
                        // sStat->solOrder[k].push_back(auxVec[i].second);
                        orderVec.push_back(auxVec[i].first);
                        orderVec.push_back(auxVec[i].second);

                        auxVec.erase(auxVec.begin()+i);
                    }
                }
            }
            else{

                for (int j = 0; j < auxVec.size(); j++){
                    // if(auxVec[j].first == sStat->solOrder[k].back()){
                    if(auxVec[j].first == orderVec.back()){
                        // sStat->solOrder[k].push_back(auxVec[j].second);
                        orderVec.push_back(auxVec[j].second);

                        auxVec.erase(auxVec.begin()+j);
                    }
                }
            }
            // cout << "innode3" << endl;
            // getchar();            
        // cout<< "auxvec size: " << auxVec.size();
        // getchar();
        }
        // cout<< "here3";
        // getchar();
        sStat->solOrder.push_back(orderVec);
        orderVec.clear();
    }

    for (int k = 0; k < inst->K; k++){
        sStat->solInNode.push_back(orderVec);
        for(int i = 0; i < sStat->solOrder[k].size(); i++){
            for (int j = 0; j < bStat->bundleVec[sStat->solOrder[k][i]].size(); j++){
                sStat->solInNode[k].push_back(bStat->bundleVec[sStat->solOrder[k][i]][j]);
            }
        }
    }


    // cout << "Values before: " << endl;

    // cout << "\ncosts: " << sStat->costs << endl;
    // cout << "\nparcel profit: " << sStat->pProfit << endl;

    // getchar();
    cout << "\nProfits: " << endl;


    for (int k = 0; k < inst->K; k++){
        for (int i = 0; i < sStat->solInNode[k].size() - 1; i++){
            if (sStat->solInNode[k][i] >= inst->n){
                sStat->pProfit += nodeVec[sStat->solInNode[k][i]].profit;
                cout << sStat->solInNode[k][i] << ": " << nodeVec[sStat->solInNode[k][i]].profit << endl;
            }
            sStat->costs += (double)inst->costkm*mdist[sStat->solInNode[k][i]][sStat->solInNode[k][i+1]];
        }
    }

    cout << "\n\nCustomer profit: " << inst->totalCustomProfit << endl;
    cout << "Parcel profit: " << sStat->pProfit << endl;
    cout << "Costs: " << sStat->costs << endl;

    cout << "\nSolution by nodes: " << endl;

    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solInNode[k].size(); i++){
            if (i < sStat->solInNode[k].size() - 1){
                cout << sStat->solInNode[k][i] << " - ";
            }
            else{
                cout << sStat->solInNode[k][i];
            }
        }
        cout << endl;
    }
    cout << endl;

    cout << "\nSolution structure: " << endl;
    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solInNode[k].size(); i++){
            if (i < sStat->solInNode[k].size() - 1){
                if (sStat->solInNode[k][i] < inst->n){
                    cout << "d" << " - ";
                }
                else if (sStat->solInNode[k][i] < inst->n + inst->m){
                    cout << "P" << " - ";
                    sStat->servedParcels++;

                }
                else if (sStat->solInNode[k][i] < inst->n + 2*inst->m){
                    cout << "D" << " - ";
                }
                else if (sStat->solInNode[k][i] < inst->n + 2*inst->m + inst->K){
                    cout << "S" << " - ";
                }                                      
            }
            else{

                cout << "f";
            }
        }
        cout << endl;
    }
    cout << endl;   
    // getchar(); 
    // getchar();
}

void mipSolStats2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    int load;
    double distPass;
    // load = 0;
    double dij;
    int currNode;
    int nextNode;

    double stop;
    double tij;

    for (int k = 0; k < inst->K; k++){
        load = 0;
        for (int i = 0; i < sStat->solInNode[k].size() - 2; i++){
            // dij = mdist[sStat->solInNode[k][i]][sStat->solInNode[k][i + 1]];
            currNode = sStat->solInNode[k][i];
            nextNode = sStat->solInNode[k][i + 1];
            dij = mdist[currNode][nextNode];            

            if(currNode < inst->n){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta;   

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                    }
                }

                else if (nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }
                }

                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else if (currNode < inst->n + inst->m){
                if (nextNode < inst->n){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tBoth += nodeVec[nextNode].delta;

                    sStat->dParcel += dij;
                    distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                    sStat->dBoth += distPass;  
                }
                else if(nextNode < inst->n + inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load++;

                    sStat->dParcel += dij;         
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else if (currNode < inst->n + 2*inst->m){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta; 

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;                           
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                                               
                    }   
                }
                else if(nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }                  
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else{
                if(nextNode < inst->n){
                    sStat->tNone += dij/inst->vmed;
                    sStat->tPass += nodeVec[nextNode].delta;
                    load = 0;

                    sStat->dNone += dij;
                    distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                    sStat->dPass += distPass;  
                }
                else if(nextNode < inst->n + inst->m){
                    sStat->tNone += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load++;

                    sStat->dNone += dij;
                }
            }
        }
    }
}

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, vector< vector<int> > &clusterVec, vector< pair<int,int> > &cArcVec, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;
	int csetN = clusterVec.size() - inst->K - 1;
	int currSP;
	int currParcel;
	int currCluster;
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, bStat->bundleVec.size());

	for (int i = 0; i < bStat->bundleVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, bStat->bundleVec.size());
		for(int j = 0; j < bStat->bundleVec.size(); ++j){
			if (bStat->bArcs[i][j] != true){
				continue; // If arc i to j is invalid
			} 

			x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles

			for(int k = 0; k < inst->K; k++){
				sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}
		}	
	}

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int i = 0; i < bStat->bArcVec.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += bStat->bundleProfVec[bStat->bArcVec[i].first] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
			//objFunction += bStat->bundleProfVec[bStat->bArcVec[i].second] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}
	int lastElOfi; //last element of starting bundle of the arc
	int firstElOfj; //first element of the ending bundle of the arc
    //compute costs in between bundles
	for (int i = 0; i < bStat->bArcVec.size(); i++){
		lastElOfi = bStat->bundleVec[bStat->bArcVec[i].first][bStat->bundleVec[bStat->bArcVec[i].first].size() - 1];
		firstElOfj = bStat->bundleVec[bStat->bArcVec[i].second][0];
        // cout << "Last: " << lastElOfi << " - First: " << firstElOfj;
        
		for (int k = 0; k < inst->K; k++){
            objFunction -= (double)(inst->costkm*mdist[lastElOfi][firstElOfj]) * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
            // cout << "Cost: " << inst->costkm*mdist[lastElOfi][firstElOfj] << endl;
            // getchar();
        }

	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster

	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < cArcPlus[i].size(); j++){
				exp += x[cArcPlus[i][j].first][cArcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 2 - Only one arc comes into the cluster

	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < cArcMinus[i].size(); j++){
				exp += x[cArcMinus[i][j].first][cArcMinus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 3 - Each vehicle leaves its starting node
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		currSP = setN + k;
		for (int i = 0; i < bStat->bArcPlus[currSP].size(); i++){
			exp += x[bStat->bArcPlus[currSP][i].first][bStat->bArcPlus[currSP][i].second][k];
		}
		sprintf (var, "Constraint3_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 4 - All vehicles end the trip at the dummy node f
	
	//Old version of constraint
	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
	// 	for (int i = 0; i < bStat->bArcMinus[bStat->bundleVec.size()-1].size(); i++){
	// 		exp += x[bStat->bArcMinus[bStat->bundleVec.size()-1][i].first][bStat->bArcMinus[bStat->bundleVec.size()-1][i].second][k];
	// 	}
	// 	sprintf (var, "Constraint4_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }	
	//new version of constraint
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int j = 0; j < clusterVec.back().size(); j++){
			for (int i = 0; i < bStat->bArcMinus[clusterVec.back()[j]].size(); i++){
				exp += x[bStat->bArcMinus[clusterVec.back()[j]][i].first][bStat->bArcMinus[clusterVec.back()[j]][i].second][k];
			}
		}
		sprintf (var, "Constraint4_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 5 - No parcel can be served more than once

	for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
			for (int l = 0; l < bStat->bArcPlus[bStat->parcelBundleVec[i][j]].size(); l++){
				for (int k = 0; k < inst->K; k++){
					exp += x[bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first][bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second][k];
				}
			}
		}
		sprintf (var, "Constraint5_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}	

	//Constraint 6 - Flow conservation between clusters(bundles)

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int j = 0; j < bStat->bArcPlus[i].size(); j++){
				// cout << bStat->bArcPlus[i][j].first << " " << bStat->bArcPlus[i][j].second << "||";

				exp1 += x[bStat->bArcPlus[i][j].first][bStat->bArcPlus[i][j].second][k];
			}

			for (int j = 0; j < bStat->bArcMinus[i].size(); j++){
				exp2 += x[bStat->bArcMinus[i][j].first][bStat->bArcMinus[i][j].second][k];
			}

			sprintf (var, "Constraint6_%d_%d", i, k);
			IloRange cons = (exp1 - exp2 == 0);
			cons.setName(var);
			model.add(cons);			
		}
	}

 //*******************************

    //Forcing constraints
    // IloExpr exp3(env);

    // exp3 = x[55][17][0];

    // sprintf (var, "Constraint7");
    // IloRange cons1 = (exp3 == 1);
    // cons1.setName(var);
    // model.add(cons1); 


	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, 8);

    bSARP.setParam(IloCplex::Param::TimeLimit, 3600);

    IloNum start;
    IloNum time;
    start = bSARP.getTime();
	bSARP.solve();
    time = bSARP.getTime() - start;

	cout << "\nSol status: " << bSARP.getStatus() << endl;
	sStat->feasible = bSARP.isPrimalFeasible();

	if(sStat->feasible){
        solStatIni(sStat);
		cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

		sStat->solprofit = bSARP.getObjValue();

        cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

		for (int k = 0; k < inst->K; k++){
	 		sStat->solvec.push_back(auxPairVec);
		}

		for (int i = 0; i < bStat->bundleVec.size(); i++){
			for(int j = 0; j < bStat->bundleVec.size(); ++j){
				for (int k = 0; k < inst->K; k++){
					if (bStat->bArcs[i][j] == true){
						if (bSARP.getValue(x[i][j][k]) > 0.5){
							auxPair.first = i;
							auxPair.second = j;
							sStat->solvec[k].push_back(auxPair);
							// cout << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
							// getchar();
						}
					}
				}
			}	
		}
		
		for (int k = 0; k < inst->K; k++){
			for (int i = 0; i < sStat->solvec[k].size(); i++){
				cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
			}
		}		
    }
    
	env.end();
}

void stillTimeBundle(instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    double timePoint = 0;
    int cBundle, nBundle;

    double bbEarlier;
    double service = 5;

    double lastTime = 17;
    double timeDiff;
    int setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;

    for (int k = 0; k < inst->K; k++){
        // cout << "\nfirst time: " << nodeVec[sStat->solInNode[k][0]].e << endl;
        // first element of bundle: bStat->bundleVec[cBundle][0];
        timePoint = 0;

        for(int i = 0; i < sStat->solOrder[k].size() - 2; i++){
            cBundle = sStat->solOrder[k][i];
            nBundle = sStat->solOrder[k][i+1];

            timePoint = bStat->bundleEnd[cBundle] + (mdist[bStat->lastElement[cBundle]][bStat->firstElement[nBundle]])/inst->vmed;            

            bbEarlier = bStat->bundleStart[nBundle];
            
            sStat->tStillG += (bbEarlier - timePoint);
            timePoint = bStat->bundleEnd[cBundle];
        }
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
        //Accounting for the difference btw the waiting time in nodes and in bundles.
        //In bundles, the delivery of the last parcel is not delayed until the last possible moment.
        
        // timeDiff = lastTime - bStat->bundleEnd[sStat->solOrder[k][sStat->solOrder[k].size() - 2]];
        // sStat->tStillG += timeDiff;
        //%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    }

    sStat->tStill = sStat->tStillG + sStat->tStillP;

}

void bundleMethod(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
    bundleStat bStat;
    vector< pair<int,int> > auxVec;
    vector< vector<bParcelStruct> > clsParcel;
    vector<int> vecOfInt;
    vector<bParcelStruct> auxbpsvec;

    vector< vector< pair<int,int> > > arcPlus;
    vector< vector< pair<int,int> > > arcMinus;
    vector< pair<int,int> > arcNN;
    
    vector< vector<int> >clusterVec;
    vector<int> clusters;

    vector<bool> cArcRow;
    pair<int, int> cFArc;
    vector< vector<bool> > cArcs;
    vector< pair<int,int> > cArcVec;
    vector< vector< pair<int,int> > > cArcPlus;
    vector< vector< pair<int,int> > > cArcMinus;

    int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
    int Q = 5;

    // readData(argc, argv, node, inst, nodeVec, &mdist, problem);

    for (int i = 0; i < inst->n; i++){
        clsParcel.push_back(auxbpsvec);
    }

    makeSmallerProblem(inst, nodeVec, mdist, p, clsParcel, problem, Q);


    makeBundles(inst, nodeVec, &bStat, clusters, clusterVec, clsParcel, problem);

    
    // cout << "\nNumber of Vehicles: " << inst->K;
    // getchar();

 //    cout << "\nBundle Vector: [";
    // for (int i = 0; i < bStat.bundleVec.size(); i++){
    //  cout << "[";
    //  for (int j = 0; j < bStat.bundleVec[i].size(); j++){
    //      cout << setw(3) << std:: right << bStat.bundleVec[i][j];
    //      if (j < bStat.bundleVec[i].size() - 1){
 //                cout << ", ";
 //            }
    //  }
    //  cout << "],";
    // }
    // cout << "]" << endl;




    if (problem->scen == "1A"){
        for (int i = 0; i < inst->m; i++){
            bStat.parcelBundleVec.push_back(bStat.parcelBundle);
        }
    }

    else if (problem->scen == "2A"){
        for (int i = 0; i < 2*inst->m; i++){
            bStat.parcelBundleVec.push_back(bStat.parcelBundle);
        }
    }

    makeParcelBundles(inst, nodeVec, &bStat, problem);

    // cout << "\nParcel Bundles: " << endl;

    // for (int i = 0 ; i < bStat.parcelBundleVec.size(); i++){
    //  cout << i + inst->n << ": ";
    //  for(int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
    //      cout << bStat.parcelBundleVec[i][j] << " ";
    //  }
    //  cout << endl;
    // }
    // cout << endl;
    // getchar();

    bundleProfit(inst, mdist, nodeVec, &bStat);

    // cout << "\nBundle Profit: [";
    // for (int i = 0; i < bStat.bundleProfVec.size(); i++){
    //  cout << setw(3) << std:: right << bStat.bundleProfVec[i];
    //  if (i < bStat.bundleProfVec.size() - 1){
    //         cout << ", ";
    //     }
    // }
    // cout << endl;

    // getchar();

    for (int i = 0; i < bStat.bundleVec.size(); i++){
        for (int j = 0; j < bStat.bundleVec.size(); j++){
            bStat.bArcRow.push_back(false);
        }
        bStat.bArcs.push_back(bStat.bArcRow);
        bStat.bArcRow.clear();
    }
    
    for (int i = 0; i < bStat.bundleVec.size(); i++){
        auxVec.push_back(bStat.bFArc);
    }
    auxVec.clear();

    for (int i = 0; i < bStat.bundleVec.size(); i++){
        bStat.bArcMinus.push_back(auxVec);
        bStat.bArcPlus.push_back(auxVec);
    }

    makeStartTimes(inst, mdist, nodeVec, &bStat, problem);

    makeBundleReference(inst, mdist, nodeVec, &bStat);


    cout << "\nBundle Service Times: ";
    for (int i = 0; i < bStat.bundleServVec.size(); i++){
     cout << i  << ": " <<   bStat.bundleServVec[i] << endl;
    }
    cout << endl;

    getchar();


    feasibleBundleArcs(inst, mdist, nodeVec, &bStat, p, problem);

    // cout<< "\nFeasible arcs between bundles:" << endl;
    // for (int i = 0; i < bStat.bundleVec.size(); i++){
    //  if (i == 0){
    //      cout << setw(3) << " ";
    //  }
    //  cout << setw(3) << std::right << i;
    // }
    // cout << endl;

    // for (int i = 0; i < bStat.bundleVec.size(); i++){
    //  cout << setw(3) << std::right << i;
    //  for (int j = 0; j < bStat.bundleVec.size(); j++){
    //      cout << setw(3) << std:: right << bStat.bArcs[i][j];
    //  }
    //  cout << endl;
    // }
    // getchar();

    cout << "Bundle beginning times: " << endl;
    for (int i = 0; i < bStat.bundleStart.size(); i++){
        cout << setw(3) << std::right << i << ": " << std:: right << bStat.bundleStart[i];
        cout << endl;
    }
    getchar();


    cout << "Bundle ending times: " << endl;
    for (int i = 0; i < bStat.bundleEnd.size(); i++){
        cout << setw(3) << std::right << i << ": " << std:: right << bStat.bundleEnd[i];
        cout << endl;
    }
    getchar();

    for (int i = 0; i < clusterVec.size(); i++){
        for (int j = 0; j < clusterVec.size(); j++){
            cArcRow.push_back(false);
        }
        cArcs.push_back(cArcRow);
    }
    
    for (int i = 0; i < clusterVec.size(); i++){
        auxVec.push_back(cFArc);
    }
    auxVec.clear();

    for (int i = 0; i < clusterVec.size(); i++){
        cArcMinus.push_back(auxVec);
        cArcPlus.push_back(auxVec);
    }

    for (int i = 0; i < clusterVec.size(); i++){
        cout << "\nCluster " << i << ": [";
        for(int k = 0; k < clusterVec[i].size(); k++){
            cout << "(" << clusterVec[i][k] << ") " << "[";
            for (int j = 0; j < bStat.bundleVec[clusterVec[i][k]].size(); j++){
                cout << setw(3) << std:: right << bStat.bundleVec[clusterVec[i][k]][j];
                if (j < bStat.bundleVec[clusterVec[i][k]].size() - 1){
                    cout << ",";
                }
                else{
                    cout << "] ";
                }
            }
            
        }
        cout << "]" << endl;
    }

    feasibleClusterArcs(inst, nodeVec, &bStat, clusterVec, cFArc, cArcs, cArcPlus, cArcMinus, p, problem);

    // cout<< "\nFeasible arcs between clusters:" << endl;
    // for (int i = 0; i < clusterVec.size(); i++){
    //  if (i == 0){
    //      cout << setw(3) << " ";
    //  }
    //  cout << setw(3) << std::right << i;
    // }
    // cout << endl;
    // for (int i = 0; i < clusterVec.size(); i++){
    //  cout << setw(3) << std::right << i;
    //  for (int j = 0; j < clusterVec.size(); j++){
    //      cout << setw(3) << std:: right << cArcs[i][j];
    //  }
    //  cout << endl;
    // }

    // // getchar();

    for (int i = 0; i < bStat.bundleVec.size(); i++){
        for (int j = 0; j < bStat.bundleVec.size(); j++){
            if(bStat.bArcs[i][j]){
                bStat.bFArc.first = i;
                bStat.bFArc.second = j;
                bStat.bArcVec.push_back(bStat.bFArc);
            }
        }
    }


    for (int i = 0; i < clusterVec.size(); i++){
        for (int j = 0; j < clusterVec.size(); j++){
            if(cArcs[i][j]){
                cFArc.first = i;
                cFArc.second = j;
                cArcVec.push_back(cFArc);
            }
        }
    }
    
    mipbundle(inst, nodeVec, mdist, &bStat, clusterVec, cArcVec, cArcPlus, cArcMinus, problem, sStat);
    
    if(sStat->feasible){
        // solStatIni(sStat);

        nodeSolution (inst, mdist, &bStat, nodeVec, sStat);
        
        stillTimeBundle(inst, mdist, &bStat, nodeVec, sStat);

        mipSolStats2 (inst, mdist, &bStat, nodeVec, sStat);

        // // cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

        printStats(inst, sStat);
    }

    for ( int i = 0; i < inst->V + inst->dummy; i++) {
        delete[] mdist[i];
    }
    delete[] mdist;
}