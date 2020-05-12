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
#include "readdata.h"
#include "functions.h"
#include "mip.h"

using namespace std;

int main (int argc, char *argv[]) {

	double **distMatrix;
	double **auxdist;

	nodeStat node;
	instanceStat inst;
	probStat problem;
	bundleStat bStat;
	solStats sStat;

	nodeArcsStruct nas;

	vector<nodeStat> nodeVec;
	// vector<bool> arcRow;
	// vector< vector<bool> > arcs;
	// pair<int, int> fArc;
	// vector< pair<int,int> > arcVec;
	// vector< pair<int,int> > nodummyarcVec;
	// vector<double> nodeProfit;

	vector< pair<int,int> > auxVec;

	int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
	vector<int> vecOfInt;

	readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem);

	// for (int i = 0; i < inst.V; i++){
	// 	cout << "profit " << i << ": " << nodeVec[i].profit << endl;
	// }

	// for (int i = 0; i < inst.V; i++){
	// 	cout << "later " << i << ": " << nodeVec[i].l << endl;
	// }
	// getchar();
	
	// for (int i = 0; i < inst.n; i++){
	// 	cout << "delta " << i << ": " << nodeVec[i].delta << endl;
	// }

	// for (int i = 0; i < inst.n; i++){
	// 	clsParcel.push_back(auxbpsvec);
	// }

	cout << "\nDistance Matrix: " << endl;

	for (int i = 0; i < inst.V + inst.dummy; i++){
		for (int j = 0; j < inst.V + inst.dummy; j++){
			cout << setw(5) << distMatrix[i][j] << " ";
		}
		cout << endl;
	}
	getchar();

	initArcs(&inst, &nas);
	feasibleArcs (&inst, &nas, &problem);
	
	cout<< "\nFeasible arcs between nodes:" << endl;
    for (int i = 0; i < nas.arcs.size(); i++){
        if (i == 0){
            cout << setw(3) << " ";
        }
        cout << setw(3) << std::right << i << " ";
    }
    cout << endl;
    for(int i = 0; i < nas.arcs.size(); i++){
        cout << setw(3) << std::right << i;
        for(int j = 0; j < nas.arcs[i].size(); j++){
            cout << setw(3) <<  nas.arcs[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

	// getchar();

	// cout << "Arcs that leave a pickup: " << endl;

	// for (int i = 0; i < nas.arcPP.size(); i++){
	// 	cout<< nas.arcPP[i].first << "-" << nas.arcPP[i].second << "  |  ";
	// }
	// cout << endl;
	// for (int i = 0; i < clusterVec.size(); i++){
	// 	for (int j = 0; j < clusterVec.size(); j++){
	// 		cArcRow.push_back(false);
	// 	}
	// 	cArcs.push_back(cArcRow);
	// }
	
	// for (int i = 0; i < clusterVec.size(); i++){
	// 	auxVec.push_back(cFArc);
	// }
	// auxVec.clear();

	// for (int i = 0; i < clusterVec.size(); i++){
	// 	cArcMinus.push_back(auxVec);
	// 	cArcPlus.push_back(auxVec);
	// }

	// for (int i = 0; i < clusterVec.size(); i++){
	// 	cout << "\nCluster " << i << ": [";
	// 	for(int k = 0; k < clusterVec[i].size(); k++){
	// 		cout << "(" << clusterVec[i][k] << ") " << "[";
	// 		for (int j = 0; j < bStat.bundleVec[clusterVec[i][k]].size(); j++){
	// 			cout << setw(3) << std:: right << bStat.bundleVec[clusterVec[i][k]][j];
	//             if (j < bStat.bundleVec[clusterVec[i][k]].size() - 1){
	// 				cout << ",";
	// 			}
	// 			else{
	// 				cout << "] ";
	// 			}
	// 		}
			
	// 	}
	// 	cout << "]" << endl;
	// }



	// feasibleClusterArcs(&inst, nodeVec, &bStat, clusterVec, cFArc, cArcs, cArcPlus, cArcMinus, p, &problem);

	// cout<< "\nFeasible arcs between clusters:" << endl;
	// for (int i = 0; i < clusterVec.size(); i++){
	// 	if (i == 0){
	// 		cout << setw(3) << " ";
	// 	}
	// 	cout << setw(3) << std::right << i;
	// }
	// cout << endl;
	// for (int i = 0; i < clusterVec.size(); i++){
	// 	cout << setw(3) << std::right << i;
	// 	for (int j = 0; j < clusterVec.size(); j++){
	// 		cout << setw(3) << std:: right << cArcs[i][j];
	// 	}
	// 	cout << endl;
	// }

	// // getchar();

	// for (int i = 0; i < bStat.bundleVec.size(); i++){
	// 	for (int j = 0; j < bStat.bundleVec.size(); j++){
	// 		if(bStat.bArcs[i][j]){
	// 			bStat.bFArc.first = i;
	// 			bStat.bFArc.second = j;
	// 			bStat.bArcVec.push_back(bStat.bFArc);
	// 		}
	// 	}
	// }


	// for (int i = 0; i < clusterVec.size(); i++){
	// 	for (int j = 0; j < clusterVec.size(); j++){
	// 		if(cArcs[i][j]){
	// 			cFArc.first = i;
	// 			cFArc.second = j;
	// 			cArcVec.push_back(cFArc);
	// 		}
	// 	}
	// }
	// cout << "\nPlus arcs: " << endl;
	// for (int i = 0; i < cArcPlus.size(); i++){
	// 	cout << "Cluster " << i << ": ";
	// 	for (int j = 0; j < cArcPlus[i].size(); j++){
	// 		cout << cArcPlus[i][j].first << " - " << cArcPlus[i][j].second << ", ";
	// 	}
	// }

	// cout << "\nPlus arcs of bundles: " << endl;
	// for (int i = 0; i < bStat.bArcPlus.size(); i++){
	// 	cout << "Bundle " << i << ": ";
	// 	for (int j = 0; j < bStat.bArcPlus[i].size(); j++){
	// 		cout << bStat.bArcPlus[i][j].first << " - " << bStat.bArcPlus[i][j].second << ", ";
	// 	}
	// }
	// getchar();
	// cout << "\nMinus arcs: " << endl;
	// for (int i = 0; i < bStat.bArcMinus.size(); i++){
	// 	cout << "Bundle " << i << ": ";
	// 	for (int j = 0; j < bStat.bArcMinus[i].size(); j++){
	// 		cout << bStat.bArcMinus[i][j].first << " - " << bStat.bArcMinus[i][j].second << ", ";
	// 	}
	// }

	// getchar();
	// int currSP;
	// int setN = bStat.bundleVec.size() - inst.K - inst.endCluster;
	// for (int k = 0; k < inst.K; k++){
	// 	currSP = setN + k;
	// 	cout << "current starting point: " << currSP << endl;
	// 	for (int i = 0; i < bStat.bArcPlus[currSP].size(); i++){
	// 		cout << bStat.bArcPlus[currSP][i].first << " - " << bStat.bArcPlus[currSP][i].second;
	// 		// exp += x[bStat.bArcPlus[currSP][i].first][bStat.bArcPlus[currSP][i].second][k];
	// 	}
	// }
	// getchar();
	// cout << "beforemip" << endl;
	// getchar();
	// int currParcel;
	// for (int i = 0; i < inst.m; i++){
	// 		currParcel = inst.n + i;
	// 		cout << "Parcel: " << currParcel<< endl;
	// 		for (int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
	// 			for (int l = 0; l < bStat.bArcPlus[bStat.parcelBundleVec[i][j]].size(); l++){
	// 				cout << bStat.bArcPlus[bStat.parcelBundleVec[i][j]][l].first << " " << bStat.bArcPlus[bStat.parcelBundleVec[i][j]][l].second << "||";
	// 				// exp1 += x[bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first][bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second][k];
	// 			}					
	// 		}
			
	// 		for (int j = 0; j < bStat.parcelBundleVec[i + inst.m].size(); j++){
	// 			for (int l = 0; l < bStat.bArcMinus[bStat.parcelBundleVec[i + inst.m][j]].size(); l++){
	// 				cout << bStat.bArcMinus[bStat.parcelBundleVec[i + inst.m][j]][l].first << " " << bStat.bArcMinus[bStat.parcelBundleVec[i + inst.m][j]][l].second << "||";
	// 			}
	// 		}
	// 		cout << endl;
	// 		getchar();
	// }


	// mip(&inst, nodeVec, distMatrix, &bStat, clusterVec, cArcVec, cArcPlus, cArcMinus, &problem, &sStat);



	// while(!sStat.feasible && inst.K < inst.n){
	// 	auxdist = distMatrix;
	// 	increaseK (&inst, nodeVec, &distMatrix, &bStat, clusterVec, cArcVec, cArcPlus, cArcMinus, &problem, &sStat, auxdist);

	// 	// cout << "New Distance Matrix: " << endl;

	// 	// for (int i = 0; i < inst.V + inst.dummy; i++){
	// 	// 	for (int j = 0; j < inst.V + inst.dummy; j++){
	// 	// 		cout << setw(5) << distMatrix[i][j] << " ";
	// 	// 	}
	// 	// 	cout << endl;
	// 	// }
	// 	// getchar();

	// 	mip(&inst, nodeVec, distMatrix, &bStat, clusterVec, cArcVec, cArcPlus, cArcMinus, &problem, &sStat);
	// }
	mipnode(&inst, nodeVec, distMatrix, &problem, &nas, &sStat);
	// nodeSolution (&inst, distMatrix, &bStat, nodeVec, &sStat);

	// solStatIni(&sStat);

	// mipSolStats (&inst, distMatrix, &bStat, nodeVec, &sStat);

	// cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

	// printStats(&inst, &bStat, &sStat);

	for ( int i = 0; i < inst.V + inst.dummy; i++) {
		delete[] distMatrix[i];
	}
	delete[] distMatrix;

	return 0;
}