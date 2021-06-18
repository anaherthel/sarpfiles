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

    // for (int i = inst->n; i < inst->m; i++){
    //     sumPD.push_back());
    // }
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
                // if (i < inst->n){//if i is customer
                //     if (j < inst->n){ //if j is customer
                //         // if (problem->p1){ //if multiple customers can be served in a sequence
                //         //     if (twPos[i] > twPos[j]){
                //         //         continue;
                //         //     }
                //         // }
                //         // else{
                //         //     continue;
                //         // }
                //         continue;
                //     }
                
                // }
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

void hbundle::buildBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){

    orderRequests(inst, nodeVec, Mdist, problem);
    buildDistVec(inst, nodeVec, Mdist, problem);
    int nextClose;
    int firstClose;

    // int counter1, counter2; //number of pairings done; iterator for the proximity list

    // bundleStat bStat;
    // clSt cStat;

    // vector< vector<bParcelStruct> > clsParcel;

    // initVecs(inst,clsParcel,&bStat);

    // for (int i = 0; i < inst->n; i++){
    //     bStat.bundle.push_back(i);
    //     bStat.bundleVec.push_back(bStat.bundle);
    //     cStat.clusters.push_back(bStat.bundleVec.size()-1);
    //     bStat.bundle.clear();
    //     bStat.bundle.push_back(i);    
    //     for (int j = 0; j < pairings; j++){
    //         firstClose = proxSort[i][j];
    //         bStat.bundle.push_back(firstClose);
    //         counter1 = 0;
    //         counter2 = 0;
            
    //         // cout << "i: " << i << " - closest to: " << firstClose << " which is " << nodeVec[firstClose].load << endl;
    //         // getchar();
    //         while (counter1 < pairings && counter2 < proxSort[firstClose].size()){
    //             nextClose = proxSort[firstClose][counter2];

    //             if (i != nextClose){
    //                 if (nextClose < inst->n){
    //                     if (twPos[i] > twPos[nextClose]){
    //                         counter2++;
    //                         continue;
    //                     }
    //                 }
    //                 bStat.bundle.push_back(nextClose);
    //                 bStat.bundleTimes.push_back(nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
    //                 bStat.bundleVec.push_back(bStat.bundle);
    //                 // cStat.clusters.push_back(bStat.bundleVec.size()-1);
    //                 // cStat.clindex.push_back(i);
    //                 bStat.bundle.pop_back();
    //                 counter1++;
    //                 counter2++;
    //             }
    //             else{
    //                 counter2++;
    //                 continue;
    //             }
    //         }

    //         bStat.bundle.pop_back();
    //     }            
    //     // cStat.clusterVec.push_back(cStat.clusters);
    //     // cStat.clusters.clear();
    //     bStat.bundle.clear();
    // }

    // // cStat.clusterVec.push_back(cStat.clusters);
    // // cout << "First part:" << endl;
    
    // // for (int a = 0; a < cStat.clusterVec.size(); a++){
    // //     cout << "\nCluster " << a << ": [";
    // //     for(int b = 0; b < cStat.clusterVec[a].size(); b++){
    // //         cout << "(" << cStat.clusterVec[a][b] << ") " << "[";
    // //         for (int c = 0; c < bStat.bundleVec[cStat.clusterVec[a][b]].size(); c++){
    // //             cout << setw(3) << std:: right << bStat.bundleVec[cStat.clusterVec[a][b]][c];
    // //             if (c < bStat.bundleVec[cStat.clusterVec[a][b]].size() - 1){
    // //                 cout << ",";
    // //             }
    // //             else{
    // //                 cout << "] ";
    // //             }
    // //         }
            
    // //     }
    // //     cout << "]" << endl;
    // // }
    // // getchar();
    
    // for (int i = inst->n; i < inst->n + inst->m*2; i++){//i is parcel P or D
    //     bStat.bundle.push_back(i);     
    //     for (int j = 0; j < pairings; j++){
    //         counter1 = 0;
    //         counter2 = 0;
    //         firstClose = proxSort[i][j];
    //         bStat.bundle.push_back(firstClose);
    //         // if (firstClose < inst->n){
    //         //     hasPass = 1;
    //         //     firstPass = firstClose;
    //         // }


    //         // cout << "\n\ni: " << i << " - closest to: " << proxSort[i][j] << " which is " << nodeVec[proxSort[i][j]].load << endl;
    //         // getchar();
    //         while (counter1 < pairings && counter2 < proxSort[firstClose].size()){
    //             nextClose = proxSort[firstClose][counter2];
    //             // cout << "FC: " << firstClose << " - closest to: " << nextClose << " which is " << nodeVec[nextClose].load << endl;
    //             // getchar();
    //             if (nextClose < inst->n){
    //                 if (firstClose < inst->n){
    //                     if (twPos[firstClose] > twPos[nextClose]){//if both first and next close are passengers, we test their service sequence in time.
    //                         counter2++;
    //                         continue;
    //                     }
    //                 }
    //                 // if (!hasPass){
    //                 //     hasPass = 1;
    //                 //     firstPass = nextClose; //we assign the passenger number to the bundle
    //                 // }
    //             }

    //             if (i != nextClose){

    //                 bStat.bundle.push_back(nextClose);

    //                 cout << "\n\nFormed bundle: [";
    //                 for (int p = 0; p < bStat.bundle.size() - 1; p++){
    //                     cout << bStat.bundle[p] << ",";
    //                 }
    //                 cout << bStat.bundle[bStat.bundle.size()-1] << "]" << endl;
    //                 getchar();

    //                 bTimeTest(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bStat);
    //                 // if (hasPass){
    //                     // insertPos = 0;
    //                     // for (int k = 0; k < firstPass + 1; k++){
    //                     //     insertPos += cStat.clusterVec[k].size();
    //                     // }

    //                 bStat.bundleTimes.push_back(nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
    //                 bStat.bundleVec.push_back(bStat.bundle);

    //                     // bStat.bundleTimes.insert(bStat.bundleTimes.begin() + insertPos, nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
    //                     // bStat.bundleVec.insert(bStat.bundleVec.begin() + insertPos, bStat.bundle);                        
    //                     // cStat.clusterVec[firstPass].push_back(insertPos);
    //                 // }
    //                 // else{
    //                 //     bStat.bundleTimes.push_back(nodeVec[firstClose].delta + nodeVec[i].delta + nodeVec[nextClose].delta);
    //                 //     bStat.bundleVec.push_back(bStat.bundle);
    //                 //     // cStat.clusterVec[inst->n].push_back(bStat.bundleVec.size()-1);
    //                 // }                    

    //                 bStat.bundle.pop_back();
    //                 counter1++;
    //                 counter2++;
    //             }
    //             else{
    //                 counter2++;
    //                 continue;
    //             }

    //             cout << "\n\nAdding a single bundle: " << endl;
    //             for (int a = 0; a < bStat.bundleVec.size(); a++){
    //                 cout << "[";
    //                 for (int b = 0; b < bStat.bundleVec[a].size(); b++){
    //                     cout << setw(3) << std:: right << bStat.bundleVec[a][b];
    //                     if (b < bStat.bundleVec[a].size() - 1){
    //                         cout << ",";
    //                     }
    //                     else{
    //                         cout << "] ";
    //                     }
    //                 }
    //             }                    
    //             getchar();
    //         }
    //         bStat.bundle.pop_back();
    //     }

    //     // cStat.clusterVec.push_back(cStat.clusters);
    //     // cStat.clusters.clear();
    //     bStat.bundle.clear();        
    // }

}

