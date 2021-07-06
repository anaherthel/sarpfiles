#include "sarpRoute.h"

#include <iterator>
#include <climits>

sarpRoute::sarpRoute(instanceStat *inst){
    nodes_.push_back(inst->n+2*inst->m);
    nodes_.push_back(inst->n+2*inst->m+1);
    cost_ = 0;
    length_ = 0;
}

void sarpRoute::insertRequest(int id, int pos) {
    this->nodes_.insert(this->nodes_.begin() + pos, id);
}

void sarpRoute::calcCost(double **Mdist) {
    this->cost_ = 0;
    int u, v;

    for (int i = 0; i < getNodesSize() - 1; i++) {
        u = nodes_[i];
        v = nodes_[i + 1];
        this->cost_ += Mdist[u][v];
    }
    
}

bool sarpRoute::fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request){

    bool feasible;
    double totalTime;

    prevReq.push_back(nodes_[0]);
    postReq.push_back(nodes_[nodes_.size()-1]);
    firstPass = request;
    lastPass = request;
    firstPassPos = 1;
    lastPassPos = 1;

    starttime = nodeVec[request].e - ((Mdist[nodes_[0]][request])/inst->vmed);
    endtime = nodeVec[request].e + nodeVec[request].delta;

    totalTime = endtime - starttime;

    cout << "Starting route: " << endl;
    cout << "Start time: " << starttime << endl;
    cout << "End time: " << endtime << endl;
    cout << "Total time: " << totalTime << endl;
    getchar();
    
    if (endtime < inst->T && totalTime < inst->maxTime && starttime > 0){
        feasible = 1;
    }

    return feasible;

    // for (int i = 0; i < nodes_.size(); i++){
    //     if(nodes_[i] < inst->n){
    //         firstPass = nodes_[i];
    //         firstPassPos = i;
    //         break;
    //     }
    //     else{
    //         prevReq.push_back(nodes_[i]);
    //     }
    // }

    // for (int i = nodes_.size() - 1; i >= 0; i--){
    //     if(nodes_[i] < inst->n){
    //         lastPass = nodes_[i];
    //         lastPassPos = i;
    //         break;
    //     }
    //     else{
    //         postReq.insert(postReq.begin(), nodes_[i]);
    //     }
    // }

    // cout << "First passenger: " << firstPass << endl;
    // cout << "Last passenger: " << lastPass << endl;
    // getchar();
}

bool sarpRoute::routeTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request){
    double prevTime, postTime, totalTime;

    bool feasible = 0;

    if (nodes_.size() < 3){

    }
    //calculating starting times
    //there will always be the initial depot before any passenger

    for (int i = 0; i < prevReq.size() - 1; i++){
        prevTime += ((Mdist[prevReq[i]][prevReq[i + 1]])/inst->vmed) + nodeVec[prevReq[i]].delta;    
    }
    //if there are requests before the passenger, we add the time from the last prev to the
    //passenger, as well as the delta for the last of the prev requests.

    prevTime += ((Mdist[prevReq.back()][firstPass])/inst->vmed) + nodeVec[prevReq.back()].delta;
    starttime = nodeVec[firstPass].e - prevTime;

    if (postReq.size() > 0){
        for (int i = 0; i < postReq.size() - 1; i++){
            postTime += ((Mdist[postReq[i]][postReq[i + 1]])/inst->vmed) + nodeVec[postReq[i]].delta;
        }
    //if there are requests after the passenger, we add the time from the passenger to the first
    //post request, as well as the delta for the last of the post requests.

        postTime += ((Mdist[lastPass][postReq[0]])/inst->vmed) + nodeVec[postReq.back()].delta;
        endtime = nodeVec[lastPass].e + nodeVec[lastPass].delta + postTime;
    }
    else{
        endtime = nodeVec[lastPass].e + nodeVec[lastPass].delta;
    }

    totalTime = endtime - starttime;
    if (endtime < inst->T && totalTime < inst->maxTime && starttime > 0){
        feasible = 1;
    }

    return feasible;
}
// ostream & operator<<(ostream &os, const Route &route){
//     os << " Route visiting nodes ";
//     copy(route.begin(), route.end(), ostream_iterator<int>(cout, " "));
//     os << "(cost=" << route.cost() << ", length=" << route.length()
//        << ", load=" << route.load() << ")";
//     return os;
// }

// // evaluate the cost of cheapest insertion of node in this route
// // returns a <position, cost> pair
// pair<int, int> Route::cheapestInsertion(const CVRPInstance &inst, int node) const {
//     int cheapest = INT_MAX;
//     int bestpos = -1;
//     // only check insertions if customer demand fits in the truck
//     if (load_ + inst.q(node) <= inst.Q()) {
//         // try inserting at every position
//         for (unsigned int p=1; p < nodes_.size(); p++) {
//             // cost of inserting node before node currently at position p
//             int delta = inst.d(nodes_[p-1], node) + inst.d(node, nodes_[p])
//                 - inst.d(nodes_[p-1], nodes_[p]);
//             if (delta + length_ + inst.s() > inst.L()) {
//                 // case where max. length is exceeded: skip this insertion
//                 continue;
//             } else if (delta < cheapest) {
//                 cheapest = delta;
//                 bestpos = p;
//             }
//         }
//     }
//     return make_pair(bestpos, cheapest);
// }

// void Route::insert(const CVRPInstance &inst, int node, int position) {
//     int delta = inst.d(nodes_[position-1], node)
//         + inst.d(node, nodes_[position])
//         - inst.d(nodes_[position-1], nodes_[position]); 
//     cost_ += delta;
//     length_ += delta + inst.s();
//     nodes_.insert(nodes_.begin() + position, node);
//     load_ += inst.q(node);
// }

// void Route::erase(const CVRPInstance &inst, int position) {
//     int node = nodes_[position];
//     int delta = inst.d(nodes_[position - 1], nodes_[position + 1])
//         - inst.d(nodes_[position - 1], nodes_[position])
//         - inst.d(nodes_[position], nodes_[position + 1]);
//     cost_ -= delta;
//     length_ -= delta + inst.s();
//     nodes_.erase(nodes_.begin() + position);
//     load_ -= inst.q(node);
// }
