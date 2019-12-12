#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, double **mdist, vector< pair<int,int> > &arcVec, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "SARP");
	//long M = numeric_limits<long>::max();
	long M = 2*inst->T;
	long W = 2*inst->m;

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

	IloArray <IloBoolVarArray> y(env, nodeVec.size()); //creates boolean variable (y_i = 1 se nó i é visitado; 0 cc)
	for (int i = 0; i < nodeVec.size(); i++){
		y[i] = IloBoolVarArray (env, inst->K);
		for (int k = 0; k < inst->K; k++){
			sprintf(var, "y(%d,%d)", i, k);
			y[i][k].setName(var);
			model.add(y[i][k]);
		}
	}

	IloNumVarArray b(env, nodeVec.size(), 0, inst->T); //creates continuous variable with the specified bounds

	for (int i = 0; i < nodeVec.size(); ++i){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	IloNumVarArray w(env, nodeVec.size(), 0, inst->m + 1); //creates continuous variable with the specified bounds

	for (int i = 0; i < nodeVec.size(); ++i){
		sprintf(var, "w(%d)", i);
		w[i].setName(var);
		model.add(w[i]);
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
					objFunction += (double)inst->gamma * x[i][arcVec[j].second][k];
					objFunction += (double)inst->mu * mdist[i][i + inst->m] * x[i][arcVec[j].second][k];
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
	
	for (int i = inst->n; i < inst->n + inst->m; i++){
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

	//Constraints 7 and 8 - TW

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		exp = b[i];

		sprintf (var, "Constraint7_%d", i);
		IloRange cons1 = (exp <= nodeVec[i].l);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint8_%d", i);
		IloRange cons2 = (nodeVec[i].e <= exp);
		cons2.setName(var);
		model.add(cons2);
	}

	//Constraint 9 - bound begining of service to pickup and delivery

	for (int i = inst->n; i < inst->n + inst->m; i++){
		IloExpr exp(env);
		exp = b[i] + nodeVec[i].delta - b[i + inst->m];

		sprintf (var, "Constraint9_%d", i);
		IloRange cons = (exp <= 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraints 10 - TW 

	for (int i = 0; i < arcVec.size(); i++){
		IloExpr exp(env);
		IloExpr sumx(env);

		for (int k = 0; k < inst->K; k++){
			sumx += x[arcVec[i].first][arcVec[i].second][k];
		}

			exp = b[arcVec[i].first] - b[arcVec[i].second] + nodeVec[arcVec[i].first].delta + (mdist[arcVec[i].first][arcVec[i].second]/inst->vmed) - M * (1 - sumx);
			sprintf (var, "Constraint10_%d_%d", arcVec[i].first, arcVec[i].second);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);					

	}

	//Constraints 11  - On-duty time
	for (int k = 0; k < inst->K; k++){	// cout << "\nDeltas: ";
	// for (int i =0; i < nodeVec.size(); i++){
	// 	cout << nodeVec[i].load << " ";
	// }
	// cout << endl;
	// getchar();
		for (int i = 0; i < arcPlus[inst->V - inst->K + k].size(); i++){
			IloExpr exp(env);
			exp = b[arcPlus[inst->V - inst->K + k][i].first] - mdist[arcPlus[inst->V - inst->K + k][i].first][arcPlus[inst->V - inst->K + k][i].second]/inst->vmed;

			sprintf (var, "Constraint11_%d_%d",k, i);
			IloRange cons = (nodeVec[inst->V - inst->K + k].e <= exp);
			cons.setName(var);
			model.add(cons);
			
		}		
	}
    //Constraints 12  - Off-duty time
	for (int k = 0; k < inst->K; k++){
		for (int i = 0; i < arcMinus[inst->V + k].size(); i++){
			IloExpr exp(env);
			//exp = b[i] + nodeVec[i].delta;
			exp = b[i] + nodeVec[i].delta;
			sprintf (var, "Constraint12_%d_%d",k, i);
			IloRange cons = (exp <= nodeVec[inst->V - inst->K + k].l);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraints 13 and 14 - transported capacity
	//Scenarios 1-A and 2-A: Q = 1;
	for (int i = 0; i < arcVec.size(); i++){
		IloExpr exp(env);
		IloExpr sumx(env);
		for (int k = 0; k < inst->K; k++){
			sumx += x[arcVec[i].first][arcVec[i].second][k];
		}

		exp = w[arcVec[i].second] - w[arcVec[i].first] - nodeVec[arcVec[i].second].load + W * (1 - sumx);

		sprintf (var, "Constraint13_%d_%d", arcVec[i].first, arcVec[i].second);
		IloRange cons1 = (0 <= exp);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint14_%d_%d", arcVec[i].first, arcVec[i].second);
		IloRange cons2 = (w[arcVec[i].second] <= 1);
		cons2.setName(var);
		model.add(cons2);
	} 
	//Scenarios 1-B and 2-B Q > 1:
	// for (int i = 0; i < arcVec.size(); i++){
	// 	IloExpr exp(env);
	// 	IloExpr sumx(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		sumx += x[arcVec[i].first][arcVec[i].second][k];
	// 	}

	// 	exp = w[arcVec[i].second] - w[arcVec[i].first] - nodeVec[i].load + W * (1 - sumx);

	// 	sprintf (var, "Constraint12_%d", i);
	// 	IloRange cons1 = (0 <= exp);
	// 	cons1.setName(var);
	// 	model.add(cons1);
		
	// 	sprintf (var, "Constraint13_%d", i);
	// 	IloRange cons2 = (exp <= floor(inst->m/2));
	// 	cons2.setName(var);
	// 	model.add(cons2);
	// } 

	IloCplex SARP(model);
	SARP.exportModel("SARP.lp");

	SARP.solve();
	cout << "\nObj Val: " << setprecision(15) << SARP.getObjValue() << endl;

	env.end();

	//infile.close();
	

}