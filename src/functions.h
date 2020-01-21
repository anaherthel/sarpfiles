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

using namespace std;

struct nodeStat{

	double xs;
	double ys;
	char label;
	int load;
	double e;
	double l;
	double xf;
	double yf;
    double delta;

};

struct instanceStat{

    int n;
    int m;
    int K;
    int T;
    int V;
    int dummy;
   	double gamma = 3;
	double mu = 2;
	double vmed = 30;
};

struct probStat{

	bool sim;
	bool seq;
	string scen;
};

// struct Arcset{
// 	vector< vector<bool> > arcs;
// 	vector< vector< pair<int,int> > > arcMinus;
// 	vector< vector< pair<int,int> > > arcPlus;
// 	vector< pair<int,int> > arcNN;
// 	vector< pair<int,int> > nodummyarcVec;
// }

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem);
double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J);
void feasibleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, pair<int, int> &fArc, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus, probStat* problem, vector< pair<int,int> > &arcNN);
string getInstanceType (char **argv);
void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, vector<int> &bundle, vector<double> &bundleTimes, vector< vector<int> > &clusters, vector< vector< vector<int> > > &clusterVec);
void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, vector<double> &bundleProfVec);
void feasibleBundleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, pair<int, int> &bFArc, vector< vector<bool> > &bArcs, vector< vector< pair<int,int> > > &bArcPlus, vector< vector< pair<int,int> > > &bArcMinus);
void makeParcelBundles(instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, vector< vector<int> > &parcelBundleVec);


#endif