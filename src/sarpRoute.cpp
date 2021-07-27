#include "sarpRoute.h"

#include <iterator>
#include <climits>

sarpRoute::sarpRoute(instanceStat *inst){
    nodes_.push_back(inst->n+2*inst->m);
    nodes_.push_back(inst->n+2*inst->m+inst->K);
    cost_ = 0;
    length_ = 0;
}

void sarpRoute::calcCost(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist) {
    this->cost_ = 0;
    double revenue = 0; //revenues from requests
    double trvCost = 0; //travel cost
    int u, v;

    for (int i = 0; i < getNodesSize() - 1; i++) {
        u = nodes_[i];
        v = nodes_[i + 1];
        if (u < inst->n + inst->m){
            revenue += nodeVec[u].profit;
        }
        trvCost += inst->costkm*Mdist[u][v];
    }
    this->cost_ = revenue - trvCost;    
}

bool sarpRoute::fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request){

    bool feasible;
    double totalTime;

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
}

bool sarpRoute::testInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request){
    double prevTime, postTime, totalTime;
    double tstart = 0;
    double tend = 0;
    int prevPass = -1;
    int prevPasspos = -1;
    int nextPass = -1;
    int nextPasspos = -1;
    int stoppingLoop;

    bool feasible = 0;
    prevReq.clear();
    postReq.clear();

    //calculating starting times
    //there will always be the initial depot before any passenger
    //the request is a passenger node

    if (position <= firstPassPos){
        if (request < inst->n){
            stoppingLoop = position;
        }
        else{
            stoppingLoop = firstPassPos;
        }
        prevTime = nodeVec[nodes_[0]].e;
        for (int i = 0; i < stoppingLoop; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
        }
        prevTime += ((Mdist[nodes_[position-1]][request])/inst->vmed) 
                  - ((Mdist[nodes_[position-1]][position])/inst->vmed);
        
        if (request >= inst->n){
            prevTime += ((Mdist[nodes_[request]][position])/inst->vmed) + nodeVec[nodes_[request]].delta;
        }
        cout << "\nRequest: " << request << endl;
        cout << "\nPosition of insertion: " << position << endl;
        cout << "\nPrevious time: " << prevTime << endl;
        cout << "\nService time: " << nodeVec[request].e << endl;

        if (request < inst->n){
            if (prevTime <= nodeVec[request].e){
                feasible = 1;
                cout << "the insertion is feasible from part 1." << endl;
            }
            else{
                feasible = 0;
                return feasible;
            }
        }
        else{
            if (prevTime <= nodeVec[firstPass].e){
                feasible = 1;
                cout << "the insertion is feasible from part 1." << endl;
            }
            else{
                feasible = 0;
                return feasible;
            }            
        }
        if (request < inst->n && feasible){
            postTime = nodeVec[request].e + nodeVec[request].delta + ((Mdist[request][nodes_[position]])/inst->vmed);           
            for(int i = position; i < firstPassPos; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
            }
            if (postTime <= nodeVec[firstPass].e){
                feasible = 1;
                firstPass = request;
                firstPassPos = position;
                tstart = nodeVec[request].e - prevTime;
                totalTime = endtime - tstart;
                cout << "the insertion is feasible from part 2." << endl;

                if (totalTime > inst->maxTime){
                    feasible = 0;
                    cout << "p1: the maximum riding time is exceeded." << endl;
                }
            }
            else{
                feasible = 0;
            }
        }
        return feasible;
    }
    else if (position >= lastPassPos){
        if (request < inst->n){
            postTime = nodeVec[request].e + nodeVec[request].delta + ((Mdist[request][nodes_[position]])/inst->vmed);
            for(int i = position; i < nodes_.size(); i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
            }

        }
        else{
            postTime = nodeVec[lastPass].e;
        }        

        if (postTime <= inst->T){
            feasible = 1;

            totalTime = postTime - starttime;
            cout << "the insertion is feasible from part 1." << endl;

            if (totalTime > inst->maxTime){
                feasible = 0;
                cout << "p1: the maximum riding time is exceeded." << endl;
            }
        }
        else{
            feasible = 0;
        }

        prevTime = nodeVec[lastPass].e;
        for (int i = lastPassPos; i < position; i++){
            if (i == position - 1){
                prevTime += nodeVec[nodes_[i]].delta + ((Mdist[nodes_[i]][request])/inst->vmed);
            }
            else{
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }
        }



        cout << "\nPosition after last passenger." << endl;
        cout << "\nRequest: " << request << endl;
        cout << "\nPosition of insertion: " << position << endl;
        cout << "\nPrevious time: " << prevTime << endl;
        cout << "\nService time: " << nodeVec[request].e << endl;

        if (prevTime <= nodeVec[request].e){
            feasible = 1;
            cout << "the insertion is feasible from part 1." << endl;
        }
        else{
            feasible = 0;
            return feasible;
        }

        
        
        return feasible;
    }        
    else{
        cout << "\n\nPassenger or parcel request, start time in the middle" << endl;
        getchar();
        for (int i = 0; i < passandpos.size(); i++){
            if (passandpos[i].second < position){
                prevPass = passandpos[i].first.index;
                prevPasspos = passandpos[i].second;
            }
            else{
                nextPass = passandpos[i].first.index;
                nextPasspos = passandpos[i].second;
                break;
            }
        }
        cout << "\nPrevious passenger: " << prevPass << " in position " << prevPasspos << endl;
        cout << "\nNext passenger: " << nextPass << " in position " << nextPasspos << endl;
        getchar();
        
        prevTime = nodeVec[prevPass].e;
        for (int i = prevPasspos; i < position; i++){
            if (i == position - 1){
                prevTime += nodeVec[nodes_[i]].delta + ((Mdist[nodes_[i]][request])/inst->vmed);
            }
            else{
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }
        }

        cout << "\nRequest: " << request << endl;
        cout << "\nPosition of insertion: " << position << endl;
        cout << "\nPrevious time: " << prevTime << endl;
        cout << "\nService time: " << nodeVec[request].e << endl;

        if (prevTime <= nodeVec[request].e){
            feasible = 1;
            cout << "the insertion is feasible from part 1." << endl;
        }
        else{
            feasible = 0;
            return feasible;
        }
        postTime = nodeVec[request].e + nodeVec[request].delta + ((Mdist[request][nodes_[position]])/inst->vmed);
        for(int i = position; i < nextPasspos; i++){
            postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }
        if (postTime <= nodeVec[nextPass].e){
            feasible = 1;

            cout << "the insertion is feasible from part 2." << endl;
        }
        else{
            feasible = 0;
        }
        return feasible;
    }
    return feasible;
}

