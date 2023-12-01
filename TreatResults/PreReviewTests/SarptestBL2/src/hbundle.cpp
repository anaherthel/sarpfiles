#include "hbundle.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>



void hbundle::orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist){
    //Order requests based on serving time

    for (int i = 0; i < nodeVec.size(); i++){
        cout << "i: " << i << " - load: " << nodeVec[i]->load << " - earlier: " << nodeVec[i].e << endl;
        getchar();
    }
}
