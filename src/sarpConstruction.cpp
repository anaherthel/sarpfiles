#include "sarpConstruction.h"
#include "readdata.h"

#include <cstdlib>
#include <stdio.h>

//Based on pickup location of parcels only.

bool sarpConstruction::compare(int a, int b) {
    return passNodes[a].e < passNodes[b].e;
}
//Based on pickup location of parcels only.

sarpConstruction::sarpConstruction(instanceStat *inst, vector<nodeStat> &nodeVec) {

    for (int i = 0; i < inst->n; i++) {
        CLpass.push_back(i);
        passNodes.push_back(nodeVec[i]);
    }
    for (int i = inst->n; i < inst->n + inst->m; i++) {
        CLparc.push_back(i);
    }
}

int sarpConstruction::getRandRequestFromCL() {
    int random_position = rand() % this->CLpass.size();
    int random_request  = this->CLpass[random_position];
    return random_request;
}

void sarpConstruction::ConstrProc(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem){

    //Order requests based on TW(?)
    //Define number of random insertions
    //Take random passenger, put on route. If not feasible, create another route
    //Remainder of passengers inserted in greedy manner.
    int insertions = floor(0.3*nodeVec.size());
    int candidate;
    int counter = 0;
    bool feastime = 0;
    bool feasload = 0;
    int sol_size = 0;
    double best_cost;
    vector<int> inspositions;
    int ftestpos, ltestpos;
    int position;

    bool inserted;
    
    sarpRoute sroute(inst);

    problem->seed = 12;
    srand(problem->seed);

    sort(CLpass.begin(), CLpass.end(), [this](int a, int b) {
        return compare(a, b);
    });

    cout << "Candidate list (passengers)" << endl;
    for (int i = 0; i < CLpass.size(); i++){
        cout << CLpass[i] << " ";
    }
    cout << endl;
    getchar();

    // cout << "Solution: " << endl;
    // for (auto i: sroute){
    //     cout << i << " - ";
    // }
    // cout << endl;
    // getchar();

    //first insertion
    ///////////////////////////////////////////////////////////////////////////
    //Maybe we need to change the depot at some point if no passenger is able
    //to be delivered by the first vehicle.
    ///////////////////////////////////////////////////////////////////////////

    while(!feastime){
        candidate = getRandRequestFromCL();

        feastime = sroute.fInsertion(inst, nodeVec, Mdist, candidate);
    }

    sroute.calcCost(inst, nodeVec, Mdist);
    sroute.insert(inst, Mdist, candidate, 1);
    sroute.updatePass(inst, nodeVec);
    sroute.updateLoad(inst, nodeVec);


    cout << "Solution with insertion: " << endl;
    for (auto i: sroute){
        cout << i << " - ";
    }
    cout << endl;
    getchar();

    sroute.printLoad();
    getchar();
    solution.addRoute(&sroute);

    //random assignments
    while(counter < insertions){
        candidate = getRandRequestFromCL();
        cout << "candidate number " << counter << ": "<< candidate  << endl;
        getchar();
        sol_size = solution.getRoutesSize();
        best_cost = 100000;
        inserted = 0;
        feastime = 0;
        feasload = 0;
        //if pair of positions -1 -1, create another route without testing requests.
        for (int rid = 0; rid < sol_size; rid++){
            sroute = solution.getRoute(rid);
            inspositions.clear();
            sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);
            
            /////////////////////////////////////////////
            cout << "Available positions for insertion of candidate " << candidate << endl;
            for(int i = 0; i < inspositions.size(); i++){
                cout << "Insertion positions: " << inspositions[i] << endl;
            }
            ////////////////////////////////////////////
            
            for (int i = 0; i < inspositions.size(); i++){

                int pos = inspositions[i];
                feastime = sroute.testInsertion(inst, nodeVec, Mdist, pos, candidate);
                
                if (feastime){
                    sroute.insert(inst, Mdist, candidate, 1);
                    sroute.updatePass(inst, nodeVec);
                    sroute.updateLoad(inst, nodeVec);
                    inserted = 1;
                    sroute.updateTimes(inst, nodeVec, Mdist);
                    break;
                }
            }
            if (inserted){
                break;
            }
            else{

            }
            
        }

        if (inserted){
            counter++;
        }
        

    }

    //greedy assignments

    // while (insertion && !CL.empty()){

    // }
}