#include "mipbundle.h"
#include <cstdlib>
#include <stdio.h>

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN = bStat->bundleVec.size() - (2*inst->K);
	int csetN = cStat->clusterVec.size() - (2*inst->K);
    int fcDummy = bStat->bundleVec.size() - inst->K;
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

            for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                int k = bStat->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}
		}	
	}

    // Variable start of service time depot
    IloNumVarArray u(env, inst->K, 0, inst->T);

    for (int k = 0; k < inst->K; k++){
        sprintf(var, "u(%d)", k);
        u[k].setName(var);
        model.add(u[k]);
    }

    // Variable start of service time dummy

    IloNumVarArray uf(env, inst->K, 0, inst->T);

    for (int k = 0; k < inst->K; k++){
        sprintf(var, "uf(%d)", k);
        uf[k].setName(var);
        model.add(uf[k]);
    }

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
			objFunction += bStat->bundleProfVec[i] * x[i][j][k];
			//objFunction += bStat->bundleProfVec[bStat->bArcVec[i].second] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}
    //i: last element of first bundle of the arc
    //j: first element of the second bundle of the arc
    //compute costs in between bundles
	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int u = bStat->bArcVec[a].first;
        int v = bStat->bArcVec[a].second;       
		int i = bStat->lastElement[u];
		int j = bStat->firstElement[v];
        // cout << "Last: " << i << " - First: " << j;
        
        for (int k1 = 0; k1 < bStat->arcV[u][v].size(); k1++){
            int k = bStat->arcV[u][v][k1];
            objFunction -= (double)(inst->costkm*mdist[i][j]) * x[u][v][k];
            // cout << "Cost: " << inst->costkm*mdist[lastElOfi][firstElOfj] << endl;
            // getchar();
        }

	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster
    // cout << "first part" << endl;
    // getchar();
	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < cStat->vArcPlus[i][k].size(); a++){
                int u = cStat->vArcPlus[i][k][a].first;
                int v = cStat->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons1 = (exp == 1);
		cons1.setName(var);
		model.add(cons1);
	}

    for (int i = 0; i < csetN; i++){
        IloExpr exp(env);
        for (int k = 0; k < inst->K; k++){
            for (int a = 0; a < cStat->vArcMinus[i][k].size(); a++){
                int u = cStat->vArcMinus[i][k][a].first;
                int v = cStat->vArcMinus[i][k][a].second;

                exp += x[u][v][k];
            }
        }
        sprintf (var, "Constraint2_%d", i);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// Constraint 2 - Only one arc comes into the cluster

	// for (int i = 0; i < csetN; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < cStat->cArcMinus[i].size(); j++){
	// 			exp += x[cStat->cArcMinus[i][j].first][cStat->cArcMinus[i][j].second][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint2_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// Constraint 3 - Each vehicle leaves its starting node
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		currSP = setN + k;
		for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
            int u = bStat->vArcPlus[currSP][k][a].first;
            int v = bStat->vArcPlus[currSP][k][a].second;
			exp += x[u][v][k];
		}
		sprintf (var, "Constraint3_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// // Constraint 4 - All vehicles end the trip at the dummy node f
	
	// //new version of constraint
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
        int cDummy = fcDummy + k;
		// for (int j = 0; j < cStat->clusterVec[fcDummy+k].size(); j++){
			for (int a = 0; a < bStat->vArcMinus[cDummy][k].size(); a++){
				int u = bStat->vArcMinus[cDummy][k][a].first;
                int v = bStat->vArcMinus[cDummy][k][a].second;

                exp += x[u][v][k];
			}
		// }
		sprintf (var, "Constraint4_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 5 - No parcel can be served more than once

	for (int i = 0; i < bStat->parcelBundleVec.size(); i++){ //parcelBundleVec is the size of parcels
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
            int h = bStat->parcelBundleVec[i][j];
			for (int k = 0; k < inst->K; k++){    
                for (int a = 0; a < bStat->vArcPlus[h][k].size(); a++){
                    int u = bStat->vArcPlus[h][k][a].first;
                    int v = bStat->vArcPlus[h][k][a].second;

					exp += x[u][v][k];
				}
			}
		}
		sprintf (var, "Constraint5_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}	

	// //Constraint 6 - Flow conservation between clusters(bundles)

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int a = 0; a < bStat->vArcPlus[i][k].size(); a++){
                int u = bStat->vArcPlus[i][k][a].first;
                int v = bStat->vArcPlus[i][k][a].second;
				exp1 += x[u][v][k];
			}

			for (int a = 0; a < bStat->vArcMinus[i][k].size(); a++){
                int u = bStat->vArcMinus[i][k][a].first;
                int v = bStat->vArcMinus[i][k][a].second;       
                         
				exp2 += x[u][v][k];
			}

			sprintf (var, "Constraint6_%d_%d", i, k);
			IloRange cons = (exp1 - exp2 == 0);
			cons.setName(var);
			model.add(cons);			
		}
	}

    //Constraints 7 - maximum driving time

    // for (int i = fDepot; i < fDummy; i++){
    //     IloExpr exp(env);
    //     exp = b[i + inst->K] - b[i];

    //     sprintf (var, "Constraint13_%d", i);
    //     IloRange cons1 = (exp <= inst->maxTime);
    //     cons1.setName(var);
    //     model.add(cons1);        
    // }

    // for (int k = 0; k < inst->K; k++){
    //     IloExpr exp(env);
    //     int currSt = setN + k;
    //     int currDum = fcDummy + k;

    //     for (int i = 0; i < setN; i++){
    //         for (int j = 0; j < setN; j++){
    //             exp = bStat->bundleEnd[j] - bStat->bundleStart[i] - (mdist[currSt][bStat->firstElement[i]]/inst->vmed);

    //             sprintf (var, "Constraint7_%d_%d_%d", k, i, j);
    //             IloRange cons1 = (exp <= inst->maxTime); 
    //             cons1.setName(var);
    //             model.add(cons1);                 
    //         }
    //     }      
    // }
    //Constraints 7 - calculating uk
        for (int k = 0; k < inst->K; k++){
            IloExpr exp1(env);
            IloExpr exp2(env);
            currSP = setN + k;
            int currDum = fcDummy + k;

            for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
                int u = bStat->vArcMinus[currDum][k][a].first;
                int v = bStat->vArcMinus[currDum][k][a].second;

                exp1 += (bStat->bundleEnd[u])*x[u][v][k];
            }
            for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
                int u = bStat->vArcPlus[currSP][k][a].first;
                int v = bStat->vArcPlus[currSP][k][a].second;
                double trip = (mdist[bStat->firstElement[u]][bStat->firstElement[v]])/inst->vmed;

                exp2 += (bStat->bundleStart[v] - trip)*x[u][v][k];
            }
            sprintf (var, "Constraint7_%d", k);
            IloRange cons = (exp1 - exp2 <= inst->maxTime);
            cons.setName(var);
            model.add(cons);                
        }
        
    //Constraints 8  - calculating ûk
        // for (int k = 0; k < inst->K; k++){
        //     IloExpr exp1(env);
        //     IloExpr exp2(env);
        //     int currDum = fcDummy + k;
        //     exp1 = uf[k];
        //     for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
        //         int u = bStat->vArcMinus[currDum][k][a].first;
        //         int v = bStat->vArcMinus[currDum][k][a].second;

        //         exp2 += (bStat->bundleEnd[u])*x[u][v][k];
        //     }
        //     sprintf (var, "Constraint8_%d", k);
        //     IloRange cons = (exp1 - exp2 == 0);
        //     cons.setName(var);
        //     model.add(cons);

        // }
            
    //Constraints 9 - max driving time

        // for (int k = 0; k < inst->K; k++){
        //     IloExpr exp(env);

        //     exp = uf[k] - u[k];

        //     sprintf (var, "Constraint9_%d", k);
        //     IloRange cons1 = (exp <= inst->maxTime); 
        //     cons1.setName(var);
        //     model.add(cons1);
        // }


 //*******************************

    //Forcing constraints
    // IloExpr exp3(env);

    // exp3 = x[30][4][0];

    // sprintf (var, "Constraint10");
    // IloRange cons1 = (exp3 == 1);
    // cons1.setName(var);
    // model.add(cons1); 

    //////////////////////////////////////

    int threads = 1;
	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, threads);

    bSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = bSARP.getTime();
	bSARP.solve();
    time = (bSARP.getTime() - start)/threads;

	cout << "\nSol status: " << bSARP.getStatus() << endl;
	sStat->feasible = bSARP.isPrimalFeasible();

    // cout << "here" << endl;
    // getchar();
    cout << " Tree_Size: " <<  bSARP.getNnodes() + bSARP.getNnodesLeft() + 1 << endl;
    cout << " Total Time: " << time << endl;

	if(sStat->feasible){

        cout << " LB: " << bSARP.getObjValue() << endl;
        cout << " UB: " << bSARP.getBestObjValue() << endl;

        solStatIni(sStat);
		cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

		sStat->solprofit = bSARP.getObjValue();

        cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

		for (int k = 0; k < inst->K; k++){
	 		sStat->solvec.push_back(auxPairVec);
		}

		for (int i = 0; i < bStat->bundleVec.size(); i++){
			for(int j = 0; j < bStat->bundleVec.size(); ++j){
				if (bStat->bArcs[i][j] == true){
                    for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                        int k = bStat->arcV[i][j][k1];
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
    }
    
	env.end();
}

void mipSolStats2 (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){

    int load;
    double distPass;
    // load = 0;
    double dij;
    int currNode;
    int nextNode;

    double stop;
    double tij;

    for (int k = 0; k < inst->K; k++){
        load = 0;
        for (int i = 0; i < sStat->solInNode[k].size() - 2; i++){
            // dij = mdist[sStat->solInNode[k][i]][sStat->solInNode[k][i + 1]];
            currNode = sStat->solInNode[k][i];
            nextNode = sStat->solInNode[k][i + 1];
            dij = mdist[currNode][nextNode];            

            if(currNode < inst->n){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta;   

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                    }
                }

                else if (nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }
                }

                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else if (currNode < inst->n + inst->m){
                if (nextNode < inst->n){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tBoth += nodeVec[nextNode].delta;

                    sStat->dParcel += dij;
                    distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                    sStat->dBoth += distPass;  
                }
                else if(nextNode < inst->n + inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load++;

                    sStat->dParcel += dij;         
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else if (currNode < inst->n + 2*inst->m){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta; 

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;                           
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                                               
                    }   
                }
                else if(nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }                  
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;
                }
            }
            else{
                if(nextNode < inst->n){
                    sStat->tNone += dij/inst->vmed;
                    sStat->tPass += nodeVec[nextNode].delta;
                    load = 0;

                    sStat->dNone += dij;
                    distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                    sStat->dPass += distPass;  
                }
                else if(nextNode < inst->n + inst->m){
                    sStat->tNone += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load++;

                    sStat->dNone += dij;
                }
            }
        }
    }
}

void miphbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bhSARP");
	int setN = bStat->bundleVec.size() - (2*inst->K);
	int csetN = cStat->clusterVec.size() - (2*inst->K);
    int fcDummy = bStat->bundleVec.size() - inst->K;
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

            for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                int k = bStat->arcV[i][j][k1];
                sprintf(var, "x(%d,%d,%d)", i, j, k);
				x[i][j][k].setName(var);
				model.add(x[i][j][k]);
			}
		}	
	}

    // Variable start of service time depot
    IloNumVarArray u(env, inst->K, 0, inst->T);

    for (int k = 0; k < inst->K; k++){
        sprintf(var, "u(%d)", k);
        u[k].setName(var);
        model.add(u[k]);
    }

    // Variable start of service time dummy

    IloNumVarArray uf(env, inst->K, 0, inst->T);

    for (int k = 0; k < inst->K; k++){
        sprintf(var, "uf(%d)", k);
        uf[k].setName(var);
        model.add(uf[k]);
    }

	//Creating objective function
	
	IloExpr objFunction(env);

	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        for(int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
			objFunction += bStat->bundleProfVec[i] * x[i][j][k];
			//objFunction += bStat->bundleProfVec[bStat->bArcVec[i].second] * x[bStat->bArcVec[i].first][bStat->bArcVec[i].second][k];
		}
	}
    //i: last element of first bundle of the arc
    //j: first element of the second bundle of the arc
    //compute costs in between bundles
	for (int a = 0; a < bStat->bArcVec.size(); a++){
        int u = bStat->bArcVec[a].first;
        int v = bStat->bArcVec[a].second;       
		int i = bStat->lastElement[u];
		int j = bStat->firstElement[v];
        // cout << "Last: " << i << " - First: " << j;
        
        for (int k1 = 0; k1 < bStat->arcV[u][v].size(); k1++){
            int k = bStat->arcV[u][v][k1];
            objFunction -= (double)(inst->costkm*mdist[i][j]) * x[u][v][k];
            // cout << "Cost: " << inst->costkm*mdist[lastElOfi][firstElOfj] << endl;
            // getchar();
        }

	}

	model.add(IloMaximize(env, objFunction));

	//Creating constraints

	//Constraint 1 - Only one arc leaves the cluster
    // cout << "first part" << endl;
    // getchar();
	for (int i = 0; i < csetN; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int a = 0; a < cStat->vArcPlus[i][k].size(); a++){
                int u = cStat->vArcPlus[i][k][a].first;
                int v = cStat->vArcPlus[i][k][a].second;

				exp += x[u][v][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons1 = (exp == 1);
		cons1.setName(var);
		model.add(cons1);
	}

    for (int i = 0; i < csetN; i++){
        IloExpr exp(env);
        for (int k = 0; k < inst->K; k++){
            for (int a = 0; a < cStat->vArcMinus[i][k].size(); a++){
                int u = cStat->vArcMinus[i][k][a].first;
                int v = cStat->vArcMinus[i][k][a].second;

                exp += x[u][v][k];
            }
        }
        sprintf (var, "Constraint2_%d", i);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	// Constraint 2 - Only one arc comes into the cluster

	// for (int i = 0; i < csetN; i++){
	// 	IloExpr exp(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < cStat->cArcMinus[i].size(); j++){
	// 			exp += x[cStat->cArcMinus[i][j].first][cStat->cArcMinus[i][j].second][k];
	// 		}
	// 	}
	// 	sprintf (var, "Constraint2_%d", i);
	// 	IloRange cons = (exp == 1);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// Constraint 3 - Each vehicle leaves its starting node
	
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		currSP = setN + k;
		for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
            int u = bStat->vArcPlus[currSP][k][a].first;
            int v = bStat->vArcPlus[currSP][k][a].second;
			exp += x[u][v][k];
		}
		sprintf (var, "Constraint3_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// // Constraint 4 - All vehicles end the trip at the dummy node f
	
	// //new version of constraint
	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
        int cDummy = fcDummy + k;
		// for (int j = 0; j < cStat->clusterVec[fcDummy+k].size(); j++){
			for (int a = 0; a < bStat->vArcMinus[cDummy][k].size(); a++){
				int u = bStat->vArcMinus[cDummy][k][a].first;
                int v = bStat->vArcMinus[cDummy][k][a].second;

                exp += x[u][v][k];
			}
		// }
		sprintf (var, "Constraint4_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 5 - No parcel can be served more than once

	for (int i = 0; i < bStat->parcelBundleVec.size(); i++){ //parcelBundleVec is the size of parcels
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
            int h = bStat->parcelBundleVec[i][j];
			for (int k = 0; k < inst->K; k++){    
                for (int a = 0; a < bStat->vArcPlus[h][k].size(); a++){
                    int u = bStat->vArcPlus[h][k][a].first;
                    int v = bStat->vArcPlus[h][k][a].second;

					exp += x[u][v][k];
				}
			}
		}
		sprintf (var, "Constraint5_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}	

	// //Constraint 6 - Flow conservation between clusters(bundles)

	for (int i = 0; i < setN; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

			for (int a = 0; a < bStat->vArcPlus[i][k].size(); a++){
                int u = bStat->vArcPlus[i][k][a].first;
                int v = bStat->vArcPlus[i][k][a].second;
				exp1 += x[u][v][k];
			}

			for (int a = 0; a < bStat->vArcMinus[i][k].size(); a++){
                int u = bStat->vArcMinus[i][k][a].first;
                int v = bStat->vArcMinus[i][k][a].second;       
                         
				exp2 += x[u][v][k];
			}

			sprintf (var, "Constraint6_%d_%d", i, k);
			IloRange cons = (exp1 - exp2 == 0);
			cons.setName(var);
			model.add(cons);			
		}
	}

    //Constraints 7 - maximum driving time

    // for (int i = fDepot; i < fDummy; i++){
    //     IloExpr exp(env);
    //     exp = b[i + inst->K] - b[i];

    //     sprintf (var, "Constraint13_%d", i);
    //     IloRange cons1 = (exp <= inst->maxTime);
    //     cons1.setName(var);
    //     model.add(cons1);        
    // }

    // for (int k = 0; k < inst->K; k++){
    //     IloExpr exp(env);
    //     int currSt = setN + k;
    //     int currDum = fcDummy + k;

    //     for (int i = 0; i < setN; i++){
    //         for (int j = 0; j < setN; j++){
    //             exp = bStat->bundleEnd[j] - bStat->bundleStart[i] - (mdist[currSt][bStat->firstElement[i]]/inst->vmed);

    //             sprintf (var, "Constraint7_%d_%d_%d", k, i, j);
    //             IloRange cons1 = (exp <= inst->maxTime); 
    //             cons1.setName(var);
    //             model.add(cons1);                 
    //         }
    //     }      
    // }
    //Constraints 7 - calculating uk
        for (int k = 0; k < inst->K; k++){
            IloExpr exp1(env);
            IloExpr exp2(env);
            currSP = setN + k;
            int currDum = fcDummy + k;

            for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
                int u = bStat->vArcMinus[currDum][k][a].first;
                int v = bStat->vArcMinus[currDum][k][a].second;

                exp1 += (bStat->bundleEnd[u])*x[u][v][k];
            }
            for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
                int u = bStat->vArcPlus[currSP][k][a].first;
                int v = bStat->vArcPlus[currSP][k][a].second;
                double trip = (mdist[bStat->lastElement[u]][bStat->firstElement[v]])/inst->vmed;

                exp2 += (bStat->bundleStart[v] - trip)*x[u][v][k];
            }
            sprintf (var, "Constraint7_%d", k);
            IloRange cons = (exp1 - exp2 <= inst->maxTime);
            cons.setName(var);
            model.add(cons);                
        }
        
    //Constraints 8  - calculating ûk
        // for (int k = 0; k < inst->K; k++){
        //     IloExpr exp1(env);
        //     IloExpr exp2(env);
        //     int currDum = fcDummy + k;
        //     exp1 = uf[k];
        //     for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
        //         int u = bStat->vArcMinus[currDum][k][a].first;
        //         int v = bStat->vArcMinus[currDum][k][a].second;

        //         exp2 += (bStat->bundleEnd[u])*x[u][v][k];
        //     }
        //     sprintf (var, "Constraint8_%d", k);
        //     IloRange cons = (exp1 - exp2 == 0);
        //     cons.setName(var);
        //     model.add(cons);

        // }
            
    //Constraints 9 - max driving time

        // for (int k = 0; k < inst->K; k++){
        //     IloExpr exp(env);

        //     exp = uf[k] - u[k];

        //     sprintf (var, "Constraint9_%d", k);
        //     IloRange cons1 = (exp <= inst->maxTime); 
        //     cons1.setName(var);
        //     model.add(cons1);
        // }

// //Constraint 8 - A parcel served has to be delivered

	for (int i = 0; i < bStat->parcelBundleVec.size() - inst->m; i++){ //parcelBundleVec is the size of parcels
		IloExpr exp(env);
		currParcel = inst->n + i;
		for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
            int h = bStat->parcelBundleVec[i][j];
			for (int k = 0; k < inst->K; k++){    
                for (int a = 0; a < bStat->vArcPlus[h][k].size(); a++){
                    int u = bStat->vArcPlus[h][k][a].first;
                    int v = bStat->vArcPlus[h][k][a].second;

					exp += x[u][v][k];
				}
			}
		}
		sprintf (var, "Constraint8_%d", currParcel);
		IloRange cons = (exp <= 1);
		cons.setName(var);
		model.add(cons);
	}	

 //*******************************

    //Forcing constraints
    // IloExpr exp3(env);

    // exp3 = x[30][4][0];

    // sprintf (var, "Constraint10");
    // IloRange cons1 = (exp3 == 1);
    // cons1.setName(var);
    // model.add(cons1); 

    //////////////////////////////////////

    int threads = 1;
	IloCplex bhSARP(model);
	bhSARP.exportModel("bhSARP.lp");
	bhSARP.setParam(IloCplex::Threads, threads);

    bhSARP.setParam(IloCplex::Param::TimeLimit, 7200);

    IloNum start;
    IloNum time;
    start = bhSARP.getTime();
	bhSARP.solve();
    time = (bhSARP.getTime() - start)/threads;

	cout << "\nSol status: " << bhSARP.getStatus() << endl;
	sStat->feasible = bhSARP.isPrimalFeasible();

    // cout << "here" << endl;
    // getchar();
    cout << " Tree_Size: " <<  bhSARP.getNnodes() + bhSARP.getNnodesLeft() + 1 << endl;
    cout << " Total Time: " << time << endl;

	if(sStat->feasible){

        cout << " LB: " << bhSARP.getObjValue() << endl;
        cout << " UB: " << bhSARP.getBestObjValue() << endl;

        solStatIni(sStat);
		cout << "\nObj Val: " << setprecision(15) << bhSARP.getObjValue() << endl;

		sStat->solprofit = bhSARP.getObjValue();

        cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

		for (int k = 0; k < inst->K; k++){
	 		sStat->solvec.push_back(auxPairVec);
		}

		for (int i = 0; i < bStat->bundleVec.size(); i++){
			for(int j = 0; j < bStat->bundleVec.size(); ++j){
				if (bStat->bArcs[i][j] == true){
                    for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                        int k = bStat->arcV[i][j][k1];
						if (bhSARP.getValue(x[i][j][k]) > 0.5){
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
    }
    
	env.end();
}