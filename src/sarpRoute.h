#ifndef SARPROUTE_H_
#define SARPROUTE_H_

#include <vector>
#include <algorithm>

#include "readdata.h"
#include "Statistics.h"

class sarpRoute {
protected:

    // the cost of this route
    double cost_;
    // route length
    double length_;
    //start time
    double starttime;
    //end time
    double endtime;
    //first and last passenger requests
    int firstPass;
    int lastPass;
    //first and last passenger request positions
    int firstPassPos;
    int lastPassPos;
    //route id
    int id;

    vector <pair <nodeStat, int> > passandpos; //passengers and their positions in the route
    vector < pair <int, int> > pdvec; //first: position of parcel pickup; second: position of parcel delivery in the route.
    vector <pair <int, int> > loadofroute; //first:actual load; second:passengers visited while carrying parcels.


public:
    // the route
    vector<int> nodes_;
    Runtime stats;

    // default constructor: route with only starting and ending depot visits
    sarpRoute(instanceStat *inst, int vehicle);
    // access route elements
    double cost() const { return cost_; };
    double length() const { return length_; };
    double startTime() const { return starttime; };
    double endTime() const { return endtime; };
    int fPass() const { return firstPass; };
    int lPass() const { return lastPass; };
    
    void printTotalTime();

    inline int getNodesSize() { return nodes_.size(); };
    inline int getLoadSize() { return loadofroute.size(); };
    inline int getPassPosSize() { return passandpos.size(); };
    inline int getPaPdvecSize() { return pdvec.size(); };

    pair <nodeStat, int> getPassReq(int i) { return passandpos[i]; } ;
    pair <int, int> getPDReq(int i) { return pdvec[i]; };

    int firstpos() const { return nodes_[1]; };
    int lastpos() const { return nodes_[nodes_.size()-2]; };
    int getReq(int pos) const { return nodes_[pos]; };

    int getPU(int request) const { return pdvec[request].first; };
    int getDL(int request) const { return pdvec[request].second; };

    int getNextPass(int request);
    int getPrevPass(int request);

    inline int getId() { return id; };

    double getProfit (vector<nodeStat> &nodeVec, int pos) { return nodeVec[nodes_[pos]].profit; };
    
    void updateCost(double delta) { cost_ += delta; }

    void clearPDVec();

    vector<int>::iterator begin() { return nodes_.begin(); };
    vector<int>::iterator end() { return nodes_.end(); };
    bool testInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request);
    bool testInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int pos1, int pos2, int pu, int dl);

    bool testSwap(instanceStat *inst, vector<nodeStat> &nodeVec,
                         double **Mdist,int pos1, int pos2, 
                         pair <int, int> inter1, pair <int, int> inter2);
    bool testRelocate(instanceStat *inst, vector<nodeStat> &nodeVec,
                            double **Mdist,int pos1, int pos2, 
                            pair <int, int> inter1);
    void calcCost(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist);

    //only when the first insertion is a passenger request.
    bool fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request);

    // int load() const { return load_; };
    bool fInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int pu, int dl);
    //update starting and end times after insertion or erasing.
    void updateTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist);


    //updates passenger list with their positions.
    void updatePass(instanceStat *inst, vector<nodeStat> &nodeVec);

    void updateLoad(instanceStat *inst, vector<nodeStat> &nodeVec);
    //determine available positions to add new passenger request 
    void availablePos(instanceStat *inst, vector<nodeStat> &nodeVec, int request, probStat* problem, vector<int> &positions);

    void updateParcels(int request, int pos1, int pos2);
    // // evaluate the cost of cheapest insertion of node in this route
    // // return a <position, cost> pair
    // // If no insertion is feasible, the cost is INT_MAX
    pair<int, double> cheapestInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, vector<int> &positions);
    
    //testing insertion of parcel pickup and delivery simultaneously
    void cheapestInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, int node2, vector<int> &positions, vector<int> &positions2, vector< pair<int, double> > &bestMove, probStat* problem);

    // // pre-condition: the insertion is feasible
    void insert(instanceStat *inst, double **Mdist, int node, int position, double profit);

    void erase(instanceStat *inst, double **Mdist, int position, double profit);

    void printLoad();
    
    pair <int, int> getInterval(int req);
    
    bool checkInterval(instanceStat *inst, int pos1, int pos2, pair <int, int> inter1, pair <int, int> inter2);
    bool checkDelivery(instanceStat *inst, int pos1, int pos2, probStat* problem);

    double Swap(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, probStat* problem);
    
    double relocateK(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, probStat* problem, int k);

    void updateAll (instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist);

    double rmvVal (instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int candidate, bool isparcel);

    
    // // improve route with 2-opt
    // // returns the number of improvements performed
    // int two_opt(const CVRPInstance& inst);

    // int local_search(const CVRPInstance& inst);

    // int bestTwoOpt(const CVRPInstance& inst);

    // // relocate K customers
    // int relocateK(const CVRPInstance& inst, int k);

    // //swap any 2 customers
    // int swap(const CVRPInstance& inst);

    // int VND(const CVRPInstance& inst);
};

#endif
