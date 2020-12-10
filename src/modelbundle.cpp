#include "modelbundle.h"
#include <cstdlib>
#include <stdio.h>

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, vector< vector<bParcelStruct> > &clsParcel, probStat* problem){
//*******
//For 1A:
//*******    
    // if (problem->scen == "1A"){
        int counter = 0;
        for (int i = 0; i < inst->n; i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // cStat->clusters.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            for (int j = 0; j < clsParcel[i].size(); j++){
                bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                bStat->bundle.push_back(i);
                bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);
                bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j].parcelreq + inst->m].delta);
                bStat->bundleVec.push_back(bStat->bundle);
                // cStat->clusters.push_back(bStat->bundle);
                cStat->clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();
            }
            cStat->clusterVec.push_back(cStat->clusters);
            cStat->clusters.clear();
        }

        for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // cStat->clusters.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            
            cStat->clusterVec.push_back(cStat->clusters);
            cStat->clusters.clear();
        }
    // }

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
void initVecs (instanceStat *inst, vector< vector<bParcelStruct> > &clsParcel, bundleStat *bStat){

    vector<bParcelStruct> auxbpsvec;
    vector<int> parcelBundle;

    //init bundle attributes
    for (int i = 0; i < inst->n; i++){
        clsParcel.push_back(auxbpsvec);
    }

    // if (problem->scen == "1A"){
        for (int i = 0; i < inst->m; i++){
            bStat->parcelBundleVec.push_back(parcelBundle);
        }
    // }


}
void initArcs (instanceStat *inst, bundleStat *bStat, clSt *cStat){

    vector< vector<int> > vec2d;
    vector<int> vec1d;
    vector<bool> arcRow;
    vector< pair<int,int> > auxVec;

    //init Bundle arcs

    for (int k = 0; k < inst->K; k++){
        bStat->bArcPlus.push_back(auxVec);
    }

    for(int i = 0; i < bStat->bundleVec.size(); i++){
        vec2d.push_back(vec1d);

        bStat->vArcPlus.push_back(bStat->bArcPlus);
        bStat->vArcMinus.push_back(bStat->bArcPlus);
    }

    bStat->bArcPlus.clear();

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        for (int j = 0; j < bStat->bundleVec.size(); j++){
            arcRow.push_back(false);
        }
        bStat->bArcs.push_back(arcRow);
        bStat->bArcMinus.push_back(auxVec);
        bStat->bArcPlus.push_back(auxVec);


        bStat->arcV.push_back(vec2d);

        arcRow.clear();
    }

    auxVec.clear();
    vec2d.clear();
    vec1d.clear();

    //init Cluster arcs

    for (int k = 0; k < inst->K; k++){
        cStat->cArcPlus.push_back(auxVec);
    }

    for(int i = 0; i < cStat->clusterVec.size(); i++){
        vec2d.push_back(vec1d);

        cStat->vArcPlus.push_back(cStat->cArcPlus);
        cStat->vArcMinus.push_back(cStat->cArcPlus);
    }

    cStat->cArcPlus.clear();
   
    for (int i = 0; i < cStat->clusterVec.size(); i++){
        for (int j = 0; j < cStat->clusterVec.size(); j++){
            arcRow.push_back(false);
        }
        cStat->cArcs.push_back(arcRow);
        cStat->cArcMinus.push_back(auxVec);
        cStat->cArcPlus.push_back(auxVec);

        cStat->arcV.push_back(vec2d);
        
        cStat->cArcs.push_back(arcRow);
    }

}

