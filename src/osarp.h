#ifndef OSARP_H_INCLUDED
#define OSARP_H_INCLUDED

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
#include "sarpRoute.h"
#include "sarpSolution.h"
#include "Statistics.h"
#include "modelnode.h"
#include "SarpADS.h"

using namespace std;

class oSarp {
protected:

    // vector<nodeStat> twSort;
    // vector<int> twPos;
    // vector< vector<int> > proxSort;
    // vector<double> distVec;

    // int pairings;

    // int iterILS;
    // int maxIterILS;
    // // int minK;
    // int iterMS;
    // int maxiterMS;


    
public:

    void mioopt(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
    // sarpSolution *solution;
    // sarpSolution *bestSol;
    // sarpSolution *globalBestSol;
    // Runtime stats;

    // void ILS(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    // ~sarpILS(){};

    // void RVNDIntra(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    // // void RVNDAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    // void SwapAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    // void TwoOptAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    // void relocateBlockAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    // void ThreeOptAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    // void exchangeBlocksAll(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);
    
    // void RelocateAll(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);

    // void RVNDInter(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);
    
    // void relocate(instanceStat *inst, 
    //             vector<nodeStat> &nodeVec,
    //             double **Mdist, probStat* problem);

    // void Perturbation(instanceStat *inst, vector<nodeStat> &nodeVec,double **Mdist, probStat* problem);           

};

#endif