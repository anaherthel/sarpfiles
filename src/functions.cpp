#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<int> &clusters, vector< vector<int> > &clusterVec, vector< vector<int> > &clsParcel, probStat* problem){
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
                bStat->bundle.push_back(clsParcel[i][j]);
                bStat->bundle.push_back(i);
                bStat->bundle.push_back(clsParcel[i][j]+inst->m);
                bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j]].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j]+inst->m].delta);
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
    }

//*******
//For 2A:
//******* 

    if (problem->scen == "2A"){
        int counter = 0;
        for (int i = 0; i < inst->n; i++){
            bStat->bundle.push_back(i);
            bStat->label.push_back(0);
            bStat->mainNode.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            for (int j = 0; j < clsParcel[i].size(); j++){
                if (clsParcel[i][j] > inst->n + 2*inst->m){
                    bStat->bundle.push_back(clsParcel[i][j]/inst->m);
                    bStat->bundle.push_back(i);
                    bStat->label.push_back(2);
                    bStat->mainNode.push_back(clsParcel[i][j]/inst->m);
                    bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j]].delta + nodeVec[i].delta);
                    bStat->bundleVec.push_back(bStat->bundle);
                    clusters.push_back(bStat->bundleVec.size()-1);
                    bStat->activeDL.push_back(bStat->bundleVec.size()-1);
                    bStat->bundle.clear();
                } 
                
                else if (clsParcel[i][j] < inst->n + inst->m){
                    bStat->bundle.push_back(clsParcel[i][j]);
                    bStat->bundle.push_back(i);
                    bStat->label.push_back(1);
                    bStat->mainNode.push_back(clsParcel[i][j]);
                    bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j]].delta + nodeVec[i].delta);
                    bStat->bundleVec.push_back(bStat->bundle);
                    clusters.push_back(bStat->bundleVec.size()-1);
                    bStat->activePU.push_back(bStat->bundleVec.size()-1);
                    bStat->bundle.clear();
                }

                else{
                    bStat->bundle.push_back(i);
                    bStat->bundle.push_back(clsParcel[i][j]);
                    bStat->mainNode.push_back(clsParcel[i][j]);
                    bStat->label.push_back(2);
                    bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j]].delta + nodeVec[i].delta);
                    bStat->bundleVec.push_back(bStat->bundle);
                    clusters.push_back(bStat->bundleVec.size()-1);
                    bStat->activeDL.push_back(bStat->bundleVec.size()-1);
                    bStat->bundle.clear(); 
                }               
            }
            clusterVec.push_back(clusters);
            clusters.clear();
        }

        for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            bStat->label.push_back(3);
            bStat->mainNode.push_back(i);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            
            clusterVec.push_back(clusters);
            clusters.clear();
        }
    }

//*******
//For 1B:
//*******
    // else if (problem->scen == "1B"){
    //     int counter = 0;
    //     for (int i = 0; i < inst->n; i++){
    //         bStat->bundle.push_back(i);
    //         bStat->bundleVec.push_back(bStat->bundle);
    //         // clusters.push_back(bStat->bundle);
    //         clusters.push_back(bStat->bundleVec.size()-1);
    //         bStat->bundle.clear();
    //         for (int j = 0; j < clsParcel[i].size(); j++){
    //             bStat->bundle.push_back(clsParcel[i][j]);
    //             bStat->bundle.push_back(i);
    //             bStat->bundle.push_back(clsParcel[i][j]+inst->m);
    //             bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j]].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j]+inst->m].delta);
    //             bStat->bundleVec.push_back(bStat->bundle);
    //             // clusters.push_back(bStat->bundle);
    //             clusters.push_back(bStat->bundleVec.size()-1);
    //             bStat->bundle.clear();
    //         }
    //         clusterVec.push_back(clusters);
    //         clusters.clear();
    //     }

    //     for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
    //         bStat->bundle.push_back(i);
    //         bStat->bundleVec.push_back(bStat->bundle);
    //         // clusters.push_back(bStat->bundle);
    //         clusters.push_back(bStat->bundleVec.size()-1);
    //         bStat->bundle.clear();
            
    //         clusterVec.push_back(clusters);
    //         clusters.clear();
    //     }
    // }


}

