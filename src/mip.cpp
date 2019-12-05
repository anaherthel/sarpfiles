#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, double **mdist, vector< pair<int,int> > &arcVec, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus){

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

	//Creating constraints
	//Constraint 1 - all passenger requests must be served
	
	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < arcPlus[i].size(); j++){
				exp += x[i][arcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 2 - parcel requests may be denied
	
	for (int i = inst->n - 1; i < inst->n + inst->m; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < arcPlus[i].size(); j++){
				exp += x[i][arcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}
	//Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	for (int i = inst->n; i < inst->n + inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int j = 0; j < arcPlus[i].size(); j++){
				exp1 += x[i][arcPlus[i][j].second][k];
			}
			//Right side: arc leaves i + m
			for (int j = 0; j < arcPlus[i + inst->m].size(); j++){
				exp2 += x[i + inst->m][arcPlus[i + inst->m][j].second][k];

			}
			sprintf (var, "Constraint3_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 4 - Flow conservation

	for (int i = 0; i < inst->n + 2*inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int j = 0; j < arcPlus[i].size(); j++){
				exp1 += x[arcPlus[i][j].first][arcPlus[i][j].second][k];
			}
			//Right side: arc enters i
			for (int j = 0; j < arcMinus[i].size(); j++){
				exp2 += x[arcMinus[i][j].first][arcMinus[i][j].second][k];

			}
			sprintf (var, "Constraint4_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 5 - The route of every vehicle has to start at its starting position

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < arcPlus[inst->V - inst->K + k].size(); i++){
			exp += x[arcPlus[inst->V - inst->K + k][i].first][arcPlus[inst->V - inst->K + k][i].second][k];
		}
		sprintf (var, "Constraint5_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);	
	}

	//Constraint 6 - The route of every vehicle has to end at the dummy node f.

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < arcMinus[inst->V].size(); i++){
			exp += x[arcMinus[inst->V][i].first][arcMinus[inst->V][i].second][k];
		}
		sprintf (var, "Constraint6_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);	
	}

	IloCplex SARP(model);
	SARP.exportModel("SARP.lp");

	// ShiftSKQAP.solve();
	// cout << "\nObj Val: " << setprecision(15) << ShiftSKQAP.getObjValue() << endl;

	env.end();

	//infile.close();
	

}