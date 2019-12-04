#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, double **mdist, vector< pair<int,int> > arcVec){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "SARP");
	double gamma = 3;
	double mu = 0.5;
	double vmed = 18;

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

	for (int i = 0; i < nodeVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
		for(int j = 0; j < nodeVec.size(); ++j){
			if (arcs[i][j] != true){
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

	// upperBound = upperBoundTime(data);
	// BigB = upperBound + 1;
	// printf("upperboundtime: %.2lf\n\n", upperBound);

	IloNumVarArray b(env, nodeVec.size(), 0, inst->T); //creates continuous variable with the specified bounds

	for (int i = 0; i < nodeVec.size(); ++i){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	//  Load Variable
	// upperBound = (double) upperBoundCapacity(data);
	// BigQ = upperBound + 1;
	// printf("upperboundcapacity: %.2lf\n\n", upperBound);

	// IloArray <IloNumVarArray> Q(env, 4);

	// for(int r = 0; r < 4; ++ r){
	// 	Q[r] = IloNumVarArray(env, data.V.size(), 0, upperBound);
	// 	for(int i = 0; i < data.V.size(); ++i){
	// 		sprintf(var, "Q(%d,%d)", r, i);
	// 		Q[r][i].setName(var);
	// 		model.add(Q[r][i]);
	// 	}
	// }

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int i = inst->n; i < inst->n + inst->m; i++){
		for (int j = 0; j < arcVec.size(); j++){
			if(arcVec[j].first == i){
				for (int k = 0; k < inst->K; k++){
					objFunction += (double)gamma * x[i][arcVec[j].second][k];
					objFunction += (double)mu * mdist[i][i + inst->m] * x[i][arcVec[j].second][k];
				}
			}
		}
	}
	for (int i = 0; i < arcVec.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction -= (double)mdist[arcVec[i].first][arcVec[i].second] * x[arcVec[i].first][arcVec[i].second][k];
		}
	}

	model.add(IloMaximize(env, objFunction));

	// //Creating constraints
	// //Constraint 1

	// for (int i = 0; i < occRows.size(); i++){
	// 	IloExpr exp(env);
	// 	sprintf (var, "Constraint1_%d", i);
	// 	for (int k = 0; k < shifts.size(); k++){
	// 		exp += y[i][k];
	// 	}
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 2
	
	// for (int i = 0; i < occRows.size(); i++){
	// 	for (int j = i + 1; j < occRows.size(); j++){
	// 		for (int k = 0; k < shifts.size(); k++){
	// 			for (int l = 0; l < shifts.size(); l++){
	// 				IloExpr exp(env);
	// 				sprintf (var, "Constraint2_%d_%d_%d_%d", i, j, k, l);

	// 				exp = x[i][j][k][l] - y[i][k] - y[j][l];

	// 				IloRange cons = (exp >= - 1);
	// 				cons.setName(var);
	// 				model.add(cons);
	// 			}
	// 		}
	// 	}
	// }


	IloCplex SARP(model);
	SARP.exportModel("SARP.lp");

	// ShiftSKQAP.solve();
	// cout << "\nObj Val: " << setprecision(15) << ShiftSKQAP.getObjValue() << endl;

	env.end();

	//infile.close();

}