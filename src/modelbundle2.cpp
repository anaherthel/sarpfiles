#include "modelbundle2.h"
#include <cstdlib>
#include <stdio.h>

void makeBundles2 (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, vector< vector<bParcelStruct> > &clsParcel, probStat* problem){
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
                double bundledelta = nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j].parcelreq + inst->m].delta;
                
                //passenger in the middle
                bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                bStat->bundle.push_back(i);
                bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);
                bStat->bundleTimes.push_back(bundledelta);
                bStat->bundleVec.push_back(bStat->bundle);
                cStat->clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();

                //passenger at the beginning
                bStat->bundle.push_back(i);

                bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);

                bStat->bundleTimes.push_back(bundledelta);
                bStat->bundleVec.push_back(bStat->bundle);
                cStat->clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();
                //passenger at the end

                bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);
                bStat->bundle.push_back(i);

                bStat->bundleTimes.push_back(bundledelta);
                bStat->bundleVec.push_back(bStat->bundle);
                cStat->clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();

            }
            cStat->clusterVec.push_back(cStat->clusters);
            cStat->clusters.clear();
        }

        for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){ //start and end bundles.
            
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

void bundleProfit2(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){
    double cost;
    double service;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        cost = 0;
        service = 0;
        if (bStat->bundleVec[i].size() <= 2){//bundles of single requests and for parcel direct delivery
            cost = nodeVec[bStat->bundleVec[i][0]].profit;
            bStat->bundleProfVec.push_back(cost);
            service = nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleServVec.push_back(service);
        }
        else{//bundles of size 3 or larger
            if (bStat->bundleVec[i].back() == nodeVec.size() - 1){//if it is a dummy node
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
                    }
                    else if (bStat->bundleVec[i][j] < inst->n){//passenger request
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
                        cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                        service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
                    } 
                    else if (bStat->bundleVec[i][j] > inst->n + inst->m - 1){//parcel delivery
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
                        cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                        service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);
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
void initVecs2 (instanceStat *inst, vector< vector<bParcelStruct> > &clsParcel, bundleStat *bStat, probStat * problem){

    vector<bParcelStruct> auxbpsvec;
    vector<int> parcelBundle;

    //init bundle attributes
    for (int i = 0; i < inst->n; i++){
        clsParcel.push_back(auxbpsvec);
    }

    if (problem->scen == "1A" || problem->scen == "1AD"){
        for (int i = 0; i < inst->m; i++){
            bStat->parcelBundleVec.push_back(parcelBundle);
        }
    }
    else {
        for (int i = 0; i < 2*inst->m; i++){
            bStat->parcelBundleVec.push_back(parcelBundle);
        }    
    }

}

void clearArcs(bundleStat *bStat){
    bStat->bArcPlus.clear();
    bStat->vArcPlus.clear();
    bStat->vArcMinus.clear();

    bStat->bArcs.clear();
    bStat->bArcMinus.clear(); 
    bStat->bArcPlus.clear();

    bStat->arcV.clear();

    bStat->bArcVec.clear();
}

void initArcs2 (instanceStat *inst, bundleStat *bStat, clSt *cStat){

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

void makeSmallerProblem2(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<bParcelStruct> > &clsParcel, probStat* problem, int Q){

    if (problem->scen == "1A" || problem->scen == "1AD"){
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

                sort(vecOfDist.begin(), vecOfDist.end(), compareCosts2);

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

void feasibleBundleArcs2 (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem){
    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int ref;

    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    setN = bStat->bundleVec.size() - (2*inst->K);
    setP = setN;
    ref = 3*inst->m;

    int currentCluster = 0;

    pair<int, int> bFArc;
    int auxK;

    int fDummy = bStat->bundleVec.size() - inst->K;

    for(int i = 0; i < bStat->bundleVec.size(); i++){
        if (i < fDummy){
            if(i < setP){//i is not depot or dummy or parcel only
                if (i > currentCluster*(ref + 1) + ref){
                    currentCluster++;
                }
                bStat->clofbundle.push_back(currentCluster);
                for (int j = 0; j < setN; j++){//j is not depot or dummy
                    if (i != j){
                        if (j > currentCluster*(ref + 1) + ref || j < currentCluster*(ref + 1)){\
                            if (bStat->bundleEnd[j] <= inst->T){
                                if (bStat->bundleStart[j] > 0){
                                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                        bStat->bArcs[i][j] = true;
                                    } 
                                }
                                else{
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
                currentCluster++;
                bStat->clofbundle.push_back(currentCluster);
                for (int j = 0; j < setN; j++){//j is a request bundle
                    if (bStat->bundleEnd[j] <= inst->T){
                        if (bStat->bundleStart[j] > 0){
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;                        
                            }
                        }
                        else{
                            bStat->bArcs[i][j] = true;
                        }
                    }
                }
                bStat->bArcs[i][i + inst->K] = true;//direct trip to depot (empty vehicle)
            }              
        }
        else{
            currentCluster++;
            bStat->clofbundle.push_back(currentCluster);
        }
    }

    // cout << "list of cluster allocations: " <<endl;

    // for (int i = 0; i < bStat->clofbundle.size(); i++){
    //     cout << "Bundle: (" << i << ") - " << "cluster: " << bStat->clofbundle[i] << endl;
    // }
    // getchar();
    
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
        if (i >= setN && i < fDummy){//i is a starting bundle
            auxK = i - setN; //arc from starting node to end depot
            bStat->arcV[i][i + inst->K].push_back(auxK);
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

    // Creating cluster of bundles
    bStat->cBundles.resize(currentCluster + 1);

    for (int i = 0; i < bStat->clofbundle.size(); i++) {
        bStat->cBundles[bStat->clofbundle[i]].push_back(i);
    }

    // for (int i = 0; i < bStat->cBundles.size(); i++) {
    //     cout << "cluster : " << i << endl;
    //     for (int j = 0; j < bStat->cBundles[i].size(); j++) {
    //         cout << bStat->cBundles[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // getchar();
}

void feasibleBundleArcs2next (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem, fipBundleStats *fipStat){
    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int setPD; //last index of bundles before parcel only
    int ref;
    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    
    int fDummy;

    setPD = bStat->bundleVec.size() - 3*inst->m;
    setN = setPD - (2*inst->K);
    fDummy = setPD - inst->K;

    ref = 3*inst->m;
    setP = setN;

    int currentCluster = 0;

    pair<int, int> bFArc;
    int auxK;

    cout << "aqui" << endl;

    for (int k = 0; k < fipStat->solPass.size(); k++) {
        for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
            int u = fipStat->solPass[k][i];
            int v = fipStat->solPass[k][i + 1];

            cout << u << " ";

            bStat->bArcs[u][v] = true;

            for (int w = setPD; w < bStat->bundleVec.size(); w++) {
                bStat->bArcs[u][w] = true;
                bStat->bArcs[w][v] = true;
            }
        }
        cout << endl;
    }
    
    for(int i = 0; i < bStat->bundleVec.size(); i++){
        // if (i < fDummy){
        //     if(i < setP){//i is not depot or dummy or parcel only
        //         if (i > currentCluster*(ref + 1) + ref){
        //             currentCluster++;
        //         }
        //         bStat->clofbundle.push_back(currentCluster);
        //         for (int j = 0; j < setN; j++){//j is not depot or dummy
        //             if (i != j){
        //                 if (j > currentCluster*(ref + 1) + ref || j < currentCluster*(ref + 1)){\
        //                     //if (bStat->bundleEnd[j] <= inst->T){
        //                     if (bStat->bundleStart[j] > 0){
        //                         //cout << "i: " << i << " - j: " << j << " - bundleStart[i]: " << bStat->bundleStart[i] << " - bundleServVec[i]: " << bStat->bundleServVec[i] << " - mdist: " << mdist[bStat->lastElement[i]][bStat->firstElement[j]] << " - bundleStart[j]: " << bStat->bundleStart[j] << endl;
        //                         //cout << "test: " << (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]) << endl;
        //                         if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
        //                             // TODO UNCOMMENT // bStat->bArcs[i][j] = true;
        //                         }
        //                     }
        //                     else{
        //                         // TODO UNCOMMENT // bStat->bArcs[i][j] = true;
        //                     }
        //                 }
        //             } 
        //         }
        //         for (int j = fDummy; j < setPD; j++){//j is dummy node
        //             // TODO UNCOMMENT // bStat->bArcs[i][j] = true;                                     
        //         }                    
        //         for (int j = setPD; j < bStat->bundleVec.size(); j++){//j is a parcel only bundle
        //             // TODO UNCOMMENT // bStat->bArcs[i][j] = true; 
        //         }
        //     }

        //     else if (i >= setN && i < setPD){//i is a starting point bundle
        //         currentCluster++;
        //         bStat->clofbundle.push_back(currentCluster);
        //         for (int j = 0; j < setN; j++){//j is a request bundle
        //             //if (bStat->bundleEnd[j] <= inst->T){
        //                 if (bStat->bundleStart[j] > 0){
        //                     if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
        //                         // TODO UNCOMMENT // bStat->bArcs[i][j] = true;                        
        //                     }
        //                 }
        //                 else{
        //                     // TODO UNCOMMENT // bStat->bArcs[i][j] = true;
        //                 }
        //             //}
        //         }
        //         // TODO UNCOMMENT // bStat->bArcs[i][i + inst->K] = true;//direct trip to depot (empty vehicle)

        //         for (int j = setPD; j < bStat->bundleVec.size(); j++){//j is a parcel only bundle
        //             if (bStat->firstElement[j] < inst->n+inst->m){ // arc only if bundle starts with Parcel request pickup
        //                 // TODO UNCOMMENT // bStat->bArcs[i][j] = true; 
        //             }
        //             else{
        //                 //cout << "i: " << i << " - j: " << j << " - firstElement: " << bStat->firstElement[j] << endl;
        //                 // TODO UNCOMMENT // bStat->bArcs[i][j] = false; 
        //             }
        //         }


        //     }
        //     //currentCluster++;
        //     //bStat->clofbundle.push_back(currentCluster);

        // }
        // else{
        //     currentCluster++;
        //     bStat->clofbundle.push_back(currentCluster);
        // }
        if (i >= setPD){
            // for (int j = 0; j < setN; j++){//j is a request bundle
            //     if (bStat->bundleStart[j] > 0){
            //         bStat->bArcs[i][j] = true;                        
            //     }
            // }
            // for (int j = fDummy; j < setPD; j++){//j is a dummy bundle
            //     if (bStat->lastElement[i] >= inst->n+inst->m){ // arc only if bundle ends with Parcel request dropoff
            //         bStat->bArcs[i][j] = true; 
            //     }
            //     else{
            //         //cout << "i: " << i << " - j: " << j << " - lastElement: " << bStat->lastElement[i] << endl;
            //         bStat->bArcs[i][j] = false; 
            //     }
            // }
            for (int j = setPD; j < bStat->bundleVec.size(); j++){//j is a parcel only bundle
                if (i != j){
                    bStat->bArcs[i][j] = true; 
                }
            }   
        }
     
    }

    // cout << "list of cluster allocations: " <<endl;

    // for (int i = 0; i < bStat->clofbundle.size(); i++){
    //     cout << "Bundle: (" << i << ") - " << "cluster: " << bStat->clofbundle[i] << endl;
    // }
    // getchar();
    
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
                    else if (j >= fDummy && j < setPD){ //j is a dummy bundle
                        auxK = j - fDummy; //k is the vehicle associated with dummy bundle j
                        bStat->arcV[i][j].push_back(auxK);                         
                    }
                    else if (j >= setPD){ //j is a parcel only bundle
                        for (int k = 0; k < inst->K; k++){ // in this case, all vehicles are available
                            bStat->arcV[i][j].push_back(k);
                        }
                    }                        

                }
                else if (i < fDummy){//i is a starting location bundle
                    if (j < setN){ //j is a request bundle
                        auxK = i - setN;
                        bStat->arcV[i][j].push_back(auxK);                         
                    }
                    if (j >= setPD){ //j is a parcel only bundle
                        auxK = i - setN;
                        bStat->arcV[i][j].push_back(auxK); 
                    }
                    
                }
                else if (i >= setPD){ //i is a parcel only bundle
                    if (j < setN || j >= setPD){ //j is a request bundle or a parcel only bundle
                        for (int k = 0; k < inst->K; k++){ // in this case, all vehicles are available
                            bStat->arcV[i][j].push_back(k);
                        } 
                    }
                    else if (j >= fDummy && j < setPD){ //j is a dummy bundle
                        auxK = j - fDummy; //k is the vehicle associated with dummy bundle j
                        bStat->arcV[i][j].push_back(auxK);                         
                    }                                     
                }                  
                
            }
        }
        if (i >= setN && i < fDummy){//i is a starting bundle
            auxK = i - setN; //arc from starting node to end depot
            bStat->arcV[i][i + inst->K].push_back(auxK);
        }
    }

    //creating the vectors for leaving incoming arcs with available vehicles
    for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        //cout << "varc plus size: " << bStat->vArcPlus.size() << endl;
        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            //cout <<"1" << endl;
            int k = bStat->arcV[i][j][k1];
            //cout <<"2" << endl;
            bStat->vArcPlus[i][k].push_back(bStat->bArcVec[a]);
            bStat->vArcMinus[j][k].push_back(bStat->bArcVec[a]);
        }
    }
}

void feasibleClusterArcs2 (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem){
    int reqClusters = cStat->clusterVec.size() - (inst->K*2);
    int clusterA = 0;
    int clusterB;
    int setN;
    int setP;//with passengers
    int setS;
    int cset;
    int csetP; //with passengers
    int ref;

    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    setN = bStat->bundleVec.size() - (2*inst->K);
    setP = setN;

    cset = cStat->clusterVec.size() - (inst->K*2);
    csetP = cset;
    // csetP = cStat->clusterVec.size() - (inst->K*2) - inst->m;

    ref = 3*inst->m;


    // int setf;

    pair<int, int> cFArc;
    int fDummy = bStat->bundleVec.size() - inst->K;
    int fcDummy = cStat->clusterVec.size() - inst->K;

    int auxK;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        if (i < setP){//i is a request bundle
            if (i > clusterA*(ref + 1) + ref){
                clusterA++;
            }           
        }
        else{
            if(problem->scen == "1A" || problem->scen == "1AD"){
                clusterA++;
            }
        }

        clusterB = 0;
        for (int j = 0; j < bStat->bundleVec.size(); j++){
            if (j < setP){//j is a request bundle
                if (j > clusterB*(ref + 1) + ref){
                    clusterB++;
                }                
            }
            else{
                if(problem->scen == "1A" || problem->scen == "1AD"){
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

                    cStat->cArcMinus[clusterB].push_back(cFArc);
                    cStat->cArcPlus[clusterA].push_back(cFArc);
                }
            }
        }
    }

    for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        int cA = bStat->clofbundle[i];
        int cB = bStat->clofbundle[j];

        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
            cStat->vArcPlus[cA][k].push_back(bStat->bArcVec[a]);
            cStat->vArcMinus[cB][k].push_back(bStat->bArcVec[a]);
        }
    } 

    // cout << "Vector of arcs of cluster traverssed by k: " << endl;

    // for (int i = 0; i < cStat->vArcPlus.size(); i++){
    //     for (int j = 0; j < cStat->vArcPlus[i].size(); j++){
    //         for (int k = 0; k < cStat->vArcPlus[i][j].size(); k++){
    //             cout << "cluster: " << i << " - " << "vehicle: " << j << " - " << "Bundle arc: " << cStat->vArcPlus[i][j][k].first << "-" << cStat->vArcPlus[i][j][k].second << endl;
    //         }
    //     }
    //     cout << endl;
    // }
}

