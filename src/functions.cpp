#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void initArcs (instanceStat *inst, nodeArcsStruct *nas){
    vector<bool> auxVec;
    vector< pair<int,int> > auxPairVec;

    for(int i = 0; i < inst->V + inst->dummy; i++){
        for(int j = 0; j < inst->V + inst->dummy; j++){
            auxVec.push_back(false);
        }
        nas->arcs.push_back(auxVec);
        nas->arcPlus.push_back(auxPairVec);
        nas->arcMinus.push_back(auxPairVec);
        auxVec.clear();
    }


    // cout << "Node arcs all false: " << endl;
    // for (int i = 0; i < nas->arcs.size(); i++){
    //     if (i == 0){
    //         cout << setw(3) << " ";
    //     }
    //     cout << setw(3) << std::right << i << " ";
    // }
    // cout << endl;
    // for(int i = 0; i < nas->arcs.size(); i++){
    //     cout << setw(3) << std::right << i;
    //     for(int j = 0; j < nas->arcs[i].size(); j++){
    //         cout << setw(3) << nas->arcs[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    // getchar();
}

void feasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem){

    if (problem->scen == "1A" || problem->scen == "2A"){
        for (int i = 0; i < inst->V; i++){
            if (i < inst->n){//i is a passenger req
                for(int j = 0; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
                    if(i != j){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        if (j < inst->n){
                            nas->arcNN.push_back(nas->fArc);
                        }
                        nas->arcNplus.push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->n + inst->m){//i is a parcel pickup node
                for (int j = 0; j < inst->n; j++){ //j is a passenger node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcPP.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }
            else if (i < inst->n + 2*inst->m){// i is a parcel delivery node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger node or parcel pickup node
                    if (j + inst->m != i){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }                    
                }

                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->V + inst->dummy){ // i is a starting node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }          
        }
    }
    else if (problem->scen == "1B" || problem->scen == "2B"){
        for (int i = 0; i < inst->V; i++){
            if (i < inst->n){//i is a passenger req
                for(int j = 0; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
                    if(i != j){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        if (j < inst->n){
                            nas->arcNN.push_back(nas->fArc);
                        }
                        nas->arcNplus.push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->n + inst->m){//i is a parcel pickup node
                for (int j = 0; j < inst->n + inst->m; j++){ //j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcPP.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }
            else if (i < inst->n + 2*inst->m){// i is a parcel delivery node
                for (int j = 0; j < inst->n + 2*inst->m; j++){//j is a passenger node or parcel node (pu or del)
                    if (j + inst->m != i){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }                    
                }

                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->V + inst->dummy){ // i is a starting node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }          
        }
    }
}

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

//*******
//For 2A:
//******* 

    if (problem->scen == "2A"){
        vector<int> delNodes;
        vector<int> pickNodes;
        int counter = 0;
        for (int i = 0; i < inst->n; i++){//i is a passenger request
            bStat->bundle.push_back(i);
            bStat->label.push_back(0);
            bStat->label2.push_back(-1);
            bStat->mainNode.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            for (int j = 0; j < clsParcel[i].size(); j++){//j from the set of closest parcels to i
                if (clsParcel[i][j].parcelreq < inst->n + inst->m){//if it is a pickup request
                    if (clsParcel[i][j].poslabel == 0){//if it is positioned before the passenger
                        bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                        bStat->bundle.push_back(i);
                        bStat->label.push_back(1);
                        bStat->label2.push_back(0);
                        bStat->mainNode.push_back(clsParcel[i][j].parcelreq);
                        bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta);
                        bStat->bundleVec.push_back(bStat->bundle);
                        clusters.push_back(bStat->bundleVec.size()-1);
                        bStat->activePU.push_back(bStat->bundleVec.size()-1);
                        bStat->bundle.clear();  
                        pickNodes.push_back(clsParcel[i][j].parcelreq);
                    }
                    else{
                        bStat->bundle.push_back(i);
                        bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                        bStat->label.push_back(1);
                        bStat->label2.push_back(1);
                        bStat->mainNode.push_back(clsParcel[i][j].parcelreq);
                        bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta);
                        bStat->bundleVec.push_back(bStat->bundle);
                        clusters.push_back(bStat->bundleVec.size()-1);
                        bStat->activePU.push_back(bStat->bundleVec.size()-1);
                        bStat->bundle.clear();    
                    }

                }

                else{
                    if (clsParcel[i][j].poslabel == 0){
                        bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                        bStat->bundle.push_back(i);
                        bStat->mainNode.push_back(clsParcel[i][j].parcelreq);
                        bStat->label.push_back(2);
                        bStat->label2.push_back(0);
                        bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta);
                        bStat->bundleVec.push_back(bStat->bundle);
                        clusters.push_back(bStat->bundleVec.size()-1);
                        bStat->activeDL.push_back(bStat->bundleVec.size()-1);
                        bStat->bundle.clear();
                    }
                    else{
                        bStat->bundle.push_back(i);
                        bStat->bundle.push_back(clsParcel[i][j].parcelreq);
                        bStat->mainNode.push_back(clsParcel[i][j].parcelreq);
                        bStat->label.push_back(2);
                        bStat->label2.push_back(1);
                        bStat->bundleTimes.push_back(nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta);
                        bStat->bundleVec.push_back(bStat->bundle);
                        clusters.push_back(bStat->bundleVec.size()-1);
                        bStat->activeDL.push_back(bStat->bundleVec.size()-1);
                        bStat->bundle.clear();
                        delNodes.push_back(clsParcel[i][j].parcelreq);                   
                    }
                }               
            }
            clusterVec.push_back(clusters);
            clusters.clear();
        }
        sort( pickNodes.begin(), pickNodes.end() );
        pickNodes.erase( unique( pickNodes.begin(), pickNodes.end() ), pickNodes.end() );

        for (int i = 2*inst->m + inst->n; i < nodeVec.size() - 1; i++){
            bStat->bundle.push_back(i);
            bStat->bundleVec.push_back(bStat->bundle);
            bStat->label.push_back(3);
            bStat->label2.push_back(-1);
            bStat->mainNode.push_back(i);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            
            for (int j = 0; j < pickNodes.size(); j++){
                bStat->bundle.push_back(i);
                bStat->bundle.push_back(pickNodes[j]);                
                bStat->bundleVec.push_back(bStat->bundle);
                bStat->label.push_back(5);
                bStat->label2.push_back(-1);
                bStat->mainNode.push_back(pickNodes[i]);
                clusters.push_back(bStat->bundleVec.size()-1);
                bStat->bundle.clear();
            }

            clusterVec.push_back(clusters);
            clusters.clear();
        }

        sort( delNodes.begin(), delNodes.end() );
        delNodes.erase( unique( delNodes.begin(), delNodes.end() ), delNodes.end() );

        for (int i = 0; i < delNodes.size(); i++){
            bStat->bundle.push_back(delNodes[i]);
            bStat->bundle.push_back(nodeVec.size() - 1);
            bStat->bundleVec.push_back(bStat->bundle);
            bStat->label.push_back(4);
            bStat->label2.push_back(-1);
            bStat->mainNode.push_back(delNodes[i]);
            // clusters.push_back(bStat->bundle);
            clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
        }

        bStat->bundle.push_back(nodeVec.size() - 1);
        bStat->bundleVec.push_back(bStat->bundle);
        bStat->label.push_back(3);
        bStat->label2.push_back(-1);
        bStat->mainNode.push_back(nodeVec.size() - 1);
        // clusters.push_back(bStat->bundle);
        clusters.push_back(bStat->bundleVec.size()-1);
        bStat->bundle.clear();
        
        clusterVec.push_back(clusters);
        clusters.clear();

        inst->endCluster = delNodes.size() + 1;
        inst->startCluster = pickNodes.size() + 1;
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
            if (bStat->bundleVec[i].back() == nodeVec.size() - 1){
                cost = nodeVec[bStat->bundleVec[i][0]].profit;
                bStat->bundleProfVec.push_back(cost);
                service = nodeVec[bStat->bundleVec[i][0]].delta;
                bStat->bundleServVec.push_back(service); 
            }
            // else if (bStat->bundleVec[i][0] >= inst->n + 2*inst->m){
            //     cost = nodeVec[bStat->bundleVec[i][1]].profit - mdist[bStat->bundleVec[i][0]][bStat->bundleVec[i][1]];
            //     bStat->bundleProfVec.push_back(cost);
            //     service = nodeVec[bStat->bundleVec[i][1]].delta;
            //     bStat->bundleServVec.push_back(service); 
            // }
            else{
                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    if (bStat->bundleVec[i][j] > inst->n - 1 && bStat->bundleVec[i][j] < inst->n + inst->m){ //parcel pickup
                        cost += nodeVec[bStat->bundleVec[i][j]].profit;
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
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;
                            }
                        }
                    } 
                }
                bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
            }
            else if (i >= setN){
                for (int j = 0; j < setN; j++){
                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
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
        // setN = bStat->bundleVec.size() - inst->K - inst->endCluster;
        if (p < 0){
            ref = 4*inst->m;
        }
        else{
            ref = p;
        }

        for(int i = 0; i < bStat->bundleVec.size() - inst->endCluster; i++){//i goes from 0 to the last starting node
            if (i > currentCluster*(ref + 1) + ref){
                currentCluster++;
            }
            if(i < setN){ //if bundle i is one with requests
                for (int j = 0; j < setN; j++){//for a bundle j that has requests
                    if (i != j){
                        if (j > currentCluster*(ref + 1) + ref || j < currentCluster*(ref + 1)){//if j is in a different cluster
                            if (bStat->label[i] == 1){//if i has a parcel pickup request
                                if (bStat->label[j] == 0){//if j has only passenger request
                                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                        bStat->bArcs[i][j] = true;
                                    }                                     
                                }
                                // else if (bStat->label[j] == 1){
                                //     if(bStat->mainNode[i] != bStat->mainNode[j]){  
                                //         bStat->bArcs[i][j] = true;
                                //     } 
                                // }
                                else if (bStat->label[j] == 2){//if j has a delivery request
                                    if (bStat->label2[i] == 1 && bStat->label2[j] == 0){//if the parcel request on i is located after the passenger req, and if the parcel delivery on j is located before the passenger req (direct trip for parcel)
                                        continue;
                                    }
                                    else{
                                        if (bStat->mainNode[j] == bStat->mainNode[i] + inst->m){ //if there is a non direct trip from pickup to its corresponding delivery (the vehicle cannot carry > 1 parcel at once in this scenario)
                                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                                bStat->bArcs[i][j] = true;
                                            }
                                        }                                      
                                    }
                                }
                            }
                            else if (bStat->label[i] == 2){//if i is a bundle with delivery request
                                if (bStat->label[j] == 0 || bStat->label[j] == 1){
                                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                        bStat->bArcs[i][j] = true;
                                    }  
                                }
                            }
                            else{//if i is a bundle with only passenger
                                if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                    bStat->bArcs[i][j] = true;
                                }                                   
                            }
                        }
                    } 
                }
                for(int j = setN + (inst->startCluster*inst->K); j < bStat->bundleVec.size()-1; j++){//for the dummy nodes with delivery
                    // cout << "here";
                    // getchar();
                    if (bStat->label[i] == 0){
                        // if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                           bStat->bArcs[i][j] = true;
                        // }
                    }
                    else if (bStat->label[i] == 1 && bStat->label2[i] == 0){
                        bStat->bArcs[i][j] = true;
                    }
                }
                if (bStat->label[i] == 0 || bStat->label[i] == 2){//for the dummy final node
                    bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
                }
            }
            else if (i >= setN){ //if bundle i has a starting point
                for (int j = 0; j < setN; j++){
                    if (bStat->bundleVec[i].size() > 1){
                        if (bStat->label[j] == 0){
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;
                            }
                        }
                        else if (bStat->label[j] == 2){
                            if (bStat->label2[j] == 1){
                                if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                    bStat->bArcs[i][j] = true;
                                }                                
                            }
                        }
                    }
                    else{
                        if (bStat->label[j] == 0 || bStat->label[j] == 1){
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) <= bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;
                            }
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
    int setN;
    int setS;
    // int setf;
    int ref;
    if (p < 0){
        if(problem->scen == "1A"){
            ref = inst->m;
            setN = bStat->bundleVec.size() - inst->K - 1;
        }
        else if (problem->scen == "2A"){
            ref = 4*inst->m;
            setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;
            setS = bStat->bundleVec.size() - inst->endCluster;
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
            else if(problem->scen == "2A"){
                if (i < setS){
                    clusterA++;
                }
                else{
                    if(i == setS){
                        clusterA++;
                        // cout << "i: " << i << " - cA: " << clusterA;
                        // getchar();
                    }
                }
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
                else if(problem->scen == "2A"){
                    if (j < setS){
                        clusterB++;
                        // cout << "j: " << j << " - cB: " << clusterB;
                        // getchar();
                    }
                    else{
                        if(j == setS){
                            clusterB++;
                            // cout << "j: " << j << " - cB: " << clusterB;
                            // getchar();
                        }
                    }
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

    else if (problem->scen == "2A"){
        int setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;
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
        for (int i = setN; i < bStat->bundleVec.size(); i++){
            if (bStat->bundleVec[i].size() <= 1){
                continue;
            }
            else{
                if (bStat->bundleVec[i].back() == nodeVec.size() - 1){
                    parcelReq = bStat->bundleVec[i][0];
                    bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);                    
                }
                else{
                    parcelReq = bStat->bundleVec[i][1];
                    bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);                     
                }

                // for (int j = 0; j < bStat->bundleVec[i].size(); j++){
                //     if (bStat->bundleVec[i][j] < inst->n){
                //         continue;
                //     }
                //     else{
                //         parcelReq = bStat->bundleVec[i][j];
                //         bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                //     }
                // }                
            }     
        }         
    }
}

void makeStartTimes (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){

    double parcelTime;
    double bundleTime;
    bool firstPassenger;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        parcelTime = 0;
        bundleTime = 0;
        firstPassenger = false;
        if (bStat->bundleVec[i].size() > 1){
            if(problem->scen == "2A"){
                if (bStat->label[i] == 4){//if the bundle is of type delivery-dummy
                    // cout << "Inside starting times:" << endl;
                    // cout << i << ": ";
                    bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
                    bStat->bundleStart.push_back(bundleTime);
                    // cout << bStat->bundleStart.size() << endl;
                    continue;   
                }
                else if (bStat->label[i] == 5){//if the bundle is of type start-pickup
                    // cout << "Inside starting times:" << endl;
                    // cout << i << ": ";
                    bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
                    bStat->bundleStart.push_back(bundleTime);
                    // cout << bStat->bundleStart.size() << endl;
                    continue;   
                }
            }
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
                    bps.cost = mdist[i][j];
                    bps.parcelreq = j;
                    bps.poslabel = 1;
                    vecOfDist.push_back(bps);
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j;
                    bps.poslabel = 0;
                    vecOfDist.push_back(bps);
                }
                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
                    bps.cost = mdist[i][j];
                    bps.parcelreq = j;
                    bps.poslabel = 1;
                    vecOfDist.push_back(bps);
                    bps.cost = mdist[j][i];
                    bps.parcelreq = j;
                    bps.poslabel = 0;
                    vecOfDist.push_back(bps);                   
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
            // break;
        }

        else{
            for (int i = 0; i < inst->n; i++){
                for (int j = inst->n; j < inst->n + inst->m; j++){
                    bps.parcelreq = j;
                    bps.poslabel = 0;
                    clsParcel[i].push_back(bps);
                    // bps.parcelreq = j;
                    bps.poslabel = 1;
                    clsParcel[i].push_back(bps);
                    // clsParcel[i].push_back(j);

                }
                vecOfDist.clear(); 

                for (int j = inst->n + inst->m; j < inst->n + 2*inst->m; j++){
                    bps.parcelreq = j;
                    bps.poslabel = 0;
                    clsParcel[i].push_back(bps);
                    bps.poslabel = 1;
                    clsParcel[i].push_back(bps);
                    // clsParcel[i].push_back(j);

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

void solStatIni(solStats *sStat){

    sStat->tParcel = 0;
    sStat->tPass = 0;
    sStat->tBoth = 0;
    sStat->tNone = 0;

    sStat->dParcel = 0;
    sStat->dPass = 0;
    sStat->dBoth = 0;
    sStat->dNone = 0;

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
    // getchar();
}



void mipSolStats (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    int load;
    double distPass;
    // load = 0;
    double dij;
    int currNode;
    int nextNode;
    
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

                else if (sStat->solInNode[k][i + 1] < inst->n + inst->m){
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

                else if (sStat->solInNode[k][i + 1] < inst->n + 2*inst->m){
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
            // cout << "\nTotal passenger time: " << sStat->tPass << endl;
            // cout << "\nTotal parcel time: " << sStat->tParcel << endl;
            // cout << "\nTotal combined transportation time: " << sStat->tBoth << endl;
            // cout << "\nTotal idle time: " << sStat->tNone << endl;

            // cout << "\nTotal passenger distance: " << sStat->dPass << endl;
            // cout << "\nTotal parcel distance: " << sStat->dParcel << endl;
            // cout << "\nTotal combined transportation distance: " << sStat->dBoth << endl;
            // cout << "\nTotal idle distance: " << sStat->dNone << endl;
            // getchar();

        }
    }
}

void printStats(instanceStat *inst, bundleStat *bStat, solStats *sStat){
    // for (int i = 0; i < inst->K; i++){

        cout << "\nTotal time: " << sStat->tPass + sStat->tParcel + sStat->tBoth + sStat->tNone << endl;
        cout << "\nTotal passenger time: " << sStat->tPass << endl;
        cout << "\nTotal parcel time: " << sStat->tParcel << endl;
        cout << "\nTotal combined transportation time: " << sStat->tBoth << endl;
        cout << "\nTotal idle time: " << sStat->tNone << endl;

        cout << "\nTotal distance: " << sStat->dPass + sStat->dParcel + sStat->dBoth + sStat->dNone << endl;
        cout << "\nTotal passenger distance: " << sStat->dPass << endl;
        cout << "\nTotal parcel distance: " << sStat->dParcel << endl;
        cout << "\nTotal combined transportation distance: " << sStat->dBoth << endl;
        cout << "\nTotal idle distance: " << sStat->dNone << endl;
    // }
}


void viewSol (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat){
    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    vector<int> auxSolOrder;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int currSP;
    vector<int> orderVec;

    for (int k = 0; k < inst->K; k++){
        sStat->solOrder.push_back(auxSolOrder);
    }

    for (int k = 0; k < inst->K; k++){
        currSP = inst->V - inst->K + k;

        for (int i = 0; i < sStat->solvec[k].size(); i++){
            auxPair.first = sStat->solvec[k][i].first;
            auxPair.second = sStat->solvec[k][i].second;            
            auxVec.push_back(auxPair);
        }
        // cout<< "here1";
        // getchar();
        // cout << "auxVec: " << endl;
        // for (int i = 0; i < auxVec.size(); i++){
        //     cout << auxVec[i].first << " " << auxVec[i].second << endl;
        // }
        while(!auxVec.empty()){
            if (sStat->solOrder[k].empty()){

                for (int i = 0; i < auxVec.size(); i++){
                    if (auxVec[i].first == currSP){
                        sStat->solOrder[k].push_back(auxVec[i].first);
                        sStat->solOrder[k].push_back(auxVec[i].second);

                        auxVec.erase(auxVec.begin()+i);

                    }
                }
            }
            else{

                for (int j = 0; j < auxVec.size(); j++){
                    if(auxVec[j].first == sStat->solOrder[k].back()){
                        sStat->solOrder[k].push_back(auxVec[j].second);

                        auxVec.erase(auxVec.begin()+j);
                    }
                }
            }       
        // cout<< "auxvec size: " << auxVec.size();
        // getchar();
        }
        // cout<< "here3";
        // getchar();
    }

    cout << "\nSolution: " << endl;

    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solOrder[k].size(); i++){
            if (i < sStat->solOrder[k].size() - 1){
                cout << sStat->solOrder[k][i] << " - ";
            }
            else{
                cout << sStat->solOrder[k][i];
            }
        }
        cout << endl;
    }
    cout << endl;
    // getchar();    
}
