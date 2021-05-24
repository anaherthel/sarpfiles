#include "MyIncumbentCallback.h"

MyIncumbentCallback::MyIncumbentCallback(IloEnv env,
                                         const IloBoolVarArray &y_ref,
                                         Data *data,
                                         SetPartitioning *sp) : IloCplex::IncumbentCallbackI(env),
                                                                y(y_ref),
                                                                data(data),
                                                                sp(sp),
                                                                bestIncSol(new Solution(data)) {
    bestIncSol->update();
};

void MyIncumbentCallback::main() {
    Solution *ilsSol;
    Solution *spSol;
    RoutePool *model_pool = sp->model_pool;
    RoutePool *pool       = sp->pool;
    RoutePool *pool_temp  = sp->pool_temp;

    int I_ils = sp->I_ils;
    int I_p   = sp->I_p;

    // Building Solution
    vector<Route *> routesVec;
    vector<Vehicle> vhcs;
    vector<bool> vhcUsed(data->v, false);
    int poolSize = model_pool->pool.size();

    for (int i = 0; i < data->v; ++i) {
        vhcs.push_back(data->vehicles[i]);
    }

    for (int i = 0; i < poolSize; ++i) {
        if (getValue(y[i]) > 0.0001) {
            for (int vhc = 0; vhc < vhcs.size(); ++vhc) {
                if (!vhcUsed[vhc] &&
                    (vhcs[vhc] == model_pool->pool[i]->getVehicleCopy())) {
                    vhcUsed[vhc] = true;
                    model_pool->pool[i]->setVehicle(data->vehicles[vhc]);
                    model_pool->pool[i]->updateDepot();

                    Route *r = new Route(*model_pool->pool[i]);
                    routesVec.push_back(r);
                    break;
                }
            }
        }
    }

    spSol = new Solution(data, routesVec);

    // Add extra vehiculo if solution not use all vehicles
    while (spSol->getRoutesSize() < data->v) {
        for (int k = 0; k < data->v; k++) {
            if (!vhcUsed[k]) {
                spSol->addRoute(new Route(data, data->vehicles[k]));
            }
        }
    }

    spSol->repairSol();

    // ILS in icbk solution
    ILS *ils = new ILS(data, I_ils, I_p, spSol);
    ilsSol   = ils->run(pool, pool_temp);

    if (ilsSol->getCost() < spSol->getCost()) {
        if (ilsSol->getCost() < bestIncSol->getCost() ||
            bestIncSol->getCost() == 0) {
            delete bestIncSol;
            bestIncSol = new Solution(*ilsSol);
        }

    } else {
        if (spSol->getCost() < bestIncSol->getCost() ||
            bestIncSol->getCost() == 0) {
            delete bestIncSol;
            bestIncSol = new Solution(*spSol);
        }
    }

    delete ils;
    delete spSol;
    delete ilsSol;
    clear(&routesVec);

    double lowerBound = getBestObjValue();

    if (lowerBound > bestIncSol->getCost()) {
        abort();
    }
}

void MyIncumbentCallback::clear(vector<Route *> *routesVec) {
    for (vector<Route *>::iterator it = routesVec->begin();
         it != routesVec->end(); it++) {
        delete *it;
    }
    routesVec->clear();
}