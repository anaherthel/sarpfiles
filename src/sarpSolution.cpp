#include "sarpSolution.h"


void sarpSolution::addRoute(sarpRoute *route){
    this->routes.push_back(*route);
    this->updateCost();
    cout << "vehicles: " << usedK << endl;
    getchar();
}

void sarpSolution::updateCost() {
    this->cost = 0;
    for (int i = 0; i < routes.size(); ++i) {
        this->cost += routes[i].cost();
    }
}

void sarpSolution::updateRoutes(sarpRoute *route, int idr) {
    this->routes[idr] = *route;
}

void sarpSolution::updateVehicles(){
    this->usedK++;
}

void sarpSolution::printSol(instanceStat *inst) {

    sarpRoute sroute(inst, usedK);

    cout << "SARP Solution: " << endl;
    getchar();
    for (int i = 0; i < routes.size(); i++){
        sroute = routes[i];
        for(auto j: sroute){
            cout << j << " - ";
        }
        cout << endl;
    }
    cout << endl;
}

void sarpSolution::addtounserved(int candidate) {
    unserved.push_back(candidate);
}   


void sarpSolution::printCosts(){
    cout << "Cost of routes: " << endl;

    for (int i = 0; i < this->routes.size(); i++){
        cout << "Route " << i << ": " << routes[i].cost() << endl;
    }

    cout << "\nCost of routes: " << this->cost << endl;
}

double sarpSolution::relocate (instanceStat *inst, vector<nodeStat> &nodeVec, 
                                double **Mdist, probStat* problem,
                                 int rid1, int rid2, int &currCand, pair <int, int> &currPairPos){

    sarpRoute r1(inst, 0);
    sarpRoute r2(inst, 0);
    int rsize1, rsize2;

    r1 = getRoute(rid1);
    r2 = getRoute(rid2);

    rsize1 = r1.getNodesSize();
    rsize2 = r2.getNodesSize(); 

    bool feasible = 0;

    int candidate, candidate2;
    int bestpos1, bestpos2;

    vector<int> inspositions; 
    vector<int> inspositions2;

    pair <int, double> cheapestpair; //position and delta
    
    cheapestpair.first = -1;
    cheapestpair.second = -100000;
    
    vector< pair <int, double> > cheapestMove;
    
    cheapestMove.push_back(cheapestpair);
    cheapestMove.push_back(cheapestpair); //chapest move has 2 cheapest pairs. Only 1 is used for passengers.
    
    double best_cost, compareCost, rval1, rval2, rmvVal, addVal;

    rval1 = r1.cost();
    rval2 = r2.cost();

    best_cost = rval1 + rval2;

    for (int i = 1; i < rsize1-1; i++){
        candidate = r1.getReq(i);
        compareCost = rval1 + rval2;
        // cout << "\ncandidate: " << candidate << endl;
        // cout << "Initial cost of both routes: " << compareCost << endl;

        if(nodeVec[candidate].load < 0){
            // cout << "\n Delivery node (skip)";
            // getchar();
            continue;
        }

        inspositions.clear();
        r2.availablePos(inst, nodeVec, candidate, problem, inspositions);
        
        // cout << "Available positions for insertion: " << endl;
        // for (int p = 0; p < inspositions.size(); p++){
        //     cout << inspositions[p] << " - ";
        // }
        // cout << endl;
        // getchar();
        if (candidate < inst->n){
            cheapestpair = r2.cheapestInsertion(inst, nodeVec, Mdist, candidate, inspositions);
            addVal = cheapestpair.second;

            if (addVal > 0){
                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 0);
                
                compareCost += addVal + rmvVal;           
            }
            else{
                continue;
            }
            // cout << "Compare Cost: " << compareCost;
        }
        else {
            cheapestMove[0].first = -1;
            cheapestMove[0].second = -100000;
            cheapestMove[1].first = -1;
            cheapestMove[1].second = -100000;
            // cout << "cheapest move 2 BEFORE: " <<  cheapestMove[1].second << endl;
            int node2 = candidate + inst->m;
            inspositions2.clear();
            r2.cheapestInsertionParcel(inst, nodeVec, Mdist, candidate, node2, inspositions, inspositions2, cheapestMove, problem);
            
            if(cheapestMove[0].first != cheapestMove[1].first){
                addVal = cheapestMove[0].second + cheapestMove[1].second;
            }
            else{
                addVal = nodeVec[candidate].profit + cheapestMove[1].second;
                // cout << "cheapest move 2: " <<  cheapestMove[1].second << endl;
            }  
 
            if (addVal > 0){
                // cout << "New add value: " << addVal << endl;
                // getchar();

                rmvVal = r1.rmvVal(inst, nodeVec, Mdist, i, 1);

                // cout << "New rmv value: " << rmvVal << endl;
                // getchar();
                
                // cout << "Adding at position: " << cheapestMove[0].first << " and " << cheapestMove[1].first << endl;
                // getchar();

                compareCost += addVal + rmvVal;  
            }
            else{
                continue;
            }
            // cout << "Compare Cost: " << compareCost;
        }

        if (compareCost > best_cost){
            // cout << "\nThere was an improvement" << endl;
            // cout << "\nCandidate: " << candidate << endl;
            // getchar();
            currCand = i;

            if (candidate < inst->n){
                best_cost = compareCost;
                currPairPos.first = cheapestpair.first;
                currPairPos.second = -1;

                // cout << "insert in position: "<< currPairPos.first << endl; 
                // cout << "with an improvement of " << best_cost << endl;
            }
            else{
                best_cost = compareCost;
                currPairPos.first = cheapestMove[0].first;
                currPairPos.second = cheapestMove[1].first;

                // cout << "insert in position: "<< currPairPos.first << endl; 
                // cout << "with an improvement of " << best_cost << endl;
            }

        }
    }

    double delta;

    delta = compareCost - best_cost;

    // cout << "Best cost: " << best_cost << endl;
    // cout << "cost (previous cost): " << compareCost << endl;
    // cout << "delta: " << delta << endl;


    return delta;
}