void makeParcelBundles2(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){
    int parcelReq;
    bool foundparc;
    if (problem->scen == "1A" || problem->scen == "1AD"){
        for (int i = 0; i < bStat->bundleVec.size(); i++){
            for (int j = 0; j < bStat->bundleVec[i].size(); j++){
                foundparc = false;
                if (bStat->bundleVec[i].size() < 2){//single passenger; depot
                    break;
                }
                else if (bStat->bundleVec[i][j] >= inst->n){
                    parcelReq = bStat->bundleVec[i][j];
                    bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                    foundparc = true;
                }
                if (foundparc) {
                    break;
                }
            }
        } 
    }
    else {
        for (int i = 0; i < bStat->bundleVec.size(); i++){
            for (int j = 0; j < bStat->bundleVec[i].size(); j++){
                if (bStat->bundleVec[i][j] < inst->n){
                    continue;
                }
                // else if (bStat->bundleVec[i][j] > inst->n + inst->m - 1){
                //     continue;
                // }
                else{
                    parcelReq = bStat->bundleVec[i][j];
                    bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                }
            }
        } 
    }
}

void makeStartTimes2 (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){

    double parcelTime;
    double bundleTime;
    double endTime;
    bool foundpass;//found passenger
    double tij;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        parcelTime = 0;
        bundleTime = 0;
        endTime = 0;
        foundpass = false;

        if (bStat->bundleVec[i].size() > 1){
            if (bStat->bundleVec[i][0] < inst->n){//if the 1st element of the bundle is a passenger node(d-P-D)
                foundpass = true;
                bundleTime = nodeVec[bStat->bundleVec[i][0]].e;

                tij = ((mdist[bStat->bundleVec[i][0]][bStat->bundleVec[i][1]])/inst->vmed);
                endTime = bundleTime + nodeVec[bStat->bundleVec[i][0]].delta + tij;

                for (int j = 1; j < bStat->bundleVec[i].size() - 1; j++){
                    endTime += nodeVec[bStat->bundleVec[i][j]].delta;
                    tij = ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                    endTime += tij;
                }
            }

            else if (bStat->bundleVec[i].back() < inst->n){//the last element is a passenger(P-D-d)
                endTime = nodeVec[bStat->bundleVec[i].back()].e;
                bundleTime = endTime;

                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    bundleTime -= nodeVec[bStat->bundleVec[i][j]].delta;
                    tij = ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                    bundleTime -= tij;
                }

            }

            else{
                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    tij = ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                    if (bStat->bundleVec[i][j] >= inst->n){//if the element of the bundle is not a passenger request
                        parcelTime += nodeVec[bStat->bundleVec[i][j]].delta;

                        if (bStat->bundleVec[i][j + 1] < inst->n && !foundpass){//if the next element is a passenger
                            bundleTime = nodeVec[bStat->bundleVec[i][j + 1]].e - parcelTime - tij;
                            endTime += nodeVec[bStat->bundleVec[i][j + 1]].e;
                            foundpass = true;
                            // break;
                        }

                        else{
                            parcelTime += tij;
                        }
                    }
                    else{
                        if (foundpass){
                            endTime += nodeVec[bStat->bundleVec[i][j]].delta + tij;//this for PdD
                        }
                    }
                }
            }
            
            endTime += nodeVec[bStat->bundleVec[i].back()].delta;
            bStat->bundleEnd.push_back(endTime);
            bStat->bundleStart.push_back(bundleTime);
            
        }

        else{
            foundpass = true;
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
            bStat->bundleStart.push_back(bundleTime);
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e + nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleEnd.push_back(bundleTime);
            continue;
        }
    }


    // cout << "Start and end times: " << endl;
    // for (int i = 0; i < bStat->bundleStart.size(); i++){
    //     cout << "Bundle " << i << ": " << bStat->bundleStart[i] << " - " << bStat->bundleEnd[i] << endl;
    // }
    // cout << endl;
}

