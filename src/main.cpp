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

//x(hours) = Euc_dist/vmed
int main (int argc, char *argv[]) {

	double **distMatrix;
	
	nodeStat node;
	instanceStat inst;
	probStat problem;
	bundleStat bStat;
	solStats sStat;

	vector<nodeStat> nodeVec;
	vector<bool> arcRow;
	vector< vector<bool> > arcs;
	// int vertices;
	pair<int, int> fArc;
	vector< pair<int,int> > arcVec;
	vector< pair<int,int> > nodummyarcVec;
	vector<double> nodeProfit;

	vector< pair<int,int> > auxVec;
	// vector< vector<int> > parcelSets;

	int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
	int Q = 5;
	vector< vector<int> > clsParcel;
	vector<int> vecOfInt;

	// vector<int> solInNode;

	// vector< vector< pair<int, int> > > solvec; 

	readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem, nodeProfit);
	
	for (int i = 0; i < inst.n; i++){
		cout << "delta " << i << ": " << nodeVec[i].delta << endl;
	}
	
	
	for (int i = 0; i < inst.n; i++){
		clsParcel.push_back(vecOfInt);
	}

	// if (p > -1){
	// 	makeSmallerProblem(&inst, nodeVec, distMatrix, p, clsParcel);
	// }
	// else{
	// 	for (int i = 0; i < clsParcel.size(); i++){
	// 		for(int j = inst.n; j < inst.n + inst.m; j++){
	// 			clsParcel[i].push_back(j);
	// 		}
	// 	}
	// }

	makeSmallerProblem(&inst, nodeVec, distMatrix, p, clsParcel, &problem, Q);

	// cout << "\nClosest parcels: " << endl;
	// for (int i = 0; i < clsParcel.size(); i++){
	// 	cout << i << ": ";
	// 	for (int j = 0; j < clsParcel[i].size(); j++){
	// 		cout << clsParcel[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// getchar();

	vector< vector< pair<int,int> > > arcPlus;
	vector< vector< pair<int,int> > > arcMinus;
	vector< pair<int,int> > arcNN;
	
	vector< vector<int> >clusterVec;
	vector<int> clusters;

	vector<bool> cArcRow;
	pair<int, int> cFArc;
	vector< vector<bool> > cArcs;
	vector< pair<int,int> > cArcVec;
	vector< vector< pair<int,int> > > cArcPlus;
	vector< vector< pair<int,int> > > cArcMinus;

	cout << "Distance Matrix: " << endl;

	for (int i = 0; i < inst.V + inst.dummy; i++){
		for (int j = 0; j < inst.V + inst.dummy; j++){
			cout << setw(5) << distMatrix[i][j] << " ";
		}
		cout << endl;
	}
	// getchar();

	makeBundles(&inst, nodeVec, &bStat, clusters, clusterVec, clsParcel, &problem);

	cout << "\nBundle Vector: [";
	for (int i = 0; i < bStat.bundleVec.size(); i++){
		cout << "[";
		for (int j = 0; j < bStat.bundleVec[i].size(); j++){
			cout << setw(3) << std:: right << bStat.bundleVec[i][j];
			if (j < bStat.bundleVec[i].size() - 1){
                cout << ", ";
            }
		}
		cout << "],";
	}
	cout << "]" << endl;

	// getchar();

	if (problem.scen == "1A"){
		for (int i = 0; i < inst.m; i++){
			bStat.parcelBundleVec.push_back(bStat.parcelBundle);
		}
	}

	else if (problem.scen == "2A"){
		for (int i = 0; i < 2*inst.m; i++){
			bStat.parcelBundleVec.push_back(bStat.parcelBundle);
		}
	}

	for (int i = 0; i < clusterVec.size(); i++){
		cout << "\nCluster " << i << ": [";
		for(int k = 0; k < clusterVec[i].size(); k++){
			cout << "(" << clusterVec[i][k] << ") " << "[";
			for (int j = 0; j < bStat.bundleVec[clusterVec[i][k]].size(); j++){
				cout << setw(3) << std:: right << bStat.bundleVec[clusterVec[i][k]][j];
	            if (j < bStat.bundleVec[clusterVec[i][k]].size() - 1){
					cout << ",";
				}
				else{
					cout << "] ";
				}
			}
			
		}
		cout << "]" << endl;
	}

	// getchar();

	makeParcelBundles(&inst, nodeVec, &bStat, &problem);

	// cout << "\nParcel Bundles: " << endl;

	// for (int i = 0 ; i < bStat.parcelBundleVec.size(); i++){
	// 	cout << i + inst.n << ": ";
	// 	for(int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
	// 		cout << bStat.parcelBundleVec[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// cout << endl;
	// getchar();

	bundleProfit(&inst, distMatrix, nodeVec, &bStat, nodeProfit);

	// cout << "\nBundle Profit: [";
	// for (int i = 0; i < bStat.bundleProfVec.size(); i++){
	// 	cout << setw(3) << std:: right << bStat.bundleProfVec[i];
	// 	if (i < bStat.bundleProfVec.size() - 1){
 //            cout << ", ";
 //        }
	// }
	// cout << endl;

	// getchar();

	for (int i = 0; i < bStat.bundleVec.size(); i++){
		for (int j = 0; j < bStat.bundleVec.size(); j++){
			bStat.bArcRow.push_back(false);
		}
		bStat.bArcs.push_back(bStat.bArcRow);
	}
	
	for (int i = 0; i < bStat.bundleVec.size(); i++){
		auxVec.push_back(bStat.bFArc);
	}
	auxVec.clear();

	for (int i = 0; i < bStat.bundleVec.size(); i++){
		bStat.bArcMinus.push_back(auxVec);
		bStat.bArcPlus.push_back(auxVec);
	}

	makeStartTimes(&inst, distMatrix, nodeVec, &bStat);

	makeBundleReference(&inst, distMatrix, nodeVec, &bStat);

	
	// cout << "\nfirst element Vector: [";
	// for (int i = 0; i < bStat.firstElement.size(); i++){
	// 	cout << bStat.firstElement[i] << " ";
	// }
	// cout << endl;
	// getchar();

	// cout << "\nBundle Times: ";
	// for (int i = 0; i < bStat.bundleStart.size(); i++){
	// 	cout <<	i  << ": " << bStat.bundleStart[i] << ", ";
	// }
	// cout << endl;

	// getchar();
	

	// cout << "\nBundle Service Times: ";
	// for (int i = 0; i < bStat.bundleServVec.size(); i++){
	// 	cout <<	i  << ": " <<	bStat.bundleServVec[i] << ", ";
	// }
	// cout << endl;

	// getchar();


	feasibleBundleArcs(&inst, distMatrix, nodeVec, &bStat, p, &problem);

	// cout<< "\nFeasible arcs between bundles:" << endl;
	// for (int i = 0; i < bStat.bundleVec.size(); i++){
	// 	if (i == 0){
	// 		cout << setw(3) << " ";
	// 	}
	// 	cout << setw(3) << std::right << i;
	// }
	// cout << endl;
	// for (int i = 0; i < bStat.bundleVec.size(); i++){
	// 	cout << setw(3) << std::right << i;
	// 	for (int j = 0; j < bStat.bundleVec.size(); j++){
	// 		cout << setw(3) << std:: right << bStat.bArcs[i][j];
	// 	}
	// 	cout << endl;
	// }
	// getchar();

	for (int i = 0; i < clusterVec.size(); i++){
		for (int j = 0; j < clusterVec.size(); j++){
			cArcRow.push_back(false);
		}
		cArcs.push_back(cArcRow);
	}
	
	for (int i = 0; i < clusterVec.size(); i++){
		auxVec.push_back(cFArc);
	}
	auxVec.clear();

	for (int i = 0; i < clusterVec.size(); i++){
		cArcMinus.push_back(auxVec);
		cArcPlus.push_back(auxVec);
	}

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

	// getchar();

	feasibleClusterArcs(&inst, nodeVec, &bStat, clusterVec, cFArc, cArcs, cArcPlus, cArcMinus, p, &problem);

	cout<< "\nFeasible arcs between clusters:" << endl;
	for (int i = 0; i < clusterVec.size(); i++){
		if (i == 0){
			cout << setw(3) << " ";
		}
		cout << setw(3) << std::right << i;
	}
	cout << endl;
	for (int i = 0; i < clusterVec.size(); i++){
		cout << setw(3) << std::right << i;
		for (int j = 0; j < clusterVec.size(); j++){
			cout << setw(3) << std:: right << cArcs[i][j];
		}
		cout << endl;
	}

	// getchar();

	for (int i = 0; i < bStat.bundleVec.size(); i++){
		for (int j = 0; j < bStat.bundleVec.size(); j++){
			if(bStat.bArcs[i][j]){
				bStat.bFArc.first = i;
				bStat.bFArc.second = j;
				bStat.bArcVec.push_back(bStat.bFArc);
			}
		}
	}


	for (int i = 0; i < clusterVec.size(); i++){
		for (int j = 0; j < clusterVec.size(); j++){
			if(cArcs[i][j]){
				cFArc.first = i;
				cFArc.second = j;
				cArcVec.push_back(cFArc);
			}
		}
	}
	// cout << "\nPlus arcs: " << endl;
	// for (int i = 0; i < cArcPlus.size(); i++){
	// 	cout << "Cluster " << i << ": ";
	// 	for (int j = 0; j < cArcPlus[i].size(); j++){
	// 		cout << cArcPlus[i][j].first << " - " << cArcPlus[i][j].second << ", ";
	// 	}
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

	mip(&inst, nodeVec, distMatrix, &bStat, clusterVec, cArcVec, cArcPlus, cArcMinus, &problem, &sStat);

	nodeSolution (&inst, distMatrix, &bStat, nodeVec, &sStat);

	solStatIni(&sStat);

	mipSolStats (&inst, distMatrix, &bStat, nodeVec, &sStat);

	// cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

	printStats(&inst, &bStat, &sStat);

	for ( int i = 0; i < inst.V + inst.dummy; i++) {
		delete[] distMatrix[i];
	}
	delete[] distMatrix;

	return 0;
}