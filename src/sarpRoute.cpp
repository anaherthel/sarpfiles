#include "sarpRoute.h"

#include <iterator>
#include <climits>

sarpRoute::sarpRoute(instanceStat *inst, int vehicle){
    nodes_.push_back(inst->n+2*inst->m+vehicle);
    nodes_.push_back(inst->n+2*inst->m+inst->K+vehicle);
    cost_ = 0;
    length_ = 0;
    
    pair <int, int> buildpair;
    for (int i = 0; i < inst->m; i++){
        buildpair.first = -1;
        buildpair.second = -1;
        pdvec.push_back(buildpair);
    }
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
        else{
            revenue += nodeVec[u].profit;
        }
        trvCost += inst->costkm*Mdist[u][v];
    }
    this->cost_ = revenue - trvCost;    
}

bool sarpRoute::fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request){

    bool feasible;
    double totalTime;
    feasible = 1;
    this->firstPass = request;
    this->lastPass = request;
    this->firstPassPos = 1;
    this->lastPassPos = 1;

    this->starttime = nodeVec[request].e - ((Mdist[this->nodes_[0]][request])/inst->vmed);
    this->endtime = nodeVec[request].e + nodeVec[request].delta;

    totalTime = this->endtime - this->starttime;

    // cout << "Starting route: " << endl;
    // cout << "Start time: " << this->starttime << endl;
    // cout << "End time: " << this->endtime << endl;
    // cout << "Total time: " << totalTime << endl;
    // getchar();
    
    if (this->endtime < inst->T && totalTime < inst->maxTime && this->starttime > 0){
        feasible = 1;
    }

    return feasible;
}

bool sarpRoute::fInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int pu, int dl){

    bool feasible;
    double totalTime;
    feasible = 1;
    this->firstPass = -1;
    this->lastPass = -1;
    this->firstPassPos = -1;
    this->lastPassPos = -1;
// - ((Mdist[this->nodes_[0]][pu])/inst->vmed)
    this->starttime = nodeVec[0].e;
    this->endtime = ((Mdist[this->nodes_[0]][pu])/inst->vmed) 
                  + ((Mdist[this->nodes_[pu]][dl])/inst->vmed)
                  + nodeVec[pu].delta
                  + nodeVec[dl].delta;

    totalTime = this->endtime - this->starttime;

    // cout << "Starting route: " << endl;
    // cout << "Start time: " << this->starttime << endl;
    // cout << "End time: " << this->endtime << endl;
    // cout << "Total time: " << totalTime << endl;
    // getchar();
    
    if (this->endtime < inst->T && totalTime < inst->maxTime && this->starttime > 0){
        feasible = 1;
    }

    return feasible;
}

bool sarpRoute::testSwap(instanceStat *inst, vector<nodeStat> &nodeVec,
                         double **Mdist,int pos1, int pos2, 
                         pair <int, int> inter1, pair <int, int> inter2){
   
    double prevTime, postTime, totalTime;
    double tstart = 0;
    double tend = 0;
    int prevPass = -1;
    int prevPasspos = -1;
    int nextPass = -1;
    int nextPasspos = -1;

    bool feasible = 0;

    int req1 = nodes_[pos1];
    int req2 = nodes_[pos2];

    // cout << "Positions: " << pos1 << " - " << pos2 << endl;
    // cout << "Requests: " << req1 << " - " << req2 << endl;
    // getchar();

    if (req2 < inst->n){
        inter1.second = inter2.second;//setting same end point for interval.
        prevPass = nodes_[inter1.first];
        prevTime = nodeVec[prevPass].e;

        for(int i = inter1.first; i < pos1; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                        + nodeVec[nodes_[i]].delta;
        }

        prevTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                    + ((Mdist[nodes_[pos1-1]][req2])/inst->vmed);

        if (prevTime <= nodeVec[req2].e){
            feasible = 1;

        }
        else{
            feasible = 0;
            return feasible;
        }
        if (feasible){
            postTime = nodeVec[req2].e;

            for (int i = pos1; i < inter1.second; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                        + nodeVec[nodes_[i]].delta;
            }
            if (pos2 == pos1 + 1){
                //the first term would be duplicated in sequential positions
                postTime += - ((Mdist[req1][req2])/inst->vmed)
                        + ((Mdist[req2][req1])/inst->vmed)
                        - ((Mdist[req2][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed);                
            }
            else{
                postTime += - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[req2][nodes_[pos1+1]])/inst->vmed)
                        - ((Mdist[req2][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed);
            }

            if (inter1.second < inst->n + 2*inst->m){
                if (postTime < nodeVec[nodes_[inter1.second]].e){
                    feasible = 1;
                }
                else{
                    feasible = 0;
                    return feasible;
                }
            }
            else{//last position on interval is depot.
                if (postTime < inst->T){
                    if(postTime - this->starttime < inst->maxTime){
                        feasible = 1;
                    }
                    else{
                        feasible = 0;
                        return feasible;
                    }  
                }
                else{
                    feasible = 0;
                    return feasible;
                }                
            }   
        }
    }
    else if (req1 < inst->n){
        inter2.first = inter1.first;//setting same initial point.
        prevPass = nodes_[inter2.first];
        prevTime = nodeVec[prevPass].e;

        for(int i = inter2.first; i < pos2; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                        + nodeVec[nodes_[i]].delta;
        }
         
        if (pos2 == pos1+1){
            prevTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                     - ((Mdist[req1][req2])/inst->vmed)
                     - nodeVec[req1].delta
                     + ((Mdist[nodes_[pos1-1]][req2])/inst->vmed)
                     + ((Mdist[req2][req1])/inst->vmed)
                     + nodeVec[req2].delta;     
        }
        else{
            prevTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed)
                        - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                        - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos1-1]][req2])/inst->vmed)
                        + ((Mdist[req2][nodes_[pos1+1]])/inst->vmed)
                        + nodeVec[req2].delta;
        }

        if (prevTime <= nodeVec[req1].e){
            feasible = 1;

            if (prevPass > inst->n){
                tstart = nodeVec[req1].e - prevTime;

                if (this->endtime - tstart > inst->maxTime){
                    feasible = 0;
                    // cout << "p1: the maximum riding time is exceeded." << endl;
                    return feasible;
                }
            }
        }
        else{
            feasible = 0;
            return feasible;
        }

        if (feasible){

            postTime = nodeVec[req1].e;

            for (int i = pos2; i < inter2.second; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                            + nodeVec[nodes_[i]].delta;                
            }

            postTime += - ((Mdist[req2][nodes_[pos2+1]])/inst->vmed)
                      - nodeVec[req2].delta
                      + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed)
                      + nodeVec[req1].delta;
            
            if (inter2.second < inst->n + 2*inst->m){
                if (postTime < nodeVec[nodes_[inter2.second]].e){
                    feasible = 1;
                }
                else{
                    feasible = 0;
                    return feasible;
                }
            }
            else{//last position on interval is depot.
                if (postTime < inst->T){
                    if(postTime - starttime < inst->maxTime){
                        feasible = 1;
                    }
                    else{
                        feasible = 0;
                        return feasible;
                    }  
                }
                else{
                    feasible = 0;
                    return feasible;
                }                
            }               
        }
    }
    else{
        bool sameInterval;
        sameInterval = checkInterval(inst, pos1, pos2, inter1, inter2);

        if (sameInterval){
            prevPass = nodes_[inter1.first];
            nextPass = nodes_[inter1.second];
            prevTime = nodeVec[prevPass].e;

            for(int i = inter1.first; i < inter1.second; i++){
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                            + nodeVec[nodes_[i]].delta;
            }

            prevTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        - ((Mdist[req2][nodes_[pos2+1]])/inst->vmed)
                        - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                        - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos1-1]][req2])/inst->vmed)
                        + ((Mdist[req2][nodes_[pos1+1]])/inst->vmed);

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;
                tstart = this->starttime;
                tend = this->endtime;
                if(prevPass > inst->n){
                    tstart = nodeVec[nextPass].e - prevTime;
                }
                if(nextPass > inst->n){
                    tend = prevTime;
                }
                if(tend - tstart > inst->maxTime){
                    feasible = 0;
                    // cout << "p1: the maximum riding time is exceeded." << endl;
                    return feasible;
                }
            }
            else{
                feasible = 0;
                return feasible;
            }       
        }   
        else{
            prevPass = nodes_[inter1.first];
            nextPass = nodes_[inter1.second];
            prevTime = nodeVec[prevPass].e;

            for(int i = inter1.first; i < inter1.second; i++){
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                            + nodeVec[nodes_[i]].delta;
            }
            prevTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                        - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos1-1]][req2])/inst->vmed)
                        + ((Mdist[req2][nodes_[pos1+1]])/inst->vmed);

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;
            }
            else{
                feasible = 0;
                return feasible;
            }

            int prevPass2 = -1;
            int nextPass2 = -1;

            prevPass2 = nodes_[inter2.first];
            nextPass2 = nodes_[inter2.second];
            postTime = nodeVec[prevPass2].e;

            for(int i = inter2.first; i < inter2.second; i++){ 
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                            + nodeVec[nodes_[i]].delta;
            }

            postTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        - ((Mdist[req2][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed);

            if (postTime <= nodeVec[nextPass2].e){
                feasible = 1;
            }
            else{
                feasible = 0;
                return feasible;
            }

            if(prevPass > inst->n){
                tstart = nodeVec[nextPass].e - prevTime;
            }
            else{
                tstart = this->starttime;
            }

            if (nextPass2 > inst->n){
                tend = postTime;
            }
            else{
                tend = this->endtime;
            }

            totalTime = tend - tstart;

            if (totalTime > inst->maxTime){
                feasible = 0;
                // cout << "p1: the maximum riding time is exceeded." << endl;
                return feasible;
            }
                            
        }
    }
    return feasible;
}

