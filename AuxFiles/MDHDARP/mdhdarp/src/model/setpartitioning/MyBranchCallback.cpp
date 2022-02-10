#include "MyBranchCallback.h"

#include <ilcplex/ilocplex.h>

#include <iostream>

MyBranchCallback::MyBranchCallback(IloEnv env,
                                   MyIncumbentCallback *incCbk) : IloCplex::BranchCallbackI(env),
                                                                  incCbk(incCbk) {}

// Returns a callback's copy. This method is a cplex requirement
IloCplex::CallbackI *MyBranchCallback::duplicateCallback() const {
    return new (getEnv()) MyBranchCallback(getEnv(), incCbk);
}

void MyBranchCallback::main() {
    double lowerBound = getBestObjValue();

    if (lowerBound >= incCbk->bestIncSol->getCost()) {
        // puts("ABORTING");
        abort();
    }

    if (ceil(getObjValue()) >= incCbk->bestIncSol->getCost())
        prune();
}