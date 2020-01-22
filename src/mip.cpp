#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &bundleVec, vector<double> &bundleTimes, 
	vector< vector< vector<int> > > &clusterVec, vector< vector<bool> > &bArcs, vector< vector< pair<int,int> > > &bArcPlus, 
	vector< vector< pair<int,int> > > &bArcMinus, vector< pair<int,int> > &bArcVec, vector<double> &bundleProfVec, vector< vector<int> > &parcelBundleVec){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bundleVec.size() - inst->K - 1;
	int currSP;
	int currParcel;
	//long M = numeric_limits<long>::max();
	// long M = 2*inst->T;
	// long W = 2*inst->m;

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, bundleVec.size());

	for (int i = 0; i < bundleVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, bundleVec.size());
		for(int j = 0; j < bundleVec.size(); ++j){
			if (bArcs[i][j] != true){
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
	
	// IloBoolVarArray y(env, nodeVec.size()); //creates boolean variable (y_i = 1 se nó i é visitado; 0 cc)
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "y(%d)", i);
	// 	y[i].setName(var);
	// 	model.add(y[i]);
	// }

	// IloNumVarArray b(env, nodeVec.size(), 0, inst->T); //creates continuous variable with the specified bounds

	// for (int i = 0; i < nodeVec.size(); ++i){
	// 	sprintf(var, "b(%d)", i);
	// 	b[i].setName(var);
	// 	model.add(b[i]);
	// }

	// IloNumVarArray w(env, nodeVec.size(), 0, inst->m + 1); //creates continuous variable with the specified bounds

	// for (int i = 0; i < nodeVec.size(); ++i){
	// 	sprintf(var, "w(%d)", i);
	// 	w[i].setName(var);
	// 	model.add(w[i]);
	// }

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int i = 0; i < bArcVec.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += bundleProfVec[bArcVec[i].first] * x[bArcVec[i].first][bArcVec[i].second][k];
			objFunction += bundleProfVec[bArcVec[i].second] * x[bArcVec[i].first][bArcVec[i].second][k];
		}
	}
	int lastElOfi;
	int firstElOfj;
	for (int i = 0; i < bArcVec.size(); i++){
		lastElOfi = bundleVec[bArcVec[i].first][bundleVec[bArcVec[i].first].size() - 1];
		firstElOfj = bundleVec[bArcVec[i].second][0];
		for (int k = 0; k < inst->K; k++){
			objFunction -= (double)mdist[lastElOfi][firstElOfj] * x[bArcVec[i].first][bArcVec[i].second][k];
		}
	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster

	for (int i = 0; i < setN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < bArcPlus[i].size(); j++){
				exp += x[bArcPlus[i][j].first][bArcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 2 - Only one arc comes into the cluster
	
	for (int i = 0; i < setN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < bArcMinus[i].size(); j++){
				exp += x[bArcMinus[i][j].first][bArcMinus[i][j].second][k];
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
		for (int i = 0; i < bArcPlus[currSP].size(); i++){
			exp += x[bArcPlus[currSP][i].first][bArcPlus[currSP][i].second][k];
		}
		sprintf (var, "Constraint3_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 4 - All vehicles end the trip at the dummy node f
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < bArcMinus[bundleVec.size()-1].size(); i++){
			exp += x[bArcMinus[bundleVec.size()-1][i].first][bArcMinus[bundleVec.size()-1][i].second][k];
		}
		sprintf (var, "Constraint4_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}	

	//Constraint 5 - No parcel can be served more than once

	for (int i = 0; i < parcelBundleVec.size(); i++){
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < parcelBundleVec[i].size(); j++){
			for (int l = 0; l < bArcPlus[parcelBundleVec[i][j]].size(); l++){
				for (int k = 0; k < inst->K; k++){
					exp += x[bArcPlus[parcelBundleVec[i][j]][l].first][bArcPlus[parcelBundleVec[i][j]][l].second][k];
				}
			}
		}
		sprintf (var, "Constraint5_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 6 - Flow conservation between clusters

	// for (int i = 0; i < clusterVec.size(); i++){
	// 	for (int j = 0; j < clusterVec.size(); j++){
	// 		if (i != j){

	// 		}
	// 	}
	// }

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int j = 0; j < bArcPlus[bundleVec[i]].size(); j++){
				exp1 += x[bArcPlus[bundleVec[i]][j].first][bArcPlus[bundleVec[i]][j].second][k];
			}

			// for (int j = 0; j < bArcMinus[bundleVec[i]].size(); j++){
			// 	exp2 += x[bArcMinus[bundleVec[i]][j].first][bArcMinus[bundleVec[i]][j].second][k];
			// }

			// sprintf (var, "Constraint6_%d_%d", i, k);
			// IloRange cons = (exp1 - exp2 == 0);
			// cons.setName(var);
			// model.add(cons);			
		}
	}

	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");

	// SARP.solve();
	// cout << "\nObj Val: " << setprecision(15) << SARP.getObjValue() << endl;

	env.end();

}
