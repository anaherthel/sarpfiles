#include "hbundle.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>



void hbundle::orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist){
    //Order requests based on serving time
    
    bool added;

    if ((inst->n + 2*inst->m) >= 30){
        pairings = floor((inst->n + 2*inst->m)*0.1);
    }
    else{
        pairings = floor((inst->n + 2*inst->m)/2);
    }
    
    cout << "pairings: " << pairings << endl;

    twSort.push_back(nodeVec[0]);
    for (int i = 1; i < inst->n; i++){
        added = 0;
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
        }

    }

    

    // cout << "\nTW sorted passengers: " << endl;
    // for (int i = 0; i < twSort.size(); i ++){
    //     cout << twSort[i].index << ": " << twSort[i].e << endl;

    // }
    // getchar();




}
