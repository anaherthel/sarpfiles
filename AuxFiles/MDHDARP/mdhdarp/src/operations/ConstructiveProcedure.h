#ifndef CONSTRUCTIVEPROCEDURE_H
#define CONSTRUCTIVEPROCEDURE_H

#include <cstdio>
#include <iostream>
#include <limits>

#include "../model/ADS.h"
#include "../model/Data.h"
#include "../model/MoveInfo.h"
#include "../model/Route.h"
#include "../model/Solution.h"
#include "../model/Statistics.h"
#include "../model/Utils.h"
#include "checkers/RideTimeChecker.h"

class ConstructiveProcedure {
   private:
    Data *data;
    Statistics *stats;
    ADS *ads;
    vector<int> CL;

    void initCL();
    void addRequestInEachRoute(Solution *sol);
    void verifyCL();
    void parallelInsertion(Solution *solution);
    void removeFromCL(int id);

    vector<int> newPathConstructive(MoveInfo move_info, int candidate);

    bool compare(int a, int b);

   public:
    ConstructiveProcedure(Data *data, Statistics *stats, ADS *ads);

    void initSolution(Solution *sol);
    void initSPSolution(Solution *sol);
    int getRandRequestFromCL();
};

#endif