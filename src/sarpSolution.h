#ifndef SARPSOLUTION_H
#define SARPSOLUTION_H

#include <algorithm>

#include "sarpRoute.h"
#include "readdata.h"
#include "Statistics.h"

using namespace std;

class sarpSolution {
protected:

    vector<sarpRoute> routes;
    double cost;

    vector<nodeStat> unservParc;

    int usedK;
    // vector<vector<vector<bool>>> pairNeighbStatus;
    vector <int> unserved;
    // void createRoutesVector();
    // void updateCost();
    // void updateRoutes();

public:

    Runtime stats;
    sarpSolution() { this->cost = INT32_MAX; this->usedK = 0; }

    // Solution(Data *data);
    // Solution(Data *data, vector<Route *> routesVec);
    // Solution(const Solution &other);
    ~sarpSolution(){};

    inline double getCost() { return cost; }
    vector<sarpRoute> getRoutes() { return routes; }
    sarpRoute getRoute(int idx) { return routes[idx]; }

    inline int getvehicle() { return this->usedK; }
    
    inline int getRoutesSize() const { return this->routes.size(); }

    void addRoute(sarpRoute* route);
    
    void updateRoutes(sarpRoute *route,  int idr);

    // void addCost(double delta);
    
    void printCosts();

    // access routes
    vector<sarpRoute>::iterator begin() { return routes.begin(); }
    vector<sarpRoute>::const_iterator begin() const { return routes.begin(); }
    vector<sarpRoute>::iterator end() { return routes.end(); }
    vector<sarpRoute>::const_iterator end() const { return routes.end(); }

    //access unserved parcels
    vector<int>::iterator ubegin() { return unserved.begin(); }
    vector<int>::const_iterator ubegin() const { return unserved.begin(); }
    vector<int>::iterator uend() { return unserved.end(); }
    vector<int>::const_iterator uend() const { return unserved.end(); }
    void addtounserved(int candidate);
    void clearunserved() { unserved.clear(); }
    int unservedsize() { return unserved.size(); } 

    void addunserved(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);

    void updateCost();
    void updateVehicles();

    void printSol(instanceStat *inst);

    double relocate (instanceStat *inst, vector<nodeStat> &nodeVec, 
                                double **Mdist, probStat* problem,
                                 int rid1, int rid2, int &currCand, 
                                 pair <int, int> &currPairPos);


    // sarpRoute *getRoute(int idx) { return routes[idx]; }
    // inline int getRoutesSize() const { return this->routes.size(); }s
};

#endif