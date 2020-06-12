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

using namespace std;

struct nodeArcsStruct{
	vector< vector<bool> > arcs;
	vector< pair<int,int> > allArcs;
	pair<int, int> fArc;
	vector< vector< pair<int,int> > > arcPlus;
	vector< vector< pair<int,int> > > arcMinus;
	vector< pair<int,int> > arcNN;
	vector< pair<int,int> > arcNplus;
	vector< pair<int,int> > arcPP;
	vector< pair<int,int> > arcnf;
};

void initArcs (instanceStat *inst, nodeArcsStruct *nas);
void feasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem);
void viewSol (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat);
void solStatCalc();
void mipnode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void nodeMethod (nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);

#endif