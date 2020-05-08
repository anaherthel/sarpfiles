#ifndef READDATA_H_INCLUDED
#define READDATA_H_INCLUDED

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
// #include "functions.h"

using namespace std;

struct nodeStat{

	double xs;
	double ys;
	char label;
	double load;
	double e;
	double l;
	double xf;
	double yf;
    double delta;
    double profit;
};

struct instanceStat{

    int n;
    int m;
    int K;
    int T;
    int V;
    int dummy;
    double service;
   	double gamma = 4; //initial fare for parcels
	double mu = 2; //fare per km for parcels
	double vmed;
	// double vmed = 19.3;
	// double vmed = 9;
	double gamma2 = 7; //initial fare for passengers
	double mu2 = 4; //fare per km for passengers
	int nCluster;
	int endCluster;
	int startCluster;
};

struct probStat{

	bool sim;
	bool seq;
	string scen;
};

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem);
double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J);
double CalcMan (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);
double CalcLatLong (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int n, double *slatit, double* slongit, double *flatit, double* flongit);
double CalcDistGeo (double *slatit, double* slongit, double *flatit, double* flongit, int I, int J);
string getInstanceType (char **argv);

#endif