bool sarpRoute::testRelocate(instanceStat *inst, vector<nodeStat> &nodeVec,
                         double **Mdist,int pos1, int pos2, 
                         pair <int, int> inter1){
   
    double prevTime, postTime, totalTime;
    double tstart = 0;
    double tend = 0;
    int prevPass = -1;
    int prevPasspos = -1;
    int nextPass = -1;
    int nextPasspos = -1;

    bool feasible = 0;

    int req1 = nodes_[pos1];
    int req2 = nodes_[pos2];

    // cout << "Positions: " << pos1 << " - " << pos2 << endl;
    // cout << "Requests: " << req1 << " - " << req2 << endl;
    // getchar();

    if (req1 < inst->n){

        prevPass = nodes_[inter1.first];
        prevTime = nodeVec[prevPass].e;

        for(int i = inter1.first; i < pos2; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                        + nodeVec[nodes_[i]].delta;
        }
        

        if (pos1 < pos2){
            prevTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                        - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos1-1]][nodes_[pos1+1]])/inst->vmed);
                        - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed);
        }
        else{   
            prevTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed);
        }

        if (prevTime <= nodeVec[req1].e){
            feasible = 1;
            if (pos1 < pos2 && prevPass > inst->n){ 
                tstart = nodeVec[req1].e - prevTime;
                totalTime = this->endtime - tstart;
                if (totalTime > inst->maxTime){
                    feasible = 0;
                    // cout << "p1: the maximum riding time is exceeded." << endl;
                    return feasible;
                }
            }
        }
        else{
            feasible = 0;
            return feasible;
        }

        if (feasible){

            postTime = nodeVec[req1].e;

            for (int i = pos2; i < inter1.second; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                            + nodeVec[nodes_[i]].delta;                
            }

            if (pos1 < pos2){
                postTime += nodeVec[req1].delta
                        - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[req1][req2])/inst->vmed);
            }

            else{
                postTime += -((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                            -((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                            +((Mdist[nodes_[pos1-1]][nodes_[pos1+1]])/inst->vmed)
                            + ((Mdist[req1][req2])/inst->vmed);

            }
            
            if (inter1.second < inst->n + 2*inst->m){
                if (postTime < nodeVec[nodes_[inter1.second]].e){
                    feasible = 1;
                }
                else{
                    feasible = 0;
                    return feasible;
                }
            }
            else{//last position on interval is depot.
                if (postTime < inst->T){
                    if(postTime - starttime < inst->maxTime){
                        feasible = 1;
                    }
                    else{
                        feasible = 0;
                        return feasible;
                    }  
                }
                else{
                    feasible = 0;
                    return feasible;
                }                
            }               
        }
    }
    else{
    //cases in which we move a parcel pu or dl
    //only check interval of new position
        pair <int, int> npinter;//interval of new position

        npinter = getInterval(pos2);
        bool sameInterval;
        sameInterval = checkInterval(inst, pos1, pos2, inter1, npinter);

        prevPass = nodes_[npinter.first];

        if(nodes_[pos2] < inst->n){
            npinter.second = pos2;
        }

        nextPass = nodes_[npinter.second];
        prevTime = nodeVec[prevPass].e;

        for(int i = npinter.first; i < npinter.second; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) 
                        + nodeVec[nodes_[i]].delta;
        }

        if (sameInterval){
            prevTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                        - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                        + ((Mdist[nodes_[pos1-1]][nodes_[pos1+1]])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed);

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;
                if(prevPass > inst->n){
                    tstart = nodeVec[nextPass].e - prevTime;
                    totalTime = this->endtime - tstart;
                    if (totalTime > inst->maxTime){
                        feasible = 0;
                        // cout << "p1: the maximum riding time is exceeded." << endl;
                        return feasible;
                    }
                }
                if(nextPass > inst->n){
                    tend = nodeVec[prevPass].e + prevTime;
                    if ((tend - this->starttime) > inst->maxTime){
                        feasible = 0;
                        // cout << "p1: the maximum riding time is exceeded." << endl;
                        return feasible;
                    }                    
                }
            }
            else{
                feasible = 0;
                return feasible;
            }
        }
        else{
            prevTime += - ((Mdist[nodes_[pos2-1]][req2])/inst->vmed)
                        + ((Mdist[req1][nodes_[pos2+1]])/inst->vmed)
                        + ((Mdist[nodes_[pos2-1]][req1])/inst->vmed)
                        + nodeVec[req1].delta;

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;

                if(prevPass > inst->n){//adding to start of route
                    tstart = nodeVec[nextPass].e - prevTime;
                }
                else{
                    tstart = this->starttime;
                }
                if (nodes_[inter1.second] > inst->n){//removing from end of route
                    postTime = nodeVec[lastPass].e;

                    for(int l = lastPassPos; l < nodes_.size() - 1; l++){
                        postTime += ((Mdist[nodes_[l]][nodes_[l + 1]])/inst->vmed)
                                + nodeVec[nodes_[l]].delta;    
                    }
                    postTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                                - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                                - nodeVec[req1].delta
                                + ((Mdist[nodes_[pos1-1]][nodes_[pos1+1]])/inst->vmed);
                
                    tend = postTime;
                }
                else{
                    tend = this->endtime;
                }
                if(nodes_[inter1.first] > inst->n){ //removing from start of route
                    postTime = nodeVec[nodes_[inter1.first]].e;

                    for (int l = inter1.first; l < inter1.second; l++){
                        postTime += ((Mdist[nodes_[l]][nodes_[l + 1]])/inst->vmed)
                                + nodeVec[nodes_[l]].delta;
                    }

                    postTime += - ((Mdist[nodes_[pos1-1]][req1])/inst->vmed)
                                - ((Mdist[req1][nodes_[pos1+1]])/inst->vmed)
                                - nodeVec[req1].delta
                                + ((Mdist[nodes_[pos1-1]][nodes_[pos1+1]])/inst->vmed);

                    tstart = nodeVec[nodes_[inter1.second]].e - postTime;
                }
                if (nextPass > inst->n){ //adding to end of route
                    tend = prevTime;
                }
                
                totalTime = tend - tstart;
                if (totalTime > inst->maxTime){
                    feasible = 0;
                    // cout << "p1: the maximum riding time is exceeded." << endl;
                    return feasible;
                }

            }
            else{
                feasible = 0;
                return feasible;
            }            
        }



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
    int stoppingLoop, startingLoop;

    bool feasible = 0;

    //calculating starting times
    //there will always be the initial depot before any passenger
    //the request is a passenger node

    if (position <= firstPassPos){
        stoppingLoop = position;

        prevTime = nodeVec[nodes_[0]].e;
        for (int i = 0; i < stoppingLoop; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
        }
        prevTime += ((Mdist[nodes_[position-1]][request])/inst->vmed) 
                  - ((Mdist[nodes_[position-1]][nodes_[position]])/inst->vmed);

        // cout << "*****************************" << endl;
        // cout << "pos < first passenger" << endl; 
        // cout << "*****************************" << endl;
        // cout << "\nRequest: " << request << endl;
        // cout << "\nPosition of insertion: " << position << endl;
        // cout << "\nPrevious time: " << prevTime << endl;
        // cout << "\nService time: " << nodeVec[request].e << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (prevTime <= nodeVec[request].e){
            feasible = 1;
            // cout << "the insertion is feasible from part 1." << endl;
        }
        else{
            feasible = 0;
            return feasible;
        }

        postTime = nodeVec[request].e + nodeVec[request].delta + ((Mdist[request][nodes_[position]])/inst->vmed);           
        for(int i = position; i < firstPassPos; i++){
            postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }
        if (postTime <= nodeVec[firstPass].e){
            feasible = 1;
            tstart = nodeVec[request].e - prevTime;
            totalTime = endtime - tstart;

            // cout << "*****************************" << endl;
            // cout << "pos < first passenger" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nRequest: " << request << endl;
            // cout << "\nPosition of insertion: " << position << endl;
            // cout << "\nPost time: " << postTime << endl;
            // cout << "*****************************" << endl;
            // getchar();

            // cout << "the insertion is feasible from part 2." << endl;

            if (totalTime > inst->maxTime){
                feasible = 0;
                // cout << "p1: the maximum riding time is exceeded." << endl;
                return feasible;
            }
        }
        else{
            feasible = 0;
            return feasible;
        }

    }
    else if (position > lastPassPos){

        if (position < nodes_.size()-1){
            startingLoop = position;
            postTime = nodeVec[request].e;

            for(int i = startingLoop; i < nodes_.size()-1; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
            } 

            postTime += nodeVec[request].delta +
                    ((Mdist[request][nodes_[position]])/inst->vmed);

        }
        else{
            postTime = nodeVec[request].e + nodeVec[request].delta;
        }

        // cout << "*****************************" << endl;
        // cout << "pos > last passenger" << endl; 
        // cout << "*****************************" << endl;
        // cout << "\nRequest: " << request << endl;
        // cout << "\nPosition of insertion: " << position << endl;
        // cout << "\nPost time: " << postTime << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (postTime <= inst->T){
            feasible = 1;

            totalTime = postTime - this->starttime;
            // cout << "the insertion is feasible from part 1." << endl;
            
            // cout << "start time: " <<  this->starttime << endl;
            // cout << "total time: " << totalTime << endl;
            if (totalTime > inst->maxTime){
                feasible = 0;
                // cout << "p1: the maximum riding time is exceeded." << endl;
                // getchar();
                return feasible;
            }
        }
        else{
            feasible = 0;
            return feasible;
        }
        if (feasible){
            prevTime = nodeVec[lastPass].e;
            for (int i = lastPassPos; i < position; i++){
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }
            prevTime += ((Mdist[nodes_[position-1]][request])/inst->vmed) 
                      - ((Mdist[nodes_[position-1]][nodes_[position]])/inst->vmed);
            
            // cout << "*****************************" << endl;
            // cout << "pos > last passenger" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nRequest: " << request << endl;
            // cout << "\nPosition of insertion: " << position << endl;
            // cout << "\nPrevious time: " << prevTime << endl;
            // cout << "\nService time: " << nodeVec[request].e << endl;
            // cout << "*****************************" << endl;
            // getchar();

            if (prevTime <= nodeVec[request].e){
                feasible = 1;
            }
            else{
                feasible = 0;
                return feasible;
            }
        }
    }        
    else{
            // cout << "*****************************" << endl;
            // cout << "pos in middle" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nRequest: " << request << endl;
            // cout << "\nPosition of insertion: " << position << endl;
            // getchar();

        if(passandpos.size() > 0){
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
        }

        else{
            prevPass = nodes_[0];
            prevPasspos = 0;
            nextPass = nodes_.back();
            nextPasspos = nodes_.size()-1;
        }
        // cout << "\nPrevious passenger: " << prevPass << " in position " << prevPasspos << endl;
        // cout << "\nNext passenger: " << nextPass << " in position " << nextPasspos << endl;
        // getchar();
        if (nodeVec[request].e <= nodeVec[prevPass].e){
            feasible = 0;
            return feasible;
        }
        
        stoppingLoop = position;

        prevTime = nodeVec[prevPass].e;
        for (int i = prevPasspos; i < stoppingLoop; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
        }
        prevTime += ((Mdist[nodes_[position-1]][request])/inst->vmed) 
                - ((Mdist[nodes_[position-1]][nodes_[position]])/inst->vmed);        
        
        // cout << "\nPrevious time: " << prevTime << endl;
        // cout << "\nService time: " << nodeVec[request].e << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (prevTime <= nodeVec[request].e){
            feasible = 1;
            // cout << "the insertion is feasible from prevtime." << endl;
        }

        else{
            // cout << "Not feasible" << endl;
            feasible = 0;
            return feasible;
        }

        if (feasible){
            postTime = nodeVec[request].e;

            if (position != nextPasspos){
                for(int i = position; i < nextPasspos; i++){
                    postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
                }              
                
            }

            postTime += nodeVec[request].delta 
                    + ((Mdist[request][nodes_[position]])/inst->vmed);

            if (nextPass < inst->n){
                if (postTime <= nodeVec[nextPass].e){
                    feasible = 1;

                    // cout << "*****************************" << endl;
                    // cout << "pos IN MIDDLE " << endl; 
                    // cout << "*****************************" << endl;
                    // cout << "\nRequest: " << request << endl;
                    // cout << "\nPosition of insertion: " << position << endl;
                    // cout << "\nPost time: " << postTime << endl;
                    // cout << "*****************************" << endl;
                    // getchar();

                    // cout << "the insertion is feasible from postTime." << endl;
                }
                else{
                    feasible = 0;
                    return feasible;
                }
            }
            // else{
            //     tstart = nodeVec[request].e - prevTime;

            //     if (postTime <= inst->T){
            //         feasible = 1;

            //         totalTime = postTime - tstart;

            //         if (totalTime > inst->maxTime){
            //             feasible = 0;
            //             return feasible;
            //         }
            //     }
            //     else{
            //         feasible = 0;
            //         return feasible;
            //     }
            // }
        }
    }
    return feasible;
}