void makeBundleReference2(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){

    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->lastElement.push_back(bStat->bundleVec[i][bStat->bundleVec[i].size()-1]);

    }
    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->firstElement.push_back(bStat->bundleVec[i][0]);

    }
}

bool compareCosts2(const bParcelStruct &a, const bParcelStruct &b){
    return a.cost < b.cost;
}

void nodeSolution2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat, probStat* problem, bool fip){

    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int setPD;
    int setN;
    int currSP;
    vector<int> orderVec;

    if (fip) {
        setPD = bStat->bundleVec.size() - 3*inst->m;
        setN = setPD - (2*inst->K);
    } else {
        setN = bStat->bundleVec.size() - (2*inst->K);
    }

    for (int k = 0; k < inst->K; k++){
        currSP = setN + k;
        for (int i = 0; i < sStat->solvec[k].size(); i++){
            auxPair.first = sStat->solvec[k][i].first;
            auxPair.second = sStat->solvec[k][i].second;            
            auxVec.push_back(auxPair);
        }
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
        // cout<< "auxvec size: " << auxVec.size();
        // getchar();
        }
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

    for (int k = 0; k < inst->K; k++){
        for (int i = 0; i < sStat->solInNode[k].size() - 1; i++){
            if (sStat->solInNode[k][i] >= inst->n){
                sStat->pProfit += nodeVec[sStat->solInNode[k][i]].profit;
                // cout << sStat->solInNode[k][i] << ": " << nodeVec[sStat->solInNode[k][i]].profit << endl;
            }
            sStat->costs += (double)inst->costkm*mdist[sStat->solInNode[k][i]][sStat->solInNode[k][i+1]];
        }
    }

    cout << "\n\nCustomer profit: " << inst->totalCustomProfit << endl;
    cout << "Parcel profit: " << sStat->pProfit << endl;
    cout << "Costs: " << sStat->costs << endl;

    cout << "\nNumber of Vehicles: " << inst->K << endl;

    cout << "\nSolution by nodes: " << endl;
    
    //adjust K
    int usedVehicles = 0;

    vector<int> vehicleNumbers;

    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";

        if (sStat->solInNode[k].size() > 2){
            usedVehicles++;
            vehicleNumbers.push_back(k);
        }

        for (int i = 0; i < sStat->solInNode[k].size(); i++){
            if (i < sStat->solInNode[k].size() - 1){
                cout << sStat->solInNode[k][i] << " - ";
            }
            else{
                cout << sStat->solInNode[k][i];
            }
        }
        currSP = inst->n + 2*inst->m + k;

        // cout << " // Bundle selection: " << (mdist[currSP][bStat->firstElement[sStat->solOrder[k][1]]]/inst->vmed);

        cout << " - Total time: " << bStat->bundleEnd[sStat->solOrder[k][sStat->solOrder[k].size()-2]] - bStat->bundleStart[sStat->solOrder[k][1]] + (mdist[currSP][bStat->firstElement[sStat->solOrder[k][1]]]/inst->vmed) << endl;
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

    cout << "Used vehicles: " << usedVehicles << endl;
    cout << "Vehicle numbers: ";
    for (int i = 0; i < vehicleNumbers.size(); i++){
        cout << vehicleNumbers[i] << " ";
    }
    cout << endl;
}

