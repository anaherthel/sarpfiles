#include "mipnode.h"
#include <cstdlib>
#include <stdio.h>

void mipnode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);
	long W = inst->m + 1;
	int Q;
	Q = inst->m;

    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;


	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	// if (problem->scen == "1A" || problem->scen == "2A"){
	// 	Q = 1;
	// }
	// else if (problem->scen == "1B" || problem->scen == "2B"){
	// 	Q = inst->m;
	// }
	// else{
	// 	Q = inst->m + 1;
	// }


	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

    for (int i = 0; i < nodeVec.size(); i++){
        x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
        for(int j = 0; j < nodeVec.size(); ++j){
            if (nas->arcs[i][j] != true){
                continue; // If arc i to j is invalid
            } 

            x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
            for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                int k = nas->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
                x[i][j][k].setName(var);
                model.add(x[i][j][k]);
                // cout << "x: [" << i << "][" << j << "][" << k << "]" << endl;
            }
        }
    }

	//creates boolean variable (y_i = 1 if node i is visited; 0 cc)
	IloBoolVarArray y(env, nodeVec.size()); 

	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "y(%d)", i);
		y[i].setName(var);
		model.add(y[i]);
	}

		// Variable start of service time
	IloNumVarArray b(env, nodeVec.size(), 0, inst->T);
	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	//Load variable
	IloNumVarArray w(env, nodeVec.size(), 0, Q);


	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "w(%d)", i);
		w[i].setName(var);
		model.add(w[i]);
		
	}

	IloExpr objFunction(env);

    objFunction += inst->totalCustomProfit;

    for (int a = 0; a < nas->arcPP.size(); a++){
        int i = nas->arcPP[a].first;
        int j = nas->arcPP[a].second;
        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction += nodeVec[i].profit * x[i][j][k];
        }
    }

    for (int a = 0; a < nas->allArcs.size(); a++){
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction -= (double)inst->costkm*mdist[i][j] * x[i][j][k];
        }
    }

	model.add(IloMaximize(env, objFunction));


	//Creating constraints

	//Constraint 1 - All passenger nodes must be visited

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 2 - Relationship between visit variable and parcel node arcs
	//A parcel request can be denied


	// if (problem->scen == "PC"){
	// 	for (int i = inst->n; i < inst->n + inst->m; i++){

	// 		IloExpr exp(env);
	// 		for (int k = 0; k < inst->K; k++){
	// 			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
	// 	            int u = nas->vArcPlus[i][k][a].first;
	// 	            int v = nas->vArcPlus[i][k][a].second;

	// 				exp += x[u][v][k];
	// 			}
	// 		}

	// 		sprintf (var, "Constraint2_%d", i);
	// 		IloRange cons = (exp <= 1);
	// 		cons.setName(var);
	// 		model.add(cons);
	// 	}
	// }
	// else{
		for (int i = inst->n; i < inst->n + inst->m; i++){

			IloExpr exp(env);
			IloExpr exp2(env);
			for (int k = 0; k < inst->K; k++){
				for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
	                int u = nas->vArcPlus[i][k][a].first;
	                int v = nas->vArcPlus[i][k][a].second;

					exp += x[u][v][k];
				}
			}

	        exp2 += y[i];
			sprintf (var, "Constraint2_%d", i);
			IloRange cons = (exp - exp2 == 0);
			// IloRange cons = (exp == 1);
			cons.setName(var);
			model.add(cons);
		}		
	// }
	
	//Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	for (int i = inst->n; i < inst->n + inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp1 += x[u][v][k];
			}
			//Right side: arc leaves i + m
			for (int a = 0; a < nas->vArcPlus[i + inst->m][k].size(); a++){
				int u = nas->vArcPlus[i + inst->m][k][a].first;
                int v = nas->vArcPlus[i + inst->m][k][a].second;

                exp2 += x[u][v][k];
			}
			sprintf (var, "Constraint3_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 4 - Flow conservation

	for (int a = 0; a < inst->n + 2*inst->m; a++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int b = 0; b < nas->vArcPlus[a][k].size(); b++){
                int i = nas->vArcPlus[a][k][b].first;
                int j = nas->vArcPlus[a][k][b].second;
				exp1 += x[i][j][k];
			}
			//Right side: arc enters i
			for (int b = 0; b < nas->vArcMinus[a][k].size(); b++){
                int i = nas->vArcMinus[a][k][b].first;
                int j = nas->vArcMinus[a][k][b].second;
				exp2 += x[i][j][k];

			}
			sprintf (var, "Constraint4_%d_%d", a, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 5 - The route of every vehicle has to start at its starting position


    for (int k = 0; k < inst->K; k++){
        IloExpr exp(env);
        for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
            int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
            int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

            exp += x[u][v][k];
        }
        sprintf (var, "Constraint5_%d", k);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// Constraint 6 - The route of every vehicle has to end at dummy node f

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int a = 0; a < nas->vArcMinus[inst->V + k][k].size(); a++){
            int u = nas->vArcMinus[inst->V + k][k][a].first;
            int v = nas->vArcMinus[inst->V + k][k][a].second;

        	exp += x[u][v][k];
		}
		sprintf (var, "Constraint6_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 7 - tie service begining to node visit

    for (int i = 0; i < fDepot; i++){
        IloExpr exp(env);
        exp = b[i] - M * y[i]; 
        sprintf (var, "Constraint7_%d", i);
        IloRange cons = (exp <= 0);
        cons.setName(var);
        model.add(cons);
    }


	//Constraint 8 - service of pickup must come before the delivery

	for (int i = inst->n; i < inst->n + inst->m; i++){
		IloExpr exp(env);
		exp = b[i] - b[i + inst->m];

		sprintf (var, "Constraint8_%d", i);
		IloRange cons = (exp <= 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraints 9 - TW 

	for (int a = 0; a < nas->allArcs.size(); a++){
		IloExpr exp(env);
		IloExpr sumX(env);
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            sumX += x[i][j][k];
		}
			exp = b[i] - b[j] + nodeVec[i].delta + (mdist[i][j]/inst->vmed) - M * (1 - sumX);
			sprintf (var, "Constraint9_%d_%d", i, j);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	// //Constraints 10 - load constraints

	for (int a = 0; a < nas->allArcs.size(); a++){
		
		IloExpr exp(env);
		IloExpr exp2(env);
		IloExpr sumX(env);
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;

        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
			sumX += x[i][j][k];
		}

		exp = w[i] + nodeVec[j].load - W*(1 - sumX);
		exp2 = w[j];
		
        sprintf (var, "Constraint10_%d_%d", i, j);
		
        IloRange cons1 = (exp2 - exp >= 0);
		cons1.setName(var);
		model.add(cons1);
	}

	//Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	for (int i = 0; i < nodeVec.size(); i++){
		IloExpr exp(env);
		exp = b[i];

		sprintf (var, "Constraint11_%d", i);
		IloRange cons1 = (exp <= nodeVec[i].l);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint12_%d", i);
		IloRange cons2 = (nodeVec[i].e <= exp);
		cons2.setName(var);
		model.add(cons2);			
	}

    //Constraints 13 - maximum driving time

    for (int i = fDepot; i < fDummy; i++){
        IloExpr exp(env);
        exp = b[i + inst->K] - b[i];

        sprintf (var, "Constraint13_%d", i);
        IloRange cons1 = (exp <= inst->maxTime);
        cons1.setName(var);
        model.add(cons1);        
    }
	
	//Constraint 14  - bound number of passenger visits transporting parcel
	if (problem->scen == "1A" || problem->scen == "1B"){
		for (int i = 0; i < nas->arcNN.size(); i++){
			IloExpr exp(env);
			IloExpr sumX(env);
            int u = nas->arcNN[i].first;
            int v = nas->arcNN[i].second;

			for (int k = 0; k < inst->K; k++){
				sumX += x[u][v][k];
			}
			exp = 1 - (w[u]/W);

			sprintf (var, "Constraint14_%d", i);
			IloRange cons1 = (sumX - exp <= 0);
			cons1.setName(var);
			model.add(cons1);

		}
	}

		//Constraint 14  -  no parcel transportation while serving passenger
	if (problem->scen == "BL2"){
		for (int i = 0; i < inst->n; i++){
			IloExpr exp(env);

			exp = w[i];

			sprintf (var, "Constraint14_%d", i);
			IloRange cons1 = (exp == 0);
			cons1.setName(var);
			model.add(cons1);

		}
	}

	// else if (problem->scen == "BL2"){
	// 	for (int i = 0; i < nas->arcNN.size(); i++){
	// 		IloExpr exp(env);
	// 		IloExpr sumX(env);
 //            int u = nas->arcNN[i].first;
 //            int v = nas->arcNN[i].second;

	// 		for (int k = 0; k < inst->K; k++){
	// 			sumX += x[u][v][k];
	// 		}
	// 		exp = - (w[u]);

	// 		sprintf (var, "Constraint14_%d", i);
	// 		IloRange cons1 = (sumX - exp >= 1);
	// 		cons1.setName(var);
	// 		model.add(cons1);

	// 	}
	// }

	// else if (problem->scen == "PC" && inst->min){
	// 	IloExpr exp(env);

	// 	for (int i = inst->n; i < inst->n + inst->m; i++){
	// 		for (int k = 0; k < inst->K; k++){
	// 			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
	// 	            int u = nas->vArcPlus[i][k][a].first;
	// 	            int v = nas->vArcPlus[i][k][a].second;

	// 				exp += x[u][v][k];
	// 			}
	// 		}
	// 	}

	// 	sprintf (var, "Constraint14");
	// 	IloRange cons = (exp >= 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

 //    //*******************************

    int threads;

    threads = 1;

    // if (inst->instParam[0] < 14){
    //     if (inst->instParam[0] > 12 && inst->instParam[1] == 7){
    //         threads = 8;
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }

    // else if (inst->instParam[0] > 14) {
    //     threads = 8;
    // }

    // else{
    //     if (inst->instParam[1] > 1){
    //         threads = 8;              
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }
    cout << "\nThreads: " << threads << endl;

	IloCplex nSARP(model);
	nSARP.exportModel("nSARP.lp");
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	cout << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    cout << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    cout << " Total Time: " << time << endl;

	if (sStat->feasible){

        cout << " LB: " << nSARP.getObjValue() << endl;
        cout << " UB: " << nSARP.getBestObjValue() << endl;
        sStat->solprofit = nSARP.getObjValue();
        sStat->time = time;

        for (int k = 0; k < inst->K; k++){
            sStat->solvec.push_back(auxPairVec);
        }

        for (int i = 0; i < nodeVec.size(); i++){
            for(int j = 0; j < nodeVec.size(); j++){                
                if (nas->arcs[i][j] == true){
                    for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                        int k = nas->arcV[i][j][k1];
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
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

        for (int i = 0; i < nodeVec.size(); i++){
            if (nSARP.getValue(b[i]) > 0){
                sStat->solBegin.push_back(nSARP.getValue(b[i]));
            }
            else {
                sStat->solBegin.push_back(0);
            }
        }

        for (int i = 0; i < nodeVec.size(); i++){
            if (nSARP.getValue(w[i]) > 0.5){
                sStat->solLoad.push_back(nSARP.getValue(w[i]));
            }
            else {
                sStat->solLoad.push_back(0);
            }
        }

        printResults(inst, mdist, sStat, nodeVec);

	}
	if (problem->scen == "PC"){
    	nSARP.clearModel();
    	nSARP.end();
    }

	env.end();
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

void mipnodePass(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);


    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;


	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

    for (int i = 0; i < nodeVec.size(); i++){
        x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
        for(int j = 0; j < nodeVec.size(); ++j){
            if (nas->arcs[i][j] != true){
                continue; // If arc i to j is invalid
            } 

            x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
            for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                int k = nas->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
                x[i][j][k].setName(var);
                model.add(x[i][j][k]);
                // cout << "x: [" << i << "][" << j << "][" << k << "]" << endl;
            }
        }
    }

	// 	// Variable start of service time
	IloNumVarArray b(env, nodeVec.size(), 0, inst->T);
	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	IloBoolVarArray y(env, nodeVec.size()); 

	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "y(%d)", i);
		y[i].setName(var);
		model.add(y[i]);
	}

	IloExpr objFunction(env);

    objFunction += inst->totalCustomProfit;

    // for (int a = 0; a < nas->arcPP.size(); a++){
    //     int i = nas->arcPP[a].first;
    //     int j = nas->arcPP[a].second;
    //     for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
    //         int k = nas->arcV[i][j][k1];
    //         objFunction += nodeVec[i].profit * x[i][j][k];
    //     }
    // }

    for (int a = 0; a < nas->allArcs.size(); a++){
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction -= (double)inst->costkm*mdist[i][j] * x[i][j][k];
        }
    }

	model.add(IloMaximize(env, objFunction));


	// //Creating constraints

	// //Constraint 1 - All passenger nodes must be visited

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 2 - Parcel arcs are 0
	// //A parcel request can be denied

	for (int i = inst->n; i < inst->n + inst->m; i++){

		IloExpr exp(env);
		IloExpr exp2(env);

		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}

        exp2 += y[i];
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp - exp2 == 0);
		cons.setName(var);
		model.add(cons);
	}


	// //Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	for (int i = inst->n; i < inst->n + inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp1 += x[u][v][k];
			}
			//Right side: arc leaves i + m
			for (int a = 0; a < nas->vArcPlus[i + inst->m][k].size(); a++){
				int u = nas->vArcPlus[i + inst->m][k][a].first;
                int v = nas->vArcPlus[i + inst->m][k][a].second;

                exp2 += x[u][v][k];
			}
			sprintf (var, "Constraint3_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	// //Constraint 4 - Flow conservation

	for (int a = 0; a < inst->n; a++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int b = 0; b < nas->vArcPlus[a][k].size(); b++){
                int i = nas->vArcPlus[a][k][b].first;
                int j = nas->vArcPlus[a][k][b].second;
				exp1 += x[i][j][k];
			}
			//Right side: arc enters i
			for (int b = 0; b < nas->vArcMinus[a][k].size(); b++){
                int i = nas->vArcMinus[a][k][b].first;
                int j = nas->vArcMinus[a][k][b].second;
				exp2 += x[i][j][k];

			}
			sprintf (var, "Constraint4_%d_%d", a, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	// //Constraint 5 - The route of every vehicle has to start at its starting position


    for (int k = 0; k < inst->K; k++){
        IloExpr exp(env);
        for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
            int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
            int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

            exp += x[u][v][k];
        }
        sprintf (var, "Constraint5_%d", k);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// // Constraint 6 - The route of every vehicle has to end at dummy node f

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int a = 0; a < nas->vArcMinus[inst->V + k][k].size(); a++){
            int u = nas->vArcMinus[inst->V + k][k][a].first;
            int v = nas->vArcMinus[inst->V + k][k][a].second;

        	exp += x[u][v][k];
		}
		sprintf (var, "Constraint6_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 7 - tie service begining to node visit

    for (int i = 0; i < fDepot; i++){
        IloExpr exp(env);
        exp = b[i] - M * y[i]; 
        sprintf (var, "Constraint7_%d", i);
        IloRange cons = (exp <= 0);
        cons.setName(var);
        model.add(cons);
    }


	// //Constraint 8 - service of pickup must come before the delivery

	for (int i = inst->n; i < inst->n + inst->m; i++){
		IloExpr exp(env);
		exp = b[i] - b[i + inst->m];

		sprintf (var, "Constraint8_%d", i);
		IloRange cons = (exp <= 0);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraints 9 - TW 

	for (int a = 0; a < nas->allArcs.size(); a++){
		IloExpr exp(env);
		IloExpr sumX(env);
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            sumX += x[i][j][k];
		}
			exp = b[i] - b[j] + nodeVec[i].delta + (mdist[i][j]/inst->vmed) - M * (1 - sumX);
			sprintf (var, "Constraint9_%d_%d", i, j);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	// // //Constraints 10 - load constraints

	// for (int a = 0; a < nas->allArcs.size(); a++){
		
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	IloExpr sumX(env);
 //        int i = nas->allArcs[a].first;
 //        int j = nas->allArcs[a].second;

 //        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
 //            int k = nas->arcV[i][j][k1];
	// 		sumX += x[i][j][k];
	// 	}

	// 	exp = w[i] + nodeVec[j].load - W*(1 - sumX);
	// 	exp2 = w[j];
		
 //        sprintf (var, "Constraint10_%d_%d", i, j);
		
 //        IloRange cons1 = (exp2 - exp >= 0);
	// 	cons1.setName(var);
	// 	model.add(cons1);
	// }

	// //Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	for (int i = 0; i < nodeVec.size(); i++){
		IloExpr exp(env);
		exp = b[i];

		sprintf (var, "Constraint11_%d", i);
		IloRange cons1 = (exp <= nodeVec[i].l);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint12_%d", i);
		IloRange cons2 = (nodeVec[i].e <= exp);
		cons2.setName(var);
		model.add(cons2);			
	}

 //    //Constraints 13 - maximum driving time

    for (int i = fDepot; i < fDummy; i++){
        IloExpr exp(env);
        exp = b[i + inst->K] - b[i];

        sprintf (var, "Constraint13_%d", i);
        IloRange cons1 = (exp <= inst->maxTime);
        cons1.setName(var);
        model.add(cons1);        
    }
	
	for (int i = inst->n; i < inst->n + inst->m; i++){
		IloExpr exp(env);

		exp = y[i];

		sprintf (var, "Constraint10_%d", i);
		IloRange cons = (exp == 0);
		cons.setName(var);
		model.add(cons);	

	}
	// //Constraint 14  - bound number of passenger visits transporting parcel
	// if (problem->scen == "1A" || problem->scen == "1B"){
	// 	for (int i = 0; i < nas->arcNN.size(); i++){
	// 		IloExpr exp(env);
	// 		IloExpr sumX(env);
 //            int u = nas->arcNN[i].first;
 //            int v = nas->arcNN[i].second;

	// 		for (int k = 0; k < inst->K; k++){
	// 			sumX += x[u][v][k];
	// 		}
	// 		exp = 1 - (w[u]/W);

	// 		sprintf (var, "Constraint14_%d", i);
	// 		IloRange cons1 = (sumX - exp <= 0);
	// 		cons1.setName(var);
	// 		model.add(cons1);

	// 	}
	// }

 //    //*******************************

    int threads;

    threads = 1;

    // if (inst->instParam[0] < 14){
    //     if (inst->instParam[0] > 12 && inst->instParam[1] == 7){
    //         threads = 8;
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }

    // else if (inst->instParam[0] > 14) {
    //     threads = 8;
    // }

    // else{
    //     if (inst->instParam[1] > 1){
    //         threads = 8;              
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }
    cout << "\nThreads: " << threads << endl;
    // cout << "here" << endl;
    // getchar();

	IloCplex nSARP(model);
	nSARP.exportModel("nSARPc.lp");
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	cout << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    cout << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    cout << " Total Time: " << time << endl;

	if (sStat->feasible){

        cout << " LB: " << nSARP.getObjValue() << endl;
        cout << " UB: " << nSARP.getBestObjValue() << endl;
        sStat->solprofit = nSARP.getObjValue();
        sStat->time = time;

        for (int k = 0; k < inst->K; k++){
            sStat->solvec.push_back(auxPairVec);
        }

        for (int i = 0; i < nodeVec.size(); i++){
            for(int j = 0; j < nodeVec.size(); j++){                
                if (nas->arcs[i][j] == true){
                    for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                        int k = nas->arcV[i][j][k1];
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
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

        for (int i = 0; i < nodeVec.size(); i++){
            if (nSARP.getValue(b[i]) > 0){
                sStat->solBegin.push_back(nSARP.getValue(b[i]));
            }
            else {
                sStat->solBegin.push_back(0);
            }
        }

        // for (int i = 0; i < nodeVec.size(); i++){
        //     if (nSARP.getValue(w[i]) > 0.5){
        //         sStat->solLoad.push_back(nSARP.getValue(w[i]));
        //     }
        //     else {
        //         sStat->solLoad.push_back(0);
        //     }
        // }

        printResults(inst, mdist, sStat, nodeVec);

	}

	env.end();
}
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void mipnodeParc(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);

    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;

	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;
    inst->totalCustomProfit = 0;


	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

    for (int i = 0; i < nodeVec.size(); i++){
        x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
        for(int j = 0; j < nodeVec.size(); ++j){
            if (nas->arcs[i][j] != true){
                continue; // If arc i to j is invalid
            } 

            x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
            for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                int k = nas->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
                x[i][j][k].setName(var);
                model.add(x[i][j][k]);
                // cout << "x: [" << i << "][" << j << "][" << k << "]" << endl;
            }
        }
    }

	//creates boolean variable (y_i = 1 if node i is visited; 0 cc)
	IloBoolVarArray y(env, nodeVec.size()); 

	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "y(%d)", i);
		y[i].setName(var);
		model.add(y[i]);
	}

		// Variable start of service time
	IloNumVarArray b(env, nodeVec.size(), 0, inst->T);
	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	//Load variable
	// IloNumVarArray w(env, nodeVec.size(), 0, Q);

	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "w(%d)", i);
	// 	w[i].setName(var);
	// 	model.add(w[i]);
		
	// }

	IloExpr objFunction(env);

    for (int a = 0; a < nas->arcPP.size(); a++){
        int i = nas->arcPP[a].first;
        int j = nas->arcPP[a].second;
        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction += nodeVec[i].profit * x[i][j][k];
        }
    }

    for (int a = 0; a < nas->allArcs.size(); a++){
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction -= (double)inst->costkm*mdist[i][j] * x[i][j][k];
        }
    }

	model.add(IloMaximize(env, objFunction));


	//Creating constraints

	//Constraint 1 - No passenger nodes must be visited

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 2 - Relationship between visit variable and parcel node arcs
	//A parcel request can be denied

	for (int i = inst->n; i < inst->n + inst->m; i++){

		IloExpr exp(env);
		IloExpr exp2(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}

        exp2 += y[i];
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp - exp2 == 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	for (int i = inst->n; i < inst->n + inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp1 += x[u][v][k];
			}
			//Right side: arc leaves i + m
			for (int a = 0; a < nas->vArcPlus[i + inst->m][k].size(); a++){
				int u = nas->vArcPlus[i + inst->m][k][a].first;
                int v = nas->vArcPlus[i + inst->m][k][a].second;

                exp2 += x[u][v][k];
			}
			sprintf (var, "Constraint3_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 4 - Flow conservation

	for (int a = 0; a < inst->n + 2*inst->m; a++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int b = 0; b < nas->vArcPlus[a][k].size(); b++){
                int i = nas->vArcPlus[a][k][b].first;
                int j = nas->vArcPlus[a][k][b].second;
				exp1 += x[i][j][k];
			}
			//Right side: arc enters i
			for (int b = 0; b < nas->vArcMinus[a][k].size(); b++){
                int i = nas->vArcMinus[a][k][b].first;
                int j = nas->vArcMinus[a][k][b].second;
				exp2 += x[i][j][k];

			}
			sprintf (var, "Constraint4_%d_%d", a, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 5 - The route of every vehicle has to start at its starting position


    for (int k = 0; k < inst->K; k++){
        IloExpr exp(env);
        for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
            int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
            int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

            exp += x[u][v][k];
        }
        sprintf (var, "Constraint5_%d", k);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// Constraint 6 - The route of every vehicle has to end at dummy node f

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int a = 0; a < nas->vArcMinus[inst->V + k][k].size(); a++){
            int u = nas->vArcMinus[inst->V + k][k][a].first;
            int v = nas->vArcMinus[inst->V + k][k][a].second;

        	exp += x[u][v][k];
		}
		sprintf (var, "Constraint6_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 7 - tie service begining to node visit

    for (int i = 0; i < fDepot; i++){
        IloExpr exp(env);
        exp = b[i] - M * y[i]; 
        sprintf (var, "Constraint7_%d", i);
        IloRange cons = (exp <= 0);
        cons.setName(var);
        model.add(cons);
    }


	//Constraint 8 - service of pickup must come before the delivery

	for (int i = inst->n; i < inst->n + inst->m; i++){
		IloExpr exp(env);
		exp = b[i] - b[i + inst->m];

		sprintf (var, "Constraint8_%d", i);
		IloRange cons = (exp <= 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraints 9 - TW 

	for (int a = 0; a < nas->allArcs.size(); a++){
		IloExpr exp(env);
		IloExpr sumX(env);
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            sumX += x[i][j][k];
		}
			exp = b[i] - b[j] + nodeVec[i].delta + (mdist[i][j]/inst->vmed) - M * (1 - sumX);
			sprintf (var, "Constraint9_%d_%d", i, j);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	//Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	for (int i = 0; i < nodeVec.size(); i++){
		IloExpr exp(env);
		exp = b[i];

		sprintf (var, "Constraint11_%d", i);
		IloRange cons1 = (exp <= nodeVec[i].l);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint12_%d", i);
		IloRange cons2 = (nodeVec[i].e <= exp);
		cons2.setName(var);
		model.add(cons2);			
	}

    //Constraints 13 - maximum driving time

    for (int i = fDepot; i < fDummy; i++){
        IloExpr exp(env);
        exp = b[i + inst->K] - b[i];

        sprintf (var, "Constraint13_%d", i);
        IloRange cons1 = (exp <= inst->maxTime);
        cons1.setName(var);
        model.add(cons1);        
    }
	

 //    //*******************************

    int threads;

    threads = 1;

    // if (inst->instParam[0] < 14){
    //     if (inst->instParam[0] > 12 && inst->instParam[1] == 7){
    //         threads = 8;
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }

    // else if (inst->instParam[0] > 14) {
    //     threads = 8;
    // }

    // else{
    //     if (inst->instParam[1] > 1){
    //         threads = 8;              
    //     }
    //     else{
    //         threads = 1;
    //     }
    // }
    cout << "\nThreads: " << threads << endl;

	IloCplex nSARP(model);
	nSARP.exportModel("nSARPp.lp");
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	cout << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    cout << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    cout << " Total Time: " << time << endl;


	if (sStat->feasible){

        cout << " LB: " << nSARP.getObjValue() << endl;
        cout << " UB: " << nSARP.getBestObjValue() << endl;
        sStat->solprofit = nSARP.getObjValue();
        sStat->time = time;

        for (int k = 0; k < inst->K; k++){
            sStat->solvec.push_back(auxPairVec);
        }

        for (int i = 0; i < nodeVec.size(); i++){
            for(int j = 0; j < nodeVec.size(); j++){                
                if (nas->arcs[i][j] == true){
                    for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                        int k = nas->arcV[i][j][k1];
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
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

        for (int i = 0; i < nodeVec.size(); i++){
            if (nSARP.getValue(b[i]) > 0){
                sStat->solBegin.push_back(nSARP.getValue(b[i]));
            }
            else {
                sStat->solBegin.push_back(0);
            }
        }

        // for (int i = 0; i < nodeVec.size(); i++){
        //     if (nSARP.getValue(w[i]) > 0.5){
        //         sStat->solLoad.push_back(nSARP.getValue(w[i]));
        //     }
        //     else {
        //         sStat->solLoad.push_back(0);
        //     }
        // }

        printResults(inst, mdist, sStat, nodeVec);

	}

	env.end();
}

void printResults(instanceStat *inst, double **mdist, solStats *sStat, vector<nodeStat> &nodeVec){ //model node
        solStatIni(sStat);
        cout << "\nObj Val: " << setprecision(15) << sStat->solprofit << endl;

        cout << "\nSolve Time: " << setprecision(15) << sStat->time << endl;

        for (int k = 0; k < inst->K; k++){
            for (int a = 0; a < sStat->solvec[k].size(); a++){
                int i = sStat->solvec[k][a].first;
                int j = sStat->solvec[k][a].second;
                if (i >= inst->n){
                    sStat->pProfit += nodeVec[i].profit;
                }

                sStat->costs += (double)inst->costkm*mdist[i][j];
                cout << "x(" << i << ", " << j << ", " << k << ")" << endl;

            }
        }

        for (int i = 0; i < nodeVec.size(); i++){
            cout << "b(" << i << "): " << sStat->solBegin[i] << endl;
        }

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << "w(" << i << "): " << sStat->solLoad[i] << endl;
        // }

        cout << "\n\nCustomer profit: " << inst->totalCustomProfit << endl;
        cout << "Parcel profit: " << sStat->pProfit << endl;
        cout << "Costs: " << sStat->costs << endl;
        
        if (sStat->pProfit == 0){
        	inst->min = true;
        }
        else{
            inst->min = false;
        }
}