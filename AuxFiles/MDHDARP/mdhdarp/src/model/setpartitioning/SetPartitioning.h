#ifndef SET_PARTITIONING_H
#define SET_PARTITIONING_H

#include <ilcplex/ilocplex.h>

#include <vector>

#include "../../ILS.h"
#include "../Solution.h"
#include "../Vehicle.h"
#include "MyBranchCallback.h"
#include "MyIncumbentCallback.h"
#include "RoutePool.h"

class SetPartitioning {
   private:
    Data *data;

   public:
    RoutePool *model_pool, *pool, *pool_temp;
    int I_ils, I_p, sp_tl;

    SetPartitioning(Data *data,
                    int I_ils, int I_p,
                    int sp_tl, bool is_temp,
                    RoutePool *pool,
                    RoutePool *pool_temp);
    Solution *run();
};

#endif
