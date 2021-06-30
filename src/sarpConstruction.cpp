#include "sarpConstruction.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>

//Based on pickup location of parcels only.
sarpConstruction::sarpSolution(const instanceStat *inst) {

    for (int i = 0; i < inst->n; i++) {
        CLpass.insert(i);
    }
    for (int i = inst->n; i < inst->n + inst->m; i++) {
        CLparc.insert(i);
    }
}

void sarpConstruction::ConstrProc(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){

    //Order requests based on TW(?)
    //Define number of random insertions
    //Take random passenger, put on route. If not feasible, create another route
    //Remainder of passengers inserted in greedy manner.
    int insertions = floor(0.3*nodeVec.size());
    int candidate;
    int counter = 0;
    //random assignments
    while(counter < insertions){
        candidate = getRandRequestFromCL();
        

        counter++;

    }
}

int sarpConstruction::getRandRequestFromCL() {
    int random_position = rand() % this->CLpass.size();
    int random_request  = this->CLpass[random_position];
    return random_request;
}