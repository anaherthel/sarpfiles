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
#include "modelnode.h"
#include "modelbundle.h"
#include "modeltwostage.h"
#include "mipnode.h"
#include "hbundle.h"
#include "sarpILS.h"
#include "sarpConstruction.h"

using namespace std;

int main (int argc, char *argv[]) {
	double **distMatrix;
	
	int trialK = 1;
	double trialMulti = 1.5;

	nodeStat node;
	instanceStat inst;
	probStat problem;
	solStats sStat;
	vector<nodeStat> nodeVec;

	sarpILS sILS;
	sStat.feasible = false;
	
	// while (!sStat.feasible){
		nodeVec.clear();

		readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem, trialK, trialMulti);
		
		hbundle h(&inst);

		cout << "Dist Matrix: " << endl;
		for (int i = 0; i < inst.V + inst.dummy; i++){
		    for (int j = 0; j < inst.V + inst.dummy; j++){
		        cout << setw(10) << distMatrix[i][j] << " ";
		    }
		    cout << endl;
		}

		if (problem.model == "node"){
			nodeMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);

		}
		else if (problem.model == "bundle"){
			bundleMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);
		}

		else if (problem.model == "math"){
			// h.orderRequests(&inst, nodeVec, distMatrix, &problem);
			// h.buildDistVec(&inst, nodeVec, distMatrix, &problem);
			// h.buildBundles(&inst, nodeVec, distMatrix, &problem);
			// h.orgBundles(&inst, nodeVec, distMatrix, bStat, &problem);
			// h.hbundleMethod(&inst, nodeVec, distMatrix, &problem, &sStat);
		}

		else if (problem.model == "sp"){
			// h.orderRequests(&inst, nodeVec, distMatrix, &problem);
			// h.buildDistVec(&inst, nodeVec, distMatrix, &problem);
			// h.buildBundles(&inst, nodeVec, distMatrix, &problem);
			// h.orgBundles(&inst, nodeVec, distMatrix, bStat, &problem);
			// sCon.ConstrProc(&inst, nodeVec, distMatrix, &problem);
			sILS.ILS(&inst, nodeVec, distMatrix, &problem);
			// sILS.function();
		}
		// else if (problem.model == "twostage"){
		// 	twoStageMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);			
		// }

		// if (problem.scen == "PC"){
		// 	if (trialMulti > 1){
		// 		// if (trialK < inst.n + inst.m){
		// 		if (trialK < inst.n){
		// 			trialK++;	
		// 		}

		// 		else{
		// 			break;
		// 		}
		// 	}
		// 	else {
		// 		trialMulti = 1.5;
		// 	}
		// }

		// else{
		// 	if (trialMulti > 1){
		// 		if (trialK < inst.n){
		// 			trialK++;	
		// 		}

		// 		else{
		// 			break;
		// 		}
		// 	}
		// 	else {
		// 		trialMulti = 1.5;
		// 	}	
		// }
		
	// }

	if (problem.scen == "PC"){
		nodeVec.clear();
		sStat.solvec.clear();
    	sStat.solBegin.clear();
		sStat.feasible = false;
		if (inst.min == true){
			cout << "here" << endl;
			getchar();
			// if (trialK < inst.n + inst.m){
			// 	trialK++;
			// }
			cout << "trial K: " << trialK << endl;
			getchar();

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

	return 0;
}
