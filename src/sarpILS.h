#ifndef SARPILS_H_INCLUDED
#define SARPILS_H_INCLUDED

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
#include <time.h>
#include <unistd.h>
#include "readdata.h"
#include "functions.h"
#include "hbundle.h"
#include "sarpRoute.h"
#include "sarpSolution.h"
#include "sarpConstruction.h"
#include "Statistics.h"

using namespace std;

class sarpILS {
protected:

    // vector<nodeStat> twSort;
    // vector<int> twPos;
    // vector< vector<int> > proxSort;
    // vector<double> distVec;

    // int pairings;

    int iterILS;
    int maxIterILS;
    
public:

    sarpSolution solution;
    sarpSolution bestSol;
    Runtime stats;

    void ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    void RVNDIntra(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    void SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    void RelocateAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    void RVNDInter(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    void relocate(instanceStat *inst, 
                vector<nodeStat> &nodeVec,
                double **Mdist, probStat* problem);
    
    void Perturbation(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);           
    // ~ILS();
    // void orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);
    // void buildDistVec(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);
    // void buildBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem, bundleStat* bStat);
    
    // void bTimeTest(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, bool &valid);

    // void orgBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, clSt* cStat, probStat* problem);
    // void setBundleArcs(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, bundleStat* bStat, clSt* cStat, probStat *problem);

    // void setClusterArcs(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, bundleStat* bStat, clSt* cStat, probStat *problem);

    // void hbundleMethod(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist,  probStat *problem, solStats *sStat);
};

// ostream & operator<<(ostream &os, const FLPInstance &instance);


#endif