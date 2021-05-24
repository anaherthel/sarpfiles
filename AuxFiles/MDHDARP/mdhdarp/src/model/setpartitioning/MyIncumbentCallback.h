#ifndef MY_INCUMBENT_CALLBACK_H
#define MY_INCUMBENT_CALLBACK_H

#include <ilcplex/ilocplex.h>
#include <time.h>

#include <vector>

#include "../../ILS.h"
#include "../Data.h"
#include "../Route.h"
#include "../Solution.h"
#include "../Vehicle.h"
#include "RoutePool.h"
#include "SetPartitioning.h"

/** Incumbent callback */
using namespace std;

class ILS;
class SetPartitioning;

class MyIncumbentCallback : public IloCplex::IncumbentCallbackI {
   private:
    Data *data;
    SetPartitioning *sp;
    IloBoolVarArray y;

    void clear(vector<Route *> *routesVec);

   public:
    MyIncumbentCallback(IloEnv env,
                        const IloBoolVarArray &y_ref,
                        Data *data, SetPartitioning *sp);

    IloCplex::CallbackI *duplicateCallback() const {
        return new (getEnv()) MyIncumbentCallback(*this);
    }

    void main();
    Solution *bestIncSol;
};

#endif