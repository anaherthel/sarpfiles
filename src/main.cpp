#include <ilcplex/ilocplex.h>     
#include <cstdio>
#include <iostream>
#include <iomanip> 
#include <vector>
#include <stack>
#include <algorithm>
#include <list>
#include <utility>
#include <cmath>
#include <ctime>
#include "functions.h"
#include "mip.h"

using namespace std;

int main (int argc, char *argv[]) {

	double **distMatrix;
	
	nodeStat node;
	instanceStat inst;
	std::vector<nodeStat> nodeVec;
	vector< vector<bool> > arcs;
	// int vertices;
	pair<int, int> fArc;
	vector< pair<int,int> > arcVec;
	readData(argc, argv, &node, &inst, nodeVec, &distMatrix);

	cout << "Distance Matrix: " << endl;

	for (int i = 0; i < inst.V + 1; i++){
		for (int j = 0; j < inst.V + 1; j++){
			cout << setw(5) << distMatrix[i][j] << " ";
		}
		cout << endl;
	}

	vector<bool> arcRow;
	
	for (int i = 0; i < inst.V + 1; i++){
		for (int j = 0; j < inst.V + 1; j++){
			arcRow.push_back(false);
		}
		arcs.push_back(arcRow);
	}
	// cout << "Feasible arcs: " << endl;

	feasibleArcs(&inst, nodeVec, arcs);

	for (int i = 0; i < nodeVec.size(); i++){
		for (int j = 0; j < nodeVec.size(); j++){
			if(arcs[i][j]){
				fArc.first = i;
				fArc.second = j;
				arcVec.push_back(fArc);
			}
		}
	}
	// cout << "\nArc Vector:" << endl;
	// for (int i; i < arcVec.size(); i++){
	// 	cout << "(" << arcVec[i].first << ", " << arcVec[i].second << ") ";
	// }
	// cout << endl;
	for (int i = 0; i < inst.V + 1; i++){
		if (i == 0){
			cout << setw(3) << " ";
		}
		cout << setw(3) << std::right << i;
	}
	cout << endl;
	for (int i = 0; i < inst.V + 1; i++){
		cout << setw(3) << std::right << i;
		for (int j = 0; j < inst.V + 1; j++){
			cout << setw(3) << std:: right << arcs[i][j];
		}
		cout << endl;
	}

	// cout << "Instance stats: " << inst.K  << " " << inst.delta << " " <<
	// inst.n << " " << inst.m << " " << inst.T << " " << inst.V << endl;

	mip(&inst, nodeVec, arcs, distMatrix, arcVec);

	for ( int i = 0; i < inst.V + 1; i++ ) {
		delete[] distMatrix[i];
	}

	delete[] distMatrix;

	return 0;
}