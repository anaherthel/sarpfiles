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
#include <sys/time.h>
#include "readdata.h"
#include "functions.h"
#include "modelnode.h"
#include "modelbundle.h"
#include "modeltwostage.h"
#include "mipnode.h"
#include "hbundle.h"
#include "sarpILS.h"
#include "sarpConstruction.h"
#include "cpuTime.h"
#include "Statistics.h"
#include "SarpADS.h"
#include "osarp.h"

using namespace std;

int main (int argc, char *argv[]) {
	double **distMatrix;
	
	int trialK = 0;
	double trialMulti = 1.5;

	nodeStat node;
	instanceStat inst;
	probStat problem;
	solStats sStat;
	vector<nodeStat> nodeVec;


	sStat.feasible = false;
	
	while (!sStat.feasible){
		nodeVec.clear();

		readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem, trialK, trialMulti);
		
		hbundle h(&inst);

		// cout << "Dist Matrix: " << endl;
		// for (int i = 0; i < inst.V + inst.dummy; i++){
		//     for (int j = 0; j < inst.V + inst.dummy; j++){
		//         cout << setw(10) << distMatrix[i][j] << " ";
		//     }
		//     cout << endl;
		// }

		solveselect(&node, &inst, distMatrix, nodeVec, &problem, &sStat);

		if (problem.scen == "PC"){
			nodeVec.clear();
			sStat.solvec.clear();
			sStat.solBegin.clear();
			sStat.feasible = false;
			if (inst.min == true){
				// if (trialK < inst.n + inst.m){
				// 	trialK++;
				// }

				readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem, trialK, trialMulti);

				if (problem.model == "node"){
					nodeMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);

				}
				else if (problem.model == "bundle"){
					bundleMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);
				}
				inst.min = false;
			}
		}
		trialK++;
		cout << "trial K: " << trialK << endl;
	}

	return 0;
}
