#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &bundleVec, vector<double> &bundleTimes, vector< vector< vector<int> > > &clusterVec, vector< vector<bool> > &bArcs, vector< vector< pair<int,int> > > &bArcPlus, vector< vector< pair<int,int> > > &bArcMinus, vector< pair<int,int> > &bArcVec, vector<double> &bundleProfVec){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
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



	// //Creating constraints

	// //Constraint 0 - x and y

	// for (int i = 0; i < inst->n + 2*inst->m; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < arcPlus[i].size(); j++){
	// 			exp += x[i][arcPlus[i][j].second][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint0_%d", i);
	// 	IloRange cons = (exp - y[i] == 0);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// // Constraint 1 - all passenger requests must be served
	
	// // for (int i = 0; i < inst->n; i++){
	// // 	IloExpr exp(env);
	// // 	for (int k = 0; k < inst->K; k++){
	// // 		for (int j = 0; j < arcPlus[i].size(); j++){
	// // 			exp += x[i][arcPlus[i][j].second][k];
	// // 		}
	// // 	}
	// // 	sprintf (var, "Constraint1_%d", i);
	// // 	IloRange cons = (exp == 1);
	// // 	cons.setName(var);
	// // 	model.add(cons);
	// // }

	// for (int i = 0; i < inst->n; i++){
	// 	IloExpr exp(env);
	// 	// for (int k = 0; k < inst->K; k++){
	// 	// 	for (int j = 0; j < arcPlus[i].size(); j++){
	// 	// 		exp += x[i][arcPlus[i][j].second][k];
	// 	// 	}
	// 	// }
	// 	exp = y[i];
	// 	sprintf (var, "Constraint1_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");

	// SARP.solve();
	// cout << "\nObj Val: " << setprecision(15) << SARP.getObjValue() << endl;

	env.end();

}
