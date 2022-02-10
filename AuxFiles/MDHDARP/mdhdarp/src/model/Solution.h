#ifndef SOLUTION_H
#define SOLUTION_H

#include <algorithm>

#include "../operations/checkers/ArcsChecker.h"
#include "../operations/checkers/LoadChecker.h"
#include "../operations/checkers/PairChecker.h"
#include "../operations/checkers/RideTimeChecker.h"
#include "../operations/checkers/TimeWindowChecker.h"
#include "Data.h"
#include "Route.h"
#include "Utils.h"
using namespace std;

class Solution {
   protected:
    vector<Route *> routes;
    double cost;

    vector<vector<vector<bool>>> pairNeighbStatus;

    void createRoutesVector();
    void updateCost();
    void updateRoutes();

   public:
    Data *data;
    Solution() { this->cost = INT32_MAX; }
    Solution(Data *data);
    Solution(Data *data, vector<Route *> routesVec);
    Solution(const Solution &other);
    ~Solution();

    inline double getCost() { return cost; }
    vector<Route *> getRoutes() { return routes; }
    Route *getRoute(int idx) { return routes[idx]; }
    inline int getRoutesSize() const { return this->routes.size(); }

    inline bool getInterNStatus(NEIGHBORHOOD nighborhood,
                                int route_1_idx,
                                int route_2_idx) {
        return pairNeighbStatus[nighborhood][route_1_idx][route_2_idx];
    }
    void setPairNeighbStatus(NEIGHBORHOOD nighborhood,
                             int route_1_idx,
                             int route_2_idx,
                             bool value) {
        pairNeighbStatus[nighborhood][route_1_idx][route_2_idx] = value;
    }
    void setPairNeighbStatusAllTrue(int route_1_idx, int route_2_idx);
    void setPairNeighbStatusAllTrue(int route_idx);

    void update();
    void updateRoute(int idx);
    void repairSol();
    void sortRoutes();
    void addRoute(Route *route);
    Route *popRoute();
    void removeRoute();
    void check();
    void clear();
    void print();
    bool operator<(const Solution &other) { return (cost < other.cost); }
};

#endif