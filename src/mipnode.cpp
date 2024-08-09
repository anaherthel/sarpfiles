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
	//long M = 10*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);
	long W = inst->m + 1;
	int Q;

    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;
	
	int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	if (problem->p2 < 1){ //single parcel setting
		Q = 1;
	}
	else{
		Q = inst->m;
	}

	//// TODO UNCOMMENT //  << "Printing node vec: " << endl;
	//for (int i = 0; i < nodeVec.size(); i++){
	//	// TODO UNCOMMENT //  << i << ": " << nodeVec[i].e << " - " << nodeVec[i].l << endl;
	//}
	//// TODO UNCOMMENT //  << endl;
	//getchar();

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
                // // TODO UNCOMMENT //  << "x: [" << i << "][" << j << "][" << k << "]" << endl;
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
	IloNumVarArray b(env, nodeVec.size(), 9, inst->T);
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

	IloNumVarArray u(env, nodeVec.size(), 0, 1);

	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "u(%d)", i);
		u[i].setName(var);
		model.add(u[i]);
	}

	////if (problem->p1 < 1 && problem->dParcel > 0){
	//IloNumVarArray P(env, nodeVec.size(), 1, (2*inst->m + inst->n));

	//for (int i = 0; i < nodeVec.size() - 2*inst->K; i++){
	//	sprintf(var, "P(%d)", i);
	//	P[i].setName(var);
	//	model.add(P[i]);
		
	//}			
	////}

	IloExpr objFunction(env);

    objFunction += inst->totalCustomProfit;

    for (int a = 0; a < nas->arcPN.size(); a++){
        int i = nas->arcPN[a].first;
        int j = nas->arcPN[a].second;
        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction += nodeVec[i].profit * x[i][j][k];
        }
    }

	if (problem->dParcel > 0){
		
		for (int a = 0; a < nas->arcPD.size(); a++){
			int i = nas->arcPD[a].first;
			int j = nas->arcPD[a].second;
			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction += nodeVec[i].profit * x[i][j][k];
			}
    	}

	}
	if (problem->p2 > 0){
		for (int a = 0; a < nas->arcPP.size(); a++){
			int i = nas->arcPP[a].first;
			int j = nas->arcPP[a].second;
			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction += nodeVec[i].profit * x[i][j][k];
			}
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

	//Creating extra set of summation that minimizes the number of used vehicles
	// for (int k = 0; k < inst->K; k++){
	// 	int i = inst->V - inst->K + k;
	// 	for (int a = 0; a < nas->arcPlus[i].size(); a++){
    // 		int j = nas->arcPlus[i][a].second;

	// 		objFunction -= (double)10 * x[i][j][k];
	// 	}
	// }

	model.add(IloMaximize(env, objFunction));

	// for (int k = 0; k < inst->K; k++){
    //     IloExpr exp(env);
    //     for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
    //         int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
    //         int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

    //         exp += x[u][v][k];
    //     }
    //     sprintf (var, "Constraint5_%d", k);
    //     IloRange cons = (exp <= 1);
    //     cons.setName(var);
    //     model.add(cons);
    // }

	//Creating constraints

	//Constraint 1 - All passenger nodes must be visited

	getchar();

	for (int i = 0; i < inst->n + 2*inst->m; i++){
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

	//Constraint 5 - The route of every used vehicle has to start at its starting position


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

	// Constraint 6 - The route of every used vehicle has to end at dummy node f

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
        IloRange cons = (exp <= 9);
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
			double cvalue = mdist[i][j]/inst->vmed;
			//cvalue = std::round(cvalue * multiplier) / multiplier;
			//cvalue = timeRound(cvalue);
			exp = b[i] - b[j] + nodeVec[i].delta + (cvalue) - M * (1 - sumX);
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
	if (problem->p1 < 1){
		// // TODO UNCOMMENT //  << "Constraint 14" << endl;
		// getchar();
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
	//make new constraints for route sequence


	if (problem->p1 < 1 && problem->dParcel > 0){
		
		//for (int i = inst->n + 2*inst->m; i < nodeVec.size(); i++){
		//	IloExpr exp(env);
		//	exp = u[i];

		//	sprintf (var, "Constraint15_%d", i);
			
		//	IloRange cons1 = (exp == 0);
		//	cons1.setName(var);
		//	model.add(cons1);

		//}

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

			exp = u[i] + nodeVec[j].load2 - W*(1 - sumX);
			exp2 = u[j];
			
			sprintf (var, "Constraint15_%d_%d", i, j);
			
			IloRange cons1 = (exp2 - exp >= 0);
			cons1.setName(var);
			model.add(cons1);
		}		

		// // TODO UNCOMMENT //  << "Constraint 15" << endl;
		// getchar();
		//for (int a = 0; a < nas->allArcs.size(); a++){
			
		//	IloExpr exp(env);
		//	IloExpr exp2(env);
		//	IloExpr exp3(env);
		//	IloExpr sumX(env);
			
		//	int i = nas->allArcs[a].first;
		//	int j = nas->allArcs[a].second;

		//	if (i >= inst->n + 2*inst->m || j >= inst->n + 2*inst->m){
		//		continue;
		//	}

		//	for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
		//		int k = nas->arcV[i][j][k1];
		//		sumX += x[i][j][k];
		//	}

		//	exp = M2*(sumX - 1) + P[j] - 1;
		//	exp2 = P[i];
		//	exp3 = M2*(1 - sumX) + P[j] - 1;
			
		//	sprintf (var, "Constraint15_%d_%d", i, j);
			
		//	IloRange cons1 = (exp - exp2 <= 0);
		//	cons1.setName(var);
		//	model.add(cons1);

		//	sprintf (var, "Constraint16_%d_%d", i, j);
			
		//	IloRange cons2 = (exp3 - exp2 >= 0);
		//	cons2.setName(var);
		//	model.add(cons2);			
		//}


		//for every pair of depot-customer


	}



	//end of new constraints

	////test constraints
	////// TODO UNCOMMENT //  << "here" << endl;
	//IloExpr exp(env);
	//exp = x[19][11][0];

	//sprintf (var, "Constraint15");

	//IloRange cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);
	////// TODO UNCOMMENT //  << "A" << endl;

	//exp = x[11][4][0];

	//sprintf (var, "Constraint16");

	////// TODO UNCOMMENT //  << "B" << endl;
	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[4][18][0];

	////// TODO UNCOMMENT //  << "C" << endl;
	//sprintf (var, "Constraint17");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	////// TODO UNCOMMENT //  << "after" << endl;
	//exp = x[18][10][0];

	//sprintf (var, "Constraint18");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[10][3][0];

	//sprintf (var, "Constraint19");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[3][17][0];

	//sprintf (var, "Constraint20");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[17][1][0];

	//sprintf (var, "Constraint21");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[1][0][0];

	//sprintf (var, "Constraint22");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[0][6][0];

	//sprintf (var, "Constraint23");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[6][2][0];

	//sprintf (var, "Constraint24");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[2][13][0];

	//sprintf (var, "Constraint25");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);





	//exp = x[12][2][1];

	//sprintf (var, "Constraint26");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);
		
	//exp = x[2][18][1];

	//sprintf (var, "Constraint27");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[18][0][1];

	//sprintf (var, "Constraint28");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[0][3][1];

	//sprintf (var, "Constraint29");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[3][4][1];

	//sprintf (var, "Constraint30");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[4][16][1];

	//sprintf (var, "Constraint31");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[16][5][1];

	//sprintf (var, "Constraint32");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);


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
    // TODO UNCOMMENT //  << "\nThreads: " << threads << endl;

	IloCplex nSARP(model);
	nSARP.exportModel("nSARP.lp");
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	// TODO UNCOMMENT //  << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    // TODO UNCOMMENT //  << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    // TODO UNCOMMENT //  << " Total Time: " << time << endl;

	if (sStat->feasible){

        // TODO UNCOMMENT //  << " LB: " << nSARP.getObjValue() << endl;
        // TODO UNCOMMENT //  << " UB: " << nSARP.getBestObjValue() << endl;
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
                            // // TODO UNCOMMENT //  << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
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


        for (int i = 0; i < nodeVec.size(); i++){
            //if (nSARP.getValue(u[i])){
                sStat->solLoad2.push_back(nSARP.getValue(u[i]));
            //}
            //else {
            //    sStat->solLoad2.push_back(0);
            //}
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

void printResults(instanceStat *inst, double **mdist, solStats *sStat, vector<nodeStat> &nodeVec){ //model node
        solStatIni(sStat);
        // TODO UNCOMMENT //  << "\nObj Val: " << setprecision(15) << sStat->solprofit << endl;

        // TODO UNCOMMENT //  << "\nSolve Time: " << setprecision(15) << sStat->time << endl;

        for (int k = 0; k < inst->K; k++){
            for (int a = 0; a < sStat->solvec[k].size(); a++){
                int i = sStat->solvec[k][a].first;
                int j = sStat->solvec[k][a].second;
                if (i >= inst->n){
                    sStat->pProfit += nodeVec[i].profit;
                }

                sStat->costs += (double)inst->costkm*mdist[i][j];
                // TODO UNCOMMENT //  << "x(" << i << ", " << j << ", " << k << ")" << endl;

            }
        }

        for (int i = 0; i < nodeVec.size(); i++){
            // TODO UNCOMMENT //  << "b(" << i << "): " << sStat->solBegin[i] << endl;
        }

        //for (int i = 0; i < nodeVec.size(); i++){
        //    // TODO UNCOMMENT //  << "u(" << i << "): " << sStat->solLoad2[i] << endl;
        //}		

        // for (int i = 0; i < nodeVec.size(); i++){
        //     // TODO UNCOMMENT //  << "w(" << i << "): " << sStat->solLoad[i] << endl;
        // }

        // TODO UNCOMMENT //  << "\n\nCustomer profit: " << inst->totalCustomProfit << endl;
        // TODO UNCOMMENT //  << "Parcel profit: " << sStat->pProfit << endl;
        // TODO UNCOMMENT //  << "Costs: " << sStat->costs << endl;
		// TODO UNCOMMENT //  << "Without fixed: " << sStat->costs - sStat->pProfit << endl;
        
        if (sStat->pProfit == 0){
        	inst->min = true;
        }
        else{
            inst->min = false;
        }
}

void fippass(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){

	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP1");
	int currSP;
	// long M = 2*inst->T;
	// long M2 = 2*(inst->n + inst->m + 1);
	// long W = inst->m + 1;
	// int Q;

    int fDepot = 2*inst->n + 2*inst->m;
    int fDummy = 2*inst->n + 2*inst->m + inst->K;
    int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	// // TODO UNCOMMENT //  << "service: " << inst->service << endl;

	//Creating variables only for pass PU and DL
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

    for (int i = 0; i < nodeVec.size(); i++){
        x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
        for(int j = 0; j <  nodeVec.size(); ++j){
            if (nas->arcs[i][j] != true){
				//// TODO UNCOMMENT // << i << "-" << j << ": invalid";
                continue; // If arc i to j is invalid
            } 
            x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
			//// TODO UNCOMMENT //  << "nas arc v size: " << nas->arcV[i][j].size() << endl;
            for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                int k = nas->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
                x[i][j][k].setName(var);
                model.add(x[i][j][k]);
                //// TODO UNCOMMENT //  << "x: [" << i << "][" << j << "][" << k << "]" << endl;
            }
        }
    }

	// // Variable start of service time
	// IloArray <IloNumVarArray> b(env, nodeVec.size());
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	b[i] = IloNumVarArray (env, inst->K, 0, inst->T);
	// 	for (int k = 0; k < inst->K; k++){
	// 		sprintf(var, "b(%d,%d)", i, k);
	// 		b[i][k].setName(var);
	// 		model.add(b[i][k]);
	// 		// // TODO UNCOMMENT //  << "b: [" << i << "][" << k << "]" << endl;
	// 	}
	// }

	IloNumVarArray b(env, nodeVec.size(), 9, inst->T);
	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}
	
	IloExpr objFunction(env);

	for (int i = 0; i < inst->n; i++){
		for (int j = 0; j < nodeVec.size(); j++){
			if (nas->arcs[i][j] != true){
				continue; // If arc i to j is invalid
			}
			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction += (inst->minpas + (inst->paskm*(mdist[i][i+inst->n]))) * x[i][j][k];
			}
		}
    }

	for (int i = 0; i < nodeVec.size(); i++){
		for (int j = 0; j < nodeVec.size(); j++){
			for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction -= (double)inst->costkm*mdist[i][j] * x[i][j][k];
        	}
		}
	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - All passenger pu nodes go directly to the deliveries

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		int j = i + inst->n;
		for (int k = 0; k < inst->K; k++){
			exp += x[i][j][k];
		}

		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}


	//Set all parcel variables to 0;

	for (int i = 2*inst->n; i < 2*inst->n + 2*inst->m; i++){
		IloExpr exp(env);
		for (int j = 0; j < nas->arcPlus[i].size();j++){
			int u = nas->arcPlus[i][j].first;
			int v = nas->arcPlus[i][j].second;

			for(int k1 = 0; k1 < nas->arcV[u][v].size(); k1++){
				int k = nas->arcV[u][v][k1];
				exp += x[u][v][k];
			}
		}
		for (int j = 0; j < nas->arcMinus[i].size();j++){
			int u = nas->arcMinus[i][j].first;
			int v = nas->arcMinus[i][j].second;

			for(int k1 = 0; k1 < nas->arcV[u][v].size(); k1++){
				int k = nas->arcV[u][v][k1];
				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint2_%d", i);
		IloRange cons = (exp == 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 4 - Flow conservation

	for (int a = 0; a < 2*inst->n + 2*inst->m; a++){
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
			sprintf (var, "Constraint3_%d_%d", a, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 5 - The route of every used vehicle has to start at its starting position


    for (int k = 0; k < inst->K; k++){
        IloExpr exp(env);
        for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
            int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
            int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

            exp += x[u][v][k];
        }
        sprintf (var, "Constraint4_%d", k);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// Constraint 6 - The route of every used vehicle has to end at dummy node f

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int a = 0; a < nas->vArcMinus[inst->V + k][k].size(); a++){
            int u = nas->vArcMinus[inst->V + k][k][a].first;
            int v = nas->vArcMinus[inst->V + k][k][a].second;

        	exp += x[u][v][k];
		}
		sprintf (var, "Constraint5_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraints 9 - TW bj >= (bi + tij)xijk



	// for (int a = 0; a < nas->allArcs.size(); a++){
	// 	IloExpr exp(env);
    //     int i = nas->allArcs[a].first;
    //     int j = nas->allArcs[a].second;
	// 	// double M = max(double(0),nodeVec[i].l + (mdist[i][j]/inst->vmed)-nodeVec[j].e);
	// 	double M = nodeVec[j].l;
    //     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
    //         int k = nas->arcV[i][j][k1];

	// 		exp =  b[j][k] - b[i][k] - (inst->service) - (mdist[i][j]/inst->vmed) - M * (x[i][j][k] - 1);
	// 		// exp =  b[j][k] - b[i][k] - (mdist[i][j]/inst->vmed) - M * (x[i][j][k] - 1);
	// 		sprintf (var, "Constraint7_%d_%d_%d", i, j, k);
	// 		IloRange cons = (exp >= 0);
	// 		cons.setName(var);
	// 		model.add(cons);			
	// 	}
	// }

	for (int a = 0; a < nas->allArcs.size(); a++){
		IloExpr exp(env);
		IloExpr sumX(env);
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;
		double M = inst->T;
        for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            sumX += x[i][j][k];
		}
			double cvalue = mdist[i][j]/inst->vmed;
			//cvalue = std::round(cvalue * multiplier) / multiplier;		
			exp = b[i] - b[j] + nodeVec[i].delta + (cvalue) - M * (1 - sumX);
			sprintf (var, "Constraint6_%d_%d", i, j);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	//Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	// for (int i = 0; i < nodeVec.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp(env);
	// 		exp = b[i][k];

	// 		sprintf (var, "Constraint8_%d_%d", i, k);
	// 		IloRange cons1 = (exp <= nodeVec[i].l);
	// 		cons1.setName(var);
	// 		model.add(cons1);
			
	// 		sprintf (var, "Constraint9_%d_%d", i, k);
	// 		IloRange cons2 = (nodeVec[i].e <= exp);
	// 		cons2.setName(var);
	// 		model.add(cons2);
	// 	}
			
	// }


	for (int i = 0; i < nodeVec.size(); i++){
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

    //Constraints 13 - maximum driving time

    for (int i = fDepot; i < fDummy; i++){
        IloExpr exp(env);
        exp = b[i + inst->K] - b[i];

        sprintf (var, "Constraint9_%d", i);
        IloRange cons1 = (exp <= inst->maxTime);
        cons1.setName(var);
        model.add(cons1);        
    }

	//Constraint 11 - No passenger repeated

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
                int u = nas->vArcPlus[i][k][a].first;
                int v = nas->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint10_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}


	
	//test constraints

	// IloExpr exp(env);
	// exp = x[28][1][0];

	// sprintf (var, "Constraint14");

	// IloRange cons = (exp == 1);
	// cons.setName(var);
	// model.add(cons);

	// exp = x[1][6][0];

	// sprintf (var, "Constraint14A");

	// cons = (exp == 1);
	// cons.setName(var);
	// model.add(cons);

	
	int threads;

    threads = 1;

    // TODO UNCOMMENT //  << "\nThreads: " << threads << endl;

	IloCplex nSARP1(model);
	nSARP1.setOut(env.getNullStream());
	nSARP1.exportModel("fipSARP1.lp");
	nSARP1.setParam(IloCplex::Threads, threads);
	nSARP1.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP1.getTime();
	nSARP1.solve();
    time = (nSARP1.getTime() - start)/threads;
	// TODO UNCOMMENT //  << "\nSol status: " << nSARP1.getStatus() << endl;
	sStat->feasible = nSARP1.isPrimalFeasible();

    // // TODO UNCOMMENT //  << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    // TODO UNCOMMENT //  << " Total Time: " << time << endl;

	if (sStat->feasible){

        // TODO UNCOMMENT //  << " LB: " << nSARP1.getObjValue() << endl;
        // TODO UNCOMMENT //  << " UB: " << nSARP1.getBestObjValue() << endl;
        sStat->solprofit = nSARP1.getObjValue();
		sStat->solDual = nSARP1.getBestObjValue();
        sStat->time = time;

        for (int k = 0; k < inst->K; k++){
            sStat->solvec.push_back(auxPairVec);
        }

        for (int i = 0; i < nodeVec.size(); i++){
            for(int j = 0; j < nodeVec.size(); j++){                
                if (nas->arcs[i][j] == true){
                    for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
                        int k = nas->arcV[i][j][k1];
                        if (nSARP1.getValue(x[i][j][k]) > 0.5){
                            auxPair.first = i;
                            auxPair.second = j;
                            sStat->solvec[k].push_back(auxPair);
                            // // TODO UNCOMMENT //  << i << " " << j << " " << k << ": " << nSARP1.getValue(x[i][j][k]) << endl;
                            // getchar();
                        }
                    }
                }
            }   
        }

        // for (int i = 0; i < nodeVec.size(); i++){
        //     for(int j = 0; j < nodeVec.size(); j++){                
        //         if (nas->arcs[i][j] == true){
        //             for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
        //                 int k = nas->arcV[i][j][k1];
        //                 if (nSARP1.getValue(x[i][j][k]) > 0.5){
		// 					sStat->solBegin.push_back(nSARP1.getValue(b[i]));
		// 					// // TODO UNCOMMENT //  << "b(" << i << ", " << k << "): " << nSARP1.getValue(b[i][k]);
		// 					// getchar();
        //                     // // TODO UNCOMMENT //  << i << " " << j << " " << k << ": " << nSARP1.getValue(x[i][j][k]) << endl;
        //                     // getchar();
        //                 }
        //             }
        //         }
        //     }   
        // }

		for (int i = 0; i < nodeVec.size(); i++){
            if (nSARP1.getValue(b[i]) > 0){
            	sStat->solBegin.push_back(nSARP1.getValue(b[i]));
            }
            else {
                sStat->solBegin.push_back(0);
            }
        }
		// TODO UNCOMMENT //  << "Before print: " << endl;
        printResults(inst, mdist, sStat, nodeVec);
		// TODO UNCOMMENT //  << "After print: " << endl;

	}
	env.end();

}

void fipmip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat,  fipStats *fipStat){

	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	// long M = 2*inst->T;
	// long M2 = 2*(inst->n + inst->m + 1);
	// long W = inst->m + 1;
	// int Q = 5;
	//int Q = 4; //q4 fip (only 1 parcel between passengers)
	int Q = inst->m + 3; //qm fip (multiple parcels between passengers)

    int fDepot = 2*inst->n + 2*inst->m;
    int fDummy = 2*inst->n + 2*inst->m + inst->K;
	int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);
	
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;



	//Creating variable (i in set of assigned passengers)
	IloArray <IloArray <IloBoolVarArray> > x(env, 2*inst->n+2*inst->m+inst->K);

    for (int i = 0; i < 2*inst->n+2*inst->m+inst->K; i++){
        x[i] = IloArray <IloBoolVarArray> (env, 2*inst->n+2*inst->m+inst->K);
        for(int j = 0; j < 2*inst->n+2*inst->m; ++j){
            if (nas->arcs[i][j] != true){
                continue; // If arc i to j is invalid
            } 
            x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
			if(i < 2*inst->n+2*inst->m){
				for(int k = 0; k < inst->K; k++){
					sprintf(var, "x(%d,%d,%d)", i, j, k);
					x[i][j][k].setName(var);
					model.add(x[i][j][k]);
					// // TODO UNCOMMENT //  << "x: [" << i << "][" << j << "][" << k << "]" << endl;
				}
			}
			else{
				int k = i - fDepot;
				sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}

        }
    }

	IloArray <IloNumVarArray> b(env, inst->K);
	// // TODO UNCOMMENT //  << "Size of solpass: " << fipStat->solPass.size() << endl;
	for (int k = 0; k < fipStat->solPass.size(); k++){
		// // TODO UNCOMMENT //  << "Size of solpass K: " << fipStat->solPass[k].size() << endl;
		b[k] = IloNumVarArray (env, 2*inst->n + 2*inst->m + 2*inst->K, 0, inst->T);
		for (int i = 0; i < fipStat->solPass[k].size(); i++){
			int u = fipStat->solPass[k][i];
			sprintf(var, "b(%d,%d)", k, u);
			b[k][u].setName(var);
			model.add(b[k][u]);
		}
	}


	// Variable start of service time for parcels
	IloNumVarArray s(env, 2*inst->n+2*inst->m, 0, inst->T);
	for (int i = 2*inst->n; i < 2*inst->n+2*inst->m; i++){
		sprintf(var, "s(%d)", i);
		s[i].setName(var);
		model.add(s[i]);
	}

	IloExpr objFunction(env);

	// objFunction += inst->totalCustomProfit;

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			int u = fipStat->solPass[k][i];
			int v = fipStat->solPass[k][i + 1];
			for(int j = 2*inst->n; j < 2*inst->n+2*inst->m; j++){
				if (nas->arcs[u][j] != true){
					continue; // If arc i to j is invalid
				}
				double deltaij = (mdist[u][j]) + (mdist[j][v]) - (mdist[u][v]);

				double profitj = nodeVec[j].profit;
				// objFunction += (inst->minpar + (inst->parkm*mdist[j][j+inst->m]) - (inst->costkm*deltaij)) * x[u][j][k];

				objFunction += (profitj - (inst->costkm*deltaij)) * x[u][j][k];

			}
		}
    }

	//Detour component of OF
	// IloExpr expnew(env);

	// for (int k = 0; k < fipStat->solPassOrigins.size(); k++){
	// 	if (fipStat->solPassOrigins[k].size() < 1){
	// 		continue;
	// 	}
	// 	for (int i = 0; i < fipStat->solPassOrigins[k].size(); i++){

	// 		int u = fipStat->solPassOrigins[k][i].first;
	// 		// int v = fipStat->solPass[k][fipStat->solPassOrigins[k][i].second + 1];

	// 		expnew += ((b[k][u+inst->n] - b[k][u] - inst->service)/((mdist[u][u+inst->n]/inst->vmed))) - 1;
	// 	}
	// }

	// objFunction -= inst->discpas*expnew;

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one vehicle serves a certain parcel (23)

	for(int j = 2*inst->n; j < 2*inst->n+2*inst->m; j++){
		IloExpr exp(env);
		for (int k = 0; k < fipStat->solPass.size(); k++){
			if (fipStat->solPass[k].size() < 3){
				continue;
			}
			for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
				int u = fipStat->solPass[k][i];
				exp += x[u][j][k];
			}
		}
		sprintf (var, "Constraint1_%d", j);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}

	//Constraint 2 - At most 1 parcel between passenger nodes (or starting and passenger) (24)
	//remove it for fip qm

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}

		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			IloExpr exp(env);
			int u = fipStat->solPass[k][i];
			for(int j = 2*inst->n; j < 2*inst->n+2*inst->m; j++){
				exp += x[u][j][k];
			}

			sprintf (var, "Constraint2_%d_%d", k, u);
			IloRange cons = (exp <= 1);
			cons.setName(var);
			model.add(cons);
		}
	}

	// Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle (25)

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for(int j = 2*inst->n; j < 2*inst->n+inst->m; j++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
				int u = fipStat->solPass[k][i];
				exp1 +=  x[u][j][k];
				exp2 +=  x[u][j + inst->m][k];
			}
			sprintf (var, "Constraint3_%d_%d", j, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);			
		}
	}

	//Constraint 4 - Parcel origin precedes its destination (26)

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for(int j = 2*inst->n; j < 2*inst->n+inst->m; j++){
			for (int i = 1; i < fipStat->solPass[k].size(); i++){
				int u = fipStat->solPass[k][i];
				IloExpr exp1(env);
				IloExpr exp2(env);

				for (int f = 0; f < i; f++){
					int v = fipStat->solPass[k][f];
					exp1 += x[v][j][k];
					exp2 += x[v][j+inst->m][k];
				}

				sprintf (var, "Constraint4_%d_%d_%d", k, j, u);
				IloRange cons = ((exp1-exp2) >= 0);
				cons.setName(var);
				model.add(cons);	
			}
		}
	}

	// Constraint 5 - TW constraint (27)

    for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
        for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			int u = fipStat->solPass[k][i];
			int v = fipStat->solPass[k][i + 1];
			IloExpr exp1(env);
			IloExpr exp2(env);

			// exp1 = b[k][v] - b[k][u] - (mdist[u][v]/inst->vmed);
			double cvalue = mdist[u][v]/inst->vmed;
			//cvalue = std::round(cvalue * multiplier) / multiplier;			
			exp1 = b[k][v] - (b[k][u]) - (inst->service)*(i != 0) - (cvalue);

			for(int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){
				double deltaij = (mdist[u][j]) + (mdist[j][v]) - (mdist[u][v]);
				double cvalue2 = deltaij/inst->vmed;
				//cvalue2 = std::round(cvalue2 * multiplier) / multiplier;	
				double deltatime = (cvalue2) + inst->service*(i != 0);
				// double deltatime = (deltaij/inst->vmed);

				exp2 += deltatime*x[u][j][k];
			}
            
			sprintf (var, "Constraint5_%d_%d", k, u);
			IloRange cons = (exp1 - exp2 >= 0);
			cons.setName(var);
			model.add(cons);
        }
    }

	// Constraint 6 - TW constraint (28)

    for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
        for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			int u = fipStat->solPass[k][i];

			for(int j = 2*inst->n; j < 2*inst->n+2*inst->m; j++){
				IloExpr exp1(env);
				IloExpr exp2(env);

				double cvalue2 = mdist[u][j]/inst->vmed;
				//cvalue2 = std::round(cvalue2 * multiplier) / multiplier;	
				exp1 = s[j] - (b[k][u] + (inst->service)*(i != 0)) - (cvalue2)*x[u][j][k];
				// exp1 = s[j] - b[k][u] - (mdist[u][j]/inst->vmed)*x[u][j][k];

				double bigM = nodeVec[u].l;

				exp2 = bigM*(x[u][j][k] - 1);

				sprintf (var, "Constraint6_%d_%d_%d", k, u, j);
				IloRange cons = (exp1 - exp2 >= 0);
				cons.setName(var);
				model.add(cons);

			}
        }
    }

	// Constraint 7 - Maximum driving time (29)

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		IloExpr exp(env);
		int u = fipStat->solPass[k][0];
		int v = fipStat->solPass[k][fipStat->solPass[k].size()-1];

		exp = b[k][v] - b[k][u];
		// exp = b[k][v] - b[k][u];

		sprintf (var, "Constraint7_%d", k);
		IloRange cons = (exp <= inst->maxTime);
		cons.setName(var);
		model.add(cons);
	}

	// Constraint 8 - Time windows (30)


	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for (int i = 0; i < fipStat->solPass[k].size(); i++){
			IloExpr exp(env);
			
			int u = fipStat->solPass[k][i];

			exp = b[k][u];

			sprintf (var, "Constraint8_%d_%d", u, k);
			IloRange cons1 = (exp <= nodeVec[u].l);
			cons1.setName(var);
			model.add(cons1);
			
			sprintf (var, "Constraint9_%d_%d", u, k);
			IloRange cons2 = (nodeVec[u].e <= exp);
			cons2.setName(var);
			model.add(cons2);
		}
			
	}

	//parcels TW (31)
	for (int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){
		IloExpr exp(env);

		exp = s[j];

		sprintf (var, "Constraint10_%d", j);
		IloRange cons1 = (exp <= nodeVec[j].l);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint11_%d", j);
		IloRange cons2 = (nodeVec[j].e <= exp);
		cons2.setName(var);
		model.add(cons2);
	}

	// //Passengers cannot exceed max service time (2*ti,i+sigma) (32)
	// // for (int k = 0; k < fipStat->solPassOrigins.size(); k++){
	// // 	if (fipStat->solPassOrigins[k].size() < 1){
	// // 		continue;
	// // 	}
	// // 	for (int i = 0 ; i < fipStat->solPassOrigins[k].size(); i++){
	// // 		IloExpr exp(env);
			
	// // 		int u = fipStat->solPassOrigins[k][i].first;
	// // 		int posu = fipStat->solPassOrigins[k][i].second;
	// // 		int v = fipStat->solPass[k][posu + 1];

	// // 		exp = b[k][v] - b[k][u] + inst->service;

	// // 		sprintf (var, "Constraint12_%d_%d", k, u);
	// // 		IloRange cons = (exp <= 2*(mdist[u][u+inst->n]/inst->vmed)+2*inst->service);
	// // 		cons.setName(var);
	// // 		model.add(cons);
	// // 	}
	// // }

	//Load constraints (33) Maybe remove it

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
        for (int i = 1; i < fipStat->solPass[k].size(); i++){//maybe start from i = 0
			IloExpr exp1(env);
			IloExpr exp2(env);

			int u = fipStat->solPass[k][i];

			for (int f = 0; f < i; f++){ 
				int v = fipStat->solPass[k][f];
				exp2 += nodeVec[v].load;

				for (int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){
					
					exp1 += nodeVec[j].load*x[v][j][k];
				}

	
			}
			sprintf (var, "Constraint12_%d_%d", k, u);
			IloRange cons = ((exp1+exp2) <= Q);
			cons.setName(var);
			model.add(cons);
		}
	}
	//test constraints

	//IloExpr exp(env);
	//exp = x[22][8][0];

	//sprintf (var, "Constraint14");

	//IloRange cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[8][1][0];

	//sprintf (var, "Constraint15");

	//IloRange cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	int threads;

    threads = 1;

    // TODO UNCOMMENT //  << "\nThreads: " << threads << endl;

	IloCplex nSARP(model);
	nSARP.setOut(env.getNullStream());
	nSARP.exportModel("fipSARP.lp");
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);
	// nSARP.setParam(IloCplex::Param::Preprocessing::Aggregator, 0);
	// nSARP.setParam(IloCplex::Param::MIP::Display, 5);
	// nSARP.setParam(IloCplex::Param::Advance, 2);

	//MIP Start
	// IloNumVarArray startVar(env);
    // IloNumArray startVal(env);
	// for (int k = 0; k < fipStat->solPass.size(); k++){
	// 	for (int i = 0; i < fipStat->solPass[k].size()-1; i++){
	// 		int u = fipStat->solPass[k][i];
	// 		int v = fipStat->solPass[k][i + 1];

	// 		startVar.add(x[u][v][k]);
	// 		startVal.add(1);
	// 	}
	// }
	// nSARP.addMIPStart(startVar, startVal);


	// IloNumVarArray startVar2(env);
    // IloNumArray startVal2(env);
	// for (int k = 0; k < fipStat->solPass.size(); k++){
	// 	for (int i = 0; i < fipStat->solPass[k].size()-1; i++){
	// 		int u = fipStat->solPass[k][i];

	// 		startVar2.add(b[k][u]);
	// 		startVal2.add(sStat->solBegin[u]);
	// 	}
	// }
	// nSARP.addMIPStart(startVar2, startVal2);
	// end of MIP start

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	// TODO UNCOMMENT //  << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    // // TODO UNCOMMENT //  << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    // TODO UNCOMMENT //  << " Total Time: " << time << endl;


	if (sStat->feasible){

        // TODO UNCOMMENT //  << " LB: " << nSARP.getObjValue() << endl;
        // TODO UNCOMMENT //  << " UB: " << nSARP.getBestObjValue() << endl;
        sStat->solprofit = nSARP.getObjValue();
		sStat->solDual = nSARP.getBestObjValue();
		// // TODO UNCOMMENT //  << "Solution value pii: " << fipStat->solprofit << end
        sStat->time = time;

		if (((nSARP.getBestObjValue() - nSARP.getObjValue())/nSARP.getBestObjValue()) * 100 < 0.01) {
			sStat->status = "Optimal";
		} else {
			sStat->status = "Feasible";
		}

		//new addition
		fipStat->solBegin.clear();

		for (int i = 0; i < 2*inst->n+2*inst->m+inst->K; i++){
			fipStat->solBegin.push_back(-1);
		}
		//end

        for (int k = 0; k < inst->K; k++){
			fipStat->solvec.push_back(auxPairVec);
            // sStat->solvec.push_back(auxPairVec);
        }
		
        for (int i = 0; i < 2*inst->n+2*inst->m; i++){
            for(int j = 0; j < 2*inst->n+2*inst->m; j++){                
                if (nas->arcs[i][j] == true){
                    for (int k = 0; k < inst->K; k++){
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
                            auxPair.first = i;
                            auxPair.second = j;
                            fipStat->solvec[k].push_back(auxPair);
                            // cout  << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
                            // getchar();
                        }
                    }
                }
            }   
        }

		for (int k = 0; k < inst->K; k++){
			int i = fDepot+k;
            for(int j = 0; j < 2*inst->n+2*inst->m; j++){                
                if (nas->arcs[i][j] == true){
					if (nSARP.getValue(x[i][j][k]) > 0.5){
						auxPair.first = i;
						auxPair.second = j;
						fipStat->solvec[k].push_back(auxPair);
						// cout  << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
						// getchar();
					}
                }
            }   
        }

        for (int i = 0; i < 2*inst->n; i++){
			// // TODO UNCOMMENT //  << "i: " << endl;
            for(int j = 0; j < 2*inst->n+2*inst->m; j++){
				// // TODO UNCOMMENT //  << "j: " << j << endl;             
                if (nas->arcs[i][j] == true){
                    for (int k = 0; k < inst->K; k++){
						// // TODO UNCOMMENT //  << "k: " << k << endl;
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
							// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
							fipStat->solBegin[i] = nSARP.getValue(b[k][i]);
							// TODO UNCOMMENT //  << "\nb(" << k << ", " << i << "): " << nSARP.getValue(b[k][i]);
							// getchar();
                            // cout  << " - " << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
                            // getchar();
                        }
                    }
                }
            }   
        }

		for (int k = 0; k < inst->K; k++){
			int i = fDepot+k;
            for(int j = 0; j < 2*inst->n+2*inst->m; j++){                
                if (nas->arcs[i][j] == true){
					if (nSARP.getValue(x[i][j][k]) > 0.5){
						// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
						fipStat->solBegin[i] = nSARP.getValue(b[k][i]);
						// TODO UNCOMMENT //  << "\nb(" << k << ", " << i << "): " << nSARP.getValue(b[k][i]);
						// getchar();
						// cout << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
						// getchar();
					}
                }
            }   
        }

		// for (int k = 0; k < inst->K; k++){
		// 	for (int i = 0; i < 2*inst->n; i++){
		// 		// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
		// 		// TODO UNCOMMENT //  << "\nb(" << k << ", " << i << "): " << nSARP.getValue(b[k][i]);
		// 		// getchar();
		// 		// // TODO UNCOMMENT //  << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
		// 		// getchar();
		// 	}
		// }


		for (int j = 2*inst->n; j < 2*inst->n+2*inst->m; j++){
			// // TODO UNCOMMENT //  << "i: " << endl;
            for(int i = 0; i < 2*inst->n; i++){
				// // TODO UNCOMMENT //  << "j: " << j << endl;
                if (nas->arcs[i][j] == true){
                    for (int k = 0; k < inst->K; k++){
						// // TODO UNCOMMENT //  << "k: " << k << endl;
                        if (nSARP.getValue(x[i][j][k]) > 0.5){
							fipStat->solBeginParcel.push_back(nSARP.getValue(s[j]));
							// TODO UNCOMMENT //  << "\ns(" << j << "): " << nSARP.getValue(s[j]) << " - ";
							// getchar();
                            // cout  << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
                            // getchar();
                        }
                    }
                }
            }   
        }
		// TODO UNCOMMENT //  << endl;
        // printResults(inst, mdist, sStat, nodeVec);

	}
	else{
		// TODO UNCOMMENT //  << "\n\nServed parcels: " << 0 << endl;
	}
	env.end();
    // startVal.end();
    // startVar.end();
	// startVal2.end();
    // startVar2.end();
}