// void sarpSolution::addCost(double delta){
//     cost += delta;
// }

// Solution::Solution(Data *data) : data(data), cost(__DBL_MAX__) {
//     this->createRoutesVector();
//     pairNeighbStatus = vector<vector<vector<bool>>>(
//         PAIR_ROUTES_MOVES_TOTAL,
//         vector<vector<bool>>(routes.size(),
//                              vector<bool>(routes.size(), true)));
// }

// Solution::Solution(Data *data, vector<Route *> routesVec) : data(data),
//                                                             cost(__DBL_MAX__) {
//     for (int i = 0; i < routesVec.size(); ++i) {
//         this->routes.push_back(new Route(*routesVec[i]));
//     }
//     this->update();
//     pairNeighbStatus = vector<vector<vector<bool>>>(
//         PAIR_ROUTES_MOVES_TOTAL,
//         vector<vector<bool>>(routes.size(),
//                              vector<bool>(routes.size(), true)));
// }

// Solution::Solution(const Solution &other) : data(other.data),
//                                             cost(other.cost),
//                                             pairNeighbStatus(other.pairNeighbStatus) {
//     for (int i = 0; i < other.getRoutesSize(); ++i) {
//         this->routes.push_back(new Route(*other.routes[i]));
//     }

//     // if (other.getRoutesSize() > data->v) {
//     //     this->routes.push_back(other.routes[other.getRoutesSize() - 1]);
//     // }
// }

// Solution::~Solution() {
//     if (this->getRoutesSize() > data->v) {
//         this->removeRoute();
//     }

//     for (int i = 0; i < this->getRoutesSize(); ++i) {
//         delete routes[i];
//     }
//     routes.clear();
// }

// void Solution::createRoutesVector() {
//     for (int k = 0; k < data->v; ++k) {
//         routes.push_back(new Route(data, data->vehicles[k]));
//     }
// }

// void Solution::updateRoutes() {
//     for (size_t i = 0; i < routes.size(); i++) {
//         routes[i]->update();
//     }
// }

// void Solution::update() {
//     this->updateRoutes();
//     this->updateCost();
// }

// void Solution::updateRoute(int idx) {
//     routes[idx]->update();
//     this->updateCost();
// }

