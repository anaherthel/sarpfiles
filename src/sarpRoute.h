#ifndef SARPROUTE_H_
#define SARPROUTE_H_

#include <vector>
#include <algorithm>

#include "readdata.h"

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
    vector <int> prevReq;
    vector <int> postReq;
    vector <pair <int, int> > loadofroute; //first:actual load; second:passengers visited while carrying parcels.
public:
    // the route
    vector<int> nodes_;

    // default constructor: route with only starting and ending depot visits
    sarpRoute(instanceStat *inst, int vehicle);
    // access route elements
    double cost() const { return cost_; };
    double length() const { return length_; };
    double startTime() const { return starttime; };
    double endTime() const { return endtime; };
    int fPass() const { return firstPass; };
    int lPass() const { return lastPass; };

    inline int getNodesSize() { return nodes_.size(); };
    inline int getLoadSize() { return loadofroute.size(); };

    int firstpos() const { return nodes_[1]; };
    int lastpos() const { return nodes_[nodes_.size()-2]; };

    inline int getId() { return id; };

    vector<int>::iterator begin() { return nodes_.begin(); };
    vector<int>::iterator end() { return nodes_.end(); };
    bool testInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request);
    
    bool testInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int pos1, int pos2, int pu, int dl);

    bool testScen(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request, probStat* problem);

    void calcCost(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist);

    //only when the first insertion is a passenger request.
    bool fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request);
    // int load() const { return load_; };

    //update starting and end times after insertion or erasing.
    void updateTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist);

    //updates passenger list with their positions.
    void updatePass(instanceStat *inst, vector<nodeStat> &nodeVec);

    void updateLoad(instanceStat *inst, vector<nodeStat> &nodeVec);
    //determine available positions to add new passenger request 
    void availablePos(instanceStat *inst, vector<nodeStat> &nodeVec, int request, probStat* problem, vector<int> &positions);

    // // evaluate the cost of cheapest insertion of node in this route
    // // return a <position, cost> pair
    // // If no insertion is feasible, the cost is INT_MAX
    pair<int, double> cheapestInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, vector<int> &positions);
    
    //testing insertion of parcel pickup and delivery simultaneously
    void cheapestInsertionParcel(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int node, int node2, vector<int> &positions, vector<int> &positions2, vector< pair<int, double> > &bestMove);

    // // pre-condition: the insertion is feasible
    void insert(instanceStat *inst, double **Mdist, int node, int position);

    void erase(instanceStat *inst, double **Mdist, int position);

    void printLoad();

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
