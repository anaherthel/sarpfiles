#ifndef SARPCONSTRUCTION_H_INCLUDED
#define SARPCONSTRUCTION_H_INCLUDED

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
#include "readdata.h"
#include "functions.h"
#include "hbundle.h"
#include "sarpRoute.h"
#include "sarpSolution.h"

class sarpConstruction {
protected:

    // vector<nodeStat> twSort;
    // vector<int> twPos;
    // vector< vector<int> > proxSort;
    // vector<double> distVec;

    // int pairings;

    vector<int> CLpass;
    vector<int> CLparc;
    
    vector<nodeStat> passNodes;


public:

    sarpSolution solution;

    sarpConstruction(instanceStat *inst, vector<nodeStat> &nodeVec);
    ~sarpConstruction(){};
    bool compare(int a, int b);
    int getRandRequestFromCL();
    
    void ConstrProc(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);

 

    // void orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);
    // void buildDistVec(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem);
    // void buildBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem, bundleStat* bStat);
    
    // void bTimeTest(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, bool &valid);

    // void orgBundles(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, bundleStat* bStat, clSt* cStat, probStat* problem);
    // void setBundleArcs(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, bundleStat* bStat, clSt* cStat, probStat *problem);

    // void setClusterArcs(instanceStat *inst, double **Mdist, vector<nodeStat> &nodeVec, bundleStat* bStat, clSt* cStat, probStat *problem);

    // void hbundleMethod(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist,  probStat *problem, solStats *sStat);

    // // access unassigned nodes
    // list<int>::iterator unass_begin() { return unassigned_.begin(); }
    // list<int>::const_iterator unass_begin() const{return unassigned_.begin();}
    // list<int>::iterator unass_end() { return unassigned_.end(); }
    // list<int>::const_iterator unass_end() const { return unassigned_.end(); }
};

// ostream & operator<<(ostream &os, const FLPInstance &instance);


#endif