//just needed to make a new function for cases of the same interval of pos1 and ps2
bool sarpRoute::testInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int pos1, int pos2, int pu, int dl){
    double prevTime, postTime, totalTime;
    double Time1, Time2;//variation in time for inserting requests pu and dl in positions pos1 and pos2.
    double tstart = 0;
    double tend = 0;
    int prevPass = -1;
    int prevPasspos = -1;
    int nextPass = -1;
    int nextPasspos = -1;
    int stoppingLoop, startingLoop;

    bool feasible = 0;

    //calculating starting times
    //there will always be the initial depot before any passenger
    //the request is a passenger node

    Time1 = ((Mdist[nodes_[pos1-1]][pu])/inst->vmed)
            - ((Mdist[nodes_[pos1-1]][nodes_[pos1]])/inst->vmed)
            + nodeVec[pu].delta;

    if (pos2 == pos1){
        Time1 += ((Mdist[pu][dl])/inst->vmed)
                + ((Mdist[dl][nodes_[pos1]])/inst->vmed)
                + nodeVec[dl].delta;
    }


    else{
        Time1 += ((Mdist[pu][nodes_[pos1]])/inst->vmed);

        Time2 = ((Mdist[nodes_[pos2-1]][dl])/inst->vmed) 
              - ((Mdist[nodes_[pos2-1]][nodes_[pos2]])/inst->vmed) 
              + ((Mdist[dl][nodes_[pos2]])/inst->vmed) + nodeVec[dl].delta;
    }

    if (pos1 <= firstPassPos){
        stoppingLoop = firstPassPos;

        prevTime = nodeVec[nodes_[0]].e;

        for (int i = 0; i < stoppingLoop; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
        }

        prevTime += Time1;

        if (pos2 != pos1 && pos2 <= firstPassPos){
            prevTime += Time2;
        }

        // cout << "*****************************" << endl;
        // cout << "pos < first passenger" << endl; 
        // cout << "*****************************" << endl;
        // cout << "\nRequests: " << pu << " - " << dl << endl;
        // cout << "\nPositions of insertion: " << pos1 << " - " << pos2 << endl;
        // cout << "\nPrevious time: " << prevTime << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (prevTime <= nodeVec[firstPass].e){
            feasible = 1;

            tstart = nodeVec[firstPass].e - prevTime;
            totalTime = this->endtime - tstart;

            if (totalTime > inst->maxTime){
                feasible = 0;
                // cout << "p1: the maximum riding time is exceeded." << endl;
                return feasible;
            }

            // cout << "the insertion of both is feasible if both < first pass or if only pos1 < first pass" << endl;
        }
        else{
            feasible = 0;
            return feasible;
        }
        
        if (pos2 > lastPassPos){
            postTime = nodeVec[lastPass].e;
            
            for (int i = lastPassPos; i < nodes_.size() - 1; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }

            postTime += Time2;
            
            if (postTime < inst->T){
                feasible = 1;

                tstart = nodeVec[firstPass].e - prevTime;
                tend = postTime;

                totalTime = tend - tstart;

                if (totalTime > inst->maxTime){
                    feasible = 0;
                    // cout << "p1: the maximum riding time is exceeded." << endl;
                    return feasible;
                }

            }
            else{
                feasible = 0;
                return feasible;                
            }
        }

        else{//dl will be inserted somewhere in the middle
            // cout << "*****************************" << endl;
            // cout << "pos in middle" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nRequest: " << dl << endl;
            // cout << "\nPosition of insertion: " << pos2 << endl;
            // getchar();


            for (int i = 0; i < passandpos.size(); i++){
                if (passandpos[i].second < pos2){
                    prevPass = passandpos[i].first.index;
                    prevPasspos = passandpos[i].second;
                }
                else{
                    nextPass = passandpos[i].first.index;
                    nextPasspos = passandpos[i].second;
                    break;
                }
            }


            // cout << "\nPrevious passenger: " << prevPass << " in position " << prevPasspos << endl;
            // cout << "\nNext passenger: " << nextPass << " in position " << nextPasspos << endl;
            // getchar();

            stoppingLoop = nextPasspos;

            prevTime = nodeVec[prevPass].e;

            for (int i = prevPasspos; i < stoppingLoop; i++){
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }

            prevTime += Time2;

            // cout << "\nPrevious time: " << prevTime << endl;
            // cout << "\nNext Pass  time: " << nodeVec[nextPass].e << endl;
            // cout << "*****************************" << endl;
            // getchar();

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;
            }

            else{
                feasible = 0;
                return feasible;
            }
        }
    }
    else if (pos1 > lastPassPos){//pos2 will always be > lastPassPos

        startingLoop = lastPassPos;
        postTime = nodeVec[lastPass].e;

        for(int i = startingLoop; i < nodes_.size() - 1; i++){
            postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }   

        postTime += Time1;

        if (pos2 != pos1){
            postTime += Time2;
        }
        
        // cout << "*****************************" << endl;
        // cout << "pos > last passenger" << endl; 
        // cout << "*****************************" << endl;
        // cout << "\nRequests: " << pu << " - " << dl << endl;
        // cout << "\nPositions of insertion: " << pos1 << " - " << pos2 << endl;
        // cout << "\nPrevious time: " << postTime << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (postTime <= inst->T){
            feasible = 1;

            totalTime = postTime - this->starttime;

            if (totalTime > inst->maxTime){
                feasible = 0;
                // cout << "p1: the maximum riding time is exceeded." << endl;
                return feasible;
            }
        }
        else{
            feasible = 0;
            return feasible;
        }
    }        
    else{
        bool sameInterval = 0;
        // cout << "*****************************" << endl;
        // cout << "pos in middle" << endl; 
        // cout << "*****************************" << endl;
        // cout << "\nRequests: " << pu << " - " << dl << endl;
        // cout << "\nPositions of insertion: " << pos1 << " - " << pos2 << endl;
        // cout << "*****************************" << endl;
        // getchar();

        for (int i = 0; i < passandpos.size(); i++){
            if (passandpos[i].second < pos1){
                prevPass = passandpos[i].first.index;
                prevPasspos = passandpos[i].second;
            }
            else{
                nextPass = passandpos[i].first.index;
                nextPasspos = passandpos[i].second;
                break;
            }
        }

        if (pos2 <= nextPasspos){
            sameInterval = 1;
            // cout << "They have the same interval" << endl;
            // getchar();
        }

        stoppingLoop = nextPasspos;

        prevTime = nodeVec[prevPass].e;

        for (int i = prevPasspos; i < stoppingLoop; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
        }        

        prevTime += Time1;

        if (sameInterval && pos2 != pos1){
            // cout << "\nPU and DL in the same interval" << endl;
            prevTime += Time2;
        }

        // cout << "\nPrevious time: " << prevTime << endl;
        // cout << "\nService time: " << nodeVec[nextPass].e << endl;
        // cout << "*****************************" << endl;
        // getchar();

        if (prevTime <= nodeVec[nextPass].e){
            feasible = 1;

            if (pos1 == pos2){
                return feasible;
            }
            // cout << "the insertion is feasible from prevtime." << endl;
        }

        else{
            feasible = 0;
            return feasible;
        }

        if (pos2 > lastPassPos){
            startingLoop = lastPassPos;
            postTime = nodeVec[lastPass].e;

            for(int i = startingLoop; i < nodes_.size() - 1; i++){
                postTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
            }   

            postTime += Time2;
            
            // cout << "*****************************" << endl;
            // cout << "pos2 > last passenger" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nDL: " << dl << endl;
            // cout << "\nPosition of insertion: " << pos2 << endl;
            // cout << "\nPrevious time: " << postTime << endl;
            // cout << "*****************************" << endl;
            // getchar();

            if (postTime <= inst->T){
                feasible = 1;

                totalTime = postTime - starttime;
                // cout << "the insertion is feasible from < T." << endl;

                if (totalTime > inst->maxTime){
                    feasible = 0;
                    // cout << "DL: the maximum riding time is exceeded." << endl;
                    return feasible;
                }
            }
            else{
                feasible = 0;
                return feasible;
            }
        }
        else{

            // cout << "*****************************" << endl;
            // cout << "pos2 > last passenger" << endl; 
            // cout << "*****************************" << endl;
            // cout << "\nDL: " << dl << endl;
            // cout << "\nPosition of insertion: " << pos2 << endl;
            // cout << "*****************************" << endl;
            // getchar();

            for (int i = 0; i < passandpos.size(); i++){
                if (passandpos[i].second < pos2){
                    prevPass = passandpos[i].first.index;
                    prevPasspos = passandpos[i].second;
                }
                else{
                    nextPass = passandpos[i].first.index;
                    nextPasspos = passandpos[i].second;
                    break;
                }
            }                


            // cout << "\nPrevious passenger: " << prevPass << " in position " << prevPasspos << endl;
            // cout << "\nNext passenger: " << nextPass << " in position " << nextPasspos << endl;
            stoppingLoop = nextPasspos;

            prevTime = nodeVec[prevPass].e;

            for (int i = prevPasspos; i < stoppingLoop; i++){
                prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;    
            }        
            prevTime += Time2;

            // cout << "\nPrevious time: " << prevTime << endl;
            // cout << "\nService time: " << nodeVec[nextPass].e << endl;
            // cout << "*****************************" << endl;
            // getchar();

            if (prevTime <= nodeVec[nextPass].e){
                feasible = 1;
                // cout << "the insertion is feasible from prevtime." << endl;
            }

            else{
                feasible = 0;
                return feasible;
            }
        }
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
    if (passandpos.size() > 0){
        lastPass = passandpos.back().first.index;
        lastPassPos = passandpos.back().second;
    }
    else{
        firstPassPos = 0;
        firstPass = nodes_[0];
        lastPass = nodes_.size()-1;
        lastPassPos = nodes_.back();
    }


    // cout << "First passenger: " << firstPass << " - position: " << firstPassPos << endl;
    // cout << "Last passenger: " << lastPass << " - position: " << lastPassPos << endl;
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
    int lPosIns;
    int pos;

    if (request < inst->n){
        positions.clear();
        if (passandpos.size() > 0){
            if (nodeVec[request].e < nodeVec[passandpos[0].first.index].e){
                pos = passandpos[0].second;
                for (int i = 1; i <= pos; i++){
                    positions.push_back(i);
                }
            }
            else if (nodeVec[request].e > nodeVec[passandpos[passandpos.size()-1].first.index].e){
                pos = passandpos[passandpos.size()-1].second + 1;
                for (int i = pos; i < nodes_.size(); i++){
                    positions.push_back(i);
                }
            }
            else{
                fPosIns = 1;
                lPosIns = nodes_.size()-1;
                for (int i = 0; i < passandpos.size(); i++){
                    if (nodeVec[request].e > nodeVec[passandpos[i].first.index].e){
                        fPosIns = passandpos[i].second + 1;
                    }
                    else{
                        lPosIns = passandpos[i].second;
                        break;
                    }
                }
                if (problem->p1 == 0){
                    // for(int i = fPosIns; i < loadofroute.size()-1; i++){
                    for(int i = fPosIns; i < lPosIns; i++){                    
                        if (loadofroute[i].second > 0){
                            continue;
                        }
                        else{
                            positions.push_back(i);
                        }
                    }
                }
                else{
                    for(int i = fPosIns; i <= lPosIns; i++){
                        positions.push_back(i);
                    }
                }
            }
        }
        else{
            fPosIns = 1;
            lPosIns = nodes_.size()-1;
            for(int i = fPosIns; i <= lPosIns; i++){
                positions.push_back(i);
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
            for(int i = 1; i < loadofroute.size(); i++){
                positions.push_back(i);
            }
        }
    }
    else{//if the request is a delivery node
        //first position is its pickup position + 2 (or +1 if AMcMp)
        //last position is the next pickup 
        if (problem->p2 == 0){
            for(int i = positions[0] + 1; i < loadofroute.size(); i++){
                if (loadofroute[i].first > 0){
                    continue;
                }
                else{
                    positions.push_back(i);
                }
            }
        }
        else{
            // cout << "Here for 2mm" << endl;
            // getchar();
            for(int i = positions[0]+1; i < loadofroute.size(); i++){
                positions.push_back(i);
            }
        }
    }
}

void sarpRoute::updateTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist){

    double prevTime, posTime;
    prevTime = 0;
    posTime = 0;

    if (passandpos.size() > 0){
    
        for (int i = 0; i < firstPassPos; i++){
            prevTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }   

        for(int i = lastPassPos; i < nodes_.size() - 1; i++){
            posTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }

        starttime = nodeVec[firstPass].e - prevTime;
        endtime = nodeVec[lastPass].e + posTime;
    }

    else{
        starttime = 0;

        for(int i = 0; i < nodes_.size() - 1; i++){
            posTime += ((Mdist[nodes_[i]][nodes_[i + 1]])/inst->vmed) + nodeVec[nodes_[i]].delta;
        }

        endtime = posTime;
    }

 

    // cout << "&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
    // cout << "Starting time of route: " << starttime << endl;
    // cout << "Endging time of route:  " << endtime << endl;
    // cout << "&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
}
// // evaluate the cost of cheapest insertion of node in this route
// // returns a <position, cost> pair

