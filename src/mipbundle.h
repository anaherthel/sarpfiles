#ifndef MIPBUNDLE_H_INCLUDED
#define MIPBUNDLE_H_INCLUDED

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
#include "modelbundle.h"
#include "modelbundle2.h"
#include "bundleData.h"
#include "hbundle.h"
#include "SarpADS.h"

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat);
void mipSolStats2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat);
void miphbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat);
void mipbundle2(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat);
void mipSolStatsPlus (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat);
void fipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat, fipBundleStats *fipStat);



#endif