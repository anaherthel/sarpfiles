#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

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
#include "sarpILS.h"
#include "modelnode.h"
#include "modelbundle.h"
#include "SarpADS.h"

using namespace std;

void distScale(instanceStat *inst, int *instV, vector <vector <double> > &tempData, double *curAvg, int *scale);
double calcEucDist (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);
double calcEucDist2 (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);
double CalcMan (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);
double valRound(double value);
double CalcLatLong (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int n, double *slatit, double* slongit, double *flatit, double* flongit);
double CalcDistGeo (double *slatit, double* slongit, double *flatit, double* flongit, int I, int J);
string getInstanceType (char **argv);
string getInstName (char **argv);
void getInstParam (instanceStat *inst, vector<int> &instParam);
void solveselect(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat);
int testDurations(int a, int b, int c, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec);
void generatePermutations(int a, int b, instanceStat *inst, int c, double **mdist, vector<nodeStat> &nodeVec, vector<int>& stats);
void startPermutation(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec);



#endif