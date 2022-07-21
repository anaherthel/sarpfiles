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

	int sigma;

	double minpas = 3.24; //initial fare for passengers; alfa
	double paskm = 1.03; //fare per km for passengers; gamma1

   	double minpar = 2.74; //initial fare for parcels; beta
	double parkm = 0.83; //fare per km for parcels; gamma2

	double costkm = 0.46;//cost per travelled km; gamma3
	double vmed = 41;
	bool preInst;
	string InstName;
	string instType;

	double discpas = 3.24; //discount value for passenger detour (fip/osarp); gamma4 (same value as minpas'alpha')

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
	//p1 = 1; multi customer
	//p2 = 1; multi parcel
	//dParcel = 1; allow direct delivery
	
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
	

    vector< vector<int> > solOrder; //for each k in K, the whole solution
	vector< vector<int> > solInNode;
	vector< vector< pair<int, int> > > solvec;

	vector<double> solBegin;
	vector<double> solLoad;

	bool feasible;

	double pProfit;
	double costs;
};

struct fipStats{

	vector< vector<int> > solPass; //for each k in K, the passenger locations
	vector< vector< pair<int,int> > > solPassOrigins; //for each k in K, the passenger PU locations and its position in the solution.

	bool fipstage;
	vector< vector< pair<int, int> > > solvec; //solution vector with parcel additions
	vector<double> solBegin; //stores values of b[i][k]
	vector<double> solBeginParcel;//stores values of s[j]


	vector< vector<int> > fullSol; //merged solution with passengers and parcels.
	vector< vector<double> > fullBegin; // beginning of service for all passengers and served parcels.
	double solprofit;
};

struct nodeArcsStruct{//for model node
	vector< vector<bool> > arcs; //all arcs, either true or false. For each request to each request.
	vector< pair<int,int> > allArcs; // the actual pairs of requests that form an arc
	pair<int, int> fArc; 
	vector< vector< pair<int,int> > > arcPlus; //arcs leaving a request (for req, vector of pairs, pair)
	vector< vector< pair<int,int> > > arcMinus; //arcs arriving at a request (for req, vector of pairs, pair)
	//for each request, for every vehicle, vector of arcs that are traverssed by that vehicle and contain that request
	vector< vector< vector< pair<int,int> > > > vArcPlus;
	vector< vector< vector< pair<int,int> > > > vArcMinus;
	vector< pair<int,int> > arcNN;//arcs between 2 passengers
	vector< pair<int,int> > arcNplus; //arcs leaving a passenger
	vector< pair<int,int> > arcPN; //arcs from parcel to passenger
	vector< pair<int,int> > arcnf; //arcs that do not contain ending depot
	// for each pair of requests, records the vehicles that can serve that arc
	vector< vector< vector<int> > > arcV; 
};

// struct fipSol{

// 	vector< vector<int> > setH;

// };

void solStatIni(solStats *sStat);
void mipSolStats (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat);
void printStats(instanceStat *inst, solStats *sStat);
void printStructures(nodeArcsStruct *nas);
void fipStruct(instanceStat *inst, solStats *sStat, fipStats *fipStat);
void mergeFipSol(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat, fipStats *fipStat);
// void fipStats(solStats *sStat, solStats *sStat, fipStats *fipStat);

#endif