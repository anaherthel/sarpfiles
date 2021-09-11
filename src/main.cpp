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

	sarpSolution globalSol;

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

			int mscount = 0;
			int msMax = 15;

			clock_t seed = (argc == 5) ? time(NULL) : strtol(argv[4], NULL, 10);
			
			// clock_t seed = 1631386809;

			srand(seed);


			cout << "\n\nSeed: " << std::setprecision(5) << seed << endl << endl;
			// getchar();
			// t1 = sILS.get_wall_time();
			sILS.stats.setStart();

			sILS.ILS(&inst, nodeVec, distMatrix, &problem);

			// t2 = sILS.get_wall_time();
			// deltaT = t2 - t1;

			sILS.stats.setEnd();

			cout << "\nTotal Run Time: " << std::setprecision(8) <<  sILS.stats.printTime() << endl;

			cout << "END OF METHOD" << endl;
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

	return 0;
}
