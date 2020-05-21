#ifndef MODELBUNDLE_H_INCLUDED
#define MODELBUNDLE_H_INCLUDED

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

struct bundleStat{
	vector<int> bundle;
	vector< vector<int> > bundleVec;
	vector<double> bundleTimes;
	pair<int, int> bFArc;
	vector<double> bundleProfVec;
	vector<double> bundleServVec;
	vector<bool> bArcRow;
	vector< vector<bool> > bArcs;
	vector< pair<int,int> > bArcVec;
	vector< vector< pair<int,int> > > bArcPlus;
	vector< vector< pair<int,int> > > bArcMinus;
	vector<int> parcelBundle;
	vector< vector<int> > parcelBundleVec;
	vector< vector<int> > passBundleVec;
	vector<double> bundleStart;
    vector <int> lastElement;
    vector<int> firstElement;
    vector<int> label; //0- purely passenger; 1- parcel pickup req; 2- parcel delivery req; 3- starting/dummy; 4 - delivery/dummy; 5 - start-pickup
    vector<int> label2;//1 if parcel is after passenger request in the bundle, 0 if it is before, -1 non parcel bundles.
    vector<int> mainNode;
    vector<int> activePU;
    vector<int> activeDL;
};

struct bParcelStruct{
	double cost;
	int parcelreq;
	bool poslabel; //1 if parcel is after passenger request in the bundle, 0 if it is before.
};

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<int> &clusters, vector< vector<int> > &clusterVec, vector< vector<bParcelStruct> > &clsParcel, probStat* problem);
void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<double> &passProfit);
void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, int p, probStat* problem);
void feasibleClusterArcs (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector< vector<int> > &clusterVec, pair<int, int> &cFArc, vector< vector<bool> > &cArcs, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, int p, probStat* problem);
void makeParcelBundles(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem);
void makeStartTimes (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem);
void makeBundleReference (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat);
void makeSmallerProblem(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, int p, vector< vector<bParcelStruct> > &clsParcel, probStat* problem, int Q);
bool compareCosts(const bParcelStruct &a, const bParcelStruct &b);
void makeParcelSets (instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &parcelSets);
void nodeSolution (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat);

#endif