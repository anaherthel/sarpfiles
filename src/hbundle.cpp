#include "hbundle.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>



void hbundle::orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist){
    //Order requests based on serving time
    
    bool added;

    if ((inst->n + 2*inst->m) >= 30){
        pairings = floor((inst->n + inst->m)*0.1);
    }
    else{
        pairings = floor((inst->n + inst->m)/2);
    }
    
    cout << "pairings: " << pairings << endl;

    twSort.push_back(nodeVec[0]);
    for (int i = 1; i < inst->n; i++){
        added = 0;
        twPos.push_back(0);
        // cout << "i: " << i << " - load: " << nodeVec[i].load << " - earlier: " << nodeVec[i].e << endl;
        for (int j = 0; j < twSort.size(); j++){
            if (nodeVec[i].e <= twSort[j].e){
                twSort.insert(twSort.begin() + j, nodeVec[i]);
                added = 1;
                break;
            }
        }
        if (!added){
            twSort.push_back(nodeVec[i]);
            twPos.push_back(twSort.size()-1);
        }

    }

    for (int i = 0; i < twSort.size(); i++){
        twPos[twSort[i].index] = i;
    }

    // for (int i = 0; i < twPos.size(); i++){
    //     cout << "Position of " << i << ": " << twPos[i] << endl;
    // }

    // cout << "\nTW sorted passengers: " << endl;
    // for (int i = 0; i < twSort.size(); i ++){
    //     cout << twSort[i].index << ": " << twSort[i].e << endl;

    // }
    // getchar();
}

void hbundle::buildDistVec(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist){

    vector<int> sDist;
    
    for (int i = 0; i < inst->n+2*inst->m; i++){
        proxSort.push_back(sDist);
    }

    double sumPD;

    // for (int i = inst->n; i < inst->m; i++){
    //     sumPD.push_back());
    // }
    bool added;

    for(int i = 0; i < inst->n+2*inst->m; i++){
        for (int j = 0; j < inst->n+2*inst->m; j++){
            added = 0;
            if (i != j){
                if (i < inst->n && j < inst->n){
                    if (twPos[i] > twPos[j]){
                        continue;
                    }                   
                }
                if (proxSort[i].empty()){
                    proxSort[i].push_back(j);
                    added = 1;
                    continue;
                }
                else{
                    for (int k = 0; k < proxSort[i].size(); k++){
                        if (Mdist[i][j] <= Mdist[i][proxSort[i][k]]){
                            proxSort[i].insert(proxSort[i].begin() + k, j);
                            added = 1;
                            break;
                        }
                    }
                }
                if (!added){
                    proxSort[i].push_back(j);
                }
            }
        }
    }

    cout << "Complete prox Vec: " << endl;
    for (int a = 0; a < proxSort.size(); a++){
        cout << "i: " << a << " -> ";
        for (int b = 0; b < proxSort[a].size(); b++){
            cout << proxSort[a][b] << " ";
        }
        cout << endl;
    }

}

void hbundle::buildBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){

    orderRequests(inst, nodeVec, Mdist);
    buildDistVec(inst, nodeVec, Mdist);
    int nextClose;
    int firstClose;
    
    int insertPos;

    bool valid = 0;
    
    // vector <int> passReq;
    int firstPass;

    bundleStat bStat;
    clSt cStat;

    vector< vector<bParcelStruct> > clsParcel;

    initVecs(inst,clsParcel,&bStat);

    for (int i = 0; i < inst->n; i++){
        bStat.bundle.push_back(i);
        bStat.bundleVec.push_back(bStat.bundle);
        cStat.clusters.push_back(bStat.bundleVec.size()-1);
        bStat.bundle.clear();
        bStat.bundle.push_back(i);     
        for (int j = 0; j < pairings; j++){
            firstClose = proxSort[i][j];

            bStat.bundle.push_back(firstClose);
        
            // cout << "i: " << i << " - closest to: " << firstClose << " which is " << nodeVec[firstClose].load << endl;
            // getchar();
            for (int k = 0; k < pairings; k++){
                nextClose = proxSort[firstClose][k];

                if (i != nextClose){
                    bStat.bundle.push_back(nextClose);
                    bStat.bundleTimes.push_back(nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
                    bStat.bundleVec.push_back(bStat.bundle);
                    cStat.clusters.push_back(bStat.bundleVec.size()-1);
                    bStat.bundle.pop_back();
                }
                else{
                    continue;
                }
            }
            if (bStat.bundle.size() > 1){
                bStat.bundle.pop_back();
            }
        }            
        cStat.clusterVec.push_back(cStat.clusters);
        cStat.clusters.clear();
        bStat.bundle.clear();
    }
    
    for (int i = inst->n; i < inst->n + inst->m*2; i++){
        bStat.bundle.push_back(i);     
        for (int j = 0; j < pairings; j++){
            valid  = 0;
            firstClose = proxSort[i][j];
            bStat.bundle.push_back(firstClose);
            if (firstClose < inst->n){
                valid = 1;
                firstPass = firstClose;
                // passReq.push_back(firstClose);
            }

            // cout << "i: " << i << " - closest to: " << proxSort[i][j] << " which is " << nodeVec[proxSort[i][j]].load << endl;
            // getchar();
            for (int k = 0; k < pairings; k++){
                nextClose = proxSort[firstClose][k];
                if (nextClose < inst->n){
                    if (firstClose < inst->n){
                        if (twPos[i] < twPos[firstClose]){
                            bStat.bundle.push_back(firstClose);
                        }
                        else{
                            continue;
                        }
                    }
                    if (!valid){
                        valid = 1;
                    }


                    // passReq.push_back(nextClose);
                }
                if (valid){
                    if (i != nextClose){
                        bStat.bundle.push_back(nextClose);

                        for(int l = 0; l < passReq.size(); l++){
                            cStat.clusterVec[l].push_back(bStat.bundle);
                            insertPos = cStat.clusterVec[l].size()-1;
                            // bStat.bundleTimes.push_back(nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
                            // bStat.bundleVec.push_back(bStat.bundle);
                            bStat.bundleTimes.push_back

                        }
                        bStat.bundle.pop_back();
                    }
                    else{
                        continue;
                    }
                }
            }
            bStat.bundle.pop_back();
        }            
        cStat.clusterVec.push_back(cStat.clusters);
        cStat.clusters.clear();
        bStat.bundle.clear();        
    }

    for (int a = 0; a < cStat.clusterVec.size(); a++){
        cout << "\nCluster " << a << ": [";
        for(int b = 0; b < cStat.clusterVec[a].size(); b++){
            cout << "(" << cStat.clusterVec[a][b] << ") " << "[";
            for (int c = 0; c < bStat.bundleVec[cStat.clusterVec[a][b]].size(); c++){
                cout << setw(3) << std:: right << bStat.bundleVec[cStat.clusterVec[a][b]][c];
                if (c < bStat.bundleVec[cStat.clusterVec[a][b]].size() - 1){
                    cout << ",";
                }
                else{
                    cout << "] ";
                }
            }
            
        }
        cout << "]" << endl;
    }
    getchar();

}

