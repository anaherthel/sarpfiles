#ifndef LOCAL_SEARCH_H
#define LOCAL_SEARCH_H

#include <algorithm>
#include <cstdio>
#include <iomanip>  // for std::setprecision()
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
#include "../model/setpartitioning/RoutePool.h"
#include "checkers/LoadChecker.h"
#include "checkers/PairChecker.h"
#include "checkers/RideTimeChecker.h"
#include "checkers/TimeWindowChecker.h"

using namespace std;
class LocalSearch {
    Solution *solution;
    Solution *bestSolution;
    ADS *ads;
    Data *data;
    Statistics *stats;

    vector<NEIGHBORHOOD> intraNeighborhoodList;
    vector<NEIGHBORHOOD> interNeighborhoodList;
    vector<NEIGHBORHOOD> blockNeighborhoodList;

   public:
    LocalSearch(Data *data, Statistics *stats, ADS *ads);
    ~LocalSearch();

    void run(Solution *sol, int iter, int maxIter);
    void runInter(Solution *sol);  //Extern Block -> Inter norm
    void runIntra(Solution *sol);

    Solution *getBestSolution() { return bestSolution; }
    void updateLocalSearchInter(MoveInfo move_info);
    void updateLocalSearchIntra(MoveInfo move_info);

    //inter-route
    bool rvndInter(NEIGHBORHOOD move);
    void exploreRelocateInter(bool *moveEvaluation);
    void exploreExchangeInter(bool *moveEvaluation);
    void exploreTwoOptInter(bool *moveEvaluation);
    void exploreExchangeVhcInter(bool *moveEvaluation);
    void exploreRelocateBlock(bool *moveEvaluation);
    void exploreExchangeBlock(bool *moveEvaluation);

    void applyRelocateInter(MoveInfo move_info);
    void applyExchangeInter(MoveInfo move_info);
    void applyTwoOptInter(MoveInfo move_info);
    void applyExchangeVhcInter(MoveInfo move_info);
    void applyRelocateBlock(MoveInfo move_info);
    void applyExchangeBlock(MoveInfo move_info);

    vector<int> newPathRelocateInter(MoveInfo move_info);
    vector<int> newPathExchangeInter(MoveInfo move_info);
    vector<int> newPathTwoOptInter(MoveInfo move_info);
    vector<int> newPathExchangeVhcInter(MoveInfo move_info);
    vector<int> newPathRelocateBlock(MoveInfo move_info);
    vector<int> newPathExchangeBlock(MoveInfo move_info);

    //intra-route
    bool rvndIntra(NEIGHBORHOOD move);
    void exploreRelocateIntra(bool *moveEvaluation);
    void exploreExchangeIntra(bool *moveEvaluation);
    void exploreExchangeIntra2(bool *moveEvaluation);

    void applyRelocateIntra(MoveInfo move_info);
    void applyExchangeIntra(MoveInfo move_info);

    vector<int> newPathRelocateIntra(MoveInfo move_info);
    vector<int> newPathExchangeIntra(MoveInfo move_info);

    //util
    void repopulateInter();
    void repopulateIntra();
    void repopulateBlock();

    int getRandomMove(vector<int> *list);
    NEIGHBORHOOD getRandomMove(vector<NEIGHBORHOOD> *list);

    //asserts
    bool assertRelocateInter(MoveInfo move_info);
    bool assertExchangeInter(MoveInfo move_info);
    bool assertTwoOptInter(MoveInfo move_info);
    bool assertExchangeVhcInter(MoveInfo move_info);
    bool assertRelocateIntra(MoveInfo move_info);
    bool assertExchangeIntra(MoveInfo move_info, int type);
    bool assertRelocateBlock(MoveInfo move_info);
    bool assertExchangeBlock(MoveInfo move_info);
};

#endif