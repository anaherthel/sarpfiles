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

using namespace std;

int main (int argc, char *argv[]) {
	double **distMatrix;
	
	int trialK = 2;

	nodeStat node;
	instanceStat inst;
	probStat problem;
	solStats sStat;
	vector<nodeStat> nodeVec;
	sStat.feasible = false;
	

	
	while (!sStat.feasible){
		nodeVec.clear();

		readData(argc, argv, &node, &inst, nodeVec, &distMatrix, &problem, trialK);

		if (problem.model == "node"){
			nodeMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);

		}
		else if (problem.model == "bundle"){
			bundleMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);
		}

		else if (problem.model == "twostage"){
			twoStageMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);			
		}
		
		if (trialK < inst.n){
			trialK++;	
		}
		else{
			break;
		}

	}

	return 0;
}