void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem){
    int setN = bStat->bundleVec.size() - (2*inst->K);
    int currentCluster = 0;
    int ref = inst->m;
    pair<int, int> bFArc;
    int auxK;

    int fDummy = bStat->bundleVec.size() - inst->K;

    for(int i = 0; i < fDummy; i++){
        if (i > currentCluster*(ref + 1) + ref){
            currentCluster++;
        }
        if(i < setN){//i is not depot or dummy
            for (int j = 0; j < setN; j++){//j is not depot or dummy
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

            for (int j = fDummy; j < bStat->bundleVec.size(); j++){//j is dummy node
                bStat->bArcs[i][j] = true;
                                  
            }
        }
        else if (i >= setN){//i is a starting point bundle
            for (int j = 0; j < setN; j++){//j is a request bundle
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

    for(int i = 0; i < bStat->bundleVec.size(); i++){
        for(int j = 0; j < bStat->bundleVec.size(); j++){
            if (bStat->bArcs[i][j]){
                bFArc.first = i;
                bFArc.second = j;                
                
                bStat->bArcMinus[j].push_back(bFArc);
                bStat->bArcPlus[i].push_back(bFArc);
                
                bStat->bArcVec.push_back(bFArc);

                if (i < setN){//i is a bundle of request
                    if (j < setN){ // j is a bundle of request
                        for (int k = 0; k < inst->K; k++){ // in this case, all vehicles are available
                            bStat->arcV[i][j].push_back(k);
                        }                        
                    }
                    else if (j >= fDummy){ //j is a dummy bundle
                        auxK = j - fDummy; //k is the vehicle associated with dummy bundle j
                        bStat->arcV[i][j].push_back(auxK);                         
                    }
                }
                else if (i < fDummy){//i is a starting bundle
                     if (j < setN){ //j is a request bundle
                        auxK = i - setN;
                        bStat->arcV[i][j].push_back(auxK);                         
                    }                  
                }
            }
        }
    }

    //creating the vectors for leaving incoming arcs with available vehicles
    for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;

        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
            bStat->vArcPlus[i][k].push_back(bStat->bArcVec[a]);
            bStat->vArcMinus[j][k].push_back(bStat->bArcVec[a]);
        }
    }  
}

void feasibleClusterArcs (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem){
    
    int reqClusters = cStat->clusterVec.size() - (inst->K*2);
    int clusterA = 0;
    int clusterB;
    int setN = bStat->bundleVec.size() - (inst->K*2);
    int setS;
    int cset = cStat->clusterVec.size() - (inst->K*2);
    // int setf;
    int ref = inst->m;
    pair<int, int> cFArc;
    int fDummy = bStat->bundleVec.size() - inst->K;
    int fcDummy = cStat->clusterVec.size() - inst->K;

    int auxK;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        if (i < setN){//i is a request bundle
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
            if (j < setN){//j is a request bundle
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
                    cStat->cArcs[clusterA][clusterB] = true;
                    cFArc.first = i;
                    cFArc.second = j;
                    // cout << "something" << endl;
                    // getchar();
                    cStat->cArcMinus[clusterB].push_back(cFArc);
                    cStat->cArcPlus[clusterA].push_back(cFArc);
                    // cout << "other thing" << endl;
                    // getchar();
                }
            }
        }
    }

    for (int i = 0; i < cStat->clusterVec.size(); i++){
        for (int j = 0; j < cStat->clusterVec.size(); j++){
            if(cStat->cArcs[i][j]){
                cFArc.first = i;
                cFArc.second = j;
                cStat->cArcVec.push_back(cFArc);

                if (i < cset){
                    if (j < cset){
                        for (int k = 0; k < inst->K; k++){
                            cStat->arcV[i][j].push_back(k);
                        }                        
                    }
                    else if (j >= fcDummy){
                        auxK = j - fcDummy;
                        cStat->arcV[i][j].push_back(auxK);
                    }
                }
                else if (i < fcDummy){
                    if (j < cset){
                        auxK = i - cset;
                        cStat->arcV[i][j].push_back(auxK);
                    }                    
                }
            }
        }
    }


    // for (int a = 0; a < cStat->cArcVec.size(); a++){
    //     int cA = cStat->cArcVec[a].first;
    //     int cB = cStat->cArcVec[a].second;

    //     for (int k1 = 0; k1 < cStat->arcV[cA][cB].size(); k1++){
    //         int k = cStat->arcV[i][j][k1];
    //         cStat->vArcPlus[i][k].push_back(cStat->cArcVec[a]);
    //         cStat->vArcMinus[j][k].push_back(cStat->cArcVec[a]);
    //     }
    // } 

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
    int setN = bStat->bundleVec.size() - (2*inst->K);
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

    cout << "\nNumber of Vehicles: " << inst->K << endl;

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

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bStat->bundleVec.size() - (2*inst->K);
	int csetN = cStat->clusterVec.size() - (2*inst->K);
    int fcDummy = bStat->bundleVec.size() - inst->K;
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

            for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                int k = bStat->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}
		}	
	}

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
			objFunction += bStat->bundleProfVec[i] * x[i][j][k];
			//objFunction += bStat->bundleProfVec[bStat->bArcVec[i].second] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}
    //i: last element of first bundle of the arc
    //j: first element of the second bundle of the arc
    //compute costs in between bundles
	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int u = bStat->bArcVec[a].first;
        int v = bStat->bArcVec[a].second;
		int i = bStat->lastElement[u];
		int j = bStat->firstElement[v];
        // cout << "Last: " << i << " - First: " << j;
        
        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
            objFunction -= (double)(inst->costkm*mdist[i][j]) * x[u][v][k];
            // cout << "Cost: " << inst->costkm*mdist[lastElOfi][firstElOfj] << endl;
            // getchar();
        }

	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster
    cout << "first part" << endl;
    getchar();
	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < cStat->vArcPlus[i][k].size(); a++){
                int u = cStat->vArcPlus[i][k][a].first;
                int v = cStat->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons1 = (exp == 1);
		cons1.setName(var);
		model.add(cons1);

        // for (int k = 0; k < inst->K; k++){
        //     for (int a = 0; a < cStat->vArcMinus[i][k].size(); a++){
        //         int u = cStat->vArcMinus[i][k][a].first;
        //         int v = cStat->vArcMinus[i][k][a].second;

        //         exp += x[u][v][k];
        //     }
        // }
        // sprintf (var, "Constraint2_%d", i);
        // IloRange cons = (exp == 1);
        // cons.setName(var);
        // model.add(cons);
	}

	// Constraint 2 - Only one arc comes into the cluster

	// for (int i = 0; i < csetN; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < cStat->cArcMinus[i].size(); j++){
	// 			exp += x[cStat->cArcMinus[i][j].first][cStat->cArcMinus[i][j].second][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint2_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// Constraint 3 - Each vehicle leaves its starting node
	
	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
	// 	currSP = setN + k;
	// 	for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
 //            int u = bStat->vArcPlus[currSP][k][a].first;
 //            int v = bStat->vArcPlus[currSP][k][a].second;
	// 		exp += x[u][v][k];
	// 	}
	// 	sprintf (var, "Constraint3_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// // Constraint 4 - All vehicles end the trip at the dummy node f
	
	// //new version of constraint
	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
 //        int cDummy = fcDummy + k;
	// 	// for (int j = 0; j < cStat->clusterVec[fcDummy+k].size(); j++){
	// 		for (int a = 0; a < bStat->vArcMinus[cDummy][k].size(); a++){
	// 			int u = bStat->vArcMinus[cDummy][k][a].first;
 //                int v = bStat->vArcMinus[cDummy][k][a].second;

 //                exp += x[u][v][k];
	// 		}
	// 	// }
	// 	sprintf (var, "Constraint4_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 5 - No parcel can be served more than once

	// for (int i = 0; i < bStat->parcelBundleVec.size(); i++){ //parcelBundleVec is the size of parcels
	// 	IloExpr exp(env);
	// 	currParcel = inst->n + i;
	// 	for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
 //            int h = bStat->parcelBundleVec[i][j];
	// 		for (int k = 0; k < inst->K; k++){    
 //                for (int a = 0; a < bStat->vArcPlus[h][k].size(); a++){
 //                    int u = bStat->vArcPlus[h][k][a].first;
 //                    int v = bStat->vArcPlus[h][k][a].second;

	// 				exp += x[u][v][k];
	// 			}
	// 		}
	// 	}
	// 	sprintf (var, "Constraint5_%d", currParcel);
	// 	IloRange cons = (exp <= 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }	

	// //Constraint 6 - Flow conservation between clusters(bundles)

	// for (int i = 0; i < setN; i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp1(env);
	// 		IloExpr exp2(env);

	// 		for (int a = 0; a < bStat->vArcPlus[i][k].size(); a++){
 //                int u = bStat->vArcPlus[i][k][a].first;
 //                int v = bStat->vArcPlus[i][k][a].second;
	// 			exp1 += x[u][v][k];
	// 		}

	// 		for (int a = 0; a < bStat->vArcMinus[i][k].size(); a++){
 //                int u = bStat->vArcMinus[i][k][a].first;
 //                int v = bStat->vArcMinus[i][k][a].second;       
                         
	// 			exp2 += x[u][v][k];
	// 		}

	// 		sprintf (var, "Constraint6_%d_%d", i, k);
	// 		IloRange cons = (exp1 - exp2 == 0);
	// 		cons.setName(var);
	// 		model.add(cons);			
	// 	}
	// }

 //*******************************

    //Forcing constraints
    // IloExpr exp3(env);

    // exp3 = x[55][17][0];

    // sprintf (var, "Constraint7");
    // IloRange cons1 = (exp3 == 1);
    // cons1.setName(var);
    // model.add(cons1); 

    int threads = 1;
	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, threads);

    bSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
 //    start = bSARP.getTime();
	// bSARP.solve();
 //    time = (bSARP.getTime() - start)/threads;

	// cout << "\nSol status: " << bSARP.getStatus() << endl;
	// sStat->feasible = bSARP.isPrimalFeasible();

	// if(sStat->feasible){
 //        solStatIni(sStat);
	// 	cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

	// 	sStat->solprofit = bSARP.getObjValue();

 //        cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

	// 	for (int k = 0; k < inst->K; k++){
	//  		sStat->solvec.push_back(auxPairVec);
	// 	}

	// 	for (int i = 0; i < bStat->bundleVec.size(); i++){
	// 		for(int j = 0; j < bStat->bundleVec.size(); ++j){
	// 			for (int k = 0; k < inst->K; k++){
	// 				if (bStat->bArcs[i][j] == true){
	// 					if (bSARP.getValue(x[i][j][k]) > 0.5){
	// 						auxPair.first = i;
	// 						auxPair.second = j;
	// 						sStat->solvec[k].push_back(auxPair);
	// 						// cout << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
	// 						// getchar();
	// 					}
	// 				}
	// 			}
	// 		}	
	// 	}
		
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int i = 0; i < sStat->solvec[k].size(); i++){
	// 			cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
	// 		}
	// 	}		
    // }
    
	env.end();
}

