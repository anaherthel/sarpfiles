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

    //Order requests based on TW
    //Define number of random insertions
    //Take random passenger, put on route. If not feasible, create another route
    //Remainder of passengers inserted in greedy manner.
    int insertions = floor(0.3*(inst->n));
    cout << "insertions: " << insertions << endl;
    getchar();
    int candidate;
    int counter = 1;
    bool feastime = 0;
    int sol_size = 0;
    double best_cost;
    int best_pos;
    int best_route;
    vector<int> inspositions; 
    int position;
    int vehicle;
    
    vehicle = solution.getvehicle();
    
    vector<int>::iterator iter;
    
    bool inserted;
    
    sarpRoute sroute(inst, vehicle);
    // sarpRoute sroute;
    
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


    sroute.insert(inst, Mdist, candidate, 1);
    sroute.calcCost(inst, nodeVec, Mdist);
    removeFromCL(candidate);
    sroute.updatePass(inst, nodeVec);
    sroute.updateLoad(inst, nodeVec);

    sroute.printLoad();
    getchar();

    solution.addRoute(&sroute);
    solution.updateVehicles();


    //random assignments
    while(counter < insertions){
        candidate = getRandRequestFromCL();
        // cout << "candidate number " << counter << ": "<< candidate  << endl;
        // getchar();
        sol_size = solution.getRoutesSize();
        inserted = 0;
        feastime = 0;
        //if pair of positions -1 -1, create another route without testing requests.
        for (int rid = 0; rid < sol_size; rid++){
            sroute = solution.getRoute(rid);
            // cout << "Current solution: " << endl;
            // for (auto a: sroute){
            //     cout << a << " - ";
            // }
            // cout << endl;
            // getchar();
            inspositions.clear();
            sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);
            
            /////////////////////////////////////////////
            // cout << "Available positions for insertion of candidate " << candidate << endl;
            // for(int i = 0; i < inspositions.size(); i++){
            //     cout << "Insertion positions: " << inspositions[i] << endl;
            // }
            ////////////////////////////////////////////
            
            for (int i = 0; i < inspositions.size(); i++){

                int pos = inspositions[i];
                feastime = sroute.testInsertion(inst, nodeVec, Mdist, pos, candidate);
                
                if (feastime){
                    sroute.insert(inst, Mdist, candidate, pos);
                    sroute.updatePass(inst, nodeVec);
                    sroute.updateLoad(inst, nodeVec);
                    sroute.updateTimes(inst, nodeVec, Mdist);
                    solution.updateRoutes(&sroute, rid);
                    solution.updateCost();
                    inserted = 1;
                    // cout << "*****Solution with insertion: " << endl;
                    // for (auto a: sroute){
                    //     cout << a << " - ";
                    // }
                    // cout << endl << endl;
                    break;
                }
            }
            if (inserted){
                break;
            }            
        }

        if (inserted){
            counter++;
            removeFromCL(candidate);
        }
        else{
            vehicle = solution.getvehicle();
            sarpRoute newroute(inst, vehicle);
            feastime = newroute.fInsertion(inst, nodeVec, Mdist, candidate);
            newroute.calcCost(inst, nodeVec, Mdist);
            newroute.insert(inst, Mdist, candidate, 1);
            newroute.updatePass(inst, nodeVec);
            newroute.updateLoad(inst, nodeVec);
            solution.addRoute(&newroute);
            solution.updateCost();
            removeFromCL(candidate);
            inserted = 1;
        } 
        // cout << "List of candidates so far: "  << endl;
        // if (CLpass.size() > 0){
        //     for (int c = 0; c < CLpass.size(); c++){
        //         cout << CLpass[c] << " ";
        //     }
        //     cout << endl;
        //     getchar();
        // }
        // else{        
        //     cout << "The list is empty."  << endl;
        // }
    }

    solution.printSol(inst);


    //greedy assignments
    inserted = 1;
    pair <int, double> cheapestpair;

    while (!CLpass.empty()){
        inserted = 0;

        for (int cand = 0; cand < CLpass.size(); cand++){
            candidate = CLpass[cand];
            // cout << "candidate: "<< candidate  << endl;
            // getchar();
            best_route = -1;
            sol_size = solution.getRoutesSize();
            best_cost = 100000;
            inserted = 0;
            feastime = 0;
            best_pos = -1;

            for (int rid = 0; rid < sol_size; rid++){
                cheapestpair.first = -1;
                cheapestpair.second = -1;
                sroute = solution.getRoute(rid);
                // cout << "Current route: " << endl;
                // for (auto a: sroute){
                //     cout << a << " - ";
                // }
                // cout << endl;
                // getchar();
                inspositions.clear();
                sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);
                
                /////////////////////////////////////////////
                // cout << "Available positions for insertion of candidate " << candidate << endl;
                // for(int i = 0; i < inspositions.size(); i++){
                //     cout << "Insertion positions: " << inspositions[i] << endl;
                // }
                ////////////////////////////////////////////
                
                cheapestpair = sroute.cheapestInsertion(inst, nodeVec, Mdist, candidate, inspositions);
                // cout << "Cheapest pair: " << cheapestpair.first << " - " << cheapestpair.second << endl;

                if (cheapestpair.second > 0 && cheapestpair.second < best_cost){
                    best_cost = cheapestpair.second;
                    best_route = rid;
                    best_pos = cheapestpair.first;
                    inserted = 1;
                }
            }

            // cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            // cout << "Request: " << candidate << endl;
            // cout << "Best cost: " << best_cost << endl;
            // cout << "Best route: " << best_route << endl;
            // cout << "Best position: " << best_pos<< endl;
            // cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            // getchar();

            if (best_cost == 100000){
                vehicle = solution.getvehicle();
                sarpRoute newroute(inst, vehicle);
                feastime = newroute.fInsertion(inst, nodeVec, Mdist, candidate);
                newroute.calcCost(inst, nodeVec, Mdist);
                newroute.insert(inst, Mdist, candidate, 1);
                newroute.updatePass(inst, nodeVec);
                newroute.updateLoad(inst, nodeVec);
                solution.addRoute(&newroute);
                removeFromCL(candidate);
                solution.updateVehicles();
                solution.updateCost();
                inserted = 1;
            }

            else{
                sroute = solution.getRoute(best_route);
                sroute.insert(inst, Mdist, candidate, best_pos);
                sroute.updatePass(inst, nodeVec);
                sroute.updateLoad(inst, nodeVec);
                sroute.updateTimes(inst, nodeVec, Mdist);
                solution.updateRoutes(&sroute, best_route);
                solution.updateCost();
                
                inserted = 1;
                // cout << "*****Route with insertion: " << endl;
                // for (auto a: sroute){
                //     cout << a << " - ";
                // }
                // cout << endl << endl;

                removeFromCL(candidate);
            }
        }

        cout << "List of candidates so far: "  << endl;
        if (CLpass.size() > 0){
            for (int c = 0; c < CLpass.size(); c++){
                cout << CLpass[c] << " ";
            }
            cout << endl;
            getchar();
        }
        else{        
            cout << "The list is empty."  << endl;
        }
        break;
    }
    cout << "Solution with passengers: " << endl;
    solution.printSol(inst);
    getchar();

    //inserting parcels
    //greedy assignments
    inserted = 1;

    cheapestpair.first = -1;
    cheapestpair.second = -1;
    
    //will hold best position and best cost for a parcel pickup and delivery.
    vector< pair <int, double> > bestMove;
    
    bestMove.push_back(cheapestpair);
    bestMove.push_back(cheapestpair);

    vector<int> inspositions2;

    int candidate2;

    while (inserted && !CLparc.empty()){
        inserted = 0;
        for (int cand = 0; cand < CLparc.size(); cand++){
            candidate = CLparc[cand];
            candidate2 = candidate + inst->m;
            cout << "candidate: "<< candidate  << " - candidate2: " << candidate2 << endl;
            getchar();
            best_route = -1;
            sol_size = solution.getRoutesSize();
            best_cost = 100000;
            inserted = 0;
            feastime = 0;
            best_pos = -1;

            for (int rid = 0; rid < sol_size; rid++){
                cheapestpair.first = -1;
                cheapestpair.second = -1;
                sroute = solution.getRoute(rid);
                cout << "Current route: " << endl;
                for (auto a: sroute){
                    cout << a << " - ";
                }
                cout << endl;
                getchar();
                inspositions.clear();
                inspositions2.clear();
                sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);
                if (problem->dParcel > 0){
                    inspositions2.push_back(inspositions[0] + 1);
                }
                else{
                    inspositions2.push_back(inspositions[0] + 2);
                }

                sroute.availablePos(inst, nodeVec, candidate2, problem, inspositions2);
                /////////////////////////////////////////////
                cout << "Available positions for insertion of candidate1: " << candidate << endl;
                for(int i = 0; i < inspositions.size(); i++){
                    cout << inspositions[i] << ", " << endl;
                }
                cout << "***********************************************" << endl;
                cout << "Available positions for insertion of candidate2: " << candidate2 << endl;
                for(int i = 0; i < inspositions2.size(); i++){
                    cout << inspositions2[i] << ", " << endl;
                }                
                ////////////////////////////////////////////
                getchar();

                cheapestpair = sroute.cheapestInsertion(inst, nodeVec, Mdist, candidate, inspositions);
                cout << "Cheapest pair: " << cheapestpair.first << " - " << cheapestpair.second << endl;

                if (cheapestpair.second > 0 && cheapestpair.second < best_cost){
                    best_cost = cheapestpair.second;
                    best_route = rid;
                    best_pos = cheapestpair.first;
                    inserted = 1;
                }
            }

            cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            cout << "Request: " << candidate << endl;
            cout << "Best cost: " << best_cost << endl;
            cout << "Best route: " << best_route << endl;
            cout << "Best position: " << best_pos<< endl;
            cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            getchar();

            if ( best_cost == 100000){
                if (problem->dParcel > 0){
                    vehicle = solution.getvehicle();
                    sarpRoute newroute(inst, vehicle);
                    feastime = newroute.fInsertion(inst, nodeVec, Mdist, candidate);
                    newroute.calcCost(inst, nodeVec, Mdist);
                    newroute.insert(inst, Mdist, candidate, 1);
                    newroute.updatePass(inst, nodeVec);
                    newroute.updateLoad(inst, nodeVec);
                    solution.addRoute(&newroute);
                    removeFromCL(candidate);
                    solution.updateVehicles();
                    inserted = 1;

                    cout << "List of candidates so far: "  << endl;
                    if (CLpass.size() > 0){
                        for (int c = 0; c < CLpass.size(); c++){
                            cout << CLpass[c] << " ";
                        }
                        cout << endl;
                        getchar();
                    }
                    else{        
                        cout << "The list is empty."  << endl;
                    }

                    break;
                }
                else{
                    continue;
                }
            }

            // else{
            //     sroute = solution.getRoute(best_route);
            //     sroute.insert(inst, Mdist, candidate, best_pos);
            //     sroute.updatePass(inst, nodeVec);
            //     sroute.updateLoad(inst, nodeVec);
            //     sroute.updateTimes(inst, nodeVec, Mdist);
            //     solution.updateRoutes(&sroute, best_route);

            //     inserted = 1;
            //     cout << "*****Route with insertion: " << endl;
            //     for (auto a: sroute){
            //         cout << a << " - ";
            //     }
            //     cout << endl << endl;

            //     removeFromCL(candidate);

            //     cout << "List of candidates so far: "  << endl;
            //     if (CLpass.size() > 0){
            //         for (int c = 0; c < CLpass.size(); c++){
            //             cout << CLpass[c] << " ";
            //         }
            //         cout << endl;
            //         getchar();
            //     }
            //     else{        
            //         cout << "The list is empty."  << endl;
            //     }
            //     break;
            // }
        }
    }
    cout << "Solution with passengers: " << endl;
    solution.printSol(inst);
    getchar();




}

void sarpConstruction::removeFromCL(int id) {
    for (int i = 0; i < CLpass.size(); ++i) {
        if (this->CLpass[i] == id) {
            this->CLpass.erase(this->CLpass.begin() + i);
            break;
        }
    }
}