void stillTimeBundle2(instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    double timePoint = 0;
    int cBundle, nBundle;

    double bbEarlier;
    double service = 5;

    double t1, t2, tdif;
    // int setN = bStat->bundleVec.size() - (2*inst->K);

    // vector<double> timeDiff;
    double timeDiff;

    int fB, lB, dp, dm;

    for (int k = 0; k < inst->K; k++){

        fB = sStat->solOrder[k][1];
        lB = sStat->solOrder[k][sStat->solOrder[k].size() - 2];
        dp = sStat->solOrder[k][0];
        dm = sStat->solOrder[k][sStat->solOrder[k].size() - 1];

        t1 = bStat->bundleStart[fB] - (mdist[bStat->lastElement[dp]][bStat->firstElement[fB]])/inst->vmed;
        t2 = bStat->bundleEnd[lB];

        tdif = 8 - (t2 - t1);

        if (bStat->firstElement[fB] < inst->n){
            sStat->tStillP += tdif;
        }
        else{
            sStat->tStillG += tdif;
        }
    }

    for (int k = 0; k < inst->K; k++){
        // cout << "\nfirst time: " << nodeVec[sStat->solInNode[k][0]].e << endl;
        // first element of bundle: bStat->bundleVec[cBundle][0];
        // timePoint = 0;

        for(int i = 1; i < sStat->solOrder[k].size() - 2; i++){//start from 1 bc there is no waiting time between the depot and the first served bundle. (Late departure)
            cBundle = sStat->solOrder[k][i];
            nBundle = sStat->solOrder[k][i+1];

            timePoint = bStat->bundleEnd[cBundle] + (mdist[bStat->lastElement[cBundle]][bStat->firstElement[nBundle]])/inst->vmed;            

            bbEarlier = bStat->bundleStart[nBundle];
            
            if (bStat->firstElement[nBundle] < inst->n){
                sStat->tStillP += (bbEarlier - timePoint);
            }
            else{
                sStat->tStillG += (bbEarlier - timePoint);
            }
            // timePoint = bStat->bundleEnd[cBundle];

            // cout << "Current "
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

void setUpFipBundle(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem, fipBundleStats *fipStat){

    //create parcel only bundles
    //if (problem->model == "bundle3"){
        //Parcel only bundles (direct delivery):
    int setN = bStat->bundleVec.size() - 2*inst->K;

    for (int i = setN; i < bStat->bundleVec.size(); i++){
        bStat->bundleEnd[i] = inst->T;
    }

    for (int i = inst->n; i < inst->n + inst->m; i++){
        double bundledelta = nodeVec[i].delta + nodeVec[i + inst->m].delta;

        bStat->bundle.push_back(i);
        bStat->bundle.push_back(i + inst->m);

        bStat->bundleTimes.push_back(bundledelta);            
        bStat->bundleVec.push_back(bStat->bundle);
        //cStat->clusters.push_back(bStat->bundleVec.size()-1);
        double cost = nodeVec[bStat->bundle[0]].profit - (inst->costkm*mdist[bStat->bundle[0]][bStat->bundle[1]]);
        double service = nodeVec[bStat->bundle[0]].delta + nodeVec[bStat->bundle[1]].delta
                        + ((mdist[bStat->bundle[0]][bStat->bundle[1]])/inst->vmed);
        bStat->bundleProfVec.push_back(cost);
        bStat->bundleServVec.push_back(service);    
        double bundleTime = nodeVec[bStat->bundle[0]].e;
        bStat->bundleStart.push_back(bundleTime);
        bundleTime = nodeVec[bStat->bundle[0]].l;
        bStat->bundleEnd.push_back(bundleTime);
        bStat->firstElement.push_back(bStat->bundle[0]);
        bStat->lastElement.push_back(bStat->bundle[1]);
        fipStat->bundlesPD.push_back(bStat->bundleVec.size()-1);
    

        bStat->parcelBundleVec[i - inst->n].push_back(bStat->bundleVec.size()-1);


        bStat->bundle.clear();

        //parcel only bundles single parcel single delivery bundles

        bStat->bundle.push_back(i);

        bStat->bundleTimes.push_back(nodeVec[i].delta);            
        bStat->bundleVec.push_back(bStat->bundle);
        cost = nodeVec[bStat->bundle[0]].profit;
        service = nodeVec[bStat->bundle[0]].delta;
        bStat->bundleProfVec.push_back(cost);
        bStat->bundleServVec.push_back(service);  
        bundleTime = nodeVec[bStat->bundle[0]].e;
        bStat->bundleStart.push_back(bundleTime);
        bundleTime = nodeVec[bStat->bundle[0]].l;
        bStat->bundleEnd.push_back(bundleTime);
        bStat->firstElement.push_back(bStat->bundle[0]);
        bStat->lastElement.push_back(bStat->bundle[0]);            
        bStat->parcelBundleVec[i - inst->n].push_back(bStat->bundleVec.size()-1);
        fipStat->bundlesPonly.push_back(bStat->bundleVec.size()-1);

        bStat->bundle.clear();

        bStat->bundle.push_back(i + inst->m);

        bStat->bundleTimes.push_back(nodeVec[i + inst->m].delta);            
        bStat->bundleVec.push_back(bStat->bundle);
        cost = nodeVec[bStat->bundle[0]].profit;
        service = nodeVec[bStat->bundle[0]].delta;
        bStat->bundleProfVec.push_back(cost);
        bStat->bundleServVec.push_back(service);    
        bundleTime = nodeVec[bStat->bundle[0]].e;
        bStat->bundleStart.push_back(bundleTime);
        bundleTime = nodeVec[bStat->bundle[0]].l;
        bStat->bundleEnd.push_back(bundleTime);
        bStat->firstElement.push_back(bStat->bundle[0]);
        bStat->lastElement.push_back(bStat->bundle[0]);              
        bStat->parcelBundleVec[i - inst->n].push_back(bStat->bundleVec.size()-1);
        fipStat->bundlesDonly.push_back(bStat->bundleVec.size()-1);

        bStat->bundle.clear();


    } 


     

    //}
    cout << "\nBundle Vector: [";

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        cout << "(" << i << "): [";
        for (int j = 0; j < bStat->bundleVec[i].size(); j++){
            cout << bStat->bundleVec[i][j];
            if (j < bStat->bundleVec[i].size() - 1){
                cout << ",";
            }
            else{
                cout << "] ";
            }
        }
        cout << endl;
    }
    //getchar();



}

void fipStructBundle(instanceStat *inst, solStats *sStat, bundleStat *bStat, fipBundleStats *fipStat){

    vector<int> pulocations;
    pair <int, int> pairpuloc;
    vector< pair<int, int> > pupairs;
    int fdepot;

    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int setPD; //last index of bundles before parcel only
    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    
    int fdummy;  

    setPD = bStat->bundleVec.size() - 3*inst->m;
    setN = setPD - (2*inst->K);
    fdummy = setPD - inst->K;
    fdepot = setN;
    
    for (int i = 0; i < setPD; i++){
        fipStat->solBegin.push_back(0);
    }

    for (int i = 0; i < inst->n; i++){
        fipStat->vehicleVec.push_back(-1);
    }
    for (int i = 0; i < sStat->solOrder.size(); i++){
        int depot = fdepot + i;
        int dummy = fdummy + i;
        pulocations.push_back(depot);


        for (int j = 0; j < sStat->solOrder[i].size(); j++){
            if (sStat->solOrder[i][j] < setN){
                pulocations.push_back(sStat->solOrder[i][j]);
                fipStat->solBegin[sStat->solOrder[i][j]] = bStat->bundleStart[sStat->solOrder[i][j]];
            }
        }
        pulocations.push_back(dummy);
        fipStat->solPass.push_back(pulocations);
        pulocations.clear();
    }
    pulocations.clear();

    pair<int, int> clint;

    int last_start = 0;
    for (int i = 0; i < inst->n; i++){
        clint.first = last_start;
        clint.second = inst->m*3 + clint.first;
        last_start = clint.second + 1;
        fipStat->clusterIntervals.push_back(clint);
    }

    for (int i = 0; i < inst->m; i++){
        fipStat->servedParcelsInit.push_back(0);
    }

    for (int k = 0; k < fipStat->solPass.size(); k++){
        for (int i = 0; i < fipStat->solPass[k].size(); i++){
            int u = fipStat->solPass[k][i];
            if(u < setN){
                if(bStat->bundleVec[u].size() > 1){
                    for (int j = 0; j < bStat->bundleVec[u].size(); j++){
                        if(bStat->bundleVec[u][j] >= inst->n && bStat->bundleVec[u][j] < inst->m + inst->n){
                            int parcelID = bStat->bundleVec[u][j] - inst->n;
                            fipStat->servedParcelsInit[parcelID] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    //cout << "\nInitial served parcels: " << endl;
    //for (int i = 0; i < fipStat->servedParcelsInit.size(); i++){
    //    cout << inst->n + i << ": " << fipStat->servedParcelsInit[i] << endl;
    //}

    //for (int k = 0; k < inst->K; k++){
    //    for (int i = 0; i < fipStat->solPass[k].size(); i++){
            //if(fipStat->solPass[k][i] < inst->n){
            //pairpuloc.first = fipStat->solPass[k][i];
            //pairpuloc.second = i;
            //pupairs.push_back(pairpuloc);
            //fipStat->vehicleVec[fipStat->solPass[k][i]] = k;
            //}
        //}
        //fipStat->solPassOrigins.push_back(pupairs);
        //pupairs.clear();
    //}
}

void printBundleFile (instanceStat *inst, solStats *sStat, probStat* problem) {
    if (problem->model == "bundle2") {
        string filename = "src/Aux/bundleSol/" + inst->InstName + ".txt";

        // TODO UNCOMMENT //  << filename << endl;

        ofstream oFile(filename);

        vector<pair<int, int>> emptyRoutes;
        vector<pair<int, int>> nodeInRoute;
        vector<pair<int, int>> sequencePairs;

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            if (sStat->solOrder[k].size() < 3) {
                int u = sStat->solOrder[k][0] - inst->n;
                int v = sStat->solOrder[k][1] - inst->n;

                emptyRoutes.push_back(make_pair(u, v));
            }
        }

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            for (int i = 0; i < sStat->solOrder[k].size(); i++) {
                int u = sStat->solOrder[k][i];

                if (u < inst->n) { 
                    nodeInRoute.push_back(make_pair(u, k));
                }

                if (u >= 2*inst->n + 2*inst->m) {
                    nodeInRoute.push_back(make_pair(u - inst->n, k));
                }
            }
        }

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            for (int i = 0; i < sStat->solOrder[k].size() - 1; i++) {
                int u = sStat->solOrder[k][i];
                int v = sStat->solOrder[k][i + 1];

                if (u < inst->n) continue;
                if (u >= inst->n) {
                    u -= inst->n;
                }

                if (v >= inst->n) {
                    v -= inst->n;
                }

                sequencePairs.push_back(make_pair(u, v));
            }
        }

        oFile << sStat->time << endl;
        oFile << sStat->solprofit << endl;

        oFile << emptyRoutes.size() << endl;
        for (auto arcPair : emptyRoutes) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile << nodeInRoute.size() << endl;
        for (auto arcPair : nodeInRoute) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile << sequencePairs.size() << endl;
        for (auto arcPair : sequencePairs) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile.close();
    }


    // // TODO UNCOMMENT //  << "\nNumber of Vehicles: " << inst->K << endl;

    // // TODO UNCOMMENT //  << "\nSolution: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < sStat->solOrder[k].size(); i++){
    //         if (i < sStat->solOrder[k].size() - 1){
    //             // TODO UNCOMMENT //  << sStat->solOrder[k][i] << " - ";
    //         }
    //         else{
    //             // TODO UNCOMMENT //  << sStat->solOrder[k][i];
    //         }
    //     }
    //     // TODO UNCOMMENT //  << " - Total time: " << sStat->solBegin[sStat->solOrder[k][sStat->solOrder[k].size()-2]] - sStat->solBegin[sStat->solOrder[k][0]] << endl;
    // }
    // // TODO UNCOMMENT //  << endl;

    // // disregard if fip
    // // TODO UNCOMMENT //  << "\nSolution structure: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < sStat->solOrder[k].size(); i++){
    //         if (i < sStat->solOrder[k].size() - 1){
    //             if (sStat->solOrder[k][i] < inst->n){
    //                 // TODO UNCOMMENT //  << "d" << " - ";
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + inst->m){
    //                 // TODO UNCOMMENT //  << "P" << " - ";
    //                 sStat->servedParcels++;
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + 2*inst->m){
    //                 // TODO UNCOMMENT //  << "D" << " - ";
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + 2*inst->m + inst->K){
    //                 // TODO UNCOMMENT //  << "S" << " - ";
    //             }                                      
    //         }
    //         else{

    //             // TODO UNCOMMENT //  << "f";
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }
    // // TODO UNCOMMENT //  << endl; 
    
    string filename2;
    
    if (problem->model == "bundle2") {
        filename2 = "src/Results/bundleResults/" + inst->InstName + ".csv"; 
    } else if (problem->model == "bundle3"){
        filename2 = "src/Results/budlefipResults/" + inst->InstName + ".csv"; 
    } else if (problem->model == "bundle4") {
        filename2 = "src/Results/mbudlefipResults/" + inst->InstName + ".csv"; 
    }

    ofstream oFile2(filename2);

    oFile2 << inst->InstName + ",";
    oFile2 << sStat->time;
    oFile2 << ",";
    oFile2 << sStat->solDual;
    oFile2 << ",";
    oFile2 << sStat->solprofit;
    oFile2 << ",";
    oFile2 << sStat->servedParcels;
    oFile2 << ",";
    oFile2 << sStat->tNone;
    oFile2 << ",";
    oFile2 << sStat->dNone;
    oFile2 << ",";
    oFile2 << sStat->status;

    oFile2.close();
}


void bundleMethod2(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
    bundleStat bStat;
    clSt cStat;

    vector< vector<bParcelStruct> > clsParcel;

    int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
    int Q = 5;

    initVecs2(inst, clsParcel, &bStat, problem);

    makeSmallerProblem2(inst, nodeVec, mdist, p, clsParcel, problem, Q);


    makeBundles2(inst, nodeVec, &bStat, &cStat, clsParcel, problem);

    //cout << "\nBundle Vector: [";

    //for (int i = 0; i < bStat.bundleVec.size(); i++){
    //    cout << "(" << i << "): [";
    //    for (int j = 0; j < bStat.bundleVec[i].size(); j++){
    //        cout << bStat.bundleVec[i][j];
    //        if (j < bStat.bundleVec[i].size() - 1){
    //            cout << ",";
    //        }
    //        else{
    //            cout << "] ";
    //        }
    //    }
    //    cout << endl;
    //}
    //getchar();
    
    //Uncomment from this line
    makeParcelBundles2(inst, nodeVec, &bStat, problem);

    //cout << "\nParcel Bundles: " << endl;

    //for (int i = 0 ; i < bStat.parcelBundleVec.size(); i++){
    //cout << i + inst->n << ": ";
    //for(int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
    //    cout << bStat.parcelBundleVec[i][j] << " ";
    //}
    //cout << endl;
    //}
    //cout << endl;
    //// getchar();

    bundleProfit2(inst, mdist, nodeVec, &bStat);

    //cout << "\nBundle Profit: " << endl;
    //for (int i = 0; i < bStat.bundleProfVec.size(); i++){
    //    cout << i << ": " << setw(3) << std:: right << bStat.bundleProfVec[i] << endl;
    //}
    //cout << endl;

    //// getchar();


    makeStartTimes2(inst, mdist, nodeVec, &bStat, problem);

    makeBundleReference2(inst, mdist, nodeVec, &bStat);


    //cout << "\nBundle Service Times: ";
    //for (int i = 0; i < bStat.bundleServVec.size(); i++){
    //cout << i  << ": " <<   bStat.bundleServVec[i] << endl;
    //}
    //cout << endl;

    //// getchar();

    initArcs2(inst, &bStat, &cStat);

    feasibleBundleArcs2(inst, mdist, nodeVec, &bStat, &cStat, p, problem);

    //For passenger only solutions:
    // feasibleBundleArcsPass2(inst, mdist, nodeVec, &bStat, &cStat, p, problem);

    //cout<< "\nFeasible arcs between bundles:" << endl;
    //for (int i = 0; i < bStat.bundleVec.size(); i++){
    //    if (i == 0){
    //        cout << setw(3) << " ";
    //    }
    //    cout << setw(3) << std::right << i;
    //}
    //cout << endl;

    //for (int i = 0; i < bStat.bundleVec.size(); i++){
    //    cout << setw(3) << std::right << i;
    //    for (int j = 0; j < bStat.bundleVec.size(); j++){
    //        cout << setw(3) << std:: right << bStat.bArcs[i][j];
    //    }
    //    cout << endl;
    //}

    //file with bundle arcs
    //std::ofstream file("bundleArcs.csv");

    //if (!file.is_open()) {
    //    std::cerr << "Failed to open file for writing.\n";
    //    return;
    //}
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i;
    //    if (i < bStat.bundleVec.size() - 1) file << ",";

    //}
    //file << "\n";    
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i << ",";
    //    for (int j = 0; j < bStat.bundleVec.size(); j++){
    //        file << bStat.bArcs[i][j];
    //        if (j < bStat.bundleVec.size() - 1) file << ",";
    //    }
    //    file << "\n";
    //}

    //file.close();


    // getchar();

    // cout << "Bundle beginning times: " << endl;
    // for (int i = 0; i < bStat.bundleStart.size(); i++){
    //     cout << setw(3) << std::right << i << ": " << std:: right << bStat.bundleStart[i];
    //     cout << endl;
    // }
    // // getchar();


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
    // getchar();
    
    feasibleClusterArcs2(inst, nodeVec, &bStat, &cStat, p, problem);

    //cout<< "\nFeasible arcs between clusters:" << endl;
    //for (int i = 0; i < cStat.clusterVec.size(); i++){
    //    if (i == 0){
    //        cout << setw(3) << " ";
    //    }
    //    cout << setw(3) << std::right << i;
    //}
    //cout << endl;
    //for (int i = 0; i < cStat.clusterVec.size(); i++){
    //    cout << setw(3) << std::right << i;
    //    for (int j = 0; j < cStat.clusterVec.size(); j++){
    //        cout << setw(3) << std:: right << cStat.cArcs[i][j];
    //    }
    //    cout << endl;
    //}

    //// getchar();

    /////////////////////////////////////////
    
    mipbundle2(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat);
    
    if(sStat->feasible){
        // solStatIni(sStat);

        nodeSolution2 (inst, mdist, &bStat, nodeVec, sStat, problem, false);
        
        stillTimeBundle2(inst, mdist, &bStat, nodeVec, sStat);

        mipSolStatsPlus (inst, mdist, &bStat, nodeVec, sStat);

        // // cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

        printStats(inst, sStat);

        printBundleFile(inst, sStat, problem);
    }
    
    if((problem->model == "bundle3" || problem->model == "bundle4") && sStat->servedParcels < inst->m){
        fipBundleStats fipStat;

        setUpFipBundle(inst, mdist, nodeVec, &bStat, problem, &fipStat);

        clearArcs(&bStat);
        initArcs2(inst, &bStat, &cStat);
        fipStructBundle(inst, sStat, &bStat, &fipStat);
        feasibleBundleArcs2next(inst, mdist, nodeVec, &bStat, &cStat, p, problem, &fipStat);

        clearBndStats(sStat);
        //std::ofstream file("bundleArcs.csv");

        //if (!file.is_open()) {
        //    std::cerr << "Failed to open file for writing.\n";
        //    return;
        //}
        //file << -1 << ",";
        //for (int i = 0; i < bStat.bundleVec.size(); i++) {
        //    file << i;
        //    if (i < bStat.bundleVec.size() - 1) file << ",";

        //}
        //file << "\n";    
        //for (int i = 0; i < bStat.bundleVec.size(); i++) {
        //    file << i << ",";
        //    for (int j = 0; j < bStat.bundleVec.size(); j++){
        //        file << bStat.bArcs[i][j];
        //        if (j < bStat.bundleVec.size() - 1) file << ",";
        //    }
        //    file << "\n";
        //}

        //file.close();
       
       
        cout << "\nParcel Bundles: " << endl;

        for (int i = 0 ; i < bStat.parcelBundleVec.size(); i++){
        cout << i + inst->n << ": ";
        for(int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
            cout << bStat.parcelBundleVec[i][j] << " ";
        }
        cout << endl;
        }
        cout << endl;
        
        if (problem->model == "bundle3") {
            fipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat, &fipStat);
        } else if (problem->model == "bundle4"){
            mfipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat, &fipStat);
        }
        cout << "after fip" << endl;

        if(sStat->feasible && problem->model == "bundle4"){
            // solStatIni(sStat);

            nodeSolution2 (inst, mdist, &bStat, nodeVec, sStat, problem, true);
            
            stillTimeBundle2(inst, mdist, &bStat, nodeVec, sStat);

            mipSolStatsPlus (inst, mdist, &bStat, nodeVec, sStat);

            // // // cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

            printStats(inst, sStat);

            // printBundleFile(inst, sStat, problem);
            
        }
    }

    for ( int i = 0; i < inst->V + inst->dummy; i++) {
        delete[] mdist[i];
    }
    delete[] mdist;
}