void sarpRoute::updatePass(instanceStat *inst, vector<nodeStat> &nodeVec){
    pair<nodeStat, int> pairtoadd;
    passandpos.clear();

    for (int i = 0; i < nodes_.size(); i++){
        if (nodes_[i] < inst->n){
            if (passandpos.size() < 1){
                firstPassPos = i;
                firstPass = nodes_[i];
            }
            pairtoadd.first = nodeVec[nodes_[i]];
            pairtoadd.second = i;
            passandpos.push_back(pairtoadd);
        }
    }
    lastPass = passandpos.back().first.index;
    lastPassPos = passandpos.back().second;
}

void sarpRoute::updateLoad(instanceStat *inst, vector<nodeStat> &nodeVec){
    //0 - passenger served with no parcels involved;
    //1+ - number of passegers served with parcels involved;
    //-1 -  We disregard passengers served or unserved.

    pair<int, int> pairtoadd;
    loadofroute.clear();

    pairtoadd.first = 0;
    pairtoadd.second = -1;
    loadofroute.push_back(pairtoadd);

    for (int i = 1; i < nodes_.size(); i++){
        if (nodes_[i] < inst->n){
            pairtoadd.first = loadofroute[i-1].first;
            if (loadofroute[i-1].first > 0){//if the vehicle is carrying smth
                if(loadofroute[i-1].second < 0){//if no customer has been served while carrying parcels
                    pairtoadd.second = 1;//we set the number of visited customers to 1.
                }
                else{
                    pairtoadd.second += loadofroute[i-1].second + 1;//or we increase if there were other customers visited
                }
            }
            else{
                pairtoadd.second = 0;//if no load is being carried, no sense in counting the number of passengers. 
            }
            loadofroute.push_back(pairtoadd);
        }
        else if (nodes_[i] < inst->n + inst->m){
            pairtoadd.first = (loadofroute[i-1].first)+1;
            if (pairtoadd.first < 2){
                pairtoadd.second = 0;
            }
            else{
                pairtoadd.second = (loadofroute[i-1].second);
            }

            loadofroute.push_back(pairtoadd);
        }
        else if (nodes_[i] < inst->n + 2*inst->m){
            pairtoadd.first = (loadofroute[i-1].first)-1;

            if (pairtoadd.first < 1){
                pairtoadd.second = 0;
            }
            else{
                pairtoadd.second = (loadofroute[i-1].second);
            }
            loadofroute.push_back(pairtoadd);            
        }
    }
    pairtoadd.first = 0;
    pairtoadd.second = -1;
    loadofroute.push_back(pairtoadd);
}