void mipnodefip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat, fipStats *fipStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	//long M = 10*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);
	long W = inst->m + 1;
	int Q;

    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;
	
	int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	if (problem->model == "fip"){ //single parcel setting
		Q = 1;
	}
	else if (problem->model == "nodefip" || problem->model == "bundle4"){
		Q = inst->m;
	}

	int P = 2*inst->m;
	// int P = 1;

	double NNarcs = 0;

	for (int k = 0; k < fipStat->solPass.size(); k++) {
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
			int u = fipStat->solPass[k][i];
			int v = fipStat->solPass[k][i + 1];

			NNarcs += mdist[u][v]*inst->costkm;

			cout << u << " ";
		}
		cout << endl;
	}
	getchar();

	//// TODO UNCOMMENT //  << "Printing node vec: " << endl;
	//for (int i = 0; i < nodeVec.size(); i++){
	//	// TODO UNCOMMENT //  << i << ": " << nodeVec[i].e << " - " << nodeVec[i].l << endl;
	//}
	//// TODO UNCOMMENT //  << endl;
	//getchar();

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
                // // TODO UNCOMMENT //  << "x: [" << i << "][" << j << "][" << k << "]" << endl;
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
	IloNumVarArray b(env, nodeVec.size(), 9, inst->T);
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

	IloNumVarArray u(env, nodeVec.size(), 0, P);

	for (int i = 0; i < nodeVec.size(); i++){
		sprintf(var, "u(%d)", i);
		u[i].setName(var);
		model.add(u[i]);
	}

	////if (problem->p1 < 1 && problem->dParcel > 0){
	//IloNumVarArray P(env, nodeVec.size(), 1, (2*inst->m + inst->n));

	//for (int i = 0; i < nodeVec.size() - 2*inst->K; i++){
	//	sprintf(var, "P(%d)", i);
	//	P[i].setName(var);
	//	model.add(P[i]);
		
	//}			
	////}

	IloExpr objFunction(env);

    // objFunction += NNarcs;

	// objFunction -= fipStat->solprofit;

	objFunction += inst->totalCustomProfit;

    for (int a = 0; a < nas->arcPN.size(); a++){
        int i = nas->arcPN[a].first;
        int j = nas->arcPN[a].second;
        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            objFunction += nodeVec[i].profit * x[i][j][k];
        }
    }

	if (problem->dParcel > 0){
		
		for (int a = 0; a < nas->arcPD.size(); a++){
			int i = nas->arcPD[a].first;
			int j = nas->arcPD[a].second;
			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction += nodeVec[i].profit * x[i][j][k];
			}
    	}

	}
	if (problem->p2 > 0){
		for (int a = 0; a < nas->arcPP.size(); a++){
			int i = nas->arcPP[a].first;
			int j = nas->arcPP[a].second;
			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
				int k = nas->arcV[i][j][k1];
				objFunction += nodeVec[i].profit * x[i][j][k];
			}
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

	//Creating extra set of summation that minimizes the number of used vehicles
	// for (int k = 0; k < inst->K; k++){
	// 	int i = inst->V - inst->K + k;
	// 	for (int a = 0; a < nas->arcPlus[i].size(); a++){
    // 		int j = nas->arcPlus[i][a].second;

	// 		objFunction -= (double)10 * x[i][j][k];
	// 	}
	// }

	model.add(IloMaximize(env, objFunction));

	// for (int k = 0; k < inst->K; k++){
    //     IloExpr exp(env);
    //     for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
    //         int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
    //         int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

    //         exp += x[u][v][k];
    //     }
    //     sprintf (var, "Constraint5_%d", k);
    //     IloRange cons = (exp <= 1);
    //     cons.setName(var);
    //     model.add(cons);
    // }

	//Creating constraints

	//Constraint 1 - All initial solutions elements must be used

	for (int k = 0; k < fipStat->solPass.size(); k++) {
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
			int i1 = fipStat->solPass[k][i];
			IloExpr exp(env);

			for (int a = 0; a < nas->vArcPlus[i1][k].size(); a++){
				int u = nas->vArcPlus[i1][k][a].first;
				int v = nas->vArcPlus[i1][k][a].second;

				exp += x[u][v][k];
			}

			sprintf (var, "Constraint1_%d", i1);
			IloRange cons = (exp == 1);
			cons.setName(var);
			model.add(cons);
		}
	}

	// for (int i = 0; i < inst->n; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
    //             int u = nas->vArcPlus[i][k][a].first;
    //             int v = nas->vArcPlus[i][k][a].second;

	// 			exp += x[u][v][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint1_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	//Constraint 2 - Relationship between visit variable and parcel node arcs
	//A parcel request can be denied (except those present in the initial solution)


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
		for (int i = 0; i < inst->n + inst->m; i++){

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

	//Constraint 7 - tie service begining to node visit

    for (int i = 0; i < fDepot; i++){
        IloExpr exp(env);
        exp = b[i] - M * y[i]; 
        sprintf (var, "Constraint7_%d", i);
        IloRange cons = (exp <= 9);
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
			double cvalue = mdist[i][j]/inst->vmed;
			//cvalue = std::round(cvalue * multiplier) / multiplier;
			//cvalue = timeRound(cvalue);
			exp = b[i] - b[j] + nodeVec[i].delta + (cvalue) - M * (1 - sumX);
			sprintf (var, "Constraint9_%d_%d", i, j);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	// Constraint 10 - ensure solution sequence
	for (int k = 0; k < fipStat->solPass.size(); k++) {
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
			int u = fipStat->solPass[k][i];
			int v = fipStat->solPass[k][i + 1];
			IloExpr exp(env);

			exp += b[v] - b[u] - nodeVec[u].delta - mdist[u][v]/inst->vmed;

			sprintf (var, "Constraint10_%d", i);
			IloRange cons = (exp >= 0);
			cons.setName(var);
			model.add(cons);
		}
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
	
	// //Constraint 14  - bound number of passenger visits transporting parcel
	// if (problem->p1 < 1){
	// 	// // TODO UNCOMMENT //  << "Constraint 14" << endl;
	// 	// getchar();
	// 	for (int i = 0; i < nas->arcNN.size(); i++){
	// 		IloExpr exp(env);
	// 		IloExpr sumX(env);
    //         int u = nas->arcNN[i].first;
    //         int v = nas->arcNN[i].second;

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
	// //make new constraints for route sequence

	// if (problem->p1 < 1 && problem->dParcel > 0){

	// 	for (int a = 0; a < nas->allArcs.size(); a++){
			
	// 		IloExpr exp(env);
	// 		IloExpr exp2(env);
	// 		IloExpr sumX(env);
	// 		int i = nas->allArcs[a].first;
	// 		int j = nas->allArcs[a].second;

	// 		for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
	// 			int k = nas->arcV[i][j][k1];
	// 			sumX += x[i][j][k];
	// 		}

	// 		exp = u[i] + nodeVec[j].load2 - W*(1 - sumX);
	// 		exp2 = u[j];
			
	// 		sprintf (var, "Constraint15_%d_%d", i, j);
			
	// 		IloRange cons1 = (exp2 - exp >= 0);
	// 		cons1.setName(var);
	// 		model.add(cons1);
	// 	}		
	// }

	// Fip constraints

	// //FIPConstraint 1  - number of parcels between passangers
	// for (int a = 0; a < nas->allArcs.size(); a++){
		
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	IloExpr sumX(env);
    //     int i = nas->allArcs[a].first;
    //     int j = nas->allArcs[a].second;

    //     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
    //         int k = nas->arcV[i][j][k1];
	// 		sumX += x[i][j][k];
	// 	}

	// 	exp = u[i] - P*(j < inst->n) + (j >= inst->n && j < inst->n + 2*inst->m)- (P + 1)*(1 - sumX);
	// 	exp2 = u[j];
		
    //     sprintf (var, "FIPConstraint%d_%d", i, j);
		
    //     IloRange cons1 = (exp2 - exp >= 0);
	// 	cons1.setName(var);
	// 	model.add(cons1);
	// }
	
	//end of new constraints

	////test constraints
	////// TODO UNCOMMENT //  << "here" << endl;
	//IloExpr exp(env);
	//exp = x[19][11][0];

	//sprintf (var, "Constraint15");

	//IloRange cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);
	////// TODO UNCOMMENT //  << "A" << endl;

	//exp = x[11][4][0];

	//sprintf (var, "Constraint16");

	////// TODO UNCOMMENT //  << "B" << endl;
	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[4][18][0];

	////// TODO UNCOMMENT //  << "C" << endl;
	//sprintf (var, "Constraint17");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	////// TODO UNCOMMENT //  << "after" << endl;
	//exp = x[18][10][0];

	//sprintf (var, "Constraint18");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[10][3][0];

	//sprintf (var, "Constraint19");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[3][17][0];

	//sprintf (var, "Constraint20");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[17][1][0];

	//sprintf (var, "Constraint21");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[1][0][0];

	//sprintf (var, "Constraint22");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[0][6][0];

	//sprintf (var, "Constraint23");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[6][2][0];

	//sprintf (var, "Constraint24");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[2][13][0];

	//sprintf (var, "Constraint25");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);





	//exp = x[12][2][1];

	//sprintf (var, "Constraint26");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);
		
	//exp = x[2][18][1];

	//sprintf (var, "Constraint27");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[18][0][1];

	//sprintf (var, "Constraint28");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[0][3][1];

	//sprintf (var, "Constraint29");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[3][4][1];

	//sprintf (var, "Constraint30");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[4][16][1];

	//sprintf (var, "Constraint31");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);

	//exp = x[16][5][1];

	//sprintf (var, "Constraint32");

	//cons = (exp == 1);
	//cons.setName(var);
	//model.add(cons);


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
    cout  << "\nThreads: " << threads << endl;

	IloCplex nSARP(model);
	nSARP.exportModel("nSARP.lp");
	// nSARP.setOut(env.getNullStream());
	nSARP.setParam(IloCplex::Threads, threads);
	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = nSARP.getTime();
	nSARP.solve();
    time = (nSARP.getTime() - start)/threads;
	cout  << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

    cout  << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
    cout  << " Total Time: " << time << endl;

	if (sStat->feasible){

        cout  << " LB: " << nSARP.getObjValue() << endl;
        cout  << " UB: " << nSARP.getBestObjValue() << endl;
        sStat->solprofit = nSARP.getObjValue();
		sStat->solDual = nSARP.getBestObjValue();
        sStat->time = time;

		if (((nSARP.getBestObjValue() - nSARP.getObjValue())/nSARP.getBestObjValue()) * 100 < 0.01) {
			sStat->status = "Optimal";
		} else {
			sStat->status = "Feasible";
		}

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
                            // cout  << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
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


        for (int i = 0; i < nodeVec.size(); i++){
            //if (nSARP.getValue(u[i])){
                sStat->solLoad2.push_back(nSARP.getValue(u[i]));
            //}
            //else {
            //    sStat->solLoad2.push_back(0);
            //}
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

// void mipnodefip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat, fipStats *fipStat){
// 	//MIP
// 	//Creating environment and model 
// 	char var[100];
// 	IloEnv env;
// 	IloModel model(env, "nSARP");
// 	int currSP;
// 	long M = 2*inst->T;
// 	//long M = 10*inst->T;
// 	long M2 = 2*(inst->n + inst->m + 1);
// 	long W = inst->m + 1;
// 	int Q;

//     int fDepot = inst->n + 2*inst->m;
//     int fDummy = inst->n + 2*inst->m + inst->K;
	
// 	int decimalPlaces = 4;
//     double multiplier = std::pow(10, decimalPlaces);

// 	vector< pair<int, int> > auxPairVec;
// 	pair<int, int> auxPair;

// 	if (problem->p2 < 1){ //single parcel setting
// 		Q = 1;
// 	}
// 	else{
// 		Q = inst->m;
// 	}

// 	int P = 1;
// 	// int P = 1;

// 	//// TODO UNCOMMENT //  << "Printing node vec: " << endl;
// 	//for (int i = 0; i < nodeVec.size(); i++){
// 	//	// TODO UNCOMMENT //  << i << ": " << nodeVec[i].e << " - " << nodeVec[i].l << endl;
// 	//}
// 	//// TODO UNCOMMENT //  << endl;
// 	//getchar();

// 	//Creating variables
// 	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

//     for (int i = 0; i < nodeVec.size(); i++){
//         x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
//         for(int j = 0; j < nodeVec.size(); ++j){
//             if (nas->arcs[i][j] != true){
//                 continue; // If arc i to j is invalid
//             } 

//             x[i][j] = IloBoolVarArray (env, inst->K); //Number of Vehicles
//             for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//                 int k = nas->arcV[i][j][k1];
//                 sprintf(var, "x(%d,%d,%d)", i, j, k);
//                 x[i][j][k].setName(var);
//                 model.add(x[i][j][k]);
//                 // // TODO UNCOMMENT //  << "x: [" << i << "][" << j << "][" << k << "]" << endl;
//             }
//         }
//     }

// 	//creates boolean variable (y_i = 1 if node i is visited; 0 cc)
// 	IloBoolVarArray y(env, nodeVec.size()); 

// 	for (int i = 0; i < nodeVec.size(); i++){
// 		sprintf(var, "y(%d)", i);
// 		y[i].setName(var);
// 		model.add(y[i]);
// 	}

// 	// Variable start of service time for parcels
// 	IloNumVarArray b(env, nodeVec.size(), 9, inst->T);
// 	for (int i = 0; i < nodeVec.size(); i++){
// 		sprintf(var, "b(%d)", i);
// 		b[i].setName(var);
// 		model.add(b[i]);
// 	}

// 	// Start of service time for initial solution nodes
// 	IloArray < IloNumVarArray > s(env, inst->K);
// 	for (int k = 0; k < fipStat->solPass.size(); k++) {
// 		s[k] = IloNumVarArray(env, fipStat->solPass[k].size(), 9, inst->T);

// 		for (int i = 0; i < fipStat->solPass[k].size(); i++) {
// 			sprintf(var, "s(%d,%d)", k, i);
// 			s[k][i].setName(var);
// 			model.add(s[k][i]);
// 		}
// 	}

// 	//Load variable

// 	IloNumVarArray w(env, nodeVec.size(), 0, Q);

// 	for (int i = 0; i < nodeVec.size(); i++){
// 		sprintf(var, "w(%d)", i);
// 		w[i].setName(var);
// 		model.add(w[i]);
		
// 	}

// 	IloNumVarArray u(env, nodeVec.size(), 0, P);

// 	for (int i = 0; i < nodeVec.size(); i++){
// 		sprintf(var, "u(%d)", i);
// 		u[i].setName(var);
// 		model.add(u[i]);
// 	}

// 	////if (problem->p1 < 1 && problem->dParcel > 0){
// 	//IloNumVarArray P(env, nodeVec.size(), 1, (2*inst->m + inst->n));

// 	//for (int i = 0; i < nodeVec.size() - 2*inst->K; i++){
// 	//	sprintf(var, "P(%d)", i);
// 	//	P[i].setName(var);
// 	//	model.add(P[i]);
		
// 	//}			
// 	////}

// 	IloExpr objFunction(env);

//     objFunction += inst->totalCustomProfit;

//     for (int a = 0; a < nas->arcPN.size(); a++){
//         int i = nas->arcPN[a].first;
//         int j = nas->arcPN[a].second;
//         for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//             int k = nas->arcV[i][j][k1];
//             objFunction += nodeVec[i].profit * x[i][j][k];
//         }
//     }

// 	if (problem->dParcel > 0){
		
// 		for (int a = 0; a < nas->arcPD.size(); a++){
// 			int i = nas->arcPD[a].first;
// 			int j = nas->arcPD[a].second;
// 			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
// 				int k = nas->arcV[i][j][k1];
// 				objFunction += nodeVec[i].profit * x[i][j][k];
// 			}
//     	}

// 	}
// 	if (problem->p2 > 0){
// 		for (int a = 0; a < nas->arcPP.size(); a++){
// 			int i = nas->arcPP[a].first;
// 			int j = nas->arcPP[a].second;
// 			for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
// 				int k = nas->arcV[i][j][k1];
// 				objFunction += nodeVec[i].profit * x[i][j][k];
// 			}
//     	}
// 	}

	


//     for (int a = 0; a < nas->allArcs.size(); a++){
//         int i = nas->allArcs[a].first;
//         int j = nas->allArcs[a].second;
//         for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//             int k = nas->arcV[i][j][k1];
//             objFunction -= (double)inst->costkm*mdist[i][j] * x[i][j][k];
//         }
//     }

// 	//Creating extra set of summation that minimizes the number of used vehicles
// 	// for (int k = 0; k < inst->K; k++){
// 	// 	int i = inst->V - inst->K + k;
// 	// 	for (int a = 0; a < nas->arcPlus[i].size(); a++){
//     // 		int j = nas->arcPlus[i][a].second;

// 	// 		objFunction -= (double)10 * x[i][j][k];
// 	// 	}
// 	// }

// 	model.add(IloMaximize(env, objFunction));

// 	// for (int k = 0; k < inst->K; k++){
//     //     IloExpr exp(env);
//     //     for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
//     //         int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
//     //         int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

//     //         exp += x[u][v][k];
//     //     }
//     //     sprintf (var, "Constraint5_%d", k);
//     //     IloRange cons = (exp <= 1);
//     //     cons.setName(var);
//     //     model.add(cons);
//     // }

// 	//Creating constraints

// 	//Constraint 1 - All passenger nodes must be visited

// 	for (int i = 0; i < inst->n; i++){
// 		IloExpr exp(env);
// 		for (int k = 0; k < inst->K; k++){
// 			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
//                 int u = nas->vArcPlus[i][k][a].first;
//                 int v = nas->vArcPlus[i][k][a].second;

// 				exp += x[u][v][k];
// 			}
// 		}
// 		sprintf (var, "Constraint1_%d", i);
// 		IloRange cons = (exp == 1);
// 		cons.setName(var);
// 		model.add(cons);
// 	}

// 	//Constraint 2 - Relationship between visit variable and parcel node arcs
// 	//A parcel request can be denied


// 	// if (problem->scen == "PC"){
// 	// 	for (int i = inst->n; i < inst->n + inst->m; i++){

// 	// 		IloExpr exp(env);
// 	// 		for (int k = 0; k < inst->K; k++){
// 	// 			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
// 	// 	            int u = nas->vArcPlus[i][k][a].first;
// 	// 	            int v = nas->vArcPlus[i][k][a].second;

// 	// 				exp += x[u][v][k];
// 	// 			}
// 	// 		}

// 	// 		sprintf (var, "Constraint2_%d", i);
// 	// 		IloRange cons = (exp <= 1);
// 	// 		cons.setName(var);
// 	// 		model.add(cons);
// 	// 	}
// 	// }
// 	// else{
// 		for (int i = inst->n; i < inst->n + inst->m; i++){

// 			IloExpr exp(env);
// 			IloExpr exp2(env);
// 			for (int k = 0; k < inst->K; k++){
// 				for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
// 	                int u = nas->vArcPlus[i][k][a].first;
// 	                int v = nas->vArcPlus[i][k][a].second;

// 					exp += x[u][v][k];
// 				}
// 			}

// 	        exp2 += y[i];
// 			sprintf (var, "Constraint2_%d", i);
// 			IloRange cons = (exp - exp2 == 0);
// 			// IloRange cons = (exp == 1);
// 			cons.setName(var);
// 			model.add(cons);
// 		}		
// 	// }
	
// 	//Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

// 	for (int i = inst->n; i < inst->n + inst->m; i++){
// 		for (int k = 0; k < inst->K; k++){
// 			IloExpr exp1(env);
// 			IloExpr exp2(env);
// 			//Left side: arc leaves i
// 			for (int a = 0; a < nas->vArcPlus[i][k].size(); a++){
//                 int u = nas->vArcPlus[i][k][a].first;
//                 int v = nas->vArcPlus[i][k][a].second;

// 				exp1 += x[u][v][k];
// 			}
// 			//Right side: arc leaves i + m
// 			for (int a = 0; a < nas->vArcPlus[i + inst->m][k].size(); a++){
// 				int u = nas->vArcPlus[i + inst->m][k][a].first;
//                 int v = nas->vArcPlus[i + inst->m][k][a].second;

//                 exp2 += x[u][v][k];
// 			}
// 			sprintf (var, "Constraint3_%d_%d", i, k);
// 			IloRange cons = ((exp1-exp2) == 0);
// 			cons.setName(var);
// 			model.add(cons);
// 		}
// 	}

// 	//Constraint 4 - Flow conservation

// 	for (int a = 0; a < inst->n + 2*inst->m; a++){
// 		for (int k = 0; k < inst->K; k++){
// 			IloExpr exp1(env);
// 			IloExpr exp2(env);
// 			//Left side: arc leaves i
// 			for (int b = 0; b < nas->vArcPlus[a][k].size(); b++){
//                 int i = nas->vArcPlus[a][k][b].first;
//                 int j = nas->vArcPlus[a][k][b].second;
// 				exp1 += x[i][j][k];
// 			}
// 			//Right side: arc enters i
// 			for (int b = 0; b < nas->vArcMinus[a][k].size(); b++){
//                 int i = nas->vArcMinus[a][k][b].first;
//                 int j = nas->vArcMinus[a][k][b].second;
// 				exp2 += x[i][j][k];

// 			}
// 			sprintf (var, "Constraint4_%d_%d", a, k);
// 			IloRange cons = ((exp1-exp2) == 0);
// 			cons.setName(var);
// 			model.add(cons);
// 		}
// 	}

// 	//Constraint 5 - The route of every used vehicle has to start at its starting position


//     for (int k = 0; k < inst->K; k++){
//         IloExpr exp(env);
//         for (int a = 0; a < nas->vArcPlus[inst->V - inst->K + k][k].size(); a++){
//             int u = nas->vArcPlus[inst->V - inst->K + k][k][a].first;
//             int v = nas->vArcPlus[inst->V - inst->K + k][k][a].second;

//             exp += x[u][v][k];
//         }
//         sprintf (var, "Constraint5_%d", k);
//         IloRange cons = (exp == 1);
//         cons.setName(var);
//         model.add(cons);
//     }

// 	// Constraint 6 - The route of every used vehicle has to end at dummy node f

// 	for (int k = 0; k < inst->K; k++){
// 		IloExpr exp(env);
// 		for (int a = 0; a < nas->vArcMinus[inst->V + k][k].size(); a++){
//             int u = nas->vArcMinus[inst->V + k][k][a].first;
//             int v = nas->vArcMinus[inst->V + k][k][a].second;

//         	exp += x[u][v][k];
// 		}
// 		sprintf (var, "Constraint6_%d", k);
// 		IloRange cons = (exp == 1);
// 		cons.setName(var);
// 		model.add(cons);
// 	}

// 	//Constraint 7 - tie service begining to node visit

//     for (int i = 0; i < fDepot; i++){
//         IloExpr exp(env);
//         exp = b[i] - M * y[i]; 
//         sprintf (var, "Constraint7_%d", i);
//         IloRange cons = (exp <= 9);
//         cons.setName(var);
//         model.add(cons);
//     }


// 	//Constraint 8 - service of pickup must come before the delivery

// 	for (int i = inst->n; i < inst->n + inst->m; i++){
// 		IloExpr exp(env);
// 		exp = b[i] - b[i + inst->m];

// 		sprintf (var, "Constraint8_%d", i);
// 		IloRange cons = (exp <= 0);
// 		cons.setName(var);
// 		model.add(cons);
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 1" << endl;
// 	// Constraint 9 - TW constraint

//     for (int k = 0; k < fipStat->solPass.size(); k++){

//         for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
// 			int u = fipStat->solPass[k][i];
// 			int v = fipStat->solPass[k][i + 1];
// 			IloExpr exp1(env);
// 			IloExpr exp2(env);

// 			// exp1 = b[k][v] - b[k][u] - (mdist[u][v]/inst->vmed);
// 			double cvalue = mdist[u][v]/inst->vmed;
// 			//cvalue = std::round(cvalue * multiplier) / multiplier;			
// 			exp1 = s[k][i+1] - (s[k][i]) - nodeVec[u].delta - (cvalue);
            
// 			sprintf (var, "Constraint9_%d_%d", k, i);
// 			IloRange cons = (exp1 - exp2 >= 0);
// 			cons.setName(var);
// 			model.add(cons);
//         }
//     }

// 	std::// TODO UNCOMMENT //  << "AQUI 2" << endl;

// 	// Constraint 10 - TW constraint

// 	for (int k = 0; k < fipStat->solPass.size(); k++){
// 		for (int i = 0; i < fipStat->solPass[k].size(); i++){
// 			int i1 = fipStat->solPass[k][i];

// 			for (int a = 0; a < nas->vArcMinus[i1][k].size(); a++) {
// 				int u = nas->vArcMinus[i1][k][a].first;
// 				int v = nas->vArcMinus[i1][k][a].second;

// 				if (u < inst->n || u >= fDepot) {
// 					continue;
// 				}

// 				IloExpr exp(env);

// 				double cvalue = mdist[u][v]/inst->vmed;
// 				//cvalue = std::round(cvalue * multiplier) / multiplier;
// 				//cvalue = timeRound(cvalue);
// 				exp = b[u] - s[k][i] + nodeVec[u].delta + (cvalue) - M * (1 - x[u][v][k]);
// 				sprintf (var, "Constraint10_%d_%d", u, v);
// 				IloRange cons = (exp <= 0);
// 				cons.setName(var);
// 				model.add(cons);
// 			}
// 		}
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 3" << endl;

// 	// Constraint 11 - TW constraint

// 	for (int i = inst->n; i < fDepot; i++){
// 		for (int j = inst->n; j < fDepot; j++){

// 			if (!nas->arcs[i][j]) {
// 				continue;
// 			}

// 			for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++) {
// 				int k = nas->arcV[i][j][k1];

// 				IloExpr exp(env);

// 				double cvalue = mdist[i][j]/inst->vmed;
// 				//cvalue = std::round(cvalue * multiplier) / multiplier;
// 				//cvalue = timeRound(cvalue);
// 				exp = b[i] - b[j] + nodeVec[i].delta + (cvalue) - M * (1 - x[i][j][k]);
// 				sprintf (var, "Constraint10_%d_%d", i, j);
// 				IloRange cons = (exp <= 0);
// 				cons.setName(var);
// 				model.add(cons);
// 			}
// 		}
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 4" << endl;

// 	// Constraint 12 - TW constraint

// 	for (int k = 0; k < fipStat->solPass.size(); k++){
// 		for (int i = 0; i < fipStat->solPass[k].size(); i++){
// 			int i1 = fipStat->solPass[k][i];

// 			for (int a = 0; a < nas->vArcPlus[i1][k].size(); a++) {
// 				int u = nas->vArcPlus[i1][k][a].first;
// 				int v = nas->vArcPlus[i1][k][a].second;

// 				if (v < inst->n || v >= fDepot) {
// 					continue;
// 				}

// 				IloExpr exp(env);

// 				double cvalue = mdist[u][v]/inst->vmed;
// 				//cvalue = std::round(cvalue * multiplier) / multiplier;
// 				//cvalue = timeRound(cvalue);
// 				exp = s[k][i] - b[v] + nodeVec[u].delta + (cvalue) - M * (1 - x[u][v][k]);
// 				sprintf (var, "Constraint10_%d_%d", u, v);
// 				IloRange cons = (exp <= 0);
// 				cons.setName(var);
// 				model.add(cons);
// 			}
// 		}
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 5" << endl;

// 	// Constraint 7 - Maximum driving time (29)

// 	for (int k = 0; k < fipStat->solPass.size(); k++){
// 		IloExpr exp(env);
// 		int u = fipStat->solPass[k][0];
// 		int v = fipStat->solPass[k][fipStat->solPass[k].size()-1];

// 		exp = s[k][fipStat->solPass[k].size()-1] - s[k][0];
// 		// exp = b[k][v] - b[k][u];

// 		sprintf (var, "Constraint7_%d", k);
// 		IloRange cons = (exp <= inst->maxTime);
// 		cons.setName(var);
// 		model.add(cons);
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 6" << endl;

// 	// Constraint 8 - Time windows (30)


// 	for (int k = 0; k < fipStat->solPass.size(); k++){
// 		for (int i = 0; i < fipStat->solPass[k].size(); i++){
// 			IloExpr exp(env);
			
// 			int u = fipStat->solPass[k][i];

// 			exp = s[k][i];

// 			sprintf (var, "Constraint8_%d_%d", u, k);
// 			IloRange cons1 = (exp <= nodeVec[u].l);
// 			cons1.setName(var);
// 			model.add(cons1);
			
// 			sprintf (var, "Constraint9_%d_%d", u, k);
// 			IloRange cons2 = (nodeVec[u].e <= exp);
// 			cons2.setName(var);
// 			model.add(cons2);
// 		}
			
// 	}

// 	std::// TODO UNCOMMENT //  << "AQUI 7" << endl;

// 	//parcels TW (31)
// 	for (int j = inst->n; j < inst->n + 2*inst->m; j++){
// 		IloExpr exp(env);

// 		exp = b[j];

// 		sprintf (var, "Constraint10_%d", j);
// 		IloRange cons1 = (exp <= nodeVec[j].l);
// 		cons1.setName(var);
// 		model.add(cons1);
		
// 		sprintf (var, "Constraint11_%d", j);
// 		IloRange cons2 = (nodeVec[j].e <= exp);
// 		cons2.setName(var);
// 		model.add(cons2);
// 	}

// 	// //Constraints 9 - TW 

// 	// for (int a = 0; a < nas->allArcs.size(); a++){
// 	// 	IloExpr exp(env);
// 	// 	IloExpr sumX(env);
//     //     int i = nas->allArcs[a].first;
//     //     int j = nas->allArcs[a].second;
//     //     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//     //         int k = nas->arcV[i][j][k1];
//     //         sumX += x[i][j][k];
// 	// 	}
// 	// 		double cvalue = mdist[i][j]/inst->vmed;
// 	// 		//cvalue = std::round(cvalue * multiplier) / multiplier;
// 	// 		//cvalue = timeRound(cvalue);
// 	// 		exp = b[i] - b[j] + nodeVec[i].delta + (cvalue) - M * (1 - sumX);
// 	// 		sprintf (var, "Constraint9_%d_%d", i, j);
// 	// 		IloRange cons = (exp <= 0);
// 	// 		cons.setName(var);
// 	// 		model.add(cons);			

// 	// }

// 	// // //Constraints 10 - load constraints

// 	// for (int a = 0; a < nas->allArcs.size(); a++){
		
// 	// 	IloExpr exp(env);
// 	// 	IloExpr exp2(env);
// 	// 	IloExpr sumX(env);
//     //     int i = nas->allArcs[a].first;
//     //     int j = nas->allArcs[a].second;

//     //     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//     //         int k = nas->arcV[i][j][k1];
// 	// 		sumX += x[i][j][k];
// 	// 	}

// 	// 	exp = w[i] + nodeVec[j].load - W*(1 - sumX);
// 	// 	exp2 = w[j];
		
//     //     sprintf (var, "Constraint10_%d_%d", i, j);
		
//     //     IloRange cons1 = (exp2 - exp >= 0);
// 	// 	cons1.setName(var);
// 	// 	model.add(cons1);
// 	// }

// 	// //Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

// 	// for (int i = 0; i < nodeVec.size(); i++){
// 	// 	IloExpr exp(env);
// 	// 	exp = b[i];

// 	// 	sprintf (var, "Constraint11_%d", i);
// 	// 	IloRange cons1 = (exp <= nodeVec[i].l);
// 	// 	cons1.setName(var);
// 	// 	model.add(cons1);
		
// 	// 	sprintf (var, "Constraint12_%d", i);
// 	// 	IloRange cons2 = (nodeVec[i].e <= exp);
// 	// 	cons2.setName(var);
// 	// 	model.add(cons2);			
// 	// }

//     // //Constraints 13 - maximum driving time

//     // for (int i = fDepot; i < fDummy; i++){
//     //     IloExpr exp(env);
//     //     exp = b[i + inst->K] - b[i];

//     //     sprintf (var, "Constraint13_%d", i);
//     //     IloRange cons1 = (exp <= inst->maxTime);
//     //     cons1.setName(var);
//     //     model.add(cons1);        
//     // }
	
// 	// //Constraint 14  - bound number of passenger visits transporting parcel
// 	// if (problem->p1 < 1){
// 	// 	// // TODO UNCOMMENT //  << "Constraint 14" << endl;
// 	// 	// getchar();
// 	// 	for (int i = 0; i < nas->arcNN.size(); i++){
// 	// 		IloExpr exp(env);
// 	// 		IloExpr sumX(env);
//     //         int u = nas->arcNN[i].first;
//     //         int v = nas->arcNN[i].second;

// 	// 		for (int k = 0; k < inst->K; k++){
// 	// 			sumX += x[u][v][k];
// 	// 		}
// 	// 		exp = 1 - (w[u]/W);

// 	// 		sprintf (var, "Constraint14_%d", i);
// 	// 		IloRange cons1 = (sumX - exp <= 0);
// 	// 		cons1.setName(var);
// 	// 		model.add(cons1);

// 	// 	}
// 	// }
// 	// //make new constraints for route sequence

// 	// if (problem->p1 < 1 && problem->dParcel > 0){

// 	// 	for (int a = 0; a < nas->allArcs.size(); a++){
			
// 	// 		IloExpr exp(env);
// 	// 		IloExpr exp2(env);
// 	// 		IloExpr sumX(env);
// 	// 		int i = nas->allArcs[a].first;
// 	// 		int j = nas->allArcs[a].second;

// 	// 		for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
// 	// 			int k = nas->arcV[i][j][k1];
// 	// 			sumX += x[i][j][k];
// 	// 		}

// 	// 		exp = u[i] + nodeVec[j].load2 - W*(1 - sumX);
// 	// 		exp2 = u[j];
			
// 	// 		sprintf (var, "Constraint15_%d_%d", i, j);
			
// 	// 		IloRange cons1 = (exp2 - exp >= 0);
// 	// 		cons1.setName(var);
// 	// 		model.add(cons1);
// 	// 	}		
// 	// }

// 	// Fip constraints

// 	//FIPConstraint 1  - number of parcels between passangers
// 	for (int a = 0; a < nas->allArcs.size(); a++){
		
// 		IloExpr exp(env);
// 		IloExpr exp2(env);
// 		IloExpr sumX(env);
//         int i = nas->allArcs[a].first;
//         int j = nas->allArcs[a].second;

//         for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//             int k = nas->arcV[i][j][k1];
// 			sumX += x[i][j][k];
// 		}

// 		exp = u[i] - P*(j < inst->n) + (j >= inst->n && j < inst->n + 2*inst->m)- (P + 1)*(1 - sumX);
// 		exp2 = u[j];
		
//         sprintf (var, "FIPConstraint%d_%d", i, j);
		
//         IloRange cons1 = (exp2 - exp >= 0);
// 		cons1.setName(var);
// 		model.add(cons1);
// 	}
	
// 	//end of new constraints

// 	////test constraints
// 	////// TODO UNCOMMENT //  << "here" << endl;
// 	//IloExpr exp(env);
// 	//exp = x[19][11][0];

// 	//sprintf (var, "Constraint15");

// 	//IloRange cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);
// 	////// TODO UNCOMMENT //  << "A" << endl;

// 	//exp = x[11][4][0];

// 	//sprintf (var, "Constraint16");

// 	////// TODO UNCOMMENT //  << "B" << endl;
// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[4][18][0];

// 	////// TODO UNCOMMENT //  << "C" << endl;
// 	//sprintf (var, "Constraint17");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	////// TODO UNCOMMENT //  << "after" << endl;
// 	//exp = x[18][10][0];

// 	//sprintf (var, "Constraint18");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[10][3][0];

// 	//sprintf (var, "Constraint19");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[3][17][0];

// 	//sprintf (var, "Constraint20");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[17][1][0];

// 	//sprintf (var, "Constraint21");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[1][0][0];

// 	//sprintf (var, "Constraint22");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[0][6][0];

// 	//sprintf (var, "Constraint23");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[6][2][0];

// 	//sprintf (var, "Constraint24");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[2][13][0];

// 	//sprintf (var, "Constraint25");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);





// 	//exp = x[12][2][1];

// 	//sprintf (var, "Constraint26");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);
		
// 	//exp = x[2][18][1];

// 	//sprintf (var, "Constraint27");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[18][0][1];

// 	//sprintf (var, "Constraint28");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[0][3][1];

// 	//sprintf (var, "Constraint29");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[3][4][1];

// 	//sprintf (var, "Constraint30");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[4][16][1];

// 	//sprintf (var, "Constraint31");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);

// 	//exp = x[16][5][1];

// 	//sprintf (var, "Constraint32");

// 	//cons = (exp == 1);
// 	//cons.setName(var);
// 	//model.add(cons);


//     int threads;

//     threads = 1;

//     // if (inst->instParam[0] < 14){
//     //     if (inst->instParam[0] > 12 && inst->instParam[1] == 7){
//     //         threads = 8;
//     //     }
//     //     else{
//     //         threads = 1;
//     //     }
//     // }

//     // else if (inst->instParam[0] > 14) {
//     //     threads = 8;
//     // }

//     // else{
//     //     if (inst->instParam[1] > 1){
//     //         threads = 8;              
//     //     }
//     //     else{
//     //         threads = 1;
//     //     }
//     // }
//     // TODO UNCOMMENT //  << "\nThreads: " << threads << endl;

// 	IloCplex nSARP(model);
// 	nSARP.exportModel("nSARP.lp");
// 	nSARP.setParam(IloCplex::Threads, threads);
// 	nSARP.setParam(IloCplex::Param::TimeLimit, 7200);

//     IloNum start;
//     IloNum time;
//     start = nSARP.getTime();
// 	nSARP.solve();
//     time = (nSARP.getTime() - start)/threads;
// 	// TODO UNCOMMENT //  << "\nSol status: " << nSARP.getStatus() << endl;
// 	sStat->feasible = nSARP.isPrimalFeasible();

//     // TODO UNCOMMENT //  << " Tree_Size: " <<  nSARP.getNnodes() + nSARP.getNnodesLeft() + 1 << endl;
//     // TODO UNCOMMENT //  << " Total Time: " << time << endl;

// 	if (sStat->feasible){

//         // TODO UNCOMMENT //  << " LB: " << nSARP.getObjValue() << endl;
//         // TODO UNCOMMENT //  << " UB: " << nSARP.getBestObjValue() << endl;
//         sStat->solprofit = nSARP.getObjValue();
//         sStat->time = time;

//         for (int k = 0; k < inst->K; k++){
//             sStat->solvec.push_back(auxPairVec);
//         }

//         for (int i = 0; i < nodeVec.size(); i++){
//             for(int j = 0; j < nodeVec.size(); j++){                
//                 if (nas->arcs[i][j] == true){
//                     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//                         int k = nas->arcV[i][j][k1];
//                         if (nSARP.getValue(x[i][j][k]) > 0.5){
//                             auxPair.first = i;
//                             auxPair.second = j;
//                             sStat->solvec[k].push_back(auxPair);
//                             // // TODO UNCOMMENT //  << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
//                             // getchar();
//                         }
//                     }
//                 }
//             }   
//         }

//         for (int i = 0; i < nodeVec.size(); i++){
//             if (nSARP.getValue(b[i]) > 0){
//                 sStat->solBegin.push_back(nSARP.getValue(b[i]));
//             }
//             else {
//                 sStat->solBegin.push_back(0);
//             }
//         }

//         for (int i = 0; i < nodeVec.size(); i++){
//             if (nSARP.getValue(w[i]) > 0.5){
//                 sStat->solLoad.push_back(nSARP.getValue(w[i]));
//             }
//             else {
//                 sStat->solLoad.push_back(0);
//             }
//         }


//         for (int i = 0; i < nodeVec.size(); i++){
//             //if (nSARP.getValue(u[i])){
//                 sStat->solLoad2.push_back(nSARP.getValue(u[i]));
//             //}
//             //else {
//             //    sStat->solLoad2.push_back(0);
//             //}
//         }		

//         printResults(inst, mdist, sStat, nodeVec);

// 	}
// 	if (problem->scen == "PC"){
//     	nSARP.clearModel();
//     	nSARP.end();
//     }

// 	env.end();
// }
// // %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%