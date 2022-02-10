#ifndef BUNDLEDATA_H_INCLUDED
#define BUNDLEDATA_H_INCLUDED

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
	vector<double> bundleProfVec;
	vector<double> bundleServVec;
	vector< vector<bool> > bArcs; //values of all arcs
	vector< pair<int,int> > bArcVec; //the indexes of all arcs

	vector< vector< pair<int,int> > > bArcPlus; //vector of size equal to requests, containing the arcs that start with i.
	vector< vector< pair<int,int> > > bArcMinus;//vector of size equal to requests, containing the arcs that end with i.
	vector< vector< vector< pair<int,int> > > > vArcPlus; //arcs leaving i that can be travelled by k. (Vector of pairs contais the  arcs; then vec of size K, then vec of size of bundle)
	vector< vector< vector< pair<int,int> > > > vArcMinus;
	vector< vector< vector<int> > > arcV; //set of vehicles that can travel arc a
	vector< vector<int> > parcelBundleVec;
	vector< vector<int> > passBundleVec;
	vector<double> bundleStart;
	vector<double> bundleEnd;
    vector <int> lastElement;
    vector <int> firstElement;
    vector<int> clofbundle;
	vector<int> vecofPass;
	vector< vector<int> > vecofPU;
	vector< vector<int> > vecofDL;
};

struct clSt{
	int nCluster;
	vector<int> clusters;
	vector< vector<int> > clusterVec;
	vector< vector< pair<int,int> > > cArcPlus;
	vector< vector< pair<int,int> > > cArcMinus;
	vector< vector<bool> > cArcs;
	vector< vector< vector< pair<int,int> > > > vArcPlus;
	vector< vector< vector< pair<int,int> > > > vArcMinus;
	vector< vector< vector<int> > > arcV;
	vector<int> clindex;

    vector< pair<int,int> > cArcVec;
};

struct bParcelStruct{
	double cost;
	int parcelreq;
	bool poslabel; //1 if parcel is after passenger request in the bundle, 0 if it is before.
};


#endif