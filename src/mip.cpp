#include "mip.h"
#include <cstdlib>
#include <stdio.h>

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, vector< vector<int> > &clusterVec, vector< pair<int,int> > &cArcVec, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bStat->bundleVec.size() - (inst->startCluster*inst->K) - inst->endCluster;
	int csetN = clusterVec.size() - inst->K - 1;
	int currSP;
	int currParcel;
	int currCluster;
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

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
	
	//Old version of constraint
	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
	// 	for (int i = 0; i < bStat->bArcMinus[bStat->bundleVec.size()-1].size(); i++){
	// 		exp += x[bStat->bArcMinus[bStat->bundleVec.size()-1][i].first][bStat->bArcMinus[bStat->bundleVec.size()-1][i].second][k];
	// 	}
	// 	sprintf (var, "Constraint4_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }	
	//new version of constraint
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int j = 0; j < clusterVec.back().size(); j++){
			for (int i = 0; i < bStat->bArcMinus[clusterVec.back()[j]].size(); i++){
				exp += x[bStat->bArcMinus[clusterVec.back()[j]][i].first][bStat->bArcMinus[clusterVec.back()[j]][i].second][k];
			}
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

	//Constraint 6 - Flow conservation between clusters(bundles)

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int j = 0; j < bStat->bArcPlus[i].size(); j++){
				// cout << bStat->bArcPlus[i][j].first << " " << bStat->bArcPlus[i][j].second << "||";

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
					// for (int l = 0; l < bStat->bArcPlus[bStat->parcelBundleVec[i][j]].size(); l++){
					// 	cout << bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first << " " << bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second << "||";
					// 	exp1 += x[bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].first][bStat->bArcPlus[bStat->parcelBundleVec[i][j]][l].second][k];
					// 	cout << exp1 << endl;
					// }
					// cout << "First part: ";
					for (int l = 0; l < bStat->bArcMinus[bStat->parcelBundleVec[i][j]].size(); l++){
						// cout << bStat->bArcMinus[bStat->parcelBundleVec[i][j]][l].first << " " << bStat->bArcMinus[bStat->parcelBundleVec[i][j]][l].second << endl;
						
						int u = bStat->bArcMinus[bStat->parcelBundleVec[i][j]][l].first;
						int v = bStat->bArcMinus[bStat->parcelBundleVec[i][j]][l].second;
						
						exp1 += x[u][v][k];
						// cout << exp1 << endl;
					}				
				}
								
				// for (int j = 0; j < bStat->parcelBundleVec[i + inst->m].size(); j++){
				// 	for (int l = 0; l < bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]].size(); l++){
				// 		exp2 += x[bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].first][bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].second][k];
				// 	}
				// }
				// cout << "Second part: ";

				for (int j = 0; j < bStat->parcelBundleVec[i + inst->m].size(); j++){
					for (int l = 0; l < bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]].size(); l++){
						// cout << bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].first << " " << bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].second << endl;

						int u = bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].first;
						int v = bStat->bArcMinus[bStat->parcelBundleVec[i + inst->m][j]][l].second;
						
						exp2 += x[u][v][k];
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
	// sStat->feasible = bSARP.isPrimalFeasible();

	cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

	sStat->solprofit = bSARP.getObjValue();

	for (int k = 0; k < inst->K; k++){
 		sStat->solvec.push_back(auxPairVec);
	}

	for (int i = 0; i < bStat->bundleVec.size(); i++){
		for(int j = 0; j < bStat->bundleVec.size(); ++j){
			for (int k = 0; k < inst->K; k++){
				if (bStat->bArcs[i][j] == true){
					if (bSARP.getValue(x[i][j][k]) > 0.5){
						auxPair.first = i;
						auxPair.second = j;
						sStat->solvec[k].push_back(auxPair);
						// cout << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
						// getchar();
					}
				}
			}
		}	
	}
	
	for (int k = 0; k < inst->K; k++){
		for (int i = 0; i < sStat->solvec[k].size(); i++){
			cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
		}
	}	

	env.end();
}