pair<int, double> sarpRoute::cheapestInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, vector<int> &positions) {
    double bestIns = -100000;
    int bestpos = -1;
    bool feasible;
    double delta = -100000;
    int pos;

    for(int i = 0; i < positions.size(); i++){
        pos = positions[i];

        feasible = testInsertion(inst, nodeVec, Mdist, pos, node);

        if (feasible){       
            delta = nodeVec[node].profit - inst->costkm*(Mdist[nodes_[pos-1]][node]
                + Mdist[node][nodes_[pos]]
                - Mdist[nodes_[pos-1]][nodes_[pos]]); 
        }

        // cout << "Delta so far: " << delta << endl;

        if (delta > bestIns){
            bestIns = delta;
            bestpos = pos;
        }

        // cout << "Cheapest insertion: " << cheapest << endl;
        // cout << "Best position: " << bestpos << endl;
        // getchar();
    }  

    // cout << "Value of returning delta (cheapest insertion): " << bestIns << endl;
    // getchar();
    return make_pair(bestpos, bestIns);
}

void sarpRoute::cheapestInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, int node2, vector<int> &positions, vector<int> &positions2, vector< pair<int, double> > &bestMove, probStat* problem) {
    // double cheapest1 = 100000;
    // double cheapest2 = 100000;
    pair <int, double> pair1;
    pair <int, double> pair2;
    // int bestpos1 = -1;
    // int bestpos2 = -1;
    bool feasible;
    double delta1 = 0;
    double delta2;
    double deltaTotal = 0;
    
    double bestDT = 0;
    int pos1, pos2;

    pair1.first = -1;
    pair1.second = 0;

    for(int i = 0; i < positions.size(); i++){
        bool samePos = 0;
        feasible = 0;
        pos1 = positions[i];

        pair2.first = -1;
        pair2.second = 100000;
        delta2 = 100000;

        if (problem->dParcel > 0){
            positions2.push_back(pos1);
        }

        else{
            for (int trypass = 0; trypass < passandpos.size(); trypass++){
                if (passandpos[trypass].second >= pos1){
                    positions2.push_back(passandpos[trypass].second + 1);
                    break;
                }
            }
        }

        if(positions2[0] < nodes_.size() - 1){
            availablePos(inst, nodeVec, node2, problem, positions2);
        }

        // // /////////////////////////////////////////////
        // cout << "Available positions for insertion of candidate 2: " << node2 << endl;
        // for(int i = 0; i < positions2.size(); i++){
        //     cout << positions2[i] << ", ";
        // }
        // cout << endl;
        // cout << "***********************************************" << endl;               
        // ////////////////////////////////////////////
        // getchar();

        delta1 = nodeVec[node].profit - inst->costkm*((Mdist[nodes_[pos1-1]][node])
                + (Mdist[node][nodes_[pos1]])
                - (Mdist[nodes_[pos1-1]][nodes_[pos1]]));

        // cout << "\nDelta 1: " << delta1 << endl;

        pair1.second = delta1;
        pair1.first = pos1;                

        for (int j = 0; j < positions2.size(); j++){

            pos2 = positions2[j];
            if (pos2 < pos1){
                continue;
            }
            else if (problem->dParcel < 1){
                if (pos2 < pos1 + 1){
                    continue;
                }
            }
            // cout << "\nTesting pair: " << pos1 << " - " << pos2 << endl;
            // getchar();

            feasible = testInsertionParcel(inst, nodeVec, Mdist, pos1, pos2, node, node2);
            
            if (feasible){
                if (pos1 == pos2){
                    delta2 = - inst->costkm*((Mdist[nodes_[pos1-1]][node])
                              - (Mdist[nodes_[pos1-1]][nodes_[pos1]])
                              + (Mdist[node][node2])
                              + (Mdist[node2][nodes_[pos1]]));
                }
                else{
                    delta2 = - inst->costkm*(Mdist[nodes_[pos2-1]][node2]
                        + Mdist[node2][nodes_[pos2]]
                        - Mdist[nodes_[pos2-1]][nodes_[pos2]]);
                        
                }
                // cout << "\ninsertion2 is feasible" << endl;
            }
            else{
                continue;
            }

            if (pos1 == pos2){

                deltaTotal = nodeVec[node].profit + delta2;

                // cout << "Position: " << pos1 << endl;
                // cout << "Delta total: " << deltaTotal << endl;
                // cout << "delta2: " << delta2 << endl;


            }
            else{
                deltaTotal = delta1 + delta2;
            }

            // if (delta2 < pair2.second){
            //     // cout << "Better delta 2." << endl;
            //     // cout << "New position for dl:" << pos2 << endl;
            //     pair2.second = delta2;
            //     pair2.first = pos2;
            // }

            if (deltaTotal > bestDT){
                bestDT = deltaTotal;


                // cout << "Delta total: " << deltaTotal << endl;
                // getchar();
                // cout << "Better delta 1: " << bestDT << endl;
                // cout << "New position for pu:" << pos1 << endl;      

                bestMove[0].first = pos1;
                bestMove[0].second = delta1;
                bestMove[1].first = pos2;
                bestMove[1].second = delta2;
            }              
        }       
        positions2.clear();
    }

    // cout << "Best move: " << endl;
    // for (int i = 0; i < bestMove.size(); i++){
    //     cout << bestMove[i].first << " - " <<bestMove[i].second << endl;
    // }
    // cout << endl << endl;
    // getchar(); 
}

