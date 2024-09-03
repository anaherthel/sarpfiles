#ifndef MODELBUNDLE2_H_INCLUDED
#define MODELBUNDLE2_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>	
#include <iterator>
#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <iomanip>
#include <ctime>
#include <ilcplex/ilocplex.h>
#include <stdlib.h>
#include <iostream>
#include <locale.h>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include <map>
#include <set>
#include "SarpADS.h"
#include "mipbundle.h"
#include "bundleData.h"

using namespace std;

//generate bundles and organize them into clusters
void makeBundles2 (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, vector< vector<bParcelStruct> > &clsParcel, probStat* problem);
void makeBundlesPartial (instanceStat *inst, vector<nodeStat> &nodeVec, 
                        bundleStat *bStat, bundlePStat *bPStat, clSt *cStat, clPSt *cPStat,
                        vector< vector<bParcelStruct> > &clsParcel, 
                        probStat* problem, vector< vector<int> > &fullsorted);
//calculate bundles profits (for bundles of size 1 and 2, the profit is the one from the first node)
void bundleProfit2(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void initVecs2 (instanceStat *inst, vector< vector<bParcelStruct> > &clsParcel, bundleStat *bStat, probStat* problem);
void initArcs2 (instanceStat *inst, bundleStat *bStat, clSt *cStat);
void feasibleBundleArcs2 (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem);
void feasibleClusterArcs2 (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem);
void makeParcelBundles2(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem);
//obtain start and end times of each bundle 
void makeStartTimes2 (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem);
//obtain first and last elements of each bundle
void makeBundleReference2 (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void makeSmallerProblem2(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<bParcelStruct> > &clsParcel, probStat* problem, int Q);
bool compareCosts2(const bParcelStruct &a, const bParcelStruct &b);
void makeParcelSets2 (instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &parcelSets);
void nodeSolution2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat, probStat* problem, bool fip);
void bundleMethod2(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);
void bundleMethod2(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);
void stillTimeBundle2(instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat);
void setUpFipBundle(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem, fipBundleStats *fipStat);
void clearArcs(bundleStat *bStat);
void feasibleBundleArcs2next (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem, fipBundleStats *fipStat);
void fipStructBundle(instanceStat *inst, solStats *sStat, bundleStat *bStat, fipBundleStats *fipStat, string model, bool bundleRun);
void printBundleFile (instanceStat *inst, solStats *sStat, probStat* problem);
void fillConversor(map<tuple<int, int, int>, int> &conversor, int n, int m, int v);
void printBundleInfo(instanceStat *inst, bundleStat *bStat, clSt *cStat);
void selectEligibleBundles(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat *problem, bundleStat *bStat);

#endif