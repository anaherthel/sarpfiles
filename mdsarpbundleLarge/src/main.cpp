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

using namespace std;

int main (int argc, char *argv[]) {
	double **distMatrix;
	
	int trialK = 20;
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

		if (problem.model == "node"){
			nodeMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);

		}
		else if (problem.model == "bundle"){
			bundleMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);
		}

		// else if (problem.model == "twostage"){
		// 	twoStageMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);			
		// }

		if (problem.scen == "PC"){
			if (trialMulti > 1){
				// if (trialK < inst.n + inst.m){
				if (trialK < inst.n){
					trialK++;	
				}

				else{
					break;
				}
			}
			else {
				trialMulti = 1.5;
			}
		}

		else{
			if (trialMulti > 1){
				if (trialK < inst.n){
					trialK++;	
				}

				else{
					break;
				}
			}
			else {
				trialMulti = 1.5;
			}	
		}
		
	}

	if (problem.scen == "PC"){
		nodeVec.clear();
		sStat.solvec.clear();
    	sStat.solBegin.clear();
		sStat.feasible = false;
		if (inst.min == true){
			cout << "here" << endl;
			// getchar();
			// if (trialK < inst.n + inst.m){
			// 	trialK++;
			// }
			cout << "trial K: " << trialK << endl;
			// getchar(); 

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