void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<double> &passProfit){
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
            for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                if (bStat->bundleVec[i][j] > inst->n - 1 && bStat->bundleVec[i][j] < inst->n + inst->m){ //parcel pickup
                    cost += inst->gamma + inst->mu*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j] + inst->m];
                    cost += - mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]];
                    service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                    continue;
                }
                else if (bStat->bundleVec[i][j] < inst->n){//passenger request
                    cost += nodeVec[bStat->bundleVec[i][j]].profit;
                    cost += - mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]];
                    service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                    continue;
                } 
                else if (bStat->bundleVec[i][j] > inst->n + inst->m - 1){//parcel delivery
                    cost += nodeVec[bStat->bundleVec[i][j]].profit;
                    cost += - mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]];
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

void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, int p, probStat* problem){
    int setN = bStat->bundleVec.size() - inst->K - 1;
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
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;
                            }
                        }
                    } 
                }
                bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
            }
            else if (i >= setN){
                for (int j = 0; j < setN; j++){
                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                        bStat->bArcs[i][j] = true;
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

    else if (problem->scen == "2A"){
        if (p < 0){
            ref = 3*inst->m;
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
                            if (bStat->label[i] == 1){
                                if (bStat->label[j] == 0){
                                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                        bStat->bArcs[i][j] = true;
                                    }                                     
                                }
                                // else if (bStat->label[j] == 1){
                                //     if(bStat->mainNode[i] != bStat->mainNode[j]){  
                                //         bStat->bArcs[i][j] = true;
                                //     } 
                                // }
                                else if (bStat->label[j] == 2){
                                    if (bStat->mainNode[j] == bStat->mainNode[i] + inst->m){
                                        if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                            bStat->bArcs[i][j] = true;
                                        }
                                    }  
                                }
                            }
                            else if (bStat->label[i] == 2){
                                if (bStat->label[j] == 0 || bStat->label[j] == 1){
                                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                        bStat->bArcs[i][j] = true;
                                    }  
                                }
                            }
                            else{
                                if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                    bStat->bArcs[i][j] = true;
                                }                                   
                            }
                        }
                    } 
                }
                if (bStat->label[i] == 0 || bStat->label[i] == 2){
                    bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
                }
            }
            else if (i >= setN){
                for (int j = 0; j < setN; j++){
                    if (bStat->label[j] == 0 || bStat->label[j] == 1){
                        if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                            bStat->bArcs[i][j] = true;
                        }                        
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
    int setN = bStat->bundleVec.size() - inst->K - 1;
    int ref;

    if (p < 0){
        if(problem->scen == "1A"){
            ref = inst->m;
        }
        else if (problem->scen == "2A"){
            ref = 3*inst->m;
        }
    }

    else{
        ref = p;
    }

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        if (i < setN){
            if (i > clusterA*(ref + 1) + ref){
                clusterA++;
            }           
        }
        else{
            clusterA++;
        }
        clusterB = 0;
        for (int j = 0; j < bStat->bundleVec.size(); j++){
            if (j < setN){
                if (j > clusterB*(ref + 1) + ref){
                    clusterB++;
                }                
            }
            else{
                clusterB++;
            }
            if (clusterA == clusterB){
                continue;
            }
            else{
                if (bStat->bArcs[i][j]){
                    cArcs[clusterA][clusterB] = true;
                    cFArc.first = i;
                    cFArc.second = j;
                    cArcMinus[clusterB].push_back(cFArc);
                    cArcPlus[clusterA].push_back(cFArc);
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

    else if (problem->scen == "2A"){
        int setN = bStat->bundleVec.size() - inst->K - 1;
        for (int i = 0; i < setN; i++){
            if (bStat->bundleVec[i].size() <= 1){
                continue;
            }
            else{
                for (int j = 0; j < bStat->bundleVec[i].size(); j++){
                    if (bStat->bundleVec[i][j] < inst->n){
                        continue;
                    }
                    else{
                        parcelReq = bStat->bundleVec[i][j];
                        bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                    }
                }                
            }   
        }         
    }
}

void makeStartTimes (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){

    double parcelTime;
    double bundleTime;
    bool firstPassenger;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        parcelTime = 0;
        bundleTime = 0;
        firstPassenger = false;
        if (bStat->bundleVec[i].size() > 1){
            if (bStat->bundleVec[i][0] < inst->n){
                firstPassenger = true;
                bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
                bStat->bundleStart.push_back(bundleTime);
                continue;
            }
            for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                if (bStat->bundleVec[i][j] >= inst->n){
                    parcelTime += ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed) + nodeVec[bStat->bundleVec[i][j]].delta;

                    if (bStat->bundleVec[i][j + 1] < inst->n){
                        bundleTime = nodeVec[bStat->bundleVec[i][j + 1]].e - parcelTime;
                        firstPassenger = true;
                        break;
                    }
                }
            }
            if (firstPassenger){
                bStat->bundleStart.push_back(bundleTime);
            }            
        }

        else{
            firstPassenger = true;
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
            bStat->bundleStart.push_back(bundleTime);
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

void makeSmallerProblem(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<int> > &clsParcel, probStat* problem, int Q){

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
                    clsParcel[i].push_back(vecOfDist[j].parcelreq);
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
                    clsParcel[i].push_back(vecOfDist[j].parcelreq);
                }
                vecOfDist.clear();
            }
        }
    }

    else if (problem->scen == "2A"){
        vector<bParcelStruct> vecOfDist;
        double singleCost;
        bParcelStruct bps;

        if (p > -1 && p != 0){
            //*********************************************
            //find p best of pickup and p best of delivery
            //*********************************************
            // for (int i = 0; i < inst->n; i++){
            //     for (int j = inst->n; j < inst->n + inst->m; j++){
            //         bps.cost = mdist[j][i];
            //         bps.parcelreq = j;
            //         vecOfDist.push_back(bps);
            //     }
            //     sort(vecOfDist.begin(), vecOfDist.end(), compareCosts);

            //     for (int j = 0; j < p; j++){
            //         clsParcel[i].push_back(vecOfDist[j].parcelreq);
            //     }
            //     vecOfDist.clear(); 

            //     for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
            //         bps.cost = mdist[i][j];
            //         bps.parcelreq = j;
            //         vecOfDist.push_back(bps);
            //         bps.cost = mdist[j][i];
            //         bps.parcelreq = j*inst->m;
            //         vecOfDist.push_back(bps);                   
            //     }
            //     sort(vecOfDist.begin(), vecOfDist.end(), compareCosts);

            //     for (int j = 0; j < p; j++){
            //         clsParcel[i].push_back(vecOfDist[j].parcelreq);
            //     }
            //     vecOfDist.clear(); 
            // }
            //*********************************************
            //find p best of pickup and delivery together
            //*********************************************
            for (int i = 0; i < inst->n; i++){
                for (int j = inst->n; j < inst->n + inst->m; j++){
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j;
                    vecOfDist.push_back(bps);
                }
                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
                    bps.cost = mdist[i][j];
                    bps.parcelreq = j;
                    vecOfDist.push_back(bps);
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j*inst->m;
                    vecOfDist.push_back(bps);                   
                }
                sort(vecOfDist.begin(), vecOfDist.end(), compareCosts);

                for (int j = 0; j < p; j++){
                    clsParcel[i].push_back(vecOfDist[j].parcelreq);
                }
                vecOfDist.clear();
            }
        }

        else if (p == 0){
            // break;
        }

        else{
            for (int i = 0; i < inst->n; i++){
                for (int j = inst->n; j < inst->n + inst->m; j++){
                    clsParcel[i].push_back(j);
                }
                vecOfDist.clear(); 

                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
                    
                    clsParcel[i].push_back(j);
                    clsParcel[i].push_back(j*inst->m);

                }
                vecOfDist.clear();
            }
        }  
    }    


    // else if (problem->scen == "1B"){

    //     vector< vector<int> > ppuSets;
    //     vector< vector<int> > pdoSets;
    //     vector<int> prep;
    //     vector<bParcelStruct> vecOfDist;
    //     double singleCost;
    //     int counter;
    //     bParcelStruct bps;
        
    //     for (int j = inst->n; j < inst->n + inst->m; j++){
    //         ppuSets.push_back(prep);
    //         pdoSets.push_back(prep);
    //     }
    //     counter = 0;
    //     for (int j = inst->n; j < inst->n + inst->m; j++){
    //         ppu[counter].push_back(j);
    //         for (int k = inst->n; k < inst->n + inst->m; k++){
    //             if(j != k){

                    
    //                 ppu[counter].push_back(k);
    //                 counter++;
    //             }
    //         }
    //     }
    // }



}