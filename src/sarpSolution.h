#ifndef SARPSOLUTION_H
#define SARPSOLUTION_H

#include <algorithm>

#include "sarpRoute.h"
using namespace std;

class sarpSolution {
   protected:

    vector<sarpRoute> routes;
    double cost;

    // vector<vector<vector<bool>>> pairNeighbStatus;

    // void createRoutesVector();
    // void updateCost();
    // void updateRoutes();

   public:
    
    sarpSolution() { this->cost = INT32_MAX; }
    // Solution(Data *data);
    // Solution(Data *data, vector<Route *> routesVec);
    // Solution(const Solution &other);
    ~sarpSolution(){};

    inline double getCost() { return cost; }
    vector<sarpRoute> getRoutes() { return routes; }
    sarpRoute getRoute(int idx) { return routes[idx]; }

    inline int getRoutesSize() const { return this->routes.size(); }

    void addRoute(sarpRoute* route);

    // access routes
    vector<sarpRoute>::iterator begin() { return routes.begin(); }
    vector<sarpRoute>::const_iterator begin() const { return routes.begin(); }
    vector<sarpRoute>::iterator end() { return routes.end(); }
    vector<sarpRoute>::const_iterator end() const { return routes.end(); }

    // sarpRoute *getRoute(int idx) { return routes[idx]; }
    // inline int getRoutesSize() const { return this->routes.size(); }s
};

#endif