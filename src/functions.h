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
   	double gamma = 4; //initial fare for parcels
	double mu = 2; //fare per km for parcels
	double vmed = 19.3;
	double gamma2 = 7; //initial fare for passengers
	double mu2 = 4; //fare per km for passengers
	int nCluster;
};

struct probStat{

	bool sim;
	bool seq;
	string scen;
};

struct bundleStat{
	vector<int> bundle;
	vector< vector<int> > bundleVec;
	vector<double> bundleTimes;
	pair<int, int> bFArc;
	vector<double> bundleProfVec;
	vector<bool> bArcRow;
	vector< vector<bool> > bArcs;
	vector< pair<int,int> > bArcVec;
	vector< vector< pair<int,int> > > bArcPlus;
	vector< vector< pair<int,int> > > bArcMinus;
	vector<int> parcelBundle;
	vector< vector<int> > parcelBundleVec;
	vector<double> bundleStart;
    vector <int> lastElement;
    vector<int> firstElement;

};

struct bParcelStruct{
	double cost;
	int parcelreq;
};

// struct Arcset{
// 	vector< vector<bool> > arcs;
// 	vector< vector< pair<int,int> > > arcMinus;
// 	vector< vector< pair<int,int> > > arcPlus;
// 	vector< pair<int,int> > arcNN;
// 	vector< pair<int,int> > nodummyarcVec;
// }

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem, vector<double> &passProfit);
double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J);
double CalcMan (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);

double CalcLatLong (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int n, double *slatit, double* slongit, double *flatit, double* flongit);
double CalcDistGeo (double *slatit, double* slongit, double *flatit, double* flongit, int I, int J);

void feasibleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, pair<int, int> &fArc, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus, probStat* problem, vector< pair<int,int> > &arcNN);
string getInstanceType (char **argv);

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<int> &clusters, vector< vector<int> > &clusterVec, vector< vector<int> > &clsParcel);
void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<double> &passProfit);
void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void feasibleClusterArcs (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector< vector<int> > &clusterVec, pair<int, int> &cFArc, vector< vector<bool> > &cArcs, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus);
void makeParcelBundles(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat);
void makeStartTimes (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void makeBundleReference (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);

void makeSmallerProblem(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<int> > &clsParcel);

bool compareCosts(const bParcelStruct &a, const bParcelStruct &b);

#endif