#ifndef SARPADS_H_INCLUDED
#define SARPADS_H_INCLUDED

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
    double trip;
    int index;
};

struct instanceStat{

    int n;
    int m;
    int K;
    double T = 24;
    int V;
    double maxTime = 8;
    int dummy;
    double service;

	double minpas = 3.24; //initial fare for passengers
	double paskm = 1.03; //fare per km for passengers

   	double minpar = 2.74; //initial fare for parcels
	double parkm = 0.83; //fare per km for parcels

	double costkm = 0.46;
	double vmed = 41;
	bool preInst;
	string InstName;
	string instType;

	vector<int> instParam;
	double totalCustomProfit;

	double realAvg = 16;
	double realStddv = 1.03;

	bool min;

};

struct probStat{

	bool sim;
	bool seq;
	string scen;
	string model;

	bool p1, p2, dParcel;//1 is multi, 0 is single; p1 refers to customer, p2 to parcel, dParcel 1 allows for direct parcel delivery
	int seed;
};

struct solStats{
	double solprofit;
	double tParcel;
	double tPass;
	double tBoth;
	double tNone;

	double tStillP;
	double tStillG;
	double tStill;

	double dParcel;
	double dPass;
	double dBoth;
	double dNone;

	double time;

	int servedParcels;
	
    vector< vector<int> > solOrder;
	vector< vector<int> > solInNode;
	vector< vector< pair<int, int> > > solvec;

	vector<double> solBegin;
	vector<double> solLoad;

	bool feasible;

	double pProfit;
	double costs;

};

struct nodeArcsStruct{//for model node
	vector< vector<bool> > arcs;
	vector< pair<int,int> > allArcs;
	pair<int, int> fArc;
	vector< vector< pair<int,int> > > arcPlus;
	vector< vector< pair<int,int> > > arcMinus;
	vector< vector< vector< pair<int,int> > > > vArcPlus;
	vector< vector< vector< pair<int,int> > > > vArcMinus;
	vector< pair<int,int> > arcNN;
	vector< pair<int,int> > arcNplus;
	vector< pair<int,int> > arcPP;
	vector< pair<int,int> > arcnf;
	vector< vector< vector<int> > > arcV;
	vector< pair<int,int> > arcPass;
	vector< pair<int,int> > arcParc;
};

void solStatIni(solStats *sStat);
void mipSolStats (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat);
void printStats(instanceStat *inst, solStats *sStat);

#endif