void hbundle::orgBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){

    // for (int i = 0; i <= inst->n; i++){
    //     cStat.clusterVec.push_back(cStat.clusters);
    // }
    // cout << "Size of cluster vec: " <<  cStat.clusterVec.size() << endl;
    // getchar();

    // int request;
    
    // for (int i = 0; i < bStat.bundleVec.size(); i++){
    //     for (int j = 0; j < bStat.bundleVec[i].size(); j++){
    //         request = bStat.bundleVec[i][j];
    //         if (request < inst->n){ //if the request is a customer, add it to the vector to track which bundles have which customers.
    //             cStat.clusterVec[request].push_back(i);
    //         }
    //     }
    // }


    // cout << "\n\nOrganized bundles: " << endl;
    // int currBundle;
    // for (int i = 0; i < cStat.clusterVec.size(); i++){
    //     cout << "\nRequest " << a << ": [";
    //     for (int j = 0; j < cStat.clusterVec[i].size(); j++){
    //         currBundle = cStat.clusterVec[i][j];
    //         cout << "(" << currBundle << ") " << "[";
    //         for (int k = 0; k < bStat.bundleVec[currBundle].size(); k++){
    //             cout << setw(3) << std:: right << bStat.bundleVec[currBundle][k];
    //             if (k < bStat.bundleVec[currBundle].size() - 1){
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
}

void hbundle::bTimeTest(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat){
    // double parcelTime;
    // double bundleTime;
    // double endTime;
    // bool firstPassenger = 0;

    // // for (int i = 0; i < bStat->bundleVec.size(); i++){
    //     parcelTime = 0;
    //     bundleTime = 0;
    //     startingTime = 0;
    //     endTime = 0;
        
    //     cout << "\n\nFormed bundle: [";
    //     for (int p = 0; p < bStat.bundle.size() - 1; p++){
    //         cout << bStat.bundle[p] << ",";
    //     }
    //     cout << bStat.bundle[bStat.bundle.size()-1] << "]" << endl;
    //     getchar();

    //     if (bStat->bundle > 1){
    //         for (int i = 0; i < bStat->bundle.size() - 1; i++){
    //             if (bStat->bundle[0] < inst->n){//if the first element of the bundle is a passenger node
    //                 bundleTime = nodeVec[bStat->bundle[0]].e;
    //                 firstPassenger = 1;
    //             }
                    
    //             if (!firstPassenger){//if the 1st element of the bundle is not a passenger request
    //                 parcelTime += ((mdist[bStat->bundle[i]][bStat->bundle[i + 1]])/inst->vmed) + nodeVec[bStat->bundle[i]].delta;

    //                 if (bStat->bundleVec[i][j + 1] < inst->n){//if the next element is a passenger
    //                     bundleTime = nodeVec[bStat->bundleVec[i][j + 1]].e - parcelTime;
    //                     endTime += bundleTime + parcelTime;
    //                     firstPassenger = true;
    //                     // break;
    //                 }

    //                 else{
    //                     endTime += nodeVec[bStat->bundleVec[i][j]].delta + ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
    //                 }
    //             }
    //             else{
    //                 endTime += nodeVec[bStat->bundleVec[i][j]].delta + ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
    //             }
    //         }
    //         endTime += nodeVec[bStat->bundleVec[i].back()].delta;
    //         bStat->bundleEnd.push_back(endTime);

    //         if (firstPassenger){
    //             bStat->bundleStart.push_back(bundleTime);
    //         }
            
    //     }

    //     else{
    //         firstPassenger = true;
    //         bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
    //         bStat->bundleStart.push_back(bundleTime);
    //         bundleTime = nodeVec[bStat->bundleVec[i][0]].e + nodeVec[bStat->bundleVec[i][0]].delta;
    //         bStat->bundleEnd.push_back(bundleTime);
    //         continue;
    //     }
    //     // bStat->bundleStart.push_back(bundleTime); -----> Push back after all tests of feasibility are performed
    // }
}