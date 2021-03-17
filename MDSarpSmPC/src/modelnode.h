#ifndef MODELNODE_H_INCLUDED
#define MODELNODE_H_INCLUDED

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
#include "mipnode.h"

using namespace std;

void initArcs (instanceStat *inst, nodeArcsStruct *nas);
void feasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem);
void viewSol (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat);
void nodeMethod (nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);
// void mtznode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void output(instanceStat *inst, vector<nodeStat> &nodeVec, solStats *sStat, probStat* problem);

#endif