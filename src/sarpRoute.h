#ifndef SARPROUTE_H_
#define SARPROUTE_H_

#include <vector>
#include <algorithm>

#include "readdata.h"

class sarpRoute {
protected:
    // the route
    vector<int> nodes_;
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

    vector <int> prevReq;
    vector <int> postReq;
public:
    // default constructor: route with only starting and ending depot visits
    sarpRoute(instanceStat *inst);
        // access route elements
    double cost() const { return cost_; };
    double length() const { return length_; };
    double startTime() const { return starttime; };
    double endTime() const { return endtime; };
    int fPass() const { return firstPass; };
    int lPass() const { return lastPass; };

    inline int getNodesSize() { return nodes_.size(); };

    int firstpos() const { return nodes_[1]; };
    int lastpos() const { return nodes_[nodes_.size()-1]; };

    inline int getId() { return id; };

    vector<int>::iterator begin() { return nodes_.begin(); };
    vector<int>::iterator end() { return nodes_.end(); };
    void insertRequest(int id, int position);
    bool routeTimes(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int position, int request);

    void calcCost(double **Mdist);

    //only when the first insertion is a passenger request.
    bool fInsertion(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, int request);
    // int load() const { return load_; };
    

    // // evaluate the cost of cheapest insertion of node in this route
    // // return a <position, cost> pair
    // // If no insertion is feasible, the cost is INT_MAX
    // pair<int, int> cheapestInsertion(const CVRPInstance& inst, int node) const;

    // // pre-condition: the insertion is feasible
    // void insert(const CVRPInstance& inst, int node, int position);

    // void erase(const CVRPInstance& inst, int position);

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