void stillTimeBundle(instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    double timePoint = 0;
    int cBundle, nBundle;

    double bbEarlier;
    double service = 5;

    double timeDiff;
    int setN = bStat->bundleVec.size() - (2*inst->K);

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
    clSt cStat;

    vector< vector<bParcelStruct> > clsParcel;

    int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
    int Q = 5;

    initVecs(inst, clsParcel, &bStat);

    makeSmallerProblem(inst, nodeVec, mdist, p, clsParcel, problem, Q);


    makeBundles(inst, nodeVec, &bStat, &cStat, clsParcel, problem);

    cout << "\nBundle Vector: [";

    for (int i = 0; i < bStat.bundleVec.size(); i++){
        cout << "(" << i << "): [";
        for (int j = 0; j < bStat.bundleVec[i].size(); j++){
            cout << setw(2) << std:: right << bStat.bundleVec[i][j];
            if (j < bStat.bundleVec[i].size() - 1){
                cout << ",";
            }
            else{
                cout << "] ";
            }
        }
        cout << endl;
    }
    getchar();

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


    makeStartTimes(inst, mdist, nodeVec, &bStat, problem);

    makeBundleReference(inst, mdist, nodeVec, &bStat);


    // cout << "\nBundle Service Times: ";
    // for (int i = 0; i < bStat.bundleServVec.size(); i++){
    //  cout << i  << ": " <<   bStat.bundleServVec[i] << endl;
    // }
    // cout << endl;

    // getchar();

    initArcs(inst, &bStat, &cStat);

    feasibleBundleArcs(inst, mdist, nodeVec, &bStat, &cStat, p, problem);

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

    // cout << "Bundle beginning times: " << endl;
    // for (int i = 0; i < bStat.bundleStart.size(); i++){
    //     cout << setw(3) << std::right << i << ": " << std:: right << bStat.bundleStart[i];
    //     cout << endl;
    // }
    // getchar();


    // cout << "Bundle ending times: " << endl;
    // for (int i = 0; i < bStat.bundleEnd.size(); i++){
    //     cout << setw(3) << std::right << i << ": " << std:: right << bStat.bundleEnd[i];
    //     cout << endl;
    // }
    // getchar();

    for (int i = 0; i < cStat.clusterVec.size(); i++){
        cout << "\nCluster " << i << ": [";
        for(int k = 0; k < cStat.clusterVec[i].size(); k++){
            cout << "(" << cStat.clusterVec[i][k] << ") " << "[";
            for (int j = 0; j < bStat.bundleVec[cStat.clusterVec[i][k]].size(); j++){
                cout << setw(3) << std:: right << bStat.bundleVec[cStat.clusterVec[i][k]][j];
                if (j < bStat.bundleVec[cStat.clusterVec[i][k]].size() - 1){
                    cout << ",";
                }
                else{
                    cout << "] ";
                }
            }
            
        }
        cout << "]" << endl;
    }

    feasibleClusterArcs(inst, nodeVec, &bStat, &cStat, p, problem);

    cout<< "\nFeasible arcs between clusters:" << endl;
    for (int i = 0; i < cStat.clusterVec.size(); i++){
     if (i == 0){
         cout << setw(3) << " ";
     }
     cout << setw(3) << std::right << i;
    }
    cout << endl;
    for (int i = 0; i < cStat.clusterVec.size(); i++){
     cout << setw(3) << std::right << i;
     for (int j = 0; j < cStat.clusterVec.size(); j++){
         cout << setw(3) << std:: right << cStat.cArcs[i][j];
     }
     cout << endl;
    }

    getchar();

    ///////////////////////////////////////
    
    mipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat);
    
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