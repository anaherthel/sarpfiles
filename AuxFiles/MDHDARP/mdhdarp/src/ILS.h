#ifndef ILS_H
#define ILS_H

#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>

#include "model/ADS.h"
#include "model/Solution.h"
#include "model/Statistics.h"
#include "model/Utils.h"
#include "model/setpartitioning/RoutePool.h"
#include "model/setpartitioning/SetPartitioning.h"
#include "operations/ConstructiveProcedure.h"
#include "operations/LocalSearch.h"
#include "operations/Perturbation.h"
#include "operations/checkers/LoadChecker.h"
#include "operations/checkers/PairChecker.h"
#include "operations/checkers/RideTimeChecker.h"
#include "operations/checkers/TimeWindowChecker.h"
using namespace std;

class ILS {
   private:
    Data *data;
    Statistics *stats;
    int I_r, I_ils, I_p, sp_tl;
    bool incumbent;

    RoutePool *bestStartsPool;
    Route *extraRoute;
    Solution *solution;
    Solution *bestIterSolution;
    Solution *auxSol;

   public:
    ILS(Data *data, Statistics *stats, int I_r, int I_ils, int I_p, int sp_tl);
    ILS(Data *data, int I_ils, int I_p, Solution *sol);
    ~ILS();

    Solution *bestSolution;

    Solution *run(RoutePool *pool, RoutePool *pool_temp);
};

#endif