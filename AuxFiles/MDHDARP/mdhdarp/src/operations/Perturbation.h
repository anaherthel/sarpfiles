#ifndef PERTURBATION_H
#define PERTURBATION_H

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>

#include "../model/ADS.h"
#include "../model/MoveInfo.h"
#include "../model/Request.h"
#include "../model/Route.h"
#include "../model/Solution.h"
#include "../model/Statistics.h"
#include "../model/Utils.h"

using namespace std;
class Perturbation {
   private:
    ADS *ads;
    Data *data;
    Statistics *stats;
    Route *extraRoute;
    int I_p;

    MoveInfo getRandomRmvPos(Route *r, int r_idx);
    MoveInfo getRandomInsPos(Route *r);
    int getRandomRmvPosZero(int r);

    void blockPerturbation(Solution *solution);
    void requestPerturbation(Solution *solution);

   public:
    Perturbation(Data *data, Statistics *stats, ADS *ads, int I_p, Route *extraRoute);
    ~Perturbation();

    Solution *run(Solution *sol);
};

#endif