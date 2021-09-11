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

void sarpConstruction::ConstrProc(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem, sarpSolution *solution){

    //Order requests based on TW
    //Define number of random insertions
    //Take random passenger, put on route. If not feasible, create another route
    //Remainder of passengers inserted in greedy manner.
    int insertions = floor(0.3*(inst->n));
    // cout << "insertions: " << insertions << endl;
    // getchar();
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
    
    vehicle = solution->getvehicle();
    
    vector<int>::iterator iter;
    
    bool inserted;
    
    sarpRoute sroute(inst, vehicle);
    // sarpRoute sroute;
    
    // problem->seed = 12;
    // srand(problem->seed);

    sort(CLpass.begin(), CLpass.end(), [this](int a, int b) {
        return compare(a, b);
    });

    // cout << "Candidate list (passengers)" << endl;
    // for (int i = 0; i < CLpass.size(); i++){
    //     cout << CLpass[i] << " ";
    // }
    // cout << endl;
    // getchar();

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


    sroute.insert(inst, Mdist, candidate, 1, nodeVec[candidate].profit);
    sroute.calcCost(inst, nodeVec, Mdist);
    // cout << "route cost: " << sroute.cost() << endl;
    // getchar();
    removeFromCL(candidate, 1);
    sroute.updatePass(inst, nodeVec);
    sroute.updateLoad(inst, nodeVec);

    // sroute.printLoad();
    // getchar();

    solution->addRoute(&sroute);
    solution->updateVehicles();
    solution->updateCost();

    // cout << "First addition" << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // getchar();
    //random assignments
    while(counter < insertions){
        candidate = getRandRequestFromCL();
        // cout << "candidate number " << counter << ": "<< candidate  << endl;
        // getchar();
        sol_size = solution->getRoutesSize();
        inserted = 0;
        feastime = 0;
        //if pair of positions -1 -1, create another route without testing requests.
        for (int rid = 0; rid < sol_size; rid++){
            sroute = solution->getRoute(rid);
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
                    sroute.insert(inst, Mdist, candidate, pos, nodeVec[candidate].profit);
                    sroute.updatePass(inst, nodeVec);
                    sroute.updateLoad(inst, nodeVec);
                    sroute.updateTimes(inst, nodeVec, Mdist);
                    solution->updateRoutes(&sroute, rid);
                    solution->updateCost();
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
            removeFromCL(candidate, 1);
        }
        else{
            vehicle = solution->getvehicle();
            sarpRoute newroute(inst, vehicle);
            feastime = newroute.fInsertion(inst, nodeVec, Mdist, candidate);
            newroute.calcCost(inst, nodeVec, Mdist);
            newroute.insert(inst, Mdist, candidate, 1, nodeVec[candidate].profit);
            newroute.updatePass(inst, nodeVec);
            newroute.updateLoad(inst, nodeVec);
            solution->addRoute(&newroute);
            solution->updateCost();
            removeFromCL(candidate, 1);
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

    // cout << "Random additions: " << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // getchar();

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
            sol_size = solution->getRoutesSize();
            best_cost = -100000;
            inserted = 0;
            feastime = 0;
            best_pos = -1;

            for (int rid = 0; rid < sol_size; rid++){
                cheapestpair.first = -1;
                cheapestpair.second = -100000;
                sroute = solution->getRoute(rid);
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

                if (cheapestpair.second > -100000 && cheapestpair.second > best_cost){
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

            if (best_cost == -100000){
                vehicle = solution->getvehicle();
                sarpRoute newroute(inst, vehicle);
                feastime = newroute.fInsertion(inst, nodeVec, Mdist, candidate);
                newroute.calcCost(inst, nodeVec, Mdist);
                newroute.insert(inst, Mdist, candidate, 1, nodeVec[candidate].profit);
                newroute.updatePass(inst, nodeVec);
                newroute.updateLoad(inst, nodeVec);
                newroute.updateTimes(inst, nodeVec, Mdist);
                solution->addRoute(&newroute);
                removeFromCL(candidate, 1);
                solution->updateVehicles();
                solution->updateCost();
                inserted = 1;
                break;
            }

            else{
                sroute = solution->getRoute(best_route);
                sroute.insert(inst, Mdist, candidate, best_pos, nodeVec[candidate].profit);
                sroute.updatePass(inst, nodeVec);
                sroute.updateLoad(inst, nodeVec);
                sroute.updateTimes(inst, nodeVec, Mdist);
                solution->updateRoutes(&sroute, best_route);
                solution->updateCost();
                
                inserted = 1;
                // cout << "*****Route with insertion: " << endl;
                // for (auto a: sroute){
                //     cout << a << " - ";
                // }
                // cout << endl << endl;

                removeFromCL(candidate, 1);
                break;
            }
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
    // cout << "Solution with passengers: " << endl;
    // solution->printSol(inst);
    // solution->printCosts();
    // getchar();

    //inserting parcels
    //greedy assignments
    inserted = 1;

    cheapestpair.first = -1;
    cheapestpair.second = -100000;
    
    //will hold best position and best cost for a parcel pickup and delivery.
    vector< pair <int, double> > cheapestMove;
    
    cheapestMove.push_back(cheapestpair);
    cheapestMove.push_back(cheapestpair);

    vector<int> inspositions2;

    int candidate2;
    pair <int, int> bestpositions;

    while (inserted && !CLparc.empty()){
        inserted = 0;
        for (int cand = 0; cand < CLparc.size(); cand++){
            candidate = CLparc[cand];
            candidate2 = candidate + inst->m;
            // cout << "candidate: "<< candidate  << " - candidate2: " << candidate2 << endl;
            // getchar();
            best_route = -1;
            sol_size = solution->getRoutesSize();
            best_cost = -100000;
            inserted = 0;
            feastime = 0;
            bestpositions.first = -1;
            bestpositions.second = -1;

            for (int rid = 0; rid < sol_size; rid++){
                //reseting best move vector
                cheapestMove[0].first = -1;
                cheapestMove[0].second = -100000;
                cheapestMove[1].first = -1;
                cheapestMove[1].second = -100000;

                sroute = solution->getRoute(rid);
                // cout << "Current route: " << endl;
                // for (auto a: sroute){
                //     cout << a << " - ";
                // }
                // cout << endl;
                // getchar();
                inspositions.clear();
                inspositions2.clear();
                
                sroute.availablePos(inst, nodeVec, candidate, problem, inspositions);

                // /////////////////////////////////////////////
                // cout << "Available positions for insertion of candidate1: " << candidate << endl;
                // for(int i = 0; i < inspositions.size(); i++){
                //     cout << inspositions[i] << ", ";
                // }
                // cout << endl;
                // cout << "***********************************************" << endl;               
                // ////////////////////////////////////////////
                // getchar();

                sroute.cheapestInsertionParcel(inst, nodeVec, Mdist, candidate, candidate2, inspositions, inspositions2, cheapestMove, problem);
                
                if (cheapestMove[0].first == cheapestMove[1].first){
                    if (cheapestMove[0].second > best_cost){
                        best_cost = cheapestMove[0].second;
                        best_route = rid;
                        bestpositions.first = cheapestMove[0].first;
                        bestpositions.second = cheapestMove[1].first;
                        inserted = 1;
                    }                    
                }
                else{
                    if (cheapestMove[0].second + cheapestMove[1].second > best_cost){
                        best_cost = cheapestMove[0].second + cheapestMove[1].second;
                        best_route = rid;
                        bestpositions.first = cheapestMove[0].first;
                        bestpositions.second = cheapestMove[1].first;
                        inserted = 1;
                    }
                }

            }

            // cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            // cout << "Requests: " << candidate  << " - " << candidate2 << endl;
            // cout << "Best cost: " << best_cost << endl;
            // cout << "Best route: " << best_route << endl;
            // cout << "Best positions: " << bestpositions.first << " - " << bestpositions.second << endl;
            // cout << "+++++++++++++++++++++++++++++++++++++++++" << endl;
            // getchar();

            if (best_cost == -100000){
                if (problem->dParcel > 0){
                    vehicle = solution->getvehicle();
                    sarpRoute newroute(inst, vehicle);
                    feastime = newroute.fInsertionParcel(inst, nodeVec, Mdist, candidate, candidate2);
                    newroute.calcCost(inst, nodeVec, Mdist);
                    newroute.insert(inst, Mdist, candidate, 1, nodeVec[candidate].profit);
                    newroute.insert(inst, Mdist, candidate2, 2, nodeVec[candidate2].profit);
                    newroute.updateAll(inst, nodeVec, Mdist);
                    solution->addRoute(&newroute);
                    removeFromCL(candidate, 0);
                    solution->updateVehicles();
                    inserted = 1;
                    solution->updateCost();
                    break;
                }
                else{
                    solution->addtounserved(candidate);
                    removeFromCL(candidate, 0);
                    break;
                }
            }

            else{
                sroute = solution->getRoute(best_route);

                // cout << "::::******::::::******::::::" << endl;
                // cout << "Request1: " << candidate << " - Request2: " << candidate2 << endl;
                // cout << "Best position 1: " << bestpositions.first << "; Best position 2: " << bestpositions.second << endl;
                // cout << "::::******::::::******::::::" << endl;

                // getchar();

                sroute.insert(inst, Mdist, candidate, bestpositions.first, nodeVec[candidate].profit);
                sroute.insert(inst, Mdist, candidate2, bestpositions.second+1, nodeVec[candidate2].profit);
                
                sroute.updateAll(inst, nodeVec, Mdist);
                solution->updateRoutes(&sroute, best_route);
                solution->updateCost();

                inserted = 1;
                // cout << "*****Route with insertion: " << endl;
                // for (auto a: sroute){
                //     cout << a << " - ";
                // }
                // cout << "New route cost: " << sroute.cost() << endl;
                // cout << endl << endl;

                removeFromCL(candidate, 0);

                break;
            }
        }

        if (inserted && CLparc.empty()){
            CLparc.insert(CLparc.end(), solution->ubegin(), solution->uend());
            solution->clearunserved();
        }
    }
    cout << "Initial solution: " << endl;
    solution->printSol(inst);
    solution->printCosts();
    // getchar();

    // double testcost = 0;

    // double singlecost;

    // cout << "Extensive value calculation: " << endl;
    // sol_size = solution->getRoutesSize();
    // for (int rid = 0; rid < sol_size; rid++){
    //     sroute = solution->getRoute(rid);
    //     sroute.calcCost(inst, nodeVec, Mdist);
    //     singlecost = sroute.cost();
    //     cout << "route " << rid << ": " << singlecost << endl;
    //     testcost += singlecost;
    // }

    // cout << "Cost with step calculation: " << testcost << endl;
    // getchar();
}

void sarpConstruction::removeFromCL(int id, bool pass) {
    if (pass){
        for (int i = 0; i < CLpass.size(); ++i) {
            if (this->CLpass[i] == id) {
                this->CLpass.erase(this->CLpass.begin() + i);
                break;
            }
        }
    }

    else{
        for (int i = 0; i < CLparc.size(); ++i) {
            if (this->CLparc[i] == id) {
                this->CLparc.erase(this->CLparc.begin() + i);
                break;
            }
        }
    }
}