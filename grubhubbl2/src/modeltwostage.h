#ifndef MODELTWOSTAGE_H_INCLUDED
#define MODELTWOSTAGE_H_INCLUDED

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


using namespace std;

struct tsArcsStruct{
	pair<int, int> tsfArc;
	vector< pair<int,int> > tsallArcs;
	vector< vector<bool> > tsarcs;
	vector< vector< pair<int,int> > > tsarcPlus;
	vector< vector< pair<int,int> > > tsarcMinus;
	vector< pair<int,int> > tsarcNN;
	vector< pair<int,int> > tsarcNplus;
	vector< pair<int,int> > tsarcnf;
};

void tsInitArcs (instanceStat *inst, tsArcsStruct *tsas);
void prepVecs(instanceStat *inst, tsArcsStruct *tsas, vector<nodeStat> &p1vec, vector<nodeStat> &nodeVec, probStat* problem);
void mipts(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat);
void twoStageMethod(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);

#endif