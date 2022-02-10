#ifndef MIPNODE_H_INCLUDED
#define MIPNODE_H_INCLUDED

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
#include "modelnode.h"


void mipnode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void mipnodePass(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void mipnodeParc(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void printResults(instanceStat *inst, double **mdist, solStats *sStat, vector<nodeStat> &nodeVec);


#endif