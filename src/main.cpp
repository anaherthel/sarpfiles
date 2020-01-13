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

//x(hours) = Euc_dist/vmed
int main (int argc, char *argv[]) {

	double **distMatrix;
	
	nodeStat node;
	instanceStat inst;
	probStat problem;
	std::vector<nodeStat> nodeVec;
	vector< vector<bool> > arcs;
	// int vertices;
	pair<int, int> fArc;
	vector< pair<int,int> > arcVec;
	vector< pair<int,int> > nodummyarcVec;

	vector< pair<int,int> > auxVec;

	readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem);
	
	vector< vector< pair<int,int> > > arcPlus;
	vector< vector< pair<int,int> > > arcMinus;
	vector< pair<int,int> > arcNN;
	
	vector< vector<int> > bundleVec;
	vector<int> bundle;

	vector<double> bundleTimes;
	
	vector< vector< vector<int> > >clusterVec;
	vector< vector<int> > clusters;

	cout << "Distance Matrix: " << endl;

	for (int i = 0; i < inst.V + inst.dummy; i++){
		for (int j = 0; j < inst.V + inst.dummy; j++){
			cout << setw(5) << distMatrix[i][j] << " ";
		}
		cout << endl;
	}
	// getchar();
	vector<bool> arcRow;
	
	makeBundles(&inst, nodeVec, bundleVec, bundle, bundleTimes, clusters, clusterVec);

	cout << "\nBundle Vector: [";
	for (int i = 0; i < bundleVec.size(); i++){
		cout << "[";
		for (int j = 0; j < bundleVec[i].size(); j++){
			cout << setw(3) << std:: right << bundleVec[i][j];
			if (j < bundleVec[i].size() - 1){
                cout << ", ";
            }
		}
		cout << "],";
	}
	cout << "]" << endl;

	cout << "\nClusters:";
	for (int i = 0; i < clusterVec.size(); i++){
		cout << "\nCluster " << i << ": ";
		for (int j = 0; j < clusterVec[i].size(); j++){
			for (int k = 0; k < clusterVec[i][j].size(); k++){
				cout << clusterVec[i][j][k] << " ";
			}
			cout << ", ";
		}
		cout << endl;
	}

	// for (int i = 0; i < nodeVec.size(); i++){
	// 	cout << "\nNode: " << i << " - service time: " << nodeVec[i].delta;
	// }
	// cout << endl;

	// for (int i = 0; i < bundleTimes.size(); i++){
	// 	cout << "\nBundle:";
	// 	for (int j = 0; j < bundleVec[i].size(); j++){
	// 		cout << bundleVec[i][j] << " ";
	// 	}
	// 	cout << endl;
	// 	cout << "\nTime of service for the bundle: " << bundleTimes[i] << endl;
	// }

	// for (int i = 0; i < inst.V + inst.dummy; i++){
	// 	for (int j = 0; j < inst.V + inst.dummy; j++){
	// 		arcRow.push_back(false);
	// 	}
	// 	arcs.push_back(arcRow);
	// }
	// cout << "Feasible arcs: " << endl;
	
	// for (int i = 0; i < inst.V + 1; i++){
	// 	auxVec.push_back(fArc);
	// }

	// for (int i = 0; i < inst.V + inst.dummy; i++){
	// 	arcMinus.push_back(auxVec);
	// 	arcPlus.push_back(auxVec);
	// }

	// for (int i = 0; i < inst.n; i++){
	// 	arcNN.push_back(auxVec);
	// }

	// feasibleArcs(&inst, nodeVec, arcs, fArc, arcPlus, arcMinus, &problem, arcNN);

	
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	for (int j = 0; j < nodeVec.size(); j++){
	// 		if(arcs[i][j]){
	// 			fArc.first = i;
	// 			fArc.second = j;
	// 			arcVec.push_back(fArc);
	// 			if (j < inst.V){
	// 				nodummyarcVec.push_back(fArc);
	// 			}
	// 		}
	// 	}
	// }


	// cout << "\nArc Vector:" << endl;
	// for (int i = 0; i < arcVec.size(); i++){
	// 	cout << "(" << arcVec[i].first << ", " << arcVec[i].second << ") ";
	// }
	// cout << endl;

	// getchar();


	// for (int i = 0; i < inst.V + inst.dummy; i++){
	// 	if (i == 0){
	// 		cout << setw(3) << " ";
	// 	}
	// 	cout << setw(3) << std::right << i;
	// }
	// cout << endl;
	// for (int i = 0; i < inst.V + inst.dummy; i++){
	// 	cout << setw(3) << std::right << i;
	// 	for (int j = 0; j < inst.V + inst.dummy; j++){
	// 		cout << setw(3) << std:: right << arcs[i][j];
	// 	}
	// 	cout << endl;
	// }

	// mip(&inst, nodeVec, arcs, distMatrix, arcVec, nodummyarcVec, arcPlus, arcMinus, arcNN);

	for ( int i = 0; i < inst.V + inst.dummy; i++ ) {
		delete[] distMatrix[i];
	}

	delete[] distMatrix;


	return 0;
}