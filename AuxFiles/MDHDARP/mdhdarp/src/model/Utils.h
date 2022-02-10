#ifndef _UTILS_H_
#define _UTILS_H_

#include <cmath>
#include <limits>
#include <vector>

using namespace std;

const bool MMD_ACTIVATE = true;
const bool MSD_ACTIVATE = true;

const int INFEASIBLE = -1;
const double EPS     = 1e-12;

const int UNLIMITED_CAPACITY    = 100;             // Capacity for the extra vehicle
const double UNLIMITED_DURATION = 10000.0;         // Duration for the extra vehicle
const double UNLIMITED_TW[2]    = {0.0, 10000.0};  // Latest time for the extra vehicle

const int PERTURB_MAX_TRY = 100;

const int INF_COST   = 100000;  // Infinity cost for Constructive Procedure
const int PENAL_COST = 10000;   // Route penality cost

enum NEIGHBORHOOD {
    EXCHANGE_INTER = 0,
    RELOCATE_INTER = 1,
    TWO_OPT        = 2,
    EXCHANGE_VHC   = 3,
    EXCHANGE_BLOCK = 4,
    RELOCATE_BLOCK = 5,
    INSERTION      = 0,
    RELOCATE_INTRA = 1,
    EXCHANGE_INTRA = 2,
};

const vector<NEIGHBORHOOD> interList = {EXCHANGE_INTER, RELOCATE_INTER, TWO_OPT, EXCHANGE_VHC};  // List of inter-route moves
const vector<NEIGHBORHOOD> intraList = {INSERTION, RELOCATE_INTRA, EXCHANGE_INTRA};              // List of intra-route moves
const vector<NEIGHBORHOOD> blockList = {EXCHANGE_BLOCK, RELOCATE_BLOCK};                         // List of block-route moves

const int INTER_NEIGHBORHOOD_SIZE = interList.size();                                   // Number of inter-route moves
const int INTRA_NEIGHBORHOOD_SIZE = intraList.size();                                   // Number of intra-route moves
const int BLOCK_NEIGHBORHOOD_SIZE = blockList.size();                                   // Number of block-route moves
const int PAIR_ROUTES_MOVES_TOTAL = INTER_NEIGHBORHOOD_SIZE + BLOCK_NEIGHBORHOOD_SIZE;  // Number of moves between pair of routes

double epsilon(double a, double b);
bool improve(double value_1, double value_2);
bool assertEqual(double value_1, double value_2);

#endif
