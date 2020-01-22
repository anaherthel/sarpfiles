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

	vector<double> bundleProfVec;
	
	vector<bool> bArcRow;
	pair<int, int> bFArc;
	vector< vector<bool> > bArcs;
	vector< pair<int,int> > bArcVec;
	vector< vector< pair<int,int> > > bArcPlus;
	vector< vector< pair<int,int> > > bArcMinus;

	vector<int> parcelBundle;
	vector< vector<int> > parcelBundleVec;

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

	for (int i = 0; i < inst.m; i ++){
		parcelBundleVec.push_back(parcelBundle);
	}

	makeParcelBundles(&inst, nodeVec, bundleVec, parcelBundleVec);

	// cout << "\nClusters:";
	// for (int i = 0; i < clusterVec.size(); i++){
	// 	cout << "\nCluster " << i << ": ";
	// 	for (int j = 0; j < clusterVec[i].size(); j++){
	// 		for (int k = 0; k < clusterVec[i][j].size(); k++){
	// 			cout << clusterVec[i][j][k] << " ";
	// 		}
	// 		cout << ", ";
	// 	}
	// 	cout << endl;
	// }

	bundleProfit(&inst, distMatrix, nodeVec, bundleVec, bundleProfVec);

	cout << "\nBundle Profit: [";
	for (int i = 0; i < bundleProfVec.size(); i++){
		cout << setw(3) << std:: right << bundleProfVec[i];
		if (i < bundleProfVec.size() - 1){
            cout << ", ";
        }
	}
	cout << endl;

	for (int i = 0; i < bundleVec.size(); i++){
		for (int j = 0; j < bundleVec.size(); j++){
			bArcRow.push_back(false);
		}
		bArcs.push_back(bArcRow);
	}
	
	for (int i = 0; i < bundleVec.size(); i++){
		auxVec.push_back(bFArc);
	}
	auxVec.clear();

	for (int i = 0; i < bundleVec.size(); i++){
		bArcMinus.push_back(auxVec);
		bArcPlus.push_back(auxVec);
	}

	feasibleBundleArcs(&inst, nodeVec, bundleVec, bFArc, bArcs, bArcPlus, bArcMinus);

	// cout << "\nPlus arcs from 17: "<< endl;
	// for (int i = 0; i < bArcPlus.size(); i++){
	// 	for (int j = 0; j < bArcPlus[i].size(); j++){
	// 		if (i == 17){
	// 			cout  << bArcPlus[i][j].first << " - " << bArcPlus[i][j].second << endl;
	// 		}			
	// 	} 
	// }


	// cout << "\nMinus arcs from 17: "<< endl;
	// for (int i = 0; i < bArcMinus.size(); i++){
	// 	for (int j = 0; j < bArcMinus[i].size(); j++){
	// 		if (i == 17){
	// 			cout  << bArcMinus[i][j].first << " - " << bArcMinus[i][j].second << endl;
	// 		}			
	// 	} 
	// }

	// getchar();

	cout<< "\nFeasible arcs between bundles:" << endl;
	for (int i = 0; i < bundleVec.size(); i++){
		if (i == 0){
			cout << setw(3) << " ";
		}
		cout << setw(3) << std::right << i;
	}
	cout << endl;
	for (int i = 0; i < bundleVec.size(); i++){
		cout << setw(3) << std::right << i;
		for (int j = 0; j < bundleVec.size(); j++){
			cout << setw(3) << std:: right << bArcs[i][j];
		}
		cout << endl;
	}

	for (int i = 0; i < bundleVec.size(); i++){
		for (int j = 0; j < bundleVec.size(); j++){
			if(bArcs[i][j]){
				bFArc.first = i;
				bFArc.second = j;
				bArcVec.push_back(bFArc);
			}
		}
	}
	cout << "\nBundle arc vector: " << endl;
	for (int i = 0; i < bArcVec.size(); i++){
		cout << bArcVec[i].first << " - " << bArcVec[i].second << " ";
	}
	getchar();

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

	mip(&inst, nodeVec, distMatrix, bundleVec, bundleTimes, clusterVec, bArcs, bArcPlus, bArcMinus, bArcVec, bundleProfVec, parcelBundleVec);

	for ( int i = 0; i < inst.V + inst.dummy; i++ ) {
		delete[] distMatrix[i];
	}

	delete[] distMatrix;


	return 0;
}