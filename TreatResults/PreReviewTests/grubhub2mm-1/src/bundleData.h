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
	vector< vector<bool> > bArcs;
	vector< pair<int,int> > bArcVec;

	vector< vector< pair<int,int> > > bArcPlus;
	vector< vector< pair<int,int> > > bArcMinus;
	vector< vector< vector< pair<int,int> > > > vArcPlus;
	vector< vector< vector< pair<int,int> > > > vArcMinus;
	vector< vector< vector<int> > > arcV;
	vector< vector<int> > parcelBundleVec;
	vector< vector<int> > passBundleVec;
	vector<double> bundleStart;
	vector<double> bundleEnd;
    vector <int> lastElement;
    vector <int> firstElement;
    vector<int> clofbundle;
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

    vector< pair<int,int> > cArcVec;
};

struct bParcelStruct{
	double cost;
	int parcelreq;
	bool poslabel; //1 if parcel is after passenger request in the bundle, 0 if it is before.
};


#endif