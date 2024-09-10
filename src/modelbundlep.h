#ifndef MODELBUNDLEP_H_INCLUDED
#define MODELBUNDLEP_H_INCLUDED

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
#include "modelbundle2.h"

using namespace std;

////generate bundles and organize them into clusters
void makeBundlesP (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, vector< vector<bParcelStruct> > &clsParcel, probStat* problem);
void makeBundlesPartial (instanceStat *inst, vector<nodeStat> &nodeVec, 
                        bundleStat *bStat, clSt *cStat,
                        vector< vector<bParcelStruct> > &clsParcel, 
                        probStat* problem);
void orderBundles(instanceStat *inst, bundleStat *bStat, clSt *cStat);                        
void makeParcelBundlesP(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem);
void bundleProfitP(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void makeStartTimesP (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem); //obtain start and end times of each bundle
void makeBundleReferenceP (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat); //assign first and last elements of each bundle   
void selectEligibleBundlesP(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat *problem, bundleStat *bStat);
void initArcsP (instanceStat *inst, bundleStat *bStat, clSt *cStat);
void feasibleBundleArcsP (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, vector< vector<int> > &fullsorted, probStat* problem);

void feasibleClusterArcsP (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt cStat, probStat* problem);

//void makeSmallerProblem2(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<bParcelStruct> > &clsParcel, probStat* problem, int Q);
//bool compareCosts2(const bParcelStruct &a, const bParcelStruct &b);
//void makeParcelSets2 (instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &parcelSets);
//void nodeSolution2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat, probStat* problem, bool fip);
//void bundleMethod2(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);
//void stillTimeBundle2(instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat);
//void setUpFipBundle(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem, fipBundleStats *fipStat);
//void clearArcs(bundleStat *bStat);
//void feasibleBundleArcs2next (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem, fipBundleStats *fipStat);
//void fipStructBundle(instanceStat *inst, solStats *sStat, bundleStat *bStat, fipBundleStats *fipStat, string model, bool bundleRun);
//void printBundleFile (instanceStat *inst, solStats *sStat, probStat* problem);
//void fillConversor(map<tuple<int, int, int>, int> &conversor, int n, int m, int v);
void printBundleInfoP(instanceStat *inst, bundleStat *bStat, clSt *cStat);

void bundleMethodPartial(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);

#endif