void mipnode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	long W = 2*inst->m;
	int Q;

	
	if (problem->scen == "1A" || problem->scen == "1B"){
		Q = 1;
	}
	else if (problem->scen == "2A" || problem->scen == "2B"){
		Q = 2;
	}

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

	for (int i = 0; i < nodeVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
		for(int j = 0; j < nodeVec.size(); ++j){
			if (nas->arcs[i][j] != true){
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
	
	//creates boolean variable (y_i = 1 if node i is visited; 0 cc)
	IloArray <IloBoolVarArray> y(env, nodeVec.size()); 

	for (int i = 0; i < nodeVec.size(); i++){
		y[i] = IloBoolVarArray (env, inst->K);
		for (int k = 0; k < inst->K; k++){
			sprintf(var, "y(%d,%d)", i, k);
			y[i][k].setName(var);
			model.add(y[i][k]);
		}
	}

	// //Variable start of service time
	// IloNumVarArray b(env, nodeVec.size());
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "b(%d)", i);
	// 	b[i].setName(var);
	// 	model.add(b[i]);
		
	// }
	// //Load variable
	// IloNumVarArray w(env, nodeVec.size());
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "w(%d)", i);
	// 	w[i].setName(var);
	// 	model.add(w[i]);
		
	// }	

	// IloExpr objFunction(env);

	// for (int i = 0; i < nas->arcNplus.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		objFunction += nodeVec[nas->arcNplus[i].first].profit * x[nas->arcNplus[i].first][nas->arcNplus[i].second][k];
	// 	}
	// }

	// for (int i = 0; i < nas->arcPP.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		objFunction += nodeVec[nas->arcPP[i].first].profit * x[nas->arcPP[i].first][nas->arcPP[i].second][k];
	// 	}
	// }

	// for (int i = 0; i < nas->allArcs.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		objFunction -= (double)mdist[nas->allArcs[i].first][nas->allArcs[i].second] * x[nas->allArcs[i].first][nas->allArcs[i].second][k];
	// 	}
	// }

	// model.add(IloMaximize(env, objFunction));


	//Creating constraints

	//Constraint 1 - All passenger nodes must be visited

	// for (int i = 0; i < inst->n; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < nas->arcPlus[i].size(); j++){
	// 			exp += x[i][nas->arcPlus[i][j].second][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint1_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 2 - Relationship between visit variable and parcel node arcs
	// //A parcel request can be denied

	// for (int i = inst->n; i < inst->n + 2*inst->m; i++){
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < nas->arcPlus[i].size(); j++){
	// 			exp += x[i][nas->arcPlus[i][j].second][k];
	// 		}
	// 		exp2 += y[i][k];
	// 	}

	// 	sprintf (var, "Constraint2_%d", i);
	// 	IloRange cons = (exp - exp2 == 0);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	// for (int i = inst->n; i < inst->n + inst->m; i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp1(env);
	// 		IloExpr exp2(env);
	// 		//Left side: arc leaves i
	// 		for (int j = 0; j < nas->arcPlus[i].size(); j++){
	// 			exp1 += x[i][nas->arcPlus[i][j].second][k];
	// 		}
	// 		//Right side: arc leaves i + m
	// 		for (int j = 0; j < nas->arcPlus[i + inst->m].size(); j++){
	// 			exp2 += x[i + inst->m][nas->arcPlus[i + inst->m][j].second][k];

	// 		}
	// 		sprintf (var, "Constraint3_%d_%d", i, k);
	// 		IloRange cons = ((exp1-exp2) == 0);
	// 		cons.setName(var);
	// 		model.add(cons);
	// 	}
	// }

	// //Constraint 4 - Flow conservation

	// for (int i = 0; i < inst->n + 2*inst->m; i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp1(env);
	// 		IloExpr exp2(env);
	// 		//Left side: arc leaves i
	// 		for (int j = 0; j < nas->arcPlus[i].size(); j++){
	// 			exp1 += x[nas->arcPlus[i][j].first][nas->arcPlus[i][j].second][k];
	// 		}
	// 		//Right side: arc enters i
	// 		for (int j = 0; j < nas->arcMinus[i].size(); j++){
	// 			exp2 += x[nas->arcMinus[i][j].first][nas->arcMinus[i][j].second][k];

	// 		}
	// 		sprintf (var, "Constraint4_%d_%d", i, k);
	// 		IloRange cons = ((exp1-exp2) == 0);
	// 		cons.setName(var);
	// 		model.add(cons);
	// 	}
	// }
	// //Constraint 5 - The route of every vehicle has to start at its starting position

	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
	// 	for (int i = 0; i < nas->arcPlus[inst->V - inst->K + k].size(); i++){
	// 		exp += x[nas->arcPlus[inst->V - inst->K + k][i].first][nas->arcPlus[inst->V - inst->K + k][i].second][k];
	// 	}
	// 	sprintf (var, "Constraint5_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 6 - The route of every vehicle has to end at dummy node f

	// for (int k = 0; k < inst->K; k++){
	// 	IloExpr exp(env);
	// 	for (int i = 0; i < nas->arcMinus[inst->V].size(); i++){
	// 		exp += x[nas->arcMinus[inst->V][i].first][nas->arcMinus[inst->V][i].second][k];
	// 	}
	// 	sprintf (var, "Constraint6_%d", k);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraint 7 - tie service begining to node visit

	// for (int i = 0; i < inst->V; i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp(env);
	// 		exp = b[i] - M * y[i][k]; 
	// 		sprintf (var, "Constraint7_%d_%d", i, k);
	// 		IloRange cons = (exp <= 0);
	// 		cons.setName(var);
	// 		model.add(cons);
	// 	}
	// }

	// //Constraint 8 - service of pickup must come before the delivery

	// for (int i = 0; i < inst->n + inst->m; i++){
	// 	IloExpr exp(env);
	// 	exp = b[i] - b[i + inst->m];

	// 	sprintf (var, "Constraint8_%d", i);
	// 	IloRange cons = (exp <= 0);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// //Constraints 9 - TW 

	// for (int i = 0; i < nas->allArcs.size(); i++){
	// 	IloExpr exp(env);
	// 	IloExpr sumX(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		sumX += x[nas->allArcs[i].first][nas->allArcs[i].second][k];
	// 	}
	// 		exp = b[nas->allArcs[i].first] - b[nas->allArcs[i].second] + nodeVec[nas->allArcs[i].first].delta + (mdist[nas->allArcs[i].first][nas->allArcs[i].second]/inst->vmed) - M * (1 - sumX);
	// 		sprintf (var, "Constraint9_%d_%d", nas->allArcs[i].first, nas->allArcs[i].second);
	// 		IloRange cons = (exp <= 0);
	// 		cons.setName(var);
	// 		model.add(cons);			

	// }

	// //Constraints 10 and 11- load constraints

	// for (int i = 0; i < nas->arcnf.size(); i++){
		
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	IloExpr sumX(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		sumX += x[nas->arcnf[i].first][nas->arcnf[i].second][k];
	// 	}

	// 	exp = w[nas->arcnf[i].first] + nodeVec[nas->arcnf[i].first].load - W*(1 - sumX);
	// 	exp2 = w[nas->arcnf[i].second];
	// 	sprintf (var, "Constraint10_%d", i);
	// 	IloRange cons1 = (exp - exp2 <= 0);
	// 	cons1.setName(var);
	// 	model.add(cons1);
		
	// 	sprintf (var, "Constraint11_%d", i);
	// 	IloRange cons2 = (exp2 <= Q);
	// 	cons2.setName(var);
	// 	model.add(cons2);			

	// }


	// //Constraints 12 and 13 - bound the service beginning time by the earlier and later service times for each node

	// for (int i = 0; i < inst->n; i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp(env);
	// 		exp = b[i];

	// 		sprintf (var, "Constraint12_%d_%d", i, k);
	// 		IloRange cons1 = (exp <= nodeVec[i].l);
	// 		cons1.setName(var);
	// 		model.add(cons1);
			
	// 		sprintf (var, "Constraint13_%d_%d", i, k);
	// 		IloRange cons2 = (nodeVec[i].e <= exp);
	// 		cons2.setName(var);
	// 		model.add(cons2);			
	// 	}

	// }

	IloCplex nSARP(model);
	nSARP.exportModel("nSARP.lp");
	
	// nSARP.solve();
	// cout << "\nSol status: " << nSARP.getStatus() << endl;
	// // sStat->feasible = nSARP.isPrimalFeasible();

	// cout << "\nObj Val: " << setprecision(15) << nSARP.getObjValue() << endl;

	// sStat->solprofit = nSARP.getObjValue();

	env.end();
}