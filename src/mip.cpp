#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, vector< vector<int> > &clusterVec, vector< pair<int,int> > &cArcVec, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, probStat* problem, vector< vector<mipsol> > &solvec){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bStat->bundleVec.size() - inst->K - 1;
	int csetN = clusterVec.size() - inst->K - 1;
	int currSP;
	int currParcel;
	int currCluster;
	mipsol solution;
	vector <mipsol> auxsolvec;

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, bStat->bundleVec.size());

	for (int i = 0; i < bStat->bundleVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, bStat->bundleVec.size());
		for(int j = 0; j < bStat->bundleVec.size(); ++j){
			if (bStat->bArcs[i][j] != true){
				continue; // If arc i to j is invalid
			} 

			x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles

			for(int k = 0; k < inst->K; k++){
				sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}
		}	
	}

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int i = 0; i < bStat->bArcVec.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += bStat->bundleProfVec[bStat->bArcVec[i].first] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
			//objFunction += bStat->bundleProfVec[bStat->bArcVec[i].second] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}
	int lastElOfi;
	int firstElOfj;
	for (int i = 0; i < bStat->bArcVec.size(); i++){
		lastElOfi = bStat->bundleVec[bStat->bArcVec[i].first][bStat->bundleVec[bStat->bArcVec[i].first].size() - 1];
		firstElOfj = bStat->bundleVec[bStat->bArcVec[i].second][0];
		for (int k = 0; k < inst->K; k++){
			objFunction -= (double)mdist[lastElOfi][firstElOfj] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster

	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < cArcPlus[i].size(); j++){
				exp += x[cArcPlus[i][j].first][cArcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 2 - Only one arc comes into the cluster

	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < cArcMinus[i].size(); j++){
				exp += x[cArcMinus[i][j].first][cArcMinus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 3 - Each vehicle leaves its starting node
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		currSP = setN + k;
		for (int i = 0; i < bStat->bArcPlus[currSP].size(); i++){
			exp += x[bStat->bArcPlus[currSP][i].first][bStat->bArcPlus[currSP][i].second][k];
		}
		sprintf (var, "Constraint3_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 4 - All vehicles end the trip at the dummy node f
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < bStat->bArcMinus[bStat->bundleVec.size()-1].size(); i++){
			exp += x[bStat->bArcMinus[bStat->bundleVec.size()-1][i].first][bStat->bArcMinus[bStat->bundleVec.size()-1][i].second][k];
		}
		sprintf (var, "Constraint4_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}	

	//Constraint 5 - No parcel can be served more than once

	for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
			for (int l = 0; l < bStat->bArcPlus[bStat->parcelBundleVec[i][j]].size(); l++){
				for (int k = 0; k < inst->K; k++){
					exp += x[bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first][bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second][k];
				}
			}
		}
		sprintf (var, "Constraint5_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}	

	//Constraint 6 - Flow conservation between clusters

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int j = 0; j < bStat->bArcPlus[i].size(); j++){
				exp1 += x[bStat->bArcPlus[i][j].first][bStat->bArcPlus[i][j].second][k];
			}

			for (int j = 0; j < bStat->bArcMinus[i].size(); j++){
				exp2 += x[bStat->bArcMinus[i][j].first][bStat->bArcMinus[i][j].second][k];
			}

			sprintf (var, "Constraint6_%d_%d", i, k);
			IloRange cons = (exp1 - exp2 == 0);
			cons.setName(var);
			model.add(cons);			
		}
	}

	if (problem->scen == "2A"){
		//Constraint 7 - if a pickup bundle is visited, the corresponding delivery bundle must also be visited by the same vehicle
		for (int i = 0; i < inst->m; i++){
			for (int k = 0; k < inst->K; k++){
				IloExpr exp1(env);
				IloExpr exp2(env);
				currParcel = inst->n + i;
				for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
					for (int l = 0; l < bStat->bArcPlus[bStat->parcelBundleVec[i][j]].size(); l++){
						exp1 += x[bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first][bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second][k];
					}					
				}
								
				for (int j = 0; j < bStat->parcelBundleVec[i + inst->m].size(); j++){
					for (int l = 0; l < bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]].size(); l++){
						exp1 += x[bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].first][bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].second][k];
					}
				}

				sprintf (var, "Constraint7_%d_%d", currParcel, k);
				IloRange cons = (exp1 - exp2 == 0);
				cons.setName(var);
				model.add(cons);			
			}			
		}
	}	

	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");

	bSARP.solve();
	cout << "\nSol status: " << bSARP.getStatus() << endl;
	cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

	for (int k = 0; k < inst->K; k++ ){
 		solvec.push_back(auxsolvec);
	}

	for (int i = 0; i < bStat->bundleVec.size(); i++){
		for(int j = 0; j < bStat->bundleVec.size(); ++j){
			for (int k = 0; k < inst->K; k++){
				if (bStat->bArcs[i][j] == true){
					if (bSARP.getValue(x[i][j][k]) == 1){
						solution.s = i;
						solution.e = j;
						solution.k = k;
						solvec[k].push_back(solution);
						// cout << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
					}
				}
			}
		}	
	}
	
	for (int k = 0; k < inst->K; k++){
		for (int i = 0; i < solvec[k].size(); i++){
			cout << "x(" << solvec[k][i].s << ", " << solvec[k][i].e << ", " << solvec[k][i].k << ")" << endl;
		}
	}	

	env.end();
}