// void Solution::updateCost() {
//     this->cost = 0;
//     for (int i = 0; i < routes.size(); ++i) {
//         this->cost += routes[i]->getCost();
//     }
// }

// void Solution::setPairNeighbStatusAllTrue(int route_1_idx, int route_2_idx) {
//     for (int n = 0; n < PAIR_ROUTES_MOVES_TOTAL; ++n) {
//         for (int i = 0; i < this->getRoutesSize(); ++i) {
//             pairNeighbStatus[n][route_1_idx][i] = pairNeighbStatus[n][i][route_1_idx] = true;
//             pairNeighbStatus[n][route_2_idx][i] = pairNeighbStatus[n][i][route_2_idx] = true;
//         }
//     }
// }

// void Solution::setPairNeighbStatusAllTrue(int route_idx) {
//     for (int n = 0; n < PAIR_ROUTES_MOVES_TOTAL; ++n) {
//         for (int i = 0; i < this->getRoutesSize(); ++i) {
//             pairNeighbStatus[n][route_idx][i] = pairNeighbStatus[n][i][route_idx] = true;
//         }
//     }
// }

// void Solution::repairSol() {
//     update();
//     sortRoutes();
// }

// void Solution::sortRoutes() {
//     sort(begin(routes), end(routes),
//          [](const Route *lhs, const Route *rhs) -> bool {
//              return lhs->path[0] < rhs->path[0];
//          });
// }

// void Solution::addRoute(Route *route) {
//     this->routes.push_back(route);
//     this->update();
//     pairNeighbStatus.assign(
//         PAIR_ROUTES_MOVES_TOTAL,
//         vector<vector<bool>>(routes.size(),
//                              vector<bool>(routes.size(), true)));
// }

// Route *Solution::popRoute() {
//     Route *r = this->routes.back();
//     this->routes.pop_back();
//     return r;
// }

// void Solution::removeRoute() { this->routes.pop_back(); }

// void Solution::check() {
//     bool load_viability, timewindow_viability, ridetime_viability;
//     Route *route;
//     vector<int> path;
//     Vehicle *vehicle;

//     for (int i = 0; i < this->getRoutesSize(); ++i) {
//         route   = this->getRoute(i);
//         path    = route->getPath();
//         vehicle = route->getVehicle();
//         LoadChecker lc(&path, data, vehicle);
//         TimeWindowChecker twc(&path, data, vehicle);
//         RideTimeChecker rtc(&path, data);
//         PairChecker pc(&path, data, vehicle);
//         ArcsChecker ac(&path, data, vehicle);

//         if (!route->isExtra() && !ac.check()) {
//             printf("Route %d is %s for arcs\n", i, "INFEASIBLE");
//             route->printPath();
//             // getchar();
//         }
//         if (!pc.check()) {
//             printf("Route %d is %s for pairing\n", i, "INFEASIBLE");
//             route->printPath();
//             // getchar();
//         }
//         if (!route->isExtra() && !lc.check()) {
//             printf("Route %d is %s for load\n", i, "INFEASIBLE");
//             route->printPath();
//             // getchar();
//         }
//         if (!route->isExtra() && !twc.check()) {
//             printf("Route %d is %s for timewindow\n", i, "INFEASIBLE");
//             route->printPath();
//             // getchar();
//         }
//         if (!route->isExtra() && !rtc.check()) {
//             printf("Route %d is %s for ridetime\n", i, "INFEASIBLE");
//             route->printPath();
//             // getchar();
//         }
//         // printf("Route %d is %s for load\n", i, (lc.check() ? "FEASIBLE" : "INFEASIBLE"));
//         // printf("Route %d is %s for timewindow\n", i, (twc.check() ? "FEASIBLE" : "INFEASIBLE"));
//         // printf("Route %d is %s for ridetime\n", i, (rtc.check() ? "FEASIBLE" : "INFEASIBLE"));
//     }
// }

// void Solution::print() {
//     printf("\nSolution Total Cost: %.4lf\n\n", cost);
//     for (int i = 0; i < routes.size(); ++i) {
//         routes[i]->printPath();
//     }
// }

// void Solution::clear() {
//     for (int i = 0; i < this->getRoutesSize(); ++i) {
//         this->getRoute(i)->clear();
//     }
//     this->update();
// }