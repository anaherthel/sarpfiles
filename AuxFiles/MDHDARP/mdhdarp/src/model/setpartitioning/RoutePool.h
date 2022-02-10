#ifndef ROUTE_POOL_H
#define ROUTE_POOL_H

#include <set>
#include <utility>
#include <vector>

#include "../Data.h"
#include "../Route.h"
#include "../Solution.h"

using namespace std;

class RoutePool {
   private:
    bool isInPool(Route *route);
    int getHash(Route *route);

    struct RoutePtrComp {
        bool operator()(Route *lhs, Route *rhs) { return lhs->getCost() < rhs->getCost(); }
    };

   public:
    vector<Route *> pool;
    set<Route *, RoutePtrComp> setPool;

    RoutePool();
    ~RoutePool();

    void update(Solution *sol);
    void updateSet(Solution *sol);
    void addInitialSol(Solution *sol);
    void mergePool(RoutePool *other);
    void setToList();
    void clear();
    void print();
};

#endif