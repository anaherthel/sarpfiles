#ifndef MODELRFN_H_INCLUDED
#define MODELRFN_H_INCLUDED

#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <cmath>
#include <limits>
#include <iomanip>
#include <ilcplex/ilocplex.h>
#include "SarpADS.h"
#include "rfnfragment.h"

using namespace std;

// ----------------------------------------------------------
// Entry point — called from solveselect() with model="rfn"
// ----------------------------------------------------------
void rfnMethod(nodeStat *node, instanceStat *inst, double **mdist,
               vector<nodeStat> &nodeVec, probStat *problem, solStats *sStat);

// ----------------------------------------------------------
// Fragment / arc enumeration
// ----------------------------------------------------------
void buildRfnLocs(instanceStat *inst, vector<nodeStat> &nodeVec,
                  vector<RfnLoc> &puLocs, vector<RfnLoc> &dlLocs);

void enumerateFragments(instanceStat *inst, vector<nodeStat> &nodeVec,
                        double **mdist, probStat *problem,
                        FragmentNetwork &fn);

void enumerateArcs(instanceStat *inst, vector<nodeStat> &nodeVec,
                   double **mdist, FragmentNetwork &fn);

void buildIncidenceMaps(instanceStat *inst, FragmentNetwork &fn);

// ----------------------------------------------------------
// Domination
// ----------------------------------------------------------
void dominateFragments(instanceStat *inst, FragmentNetwork &fn);

// ----------------------------------------------------------
// MIP model
// ----------------------------------------------------------
void mipRfn(instanceStat *inst, vector<nodeStat> &nodeVec,
            double **mdist, probStat *problem,
            FragmentNetwork &fn, solStats *sStat);

// ----------------------------------------------------------
// Valid inequalities (added statically at root)
// ----------------------------------------------------------
void addRfnFAInequalities(IloModel &model, IloEnv &env,
                          IloBoolVarArray &xf, IloBoolVarArray &ya,
                          FragmentNetwork &fn);

void addRfnAFInequalities(IloModel &model, IloEnv &env,
                          IloBoolVarArray &xf, IloBoolVarArray &ya,
                          FragmentNetwork &fn);

// ----------------------------------------------------------
// Output
// ----------------------------------------------------------
void printRfnSolution(instanceStat *inst, double **mdist,
                      vector<nodeStat> &nodeVec, solStats *sStat);

#endif
