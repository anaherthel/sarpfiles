#include "SetPartitioning.h"

SetPartitioning::SetPartitioning(Data *data,
                                 int I_ils, int I_p,
                                 int sp_tl, bool is_temp,
                                 RoutePool *pool,
                                 RoutePool *pool_temp) : data(data),
                                                         I_ils(I_ils),
                                                         I_p(I_p),
                                                         sp_tl(sp_tl),
                                                         pool(pool),
                                                         pool_temp(pool_temp) {
    model_pool = is_temp ? pool_temp : pool;
};

Solution *SetPartitioning::run() {
    // puts("\n\n----- SET PARTITIONING ------");
    // printf("Pool size: %lu\n", pool->pool.size());
    // printf("Pool_Temp size: %lu\n", pool_temp->pool.size());

    vector<vector<int>> math_R(data->n);
    int poolSize = model_pool->pool.size();

    for (int i = 0; i < data->n; ++i) {
        for (int j = 0; j < poolSize; ++j) {
            for (int k = 1; k < model_pool->pool[j]->getPathSize() - 1; ++k) {
                if (model_pool->pool[j]->getRequestIdByPos(k) == i) {
                    math_R[i].push_back(j);
                }
            }
        }
    }

    IloEnv env;
    IloModel model(env);
    char name[10];

    // Decision variable
    IloBoolVarArray y(env, poolSize);
    for (int i = 0; i < poolSize; ++i) {
        sprintf(name, "Y(%d)", i);
        y[i].setName(name);
        model.add(y[i]);
    }

    // Objective Function
    IloExpr obj(env);
    for (int i = 0; i < poolSize; ++i) {
        obj += model_pool->pool[i]->getCost() * y[i];
    }

    model.add(IloMinimize(env, obj));

    // Constraint 1
    for (int i = 0; i < data->n; ++i) {
        IloExpr c1(env);
        for (int j = 0; j < math_R[i].size(); ++j) {
            c1 += y[math_R[i][j]];
        }
        IloRange range = (c1 == 1);
        sprintf(name, "c1_%d", i);
        range.setName(name);
        model.add(range);
    }

    // Constraint 2
    // MDHDARP
    for (int d = 0; d < data->m; ++d) {
        // HDARP
        for (int k = 0; k < data->depot_vhc[d].size(); k++) {
            // DARP
            IloExpr c2(env);
            for (int i = 0; i < poolSize; ++i) {
                if (model_pool->pool[i]->getVehicle()->getDepot() == d &&
                    model_pool->pool[i]->getVehicle()->isSameType(data->fleet[k])) {
                    c2 += y[i];
                }
            }

            IloRange range = (c2 <= data->depot_vhc[d][k]);
            sprintf(name, "c2");
            range.setName(name);
            model.add(range);
        }
    }

    IloCplex SP(model);

    // MIP START
    if (model_pool->pool.size() > data->v) {
        IloNumVarArray startVar(env);
        IloNumArray startVal(env);

        for (int i = 0; i < data->v; ++i) {
            startVar.add(y[model_pool->pool.size() - data->v + i]);
            startVal.add(1);
        }

        SP.addMIPStart(startVar, startVal);
    }

    // ***** PARAMS *****
    // SP.exportModel("SP.lp");
    SP.setOut(env.getNullStream());
    SP.setParam(IloCplex::TiLim, sp_tl);
    SP.setParam(IloCplex::Threads, 1);
    SP.setWarning(env.getNullStream());

    // ***** CALLBACKS *****
    const IloBoolVarArray &y_ref = y;

    MyIncumbentCallback *incCbk = new (env)
        MyIncumbentCallback(env, y_ref, data, this);

    MyBranchCallback *branchCbk = new (env) MyBranchCallback(env, incCbk);
    SP.use(incCbk);
    SP.use(branchCbk);

    SP.solve();

    // ******* BUILD SOLUTION *****
    vector<Route *> routesVec;
    vector<Vehicle> vhcs;
    vector<bool> vhcUsed(data->v, false);
    double value = SP.getObjValue();

    for (int i = 0; i < data->v; ++i) {
        vhcs.push_back(data->vehicles[i]);
    }

    for (int i = 0; i < poolSize; ++i) {
        if (SP.getValue(y[i]) > 0.0001) {
            for (int vhc = 0; vhc < vhcs.size(); ++vhc) {
                if (!vhcUsed[vhc] && vhcs[vhc] == model_pool->pool[i]->getVehicleCopy()) {
                    vhcUsed[vhc] = true;
                    model_pool->pool[i]->setVehicle(data->vehicles[vhc]);
                    model_pool->pool[i]->updateDepot();
                    routesVec.push_back(model_pool->pool[i]);
                    break;
                }
            }
        }
    }

    Solution *sol = new Solution(data, routesVec);

    // Add extra vehiculo if solution not use all vehicles
    while (sol->getRoutesSize() < data->v) {
        for (int k = 0; k < data->v; k++) {
            if (!vhcUsed[k]) {
                sol->addRoute(new Route(data, data->vehicles[k]));
            }
        }
    }

    sol->update();

    if (incCbk->bestIncSol->getCost() < sol->getCost()) {
        delete sol;
        sol = new Solution(*incCbk->bestIncSol);
    }

    delete incCbk->bestIncSol;
    delete branchCbk;
    delete incCbk;
    env.end();

    return sol;
}