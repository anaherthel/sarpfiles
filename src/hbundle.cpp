#include "hbundle.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>



void hbundle::orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist,  probStat *problem){
    //Order requests based on serving time
    
    bool added;

    if ((inst->n + 2*inst->m) >= 30){
        pairings = floor((inst->n + inst->m)*0.1);
    }
    else{
        pairings = floor((inst->n + inst->m)/2);
    }
    
    // cout << "pairings: " << pairings << endl;

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

    cout << "\nTW sorted passengers: " << endl;
    for (int i = 0; i < twSort.size(); i ++){
        cout << twSort[i].index << ": " << twSort[i].e << endl;

    }
    getchar();
}

void hbundle::buildDistVec(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat *problem){

    vector<int> sDist;
    
    for (int i = 0; i < inst->n+2*inst->m; i++){
        proxSort.push_back(sDist);
    }

    double sumPD;
    bool added;
    int initialj;
    for(int i = 0; i < inst->n+2*inst->m; i++){
        if (i < inst->n){
            initialj = inst->n;
        }
        else{
            initialj = 0;
        }
        for (int j = initialj; j < inst->n+2*inst->m; j++){
            added = 0;
            if (i != j){
                if (nodeVec[i].load > 0){ //if i is a parcel pickup
                    if (nodeVec[j].load > 0){ //if j is a parcel pickup
                        if (!problem->p2){ //if the problem does not allow multiple parcels
                            continue;
                        }
                    }
                    else if (i == j - inst->m){ //if j is the delivery node of i
                        if (!problem->dParcel){ //if the problem does not allow for direct delivery
                            continue;
                        }
                    }
                }

                else if (nodeVec[i].load < 0){//if i is a delivery node
                    if (j == i - inst->m){// if j is i's pickup node
                        continue;
                    }
                    if (nodeVec[j].load < 0){ //if j is a parcel delivery
                        if (!problem->p2){ //if the problem does not allow multiple parcels
                            continue;
                        }
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

void hbundle::buildBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem, bundleStat* bStat){

    int nextClose;
    int firstClose;
    int passReq;
    double bundleTime;

    int counter1, counter2; //number of pairings done; iterator for the proximity list
    bool valid;
    bool hasPass, hasPU, hasDL;

    for (int i = 0; i < inst->n; i++){//i is a customer request
        bStat->bundle.push_back(i);
        
        bundleTime = nodeVec[i].e;
        bStat->bundleStart.push_back(bundleTime);
        bundleTime += nodeVec[i].delta;
        bStat->bundleEnd.push_back(bundleTime);

        bStat->bundleVec.push_back(bStat->bundle);
        bStat->vecofPass.push_back(i);
        for (int j = 0; j < pairings; j++){
            firstClose = proxSort[i][j];
            bStat->bundle.push_back(firstClose);
            counter1 = 0;
            counter2 = 0;
            
            while (counter1 < pairings && counter2 < proxSort[firstClose].size()){
                nextClose = proxSort[firstClose][counter2];

                if (i != nextClose){
                    if (nextClose < inst->n){
                        counter2++;
                        continue;
                    }

                    bStat->bundle.push_back(nextClose);
                    bTimeTest(inst, nodeVec, Mdist, bStat, valid);

                    if (!valid){
                        bStat->bundle.pop_back();
                        counter2++;
                        continue;
                    }
                    else{
                        bStat->bundleVec.push_back(bStat->bundle);
                        bStat->vecofPass.push_back(i);
                        bStat->bundle.pop_back();
                        counter1++;
                        counter2++;
                    }
                }
                else{
                    counter2++;
                    continue;
                }
            }

            bStat->bundle.pop_back();
        }            

        bStat->bundle.clear();
    }
    
    for (int i = inst->n; i < inst->n + inst->m*2; i++){//i is parcel P or D
        bStat->bundle.push_back(i);
        hasPU = 0;
        hasDL = 0;
        passReq = -1;
        if (i < inst->n + inst->m){
            hasPU = 1;
        }
        else if (i < inst->n + 2*inst->m){
            hasDL = 1;
        }

        for (int j = 0; j < pairings; j++){
            hasPass = 0;
            counter1 = 0;
            counter2 = 0;
            firstClose = proxSort[i][j];
            
            bStat->bundle.push_back(firstClose);

            if (firstClose < inst->n){
                hasPass = 1;
                passReq = firstClose;
            }

            while (counter1 < pairings && counter2 < proxSort[firstClose].size()){
                nextClose = proxSort[firstClose][counter2];
                if (!hasPass){
                    passReq = -1;
                }

                // cout << "\n\nBundle so far: " << endl;
                // for (int a = 0; a < bStat->bundle.size(); a++){
                //     cout << bStat->bundle[a] << " - ";
                // }
                // cout << endl;
                // cout << "\n\nNext close candidate: " << nextClose << endl;
                // getchar();

                if (nextClose < inst->n){
                    if (hasPass){
                        counter2++;
                        continue;
                    }
                    else{
                        passReq = nextClose;
                    }
                }
                else if (!problem->p2){//if the problem does not allow multiple parcels in
                //the vehicle at once
                    if (firstClose < inst->n + inst->m){
                        if (hasPU){
                            counter2++;
                            continue;
                        }
                    }   
                    else if (firstClose < inst->n + 2*inst->m){
                        if (hasDL){
                            counter2++;
                            continue;
                        }
                    }
                }

                if (i != nextClose){
                    bStat->bundle.push_back(nextClose);

                    bTimeTest(inst, nodeVec, Mdist, bStat, valid);

                    if (!valid){
                        bStat->bundle.pop_back();
                        counter2++;
                        continue;
                    }
                    else{
                        bStat->bundleVec.push_back(bStat->bundle);
                        
                        bStat->bundle.pop_back();
                        counter1++;
                        counter2++;

                        bStat->vecofPass.push_back(passReq);                        
                    }
                }
                else{
                    counter2++;
                    continue;
                }

                // cout << "\n\nAdding a single bundle: " << endl;
                // for (int a = 0; a < bStat->bundleVec.size(); a++){
                //     cout << "[";
                //     for (int b = 0; b < bStat->bundleVec[a].size(); b++){
                //         cout << setw(3) << std:: right << bStat->bundleVec[a][b];
                //         if (b < bStat->bundleVec[a].size() - 1){
                //             cout << ",";
                //         }
                //         else{
                //             cout << "] ";
                //         }
                //     }
                // }                    
                // getchar();
            }
            bStat->bundle.pop_back();
        }

        // cStat.clusterVec.push_back(cStat.clusters);
        // cStat.clusters.clear();
        bStat->bundle.clear();        
    }

    // cout << "\n\nComplete vector: " << endl;
    // for (int a = 0; a < bStat->bundleVec.size(); a++){
    //     cout << "(" << a << "): " << "[";
    //     for (int b = 0; b < bStat->bundleVec[a].size(); b++){
    //         cout << setw(3) << std:: right << bStat->bundleVec[a][b];
    //         if (b < bStat->bundleVec[a].size() - 1){
    //             cout << ",";
    //         }
    //         else{
    //             cout << "] ";
    //         }
    //     }
    //     cout << " || ";
    // }                    
    // getchar();

}

void hbundle::orgBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, clSt* cStat, probStat* problem){
    for (int i = 0; i <= inst->n; i++){
        cStat->clusterVec.push_back(cStat->clusters);
    }
    // cout << "\n\nSize of cluster vec: " <<  cStat->clusterVec.size() << endl;
    // getchar();
    // for (int i = 0; i < bStat->vecofPass.size(); i++){
    //     cout << "(" << i << "): " << bStat->vecofPass[i] << " || ";
    // }

    int request;

    for (int i = 0; i < bStat->vecofPass.size(); i++){
        request = bStat->vecofPass[i];
        if (request >= 0){ //if the request is a customer, add it to the vector to track which bundles have which customers.
            cStat->clusterVec[request].push_back(i);
        }
        else{
            cStat->clusterVec[inst->n].push_back(i);
        }
    }

    cout << "\n\nOrganized bundles: " << endl;
    int currBundle;
    for (int i = 0; i < cStat->clusterVec.size(); i++){
        cout << "\nRequest " << i << ": [";
        for (int j = 0; j < cStat->clusterVec[i].size(); j++){
            currBundle = cStat->clusterVec[i][j];
            cout << "(" << currBundle << ") " << "[";
            for (int k = 0; k < bStat->bundleVec[currBundle].size(); k++){
                cout << setw(3) << std:: right << bStat->bundleVec[currBundle][k];
                if (k < bStat->bundleVec[currBundle].size() - 1){
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

void hbundle::bTimeTest(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, bool &valid){
    double prevTime, postTime, startTime, endTime, deltaTime;
    bool hasPass = 0;
    int passPos, passReq;

    vector <int> prevReq;
    vector <int> postReq;
    vector <int> orBundle;

    prevTime = 0;
    postTime = 0;
    startTime = 0;
    endTime = 0;
    deltaTime = 0;

    // bStat->bundle.clear();
    // bStat->bundle.push_back(12);
    // bStat->bundle.push_back(11);
    // bStat->bundle.push_back(1);

    for (int i = 0; i < bStat->bundle.size(); i++){
        if (bStat->bundle[i] < inst->n){    
            hasPass = 1;
            passPos = i;
            passReq = bStat->bundle[i];
            continue;
        }
        if (!hasPass){
            prevReq.push_back(bStat->bundle[i]);
        }
        else{
            postReq.push_back(bStat->bundle[i]);
        }
    }

    if (prevReq.size() > 0){
        for (int i = 0; i < prevReq.size() - 1; i++){
            prevTime += ((Mdist[prevReq[i]][prevReq[i + 1]])/inst->vmed) + nodeVec[prevReq[i]].delta;    
        }
    //if there are requests before the passenger, we add the time from the last prev to the
    //passenger, as well as the delta for the last of the prev requests.

        prevTime += ((Mdist[prevReq.back()][passReq])/inst->vmed) + nodeVec[prevReq.back()].delta;
    }

    if (postReq.size() > 0){
        for (int i = 0; i < postReq.size() - 1; i++){
            postTime += ((Mdist[postReq[i]][postReq[i + 1]])/inst->vmed) + nodeVec[postReq[i]].delta;
        }
    //if there are requests after the passenger, we add the time from the passenger to the first
    //post request, as well as the delta for the last of the post requests.

        postTime += ((Mdist[passReq][postReq[0]])/inst->vmed) + nodeVec[postReq.back()].delta;
    }

    if (hasPass){
        startTime = nodeVec[passReq].e - prevTime;
        endTime = nodeVec[passReq].e + nodeVec[passReq].delta + postTime;
        deltaTime = prevTime + postTime + nodeVec[passReq].delta;
    }
    else{
        startTime = nodeVec[bStat->bundle[0]].e;
        endTime = nodeVec[bStat->bundle.back()].l;
        deltaTime = prevTime + postTime;
    }

    //testing feasibility of bundle in regards to time constraints:
    //if it starts after 0h; if the max driving time is not exceeded
    //if the end horizon is not exceeded.
    if (startTime >= 0 && deltaTime <= inst->maxTime && endTime <= inst->T){
        valid = 1;
        bStat->bundleStart.push_back(startTime);
        bStat->bundleEnd.push_back(endTime);
    }
    else{
        valid = 0;
    }
}

void hbundle::hbundleMethod(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist,  probStat *problem){
    bundleStat bStat;
    clSt cStat;
    vector< vector<bParcelStruct> > clsParcel;

    orderRequests(inst, nodeVec, Mdist, problem);
    buildDistVec(inst, nodeVec, Mdist, problem);

    initVecs(inst,clsParcel,&bStat);

    buildBundles(inst, nodeVec, Mdist, problem, &bStat);

    orgBundles(inst, nodeVec, Mdist, &bStat, &cStat, problem);

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

    bundleProfit(inst, Mdist, nodeVec, &bStat);

    // cout << "\nBundle Profit: " << endl;
    // for (int i = 0; i < bStat.bundleProfVec.size(); i++){
    //     cout << i << ": " << setw(3) << std:: right << bStat.bundleProfVec[i] << endl;
    // }
    // cout << endl;

    // getchar();

    makeBundleReference(inst, Mdist, nodeVec, &bStat);

    // cout << "\nBundle Service Times: ";
    // for (int i = 0; i < bStat.bundleServVec.size(); i++){
    //  cout << i  << ": " <<   bStat.bundleServVec[i] << endl;
    // }
    // cout << endl;

    // getchar();

    initArcs(inst, &bStat, &cStat);

    feasibleBundleArcs(inst, Mdist, nodeVec, &bStat, &cStat, p, problem);

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

    // for (int i = 0; i < cStat.clusterVec.size(); i++){
    //     cout << "\nCluster " << i << ": [";
    //     for(int k = 0; k < cStat.clusterVec[i].size(); k++){
    //         cout << "(" << cStat.clusterVec[i][k] << ") " << "[";
    //         for (int j = 0; j < bStat.bundleVec[cStat.clusterVec[i][k]].size(); j++){
    //             cout << setw(3) << std:: right << bStat.bundleVec[cStat.clusterVec[i][k]][j];
    //             if (j < bStat.bundleVec[cStat.clusterVec[i][k]].size() - 1){
    //                 cout << ",";
    //             }
    //             else{
    //                 cout << "] ";
    //             }
    //         }
            
    //     }
    //     cout << "]" << endl;
    // }
    // getchar();

    feasibleClusterArcs(inst, nodeVec, &bStat, &cStat, p, problem);



    // cout<< "\nFeasible arcs between clusters:" << endl;
    // for (int i = 0; i < cStat.clusterVec.size(); i++){
    //  if (i == 0){
    //      cout << setw(3) << " ";
    //  }
    //  cout << setw(3) << std::right << i;
    // }
    // cout << endl;
    // for (int i = 0; i < cStat.clusterVec.size(); i++){
    //  cout << setw(3) << std::right << i;
    //  for (int j = 0; j < cStat.clusterVec.size(); j++){
    //      cout << setw(3) << std:: right << cStat.cArcs[i][j];
    //  }
    //  cout << endl;
    // }

    // getchar();

    ///////////////////////////////////////
    


    // mipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat);
    
    // if(sStat->feasible){
    //     // solStatIni(sStat);

    //     nodeSolution (inst, mdist, &bStat, nodeVec, sStat);
        
    //     stillTimeBundle(inst, mdist, &bStat, nodeVec, sStat);

    //     mipSolStats2 (inst, mdist, &bStat, nodeVec, sStat);

    //     // // cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

    //     printStats(inst, sStat);
    // }

    for ( int i = 0; i < inst->V + inst->dummy; i++) {
        delete[] Mdist[i];
    }
    delete[] Mdist;
}