void sarpRoute::availablePos(instanceStat *inst, vector<nodeStat> &nodeVec, int request, probStat* problem, vector<int> &positions){
    int fPosIns = 1;

    if (request < inst->n){
        positions.clear();
        if (nodeVec[request].e < nodeVec[passandpos[0].first.index].e){
            positions.push_back(1);
            positions.push_back(passandpos[0].second);
        }
        else if (nodeVec[request].e > nodeVec[passandpos[passandpos.size()-1].first.index].e){
            positions.push_back(passandpos[passandpos.size()-1].second + 1);
            positions.push_back(nodes_.size()-1);
        }
        else{
            for (int i = 0; i < passandpos.size(); i++){
                if (nodeVec[request].e > nodeVec[passandpos[i].first.index].e){
                    fPosIns = passandpos[i].second;
                }
                else{
                    break;
                }
            }
            if (problem->p1 == 0){
                for(int i = fPosIns; i < loadofroute.size()-1; i++){
                    if (loadofroute[i].second > 0){
                        continue;
                    }
                    else{
                        positions.push_back(i);
                    }
                }
            }
            else{
                for(int i = 1; i < nodes_.size()-1; i++){
                    positions.push_back(i);
                }
            }
        }
    }
    else if(request < inst->n + inst->m){
        positions.clear();
        if (problem->p2 == 0){
            for(int i = 1; i < loadofroute.size()-1; i++){
                if (loadofroute[i].first > 0){
                    continue;
                }
                else{
                    positions.push_back(i);
                }
            }
        }
        else{
            for(int i = 1; i < loadofroute.size()-1; i++){
                positions.push_back(i);
            }
        }
    }
    else{//if the request is a delivery node
        //first position is its pickup position + 2 (or +1 if AMcMp)
        //last position is the next pickup 
        if (problem->p2 == 0){
            for(int i = positions[0]; i < loadofroute.size()-1; i++){
                if (loadofroute[i].first > 0){
                    continue;
                }
                else{
                    positions.push_back(i);
                }
            }
        }
        else{
            for(int i = 1; i < loadofroute.size()-1; i++){
                positions.push_back(i);
            }
        }
    }
}

void sarpRoute::updateTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist){

    // double prevTime, posTime;
    // prevTime = 0;
    // posTime = 0;

    // for (int i = 0; i < firstPassPos; i++){
    //     prevTime += 
    // }   

    // for(int i = lastPassPos; i < nodes_.size(); i++){

    // }

}
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

void sarpRoute::insert(instanceStat *inst, double **Mdist, int node, int position) {
    int delta = Mdist[nodes_[position-1]][node]
        + Mdist[node][nodes_[position]]
        - Mdist[nodes_[position-1]][nodes_[position]]; 
    cost_ += delta;
    // length_ += delta + inst.s();
    nodes_.insert(nodes_.begin() + position, node);
}

void sarpRoute::erase(instanceStat *inst, double **Mdist, int position) {
    int node = nodes_[position];
    int delta = Mdist[nodes_[position - 1]][nodes_[position + 1]]
        - Mdist[nodes_[position - 1]][nodes_[position]]
        - Mdist[nodes_[position]][nodes_[position + 1]];
    cost_ -= delta;
    // length_ -= delta + inst.s();
    nodes_.erase(nodes_.begin() + position);
}

void sarpRoute::printLoad(){
    cout << "Load of route: " << endl;

    for (int i = 0; i < loadofroute.size(); i++){
        if (i != loadofroute.size()-1){
            cout << loadofroute[i].first << " - ";
        }
        else{
            cout << loadofroute[i].first;           
        }
    }

    cout << "\nVisited Passengers: " << endl;
    for (int i = 0; i < loadofroute.size(); i++){
        if (i != loadofroute.size()-1){
            cout << loadofroute[i].second << " - ";
        }
        else{
            cout << loadofroute[i].second;           
        }
    }
}