void sarpRoute::insert(instanceStat *inst, double **Mdist, int node, int position, double profit){

    double delta = profit - inst->costkm*(Mdist[this->nodes_[position-1]][node]
                + Mdist[node][this->nodes_[position]]
                - Mdist[this->nodes_[position-1]][this->nodes_[position]]);
    
    // cout << "Delta: " << delta << endl; 
    // getchar();
    cost_ += delta;
    this->nodes_.insert(this->nodes_.begin() + position, node);
}

void sarpRoute::erase(instanceStat *inst, double **Mdist, int position, double profit) {
    int node = this->nodes_[position];
    double delta = - profit - inst->costkm*(Mdist[this->nodes_[position - 1]][this->nodes_[position + 1]]
                - Mdist[this->nodes_[position - 1]][node]
                - Mdist[node][this->nodes_[position + 1]]);
    cost_ += delta;
    
    this->nodes_.erase(this->nodes_.begin() + position);
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

void sarpRoute::updateParcels(int request, int pos1, int pos2){
    pdvec[request].first = pos1;
    pdvec[request].second = pos2;
}

void sarpRoute::clearPDVec(){
    for (int i = 0; i < pdvec.size(); i++){
        pdvec[i].first = -1;
        pdvec[i].second = -1;
    }
}

void sarpRoute::updateAll (instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist){
    updatePass(inst, nodeVec);
    // cout << "Here1" << endl;
    updateLoad(inst, nodeVec);
    // cout << "Here2" << endl;
    updateTimes(inst, nodeVec, Mdist);
    // cout << "Here3" << endl;

    clearPDVec();

    for(int i = 0; i < nodes_.size(); i++){
        if(nodeVec[nodes_[i]].load > 0){
            for(int j = i+1; j < nodes_.size(); j++){
                if (nodeVec[nodes_[j]].load < 0){
                    if(nodes_[j] == nodes_[i]+inst->m){
                        updateParcels(nodes_[i]-inst->n, i, j);
                        // cout << "i: " << i << " j: " << j << endl;
                        break;
                    }
                }
            }
        }
    }
    // cout << "Here4" << endl;
}

int sarpRoute::getNextPass(int request){

    for (int i = 0; i < passandpos.size(); i++){
        if (passandpos[i].second > request){
            return passandpos[i].second;
        }
    }

    return -1;
}

int sarpRoute::getPrevPass(int request){
    int passpos = -1;

    for (int i = 0; i < passandpos.size(); i++){
        if (passandpos[i].second < request){
            passpos = passandpos[i].second;
        }
        else{
            break;
        }
    }
    return passpos;
}

pair <int, int> sarpRoute::getInterval(int req){

    int prevpas, postpas;

    prevpas = getPrevPass(req);
    postpas = getNextPass(req);

    if (prevpas < 0){
        prevpas = 0;
    }
    if (postpas < 0){
        postpas = nodes_.size()-1;
    }

    return make_pair(prevpas, postpas);
}

bool sarpRoute::checkInterval(instanceStat *inst, int pos1, int pos2, pair <int, int> inter1, pair <int, int> inter2){
    bool sameinterval = 0;
    
    if(inter1.first == inter2.first && inter1.second == inter2.second){
        sameinterval = 1;
        return sameinterval;
    }
    if (nodes_[pos1] < inst->n){
        if(inter2.first == pos1){
            sameinterval = 1;
        }
    }
    else if(nodes_[pos2] < inst->n){
        if(inter1.second == pos2){
            sameinterval = 1;
        }
    }
    
    return sameinterval;
}

bool sarpRoute::checkDelivery(instanceStat *inst, int pos1, int pos2, probStat* problem){
    int pu;
    pu = pdvec[nodes_[pos2]-inst->m-inst->n].first;

    pair<int, int> interpu;

    bool feasible = 1;

    if (problem->dParcel > 0){
        if(pos1 <= pu){
            feasible = 0;
            // cout << "\nDelivery cant be put before its pickup"
        }
    }
    else{
        interpu = getInterval(pu);
        if(pos1 <= interpu.second){
            feasible = 0;
            //there has to be a passenger between pu and dl
        }
    }

    return feasible;
}

void sarpRoute::printTotalTime(){
    double totaltime;
    totaltime = this->endtime - this->starttime;

    cout << "Total route time: " << totaltime << endl;
}

double sarpRoute::Swap(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, probStat* problem){
    double delta = 0;

	double bestDelta = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;

    int jstart, jend;

    pair <int, int> inter1, inter2, tempinter;

    int temp;

    bool feasible, feasPos, swap;

    swap = 0;
    // cout << "Current cost: " << cost_ << endl;
    // getchar();

	for (int i = 1; i < nodes_.size() - 2; i++) {
		delta = -Mdist[nodes_[i - 1]][nodes_[i]];
        inter1 = getInterval(i);

        // cout << "Interval of i: " << inter1.first <<  " - " << inter1.second << endl; 

        if(nodes_[i] < inst->n){
            jend = inter1.second;            
        }
        else if (nodes_[i] < inst->n+inst->m){
            temp = getDL(nodes_[i]-inst->n);
            // cout << "temp: " << temp << endl;
            if (problem->dParcel > 0){
                jend = temp;
            }
            else{
                jend = getPrevPass(temp);
            }
        }
        else{
            jend = nodes_.size() - 1;
        }
		for (int j = i + 1; j < jend; j++) {
            feasPos = 0;
            feasible = 0;

            if (nodes_[i] < inst->n && nodes_[j] < inst->n){//cannot interexchange passengers (TW)
                continue;
            }
            else{
                inter2 = getInterval(j);
                if (nodes_[i] < inst->n || nodes_[j] < inst->n){
                    feasPos = checkInterval(inst, i, j, inter1, inter2);
                }
                else if(nodeVec[nodes_[j]].load < 0){
                    feasPos = checkDelivery(inst, i, j, problem);
                }
                else{
                    feasPos = 1;
                }

                if (feasPos){
                    feasible = testSwap(inst, nodeVec, Mdist, i, j, inter1, inter2);

                    if (feasible){

                        if (j - i == 1) {
                            delta += - Mdist[nodes_[i]][nodes_[j]] 
                            + Mdist[nodes_[j]][nodes_[i]]                            
                            - Mdist[nodes_[j]][nodes_[j + 1]] 
                            + Mdist[nodes_[i - 1]][nodes_[j]]
                            + Mdist[nodes_[i]][nodes_[j + 1]];
                        }
                        else {
                            delta += - Mdist[nodes_[i]][nodes_[i + 1]] 
                            - Mdist[nodes_[j - 1]][nodes_[j]]
                            - Mdist[nodes_[j]][nodes_[j + 1]] 
                            + Mdist[nodes_[i - 1]][nodes_[j]] 
                            + Mdist[nodes_[j]][nodes_[i + 1]] 
                            + Mdist[nodes_[j - 1]][nodes_[i]] 
                            + Mdist[nodes_[i]][nodes_[j + 1]];
                        }

                        if (delta < 0) {
                            if (delta < bestDelta) {
                                // cout << "\nThere was an improvement with delta as " << delta << endl;
                                // getchar();
                                bestDelta = delta;
                                pos1 = i;
                                pos2 = j;
                                swap = 1;
                            }
                        }
                    }
                }          
            }			
		}
	}
    if (swap){

        tempElement = nodes_[pos1];
        nodes_[pos1] = nodes_[pos2];
        nodes_[pos2] = tempElement;

        updateAll(inst,nodeVec, Mdist);
        bestDelta = -(bestDelta*inst->costkm);
        // cout << "Best Delta: " << bestDelta << endl;

        updateCost(bestDelta);

        // cout << "New cost: " << cost_ << endl;

        calcCost(inst, nodeVec, Mdist);
        // cout << "Calculated cost: " << cost_ << endl;
        // getchar();

        // cout << "Route with swap: " << endl;
        // for (int a = 0; a < nodes_.size(); a++){
        //     cout << nodes_[a] << " - ";
        // }
        // cout << endl;
        // getchar();
    }
    else{
        delta = 0;
    }
    return bestDelta;
}

double sarpRoute::relocateK(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, probStat* problem, int k){

    double delta = 0;

	double bestDelta = 0;
	double newCost = 0;

	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;

    int jstart, jend;

    pair <int, int> inter1, tempinter;

    int temp;

    bool feasible, feasPos, relocate;

    relocate = 0;

    int candidate;
    // cout << "Current cost: " << cost_ << endl;
    // getchar();

	for (int i = 1; i < nodes_.size() - 1; i++) {
		delta = 0;
        inter1 = getInterval(i);

        candidate = nodes_[i];

        // cout << "Interval of i: " << inter1.first <<  " - " << inter1.second << endl; 

        if(candidate < inst->n){
            jstart = inter1.first + 1;
            jend = inter1.second + 1;            
        }
        else if (candidate < inst->n+inst->m){
            jstart = 1;
            
            temp = getDL(nodes_[i]-inst->n);
            // cout << "temp: " << temp << endl;
            if (problem->dParcel > 0){
                jend = temp + 1;
            }
            else{
                jend = getPrevPass(temp) + 1;
            }
        }
        else{
            temp = getPU(candidate - inst->m - inst->n);
            // cout << "temp: " << temp; 

            if (problem->dParcel > 0){
                jstart = temp + 1;
            }
            else{
                jstart = getNextPass(temp) + 1;
            }
            
            jend = nodes_.size() - 1;
        }
        // cout << "**************************" << endl;
        // cout << "Candidate: " << candidate << endl;
        // cout << "jstart: " << jstart << endl;
        // cout << "jend: " << jend << endl;
        // cout << "**************************" << endl;
        // getchar();

		for (int j = jstart; j < jend; j++) {

            if (j == i+1 || i == j+1 || i == j){
                continue;
            }

            // if (feasPos){
                feasible = testRelocate(inst, nodeVec, Mdist, i, j, inter1);
                // cout << "new position: " << j << ": " << feasible << endl;
                if (feasible){
                    delta = - Mdist[nodes_[i - 1]][nodes_[i]]
                    - Mdist[nodes_[i]][nodes_[i + 1]] 
                    - Mdist[nodes_[j - 1]][nodes_[j]]
                    + Mdist[nodes_[i - 1]][nodes_[i + 1]] 
                    + Mdist[nodes_[j - 1]][nodes_[i]] 
                    + Mdist[nodes_[i]][nodes_[j]];

                    // cout << "Value of delta: " << delta << endl;
                    // cout << "**************************" << endl;

                    if (delta < 0) {
                        if (delta < bestDelta) {

                            // cout << "Calculated delta: " << delta << endl;

                            // cout << "New position: " << j;

                            // cout << "Values: " << endl;
                            // cout << "Mdist[nodes_[i - 1]][nodes_[i]]: " << Mdist[nodes_[i - 1]][nodes_[i]]
                            // << endl << "- Mdist[nodes_[i]][nodes_[i + 1]]: " << Mdist[nodes_[i]][nodes_[i + 1]]
                            // << endl<<"- Mdist[nodes_[j - 1]][nodes_[j]]: " << Mdist[nodes_[j - 1]][nodes_[j]]
                            // << endl<< "+ Mdist[nodes_[i - 1]][nodes_[i + 1]]: " << + Mdist[nodes_[i - 1]][nodes_[i + 1]] 
                            // <<  endl<<"+ Mdist[nodes_[j - 1]][nodes_[i]]: " << Mdist[nodes_[j - 1]][nodes_[i]] 
                            // <<  endl<<"+ Mdist[nodes_[i]][nodes_[j]]: " << Mdist[nodes_[i]][nodes_[j]]; 
                            // cout << "**************************" << endl;
                            // cout << "\nThere was an improvement with delta as " << delta << endl;
                            // getchar();
                            bestDelta = delta;
                            pos1 = i;
                            pos2 = j;
                            relocate = 1;
                        }
                    }
                }
            // }          
		}
	}
    if (relocate){
        double profit;
        profit = getProfit(nodeVec, pos1);
        int candidate;
        candidate = nodes_[pos1];

        bestDelta = -(bestDelta*inst->costkm);

        if (pos1 < pos2){
            insert(inst, Mdist, candidate, pos2, profit);
            erase(inst, Mdist, pos1, profit);
        }

        else{
            erase(inst, Mdist, pos1, profit);
            insert(inst, Mdist, candidate, pos2, profit);           
        }

        // updateAll(inst,nodeVec, Mdist);

        // cout << "New cost: " << cost_ << endl;

        // calcCost(inst, nodeVec, Mdist);
        // cout << "Calculated cost: " << cost_ << endl;
        // getchar();

        // cout << "Route with relocate K: " << endl;
        // for (int a = 0; a < nodes_.size(); a++){
        //     cout << nodes_[a] << " - ";
        // }
        // cout << endl;
        // getchar();
    }
    else{
        delta = 0;
    }
    return bestDelta;
}


double sarpRoute::rmvVal(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int candidate, bool isparcel){

    double delta = 0;

    double profit = getProfit(nodeVec, candidate);

    if (isparcel){
        // cout << "It is a parcel" << endl;
        // getchar();
        int dl, dlpos;
        dl = nodes_[candidate]+inst->m;
        dlpos = pdvec[nodes_[candidate]-inst->n].second;

        // cout << "dl: " << dl << " - in position: " << dlpos << endl;
        // getchar();

        if(dlpos - candidate == 1){
            delta += - profit - inst->costkm*(- Mdist[nodes_[candidate-1]][nodes_[candidate]]
                    + Mdist[nodes_[candidate-1]][nodes_[dlpos + 1]]
                    - Mdist[nodes_[dlpos]][nodes_[dlpos + 1]]
                    - Mdist[nodes_[candidate]][nodes_[dlpos]]);
        }
        else{
            delta += - profit - inst->costkm*(- Mdist[nodes_[candidate-1]][nodes_[candidate]]
                + Mdist[nodes_[candidate-1]][nodes_[candidate + 1]]
                - Mdist[nodes_[candidate]][nodes_[candidate + 1]]
                - Mdist[nodes_[dlpos-1]][nodes_[dlpos]]
                + Mdist[nodes_[dlpos-1]][nodes_[dlpos + 1]]
                - Mdist[nodes_[dlpos]][nodes_[dlpos + 1]]);
        }
    }
    else{
        delta += - profit - inst->costkm*(- Mdist[nodes_[candidate-1]][nodes_[candidate]]
                + Mdist[nodes_[candidate-1]][nodes_[candidate + 1]]
                - Mdist[nodes_[candidate]][nodes_[candidate + 1]]);
    }

    return delta;
}

