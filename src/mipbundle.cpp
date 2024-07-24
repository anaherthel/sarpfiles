#include "mipbundle.h"
#include <cstdlib>
#include <stdio.h>

void mipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN;
    int setP;//with passengers
    int csetN;
    int csetP; //with passengers
    int fcDummy = bStat->bundleVec.size() - inst->K;
	int currSP;
	int currParcel;
	int currCluster;
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

    // if (problem->scen == "1AD"){
    //     setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    //     setN = bStat->bundleVec.size() - (2*inst->K);

    //     csetN = cStat->clusterVec.size() - (2*inst->K);
    //     csetP = cStat->clusterVec.size() - (inst->K*2) - inst->m;
    // }
    // else if (problem->scen == "1A"){
        setP = bStat->bundleVec.size() - (2*inst->K);
        setN = setP;

        csetP = cStat->clusterVec.size() - (inst->K*2);
        csetN = csetP;
    // }

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
	for (int i = 0; i < csetP; i++){
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

    for (int i = 0; i < csetP; i++){
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

    // if (problem->scen == "1AD"){
    //     //at most one arc leaves parcel only clusters
    //     for (int i = csetP; i < csetN; i++){
    //         IloExpr exp(env);
    //         for (int k = 0; k < inst->K; k++){
    //             for (int a = 0; a < cStat->vArcPlus[i][k].size(); a++){
    //                 int u = cStat->vArcPlus[i][k][a].first;
    //                 int v = cStat->vArcPlus[i][k][a].second;

    //                 exp += x[u][v][k];
    //             }
    //         }
    //         sprintf (var, "Constraint8_%d", i);
    //         IloRange cons1 = (exp <= 1);
    //         cons1.setName(var);
    //         model.add(cons1);
    //     }
        
    //     //at most one arc arrives at parcel only clusters
    //     for (int i = csetP; i < csetN; i++){
    //         IloExpr exp(env);
    //         for (int k = 0; k < inst->K; k++){
    //             for (int a = 0; a < cStat->vArcMinus[i][k].size(); a++){
    //                 int u = cStat->vArcMinus[i][k][a].first;
    //                 int v = cStat->vArcMinus[i][k][a].second;

    //                 exp += x[u][v][k];
    //             }
    //         }
    //         sprintf (var, "Constraint9_%d", i);
    //         IloRange cons = (exp <= 1);
    //         cons.setName(var);
    //         model.add(cons);
    //     }
    // }

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

	// Constraint 3 - Each used vehicle leaves its starting node
	
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

	// // Constraint 4 - Each used vehicle ends the trip at the dummy node f
	
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
        //for (int k = 0; k < inst->K; k++){
        //    IloExpr exp1(env);
        //    IloExpr exp2(env);
        //    currSP = setN + k;
        //    int currDum = fcDummy + k;

        //    for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
        //        int u = bStat->vArcMinus[currDum][k][a].first;
        //        int v = bStat->vArcMinus[currDum][k][a].second;

        //        exp1 += (bStat->bundleEnd[u])*x[u][v][k];
        //    }
        //    for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
        //        int u = bStat->vArcPlus[currSP][k][a].first;
        //        int v = bStat->vArcPlus[currSP][k][a].second;
        //        double trip = (mdist[bStat->firstElement[u]][bStat->firstElement[v]])/inst->vmed;

        //        exp2 += (bStat->bundleStart[v] - trip)*x[u][v][k];
        //    }
        //    sprintf (var, "Constraint7_%d", k);
        //    IloRange cons = (exp1 - exp2 <= inst->maxTime);
        //    cons.setName(var);
        //    model.add(cons);                
        //}
        
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

    ////cout << "Before constraints" << endl;
    ////Forcing constraints
    //IloExpr expk(env);
    //int a = 59;
    //int b = 9;
    //expk = x[a][b][3];

    //sprintf (var, "Constraint10_1");
    //IloRange cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1); 

    //////////////////////////////////////

    int threads = 1;
	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, threads);

    bSARP.setParam(IloCplex::Param::TimeLimit, 7200);
    // bSARP.setParam(IloCplex::Param::TimeLimit, 10);

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


void mipbundle2(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat){

	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "bSARP");
	int setN;
    int setP;//with passengers
    int csetN;
    int csetP; //with passengers
    int fcDummy = bStat->bundleVec.size() - inst->K;
	int currSP;
	int currParcel;
	int currCluster;
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

    // if (problem->scen == "1AD"){
    //     setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    //     setN = bStat->bundleVec.size() - (2*inst->K);

    //     csetN = cStat->clusterVec.size() - (2*inst->K);
    //     csetP = cStat->clusterVec.size() - (inst->K*2) - inst->m;
    // }
    // else if (problem->scen == "1A"){
        setP = bStat->bundleVec.size() - (2*inst->K);
        setN = setP;

        csetP = cStat->clusterVec.size() - (inst->K*2);
        csetN = csetP;
    // }

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
	for (int i = 0; i < csetP; i++){
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

    for (int i = 0; i < csetP; i++){
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

    // if (problem->scen == "1AD"){
    //     //at most one arc leaves parcel only clusters
    //     for (int i = csetP; i < csetN; i++){
    //         IloExpr exp(env);
    //         for (int k = 0; k < inst->K; k++){
    //             for (int a = 0; a < cStat->vArcPlus[i][k].size(); a++){
    //                 int u = cStat->vArcPlus[i][k][a].first;
    //                 int v = cStat->vArcPlus[i][k][a].second;

    //                 exp += x[u][v][k];
    //             }
    //         }
    //         sprintf (var, "Constraint8_%d", i);
    //         IloRange cons1 = (exp <= 1);
    //         cons1.setName(var);
    //         model.add(cons1);
    //     }
        
    //     //at most one arc arrives at parcel only clusters
    //     for (int i = csetP; i < csetN; i++){
    //         IloExpr exp(env);
    //         for (int k = 0; k < inst->K; k++){
    //             for (int a = 0; a < cStat->vArcMinus[i][k].size(); a++){
    //                 int u = cStat->vArcMinus[i][k][a].first;
    //                 int v = cStat->vArcMinus[i][k][a].second;

    //                 exp += x[u][v][k];
    //             }
    //         }
    //         sprintf (var, "Constraint9_%d", i);
    //         IloRange cons = (exp <= 1);
    //         cons.setName(var);
    //         model.add(cons);
    //     }
    // }

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

	// Constraint 3 - Each used vehicle leaves its starting node
	
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

	// // Constraint 4 - Each used vehicle ends the trip at the dummy node f
	
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
        //for (int k = 0; k < inst->K; k++){
        //    IloExpr exp1(env);
        //    IloExpr exp2(env);
        //    currSP = setN + k;
        //    int currDum = fcDummy + k;

        //    for (int a = 0; a < bStat->vArcMinus[currDum][k].size(); a++){
        //        int u = bStat->vArcMinus[currDum][k][a].first;
        //        int v = bStat->vArcMinus[currDum][k][a].second;

        //        exp1 += (bStat->bundleEnd[u])*x[u][v][k];
        //    }
        //    for (int a = 0; a < bStat->vArcPlus[currSP][k].size(); a++){
        //        int u = bStat->vArcPlus[currSP][k][a].first;
        //        int v = bStat->vArcPlus[currSP][k][a].second;
        //        double trip = (mdist[bStat->firstElement[u]][bStat->firstElement[v]])/inst->vmed;

        //        exp2 += (bStat->bundleStart[v] - trip)*x[u][v][k];
        //    }
        //    sprintf (var, "Constraint7_%d", k);
        //    IloRange cons = (exp1 - exp2 <= inst->maxTime);
        //    cons.setName(var);
        //    model.add(cons);                
        //}
        
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

    ////cout << "Before constraints" << endl;
    ////Forcing constraints
    //IloExpr expk(env);
    //int a = 176;
    //int b = 151;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_1");
    //IloRange cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1); 

    //a = 151;
    //b = 95;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_2");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1); 

    //a = 95;
    //b = 66;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_3");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);   

    //a = 66;
    //b = 27;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_4");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1); 

    //a = 27;
    //b = 46;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_5");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);   

    //a = 46;
    //b = 178;
    //expk = x[a][b][0];

    //sprintf (var, "Constraint10_6");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);   

    //a = 177;
    //b = 18;
    //expk = x[a][b][1];

    //sprintf (var, "Constraint10_7");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);   

    //a = 18;
    //b = 154;
    //expk = x[a][b][1];

    //sprintf (var, "Constraint10_8");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);    

    //a = 154;
    //b = 123;
    //expk = x[a][b][1];

    //sprintf (var, "Constraint10_9");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);   

    //a = 123;
    //b = 179;
    //expk = x[a][b][1];

    //sprintf (var, "Constraint10_10");
    //cons1 = (expk == 1);
    //cons1.setName(var);
    //model.add(cons1);           
    //////////////////////////////////////

    int threads = 1;
	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, threads);

    bSARP.setParam(IloCplex::Param::TimeLimit, 7200);
    // bSARP.setParam(IloCplex::Param::TimeLimit, 10);

    IloNum start;
    IloNum time;
    start = bSARP.getTime();
    bSARP.setOut(env.getNullStream());
	bSARP.solve();
    time = (bSARP.getTime() - start)/threads;

	// cout << "\nSol status: " << bSARP.getStatus() << endl;
	sStat->feasible = bSARP.isPrimalFeasible();

    // cout << "here" << endl;
    // getchar();
    // cout << " Tree_Size: " <<  bSARP.getNnodes() + bSARP.getNnodesLeft() + 1 << endl;
    // cout << " Total Time: " << time << endl;

	if(sStat->feasible){

        // cout << " LB: " << bSARP.getObjValue() << endl;
        // cout << " UB: " << bSARP.getBestObjValue() << endl;

        solStatIni(sStat);
		// cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

		sStat->solprofit = bSARP.getObjValue();

        int initialBundles = 0;

        for (int i = 0; i < bStat->eligibleBundleVec.size(); i++) {
            if (bStat->eligibleBundleVec[i]) {
                initialBundles++;
            }
        }

        sStat->totalBundles = bStat->bundleVec.size();
        sStat->initialBundles = initialBundles;

        // cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

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
				// cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
			}
		}		
    }
    
	env.end();

}




void mipSolStatsPlus (instanceStat *inst, double **mdist, bundleStat *bStat, vector<nodeStat> &nodeVec, solStats *sStat){
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


void fipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat, fipBundleStats *fipStat){

	char var[100];
	IloEnv env;
	IloModel model(env, "b2SARP");
	int currSP;
	//// long M = 2*inst->T;
	//// long M2 = 2*(inst->n + inst->m + 1);
	//// long W = inst->m + 1;
	//// int Q = 5;
	int Q = 4; //q4 fip (only 1 parcel between passengers)
	////int Q = inst->m + 3; //qm fip (multiple parcels between passengers)
    int ref = 3*inst->m;

    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int setPD; //last index of bundles before parcel only
    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    
    int fDummy;

    setPD = bStat->bundleVec.size() - 3*inst->m;
    setN = setPD - (2*inst->K);
    fDummy = setPD - inst->K;

    int fDepot = setN;
	//int decimalPlaces = 4;
    //double multiplier = std::pow(10, decimalPlaces);

    // for (int k = 0; k < inst->K; k++) {
    //     for (int a = 0; a < bStat->vArcPlus[47][k].size(); a++) {
    //         int u = bStat->vArcPlus[47][k][a].first;
    //         int v = bStat->vArcPlus[47][k][a].second;

    //         cout << u << " " << v << " " << k << endl;
    //     }
    //     getchar();
    // }

    // for (int j = 0; j < bStat->bundleVec.size(); j++) {
    //     if (bStat->bArcs[6][j]) {
    //         for (int k1 = 0; k1 < bStat->arcV[47][j].size(); k1++) {
    //             int k = bStat->arcV[47][j][k1];

    //             cout << 6 << " " << j << " " << k << endl;
    //         }
    //     }
    //     // for (int a = 0; a < bStat->vArcPlus[6][k].size(); a++) {
    //     //     int u = bStat->vArcPlus[6][k][a].first;
    //     //     int v = bStat->vArcPlus[6][k][a].second;

    //     //     cout << u << " " << v << " " << k << endl;
    //     // }
    //     // getchar();
    // }

    // for (int k = 0; k < bStat->vArcPlus.size(); k++) {
    //     for (int i = 0; i < bStat->vArcPlus[k].size(); i++) {
    //         for (int a = 0; a < bStat->vArcPlus[k][i].size(); a++) {
    //             int u = bStat->vArcPlus[k][i][a].first;
    //             int v = bStat->vArcPlus[k][i][a].second;

    //             cout << u << " " << v << " " << k << endl;
    //         }
    //         getchar();
    //     }
    // }
	
	//vector< pair<int, int> > auxPairVec;
	//pair<int, int> auxPair;

	//Creating variable (i in set of assigned passengers)
    //Reduce number of variables
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

	IloArray <IloNumVarArray> b(env, inst->K);
	// cout << "Size of solpass: " << fipStat->solPass.size() << endl;
	for (int k = 0; k < fipStat->solPass.size(); k++){
		// cout << "Size of solpass K: " << fipStat->solPass[k].size() << endl;
		b[k] = IloNumVarArray (env, bStat->bundleVec.size(), 0, inst->T);
		for (int i = 0; i < fipStat->solPass[k].size(); i++){
			int u = fipStat->solPass[k][i];
			sprintf(var, "b(%d,%d)", k, u);
			b[k][u].setName(var);
			model.add(b[k][u]);
		}
	}

	// Variable start of service time for parcels
    int sizeofs = bStat->bundleVec.size() - setPD;
	IloNumVarArray s(env, sizeofs, 0, inst->T);
	for (int i = setPD; i < bStat->bundleVec.size(); i++){
		sprintf(var, "s(%d)", i);
		s[i - setPD].setName(var);
		model.add(s[i - setPD]);
	}

	IloExpr objFunction(env);

	// objFunction += inst->totalCustomProfit;

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			int u = fipStat->solPass[k][i]; //bundle
			int v = fipStat->solPass[k][i + 1]; //bundle
            int a = bStat->lastElement[u];
            int b = bStat->firstElement[v];

			for(int j = setPD; j < bStat->bundleVec.size(); j++){ // j is a bundle
                int c = bStat->firstElement[j];
                int d = bStat->lastElement[j];     
				if (bStat->bArcs[u][j] != true){
					continue; // If arc i to j is invalid
				}
				double deltaij = (mdist[a][c]) + (mdist[d][b]) - (mdist[a][b]);

				double profitj = bStat->bundleProfVec[j];
				// objFunction += (inst->minpar + (inst->parkm*mdist[j][j+inst->m]) - (inst->costkm*deltaij)) * x[u][j][k];
				objFunction += (profitj - (inst->costkm*deltaij)) * x[u][j][k];
			}
		}
    }

	model.add(IloMaximize(env, objFunction));
  

	//Constraint 1 - Max of one arc coming out of any of the bundles related to a certain parcel (23)
	for(int i = 0; i < bStat->parcelBundleVec.size(); i++){
		IloExpr exp(env);
        for (int a = 0; a < bStat->parcelBundleVec[i].size(); a++){
            //for (int b = 0; b < bStat->bundleVec.size(); b++){
            for (int k = 0; k < fipStat->solPass.size(); k++){
                for (int b = 0; b < fipStat->solPass[k].size(); b++){
                    int u = bStat->parcelBundleVec[i][a];
                    int v = fipStat->solPass[k][b];
                    
                    if (u == v){
                        continue;
                    }
                    if (bStat->bArcs[v][u] == true){
                        for (int k = 0; k < inst->K; k++){
                            exp += x[v][u][k];
                        }                    
                    }
                    
                    if (bStat->bArcs[u][v] == true){
                        for (int k = 0; k < inst->K; k++){
                            exp += x[u][v][k];
                        } 
                    }
                }               
            }
        }
		sprintf (var, "Constraint1_%d", i + inst->n);
        IloRange cons = (exp <= 1);

        if (fipStat->servedParcelsInit[i] > 0){
            cons = (exp <= 0);
        }
		
		cons.setName(var);
		model.add(cons);
	}

	////Constraint 2 - At most 1 parcel between passenger nodes (or starting and passenger) (24)
	////remove it for fip qm

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}

		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			IloExpr exp(env);
			int u = fipStat->solPass[k][i];
			for(int j = setPD; j < bStat->bundleVec.size(); j++){
                if (bStat->bArcs[u][j] != true){
                    continue;
                }
				exp += x[u][j][k];
			}

			sprintf (var, "Constraint2_%d_%d", k, u);
			IloRange cons = (exp <= 1);
			cons.setName(var);
			model.add(cons);
		}
	}

    ////Constraint 2.5 - a bundle request can not be in a different vehicle

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}

		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
			IloExpr exp(env);
			int u = fipStat->solPass[k][i];
			for(int j = setPD; j < bStat->bundleVec.size(); j++){
                if (bStat->bArcs[u][j] != true){
                    continue;
                }

                for (int k1 = 0; k1 < bStat->arcV[u][j].size(); k1++) {
                    int k2 = bStat->arcV[u][j][k1];

                    if (k2 != k) {
                        exp += x[u][j][k2];
                    }
                }
			}

			sprintf (var, "Constraint2.5_%d_%d", k, u);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);
		}
	}
    // cout << "Define constraints 2" << endl;
	// Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle (25)

	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}        
		for(int j = 0; j < fipStat->bundlesPonly.size(); j++){
            int a = fipStat->bundlesPonly[j];
            int b = fipStat->bundlesDonly[j];
            //cout << "Parcel: " << j + inst->n << " a: " << a << " b: " << b << endl;
			IloExpr exp1(env);
			IloExpr exp2(env);
			for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
				int u = fipStat->solPass[k][i];
                if (bStat->bArcs[u][a] != true){
                    continue;
                }
				exp1 +=  x[u][a][k];
			}
			for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
				int u = fipStat->solPass[k][i];
                if (bStat->bArcs[u][b] != true){
                    continue;
                }
				exp2 +=  x[u][b][k];
			}

			sprintf (var, "Constraint3_%d_%d", j + inst->n, k);
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
		for(int j = 0; j < fipStat->bundlesPonly.size(); j++){
            int a = fipStat->bundlesPonly[j];
            int b = fipStat->bundlesDonly[j];            
			for (int i = 1; i < fipStat->solPass[k].size(); i++){
				int u = fipStat->solPass[k][i];
				IloExpr exp1(env);
				IloExpr exp2(env);

				for (int f = 0; f < i; f++){
					int v = fipStat->solPass[k][f];
                    if (bStat->bArcs[v][a] != true){
                        continue;
                    }
					exp1 += x[v][a][k];
				}

				for (int f = 0; f < i; f++){
					int v = fipStat->solPass[k][f];
                    if (bStat->bArcs[v][b] != true){
                        continue;
                    }                    
					exp2 += x[v][b][k];
				}                

				sprintf (var, "Constraint4_%d_%d_%d", k, a, u);
				IloRange cons = ((exp1-exp2) >= 0);
				cons.setName(var);
				model.add(cons);	
			}
		}
	}

	// Constraint 5 - TW constraint (27) // continue

    for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
        for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
            
			int b1 = fipStat->solPass[k][i];
			int b2 = fipStat->solPass[k][i + 1];
			int u = bStat->lastElement[b1];
			int v = bStat->firstElement[b2];  

            // cout << b1 << " ";   

			IloExpr exp1(env);
			IloExpr exp2(env);

			// exp1 = b[k][v] - b[k][u] - (mdist[u][v]/inst->vmed);
			double cvalue = mdist[u][v]/inst->vmed;
			//cvalue = std::round(cvalue * multiplier) / multiplier;			
			exp1 = b[k][b2] - (b[k][b1]) - bStat->bundleServVec[b1] - (cvalue);

			//for(int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){
            for(int j = setPD; j < bStat->bundleVec.size(); j++){
			    int fj = bStat->firstElement[j];   
                int lj = bStat->lastElement[j];

				double deltaij = (mdist[u][fj]) + (mdist[lj][v]) - (mdist[u][v]);
				double cvalue2 = deltaij/inst->vmed;
				//cvalue2 = std::round(cvalue2 * multiplier) / multiplier;	
				double deltatime = (cvalue2) + bStat->bundleServVec[j];

				// double deltatime = (deltaij/inst->vmed);
                if (bStat->bArcs[b1][j] != true || bStat->bArcs[j][b2] != true){
                    continue;
                }
				exp2 += deltatime*x[b1][j][k];
			}
            
			sprintf (var, "Constraint5_%d_%d", k, u);
			IloRange cons = (exp1 - exp2 >= 0);
			cons.setName(var);
			model.add(cons);
        }
        // cout << endl;
    }

	// Constraint 6 - TW constraint (28)

    for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
        for (int i = 0; i < fipStat->solPass[k].size() - 1; i++){
            int b1 = fipStat->solPass[k][i];
            int u = bStat->lastElement[b1];
			for(int j = setPD; j < bStat->bundleVec.size(); j++){
			    int fj = bStat->firstElement[j];   
                int lj = bStat->lastElement[j];                
				IloExpr exp1(env);
				IloExpr exp2(env);

				double cvalue2 = mdist[u][fj]/inst->vmed;

				//cvalue2 = std::round(cvalue2 * multiplier) / multiplier;
                if (bStat->bArcs[b1][j] != true){
                    continue;
                }
				exp1 = s[j-setPD] - (b[k][b1] + bStat->bundleServVec[b1]) - (cvalue2)*x[b1][j][k];

				// exp1 = s[j] - b[k][u] - (mdist[u][j]/inst->vmed)*x[u][j][k];
				double bigM = nodeVec[u].l + bStat->bundleServVec[b1];
				exp2 = bigM*(x[b1][j][k] - 1);

				sprintf (var, "Constraint6_%d_%d_%d", k, b1, j);
				IloRange cons = (exp1 - exp2 >= 0);
				cons.setName(var);
				model.add(cons);

			}
        }
    }

	//////// Constraint 7 - Maximum driving time (29)

	//////for (int k = 0; k < fipStat->solPass.size(); k++){
	//////	if (fipStat->solPass[k].size() < 3){
	//////		continue;
	//////	}
	//////	IloExpr exp(env);
	//////	int u = fipStat->solPass[k][0];
	//////	int v = fipStat->solPass[k][fipStat->solPass[k].size()-1];

	//////	exp = b[k][v] - b[k][u];
	//////	// exp = b[k][v] - b[k][u];

	//////	sprintf (var, "Constraint7_%d", k);
	//////	IloRange cons = (exp <= inst->maxTime);
	//////	cons.setName(var);
	//////	model.add(cons);
	//////}

	// Constraint 8 - Time windows (30)


	for (int k = 0; k < fipStat->solPass.size(); k++){
		if (fipStat->solPass[k].size() < 3){
			continue;
		}
		for (int i = 0; i < fipStat->solPass[k].size(); i++){
            int b1 = fipStat->solPass[k][i];
            int u = bStat->lastElement[b1];            
			IloExpr exp(env);
			
			//int u = fipStat->solPass[k][i];

            double endTime = (b1 < setN? bStat->bundleStart[b1] : bStat->bundleEnd[b1]);
            double startTime = (b1 < setN? bStat->bundleStart[b1] : bStat->bundleStart[b1]);

			exp = b[k][b1];

			sprintf (var, "Constraint8_%d_%d", b1, k);
			IloRange cons1 = (exp <= endTime);
			cons1.setName(var);
			model.add(cons1);
			
			sprintf (var, "Constraint9_%d_%d", b1, k);
			IloRange cons2 = (startTime <= exp);
			cons2.setName(var);
			model.add(cons2);
		}
			
	}

	//parcels TW (31)
	for (int j = setPD; j < bStat->bundleVec.size(); j++){
		IloExpr exp(env);

		exp = s[j-setPD];

		sprintf (var, "Constraint10_%d", j);
		IloRange cons1 = (exp <= bStat->bundleEnd[j]);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint11_%d", j);
		IloRange cons2 = (bStat->bundleStart[j] <= exp);
		cons2.setName(var);
		model.add(cons2);
	}

	//// //Passengers cannot exceed max service time (2*ti,i+sigma) (32)
	//// // for (int k = 0; k < fipStat->solPassOrigins.size(); k++){
	//// // 	if (fipStat->solPassOrigins[k].size() < 1){
	//// // 		continue;
	//// // 	}
	//// // 	for (int i = 0 ; i < fipStat->solPassOrigins[k].size(); i++){
	//// // 		IloExpr exp(env);
			
	//// // 		int u = fipStat->solPassOrigins[k][i].first;
	//// // 		int posu = fipStat->solPassOrigins[k][i].second;
	//// // 		int v = fipStat->solPass[k][posu + 1];

	//// // 		exp = b[k][v] - b[k][u] + inst->service;

	//// // 		sprintf (var, "Constraint12_%d_%d", k, u);
	//// // 		IloRange cons = (exp <= 2*(mdist[u][u+inst->n]/inst->vmed)+2*inst->service);
	//// // 		cons.setName(var);
	//// // 		model.add(cons);
	//// // 	}
	//// // }

	////Load constraints (33) Maybe remove it

	//for (int k = 0; k < fipStat->solPass.size(); k++){
	//	if (fipStat->solPass[k].size() < 3){
	//		continue;
	//	}
    //    for (int i = 1; i < fipStat->solPass[k].size(); i++){//maybe start from i = 0
	//		IloExpr exp1(env);
	//		IloExpr exp2(env);

	//		int u = fipStat->solPass[k][i];

	//		for (int f = 0; f < i; f++){ 
	//			int v = fipStat->solPass[k][f];
	//			exp2 += nodeVec[v].load;

	//			for (int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){
					
	//				exp1 += nodeVec[j].load*x[v][j][k];
	//			}

	
	//		}
	//		sprintf (var, "Constraint12_%d_%d", k, u);
	//		IloRange cons = ((exp1+exp2) <= Q);
	//		cons.setName(var);
	//		model.add(cons);
	//	}
	//}
	////test constraints

	////IloExpr exp(env);
	////exp = x[22][8][0];

	////sprintf (var, "Constraint14");

	////IloRange cons = (exp == 1);
	////cons.setName(var);
	////model.add(cons);

	////exp = x[8][1][0];

	////sprintf (var, "Constraint15");

	////IloRange cons = (exp == 1);
	////cons.setName(var);
	////model.add(cons);

	int threads;

    threads = 1;

    // cout << "\nThreads: " << threads << endl;

	IloCplex b2SARP(model);
	b2SARP.exportModel("fipbundleSARP.lp");
	b2SARP.setParam(IloCplex::Threads, threads);
	b2SARP.setParam(IloCplex::Param::TimeLimit, 7200);
	//// b2SARP.setParam(IloCplex::Param::Preprocessing::Aggregator, 0);
	//// b2SARP.setParam(IloCplex::Param::MIP::Display, 5);
	//// b2SARP.setParam(IloCplex::Param::Advance, 2);

	////MIP Start
	//// IloNumVarArray startVar(env);
    //// IloNumArray startVal(env);
	//// for (int k = 0; k < fipStat->solPass.size(); k++){
	//// 	for (int i = 0; i < fipStat->solPass[k].size()-1; i++){
	//// 		int u = fipStat->solPass[k][i];
	//// 		int v = fipStat->solPass[k][i + 1];

	//// 		startVar.add(x[u][v][k]);
	//// 		startVal.add(1);
	//// 	}
	//// }
	//// b2SARP.addMIPStart(startVar, startVal);


	//// IloNumVarArray startVar2(env);
    //// IloNumArray startVal2(env);
	//// for (int k = 0; k < fipStat->solPass.size(); k++){
	//// 	for (int i = 0; i < fipStat->solPass[k].size()-1; i++){
	//// 		int u = fipStat->solPass[k][i];

	//// 		startVar2.add(b[k][u]);
	//// 		startVal2.add(sStat->solBegin[u]);
	//// 	}
	//// }
	//// b2SARP.addMIPStart(startVar2, startVal2);
	//// end of MIP start

    IloNum start;
    IloNum time;
    start = b2SARP.getTime();
    b2SARP.setOut(env.getNullStream());
	b2SARP.solve();
    time = (b2SARP.getTime() - start)/threads;
	// cout << "\nSol status: " << b2SARP.getStatus() << endl;
	sStat->feasible = b2SARP.isPrimalFeasible();

    // cout << " Tree_Size: " <<  b2SARP.getNnodes() + b2SARP.getNnodesLeft() + 1 << endl;
    // cout << " Total Time: " << time << endl;

	vector< pair<int, int> > auxPairVec;
    pair<int, int> auxPair;

	if (sStat->feasible){
        // cout << " LB: " << b2SARP.getObjValue() << endl;
        // cout << " UB: " << b2SARP.getBestObjValue() << endl;
        fipStat->solprofit = b2SARP.getObjValue();
        sStat->time = time;

        map<int, bool> inSolution;
        map<int, pair<int, int>> pairSequence;

        startAux(inSolution, pairSequence, fipStat);

        solStatIni(sStat);
		// cout << "\nObj Val: " << setprecision(15) << b2SARP.getObjValue() << endl;

		sStat->solprofit = b2SARP.getObjValue();
        sStat->solDual = b2SARP.getBestObjValue();
        sStat->time = time;

        if (((b2SARP.getBestObjValue() - b2SARP.getObjValue())/b2SARP.getBestObjValue()) * 100 < 0.01) {
			sStat->status = "Optimal";
		} else {
			sStat->status = "Feasible";
		}

        // cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

        sStat->solvec.clear();

		for (int k = 0; k < inst->K; k++){
	 		sStat->solvec.push_back(auxPairVec);
		}

		for (int i = 0; i < bStat->bundleVec.size(); i++){
			for(int j = 0; j < bStat->bundleVec.size(); ++j){
				if (bStat->bArcs[i][j] == true){
                    for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                        int k = bStat->arcV[i][j][k1];
						if (b2SARP.getValue(x[i][j][k]) > 0.5){
							auxPair.first = i;
							auxPair.second = j;
							sStat->solvec[k].push_back(auxPair);
                            inSolution[auxPair.first] = true;
							// cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
							// getchar();
						}
					}
				}
            }
		}

        for (int k = 0; k < fipStat->solPass.size(); k++) {
            for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
                int node = fipStat->solPass[k][i];

                if (!inSolution[node]) {
                    int vehicle = pairSequence[node].first;
                    auxPair.first = node;
                    auxPair.second = pairSequence[node].second;

                    sStat->solvec[vehicle].push_back(auxPair);
                }
            }
        }

        for (int k = 0; k < inst->K; k++){
			for (int i = 0; i < sStat->solvec[k].size(); i++){
                auxPair.first = sStat->solvec[k][i].first;
                auxPair.second = sStat->solvec[k][i].second;

				if (auxPair.second >= setPD) {
                    int prevCustomer = auxPair.first;
                    auxPair.first = auxPair.second;
                    auxPair.second = pairSequence[prevCustomer].second;

                    sStat->solvec[k].push_back(auxPair);
                }
			}
		}
		
		for (int k = 0; k < inst->K; k++){
			for (int i = 0; i < sStat->solvec[k].size(); i++){
				// cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ") = " << b2SARP.getValue(x[sStat->solvec[k][i].first][sStat->solvec[k][i].second][k]) << endl;
			}
		}

		// //new addition
		// fipStat->solBegin.clear();

		// for (int i = 0; i < bStat->bundleVec.size(); i++){
		// 	fipStat->solBegin.push_back(-1);
		// }
		// //end

        // for (int k = 0; k < inst->K; k++){
		// 	fipStat->solvec.push_back(auxPairVec);
        //     // sStat->solvec.push_back(auxPairVec);
        // }
		
        // for (int i = 0; i < bStat->bundleVec.size(); i++){
        //     if (i >= setN && i < setPD){
        //         continue;
        //     }
        //     for(int j = 0; j < bStat->bundleVec.size(); j++){     
        //         if (j >= setN && j < setPD){
        //             continue;
        //         }                           
        //         if (bStat->bArcs[i][j] == true){
        //             for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
        //                 int k = bStat->arcV[i][j][k1];
        //                 if (b2SARP.getValue(x[i][j][k]) > 0.5){
        //                     auxPair.first = i;
        //                     auxPair.second = j;
        //                     fipStat->solvec[k].push_back(auxPair);
        //                     cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
        //                     // getchar();
        //                 }
        //             }
        //         }
        //     }   
        // }

		// for (int k = 0; k < inst->K; k++){
		// 	int i = fDepot+k;
        //     for(int j = 0; j < bStat->bundleVec.size(); j++){                
        //         if (bStat->bArcs[i][j] == true){
		// 			if (b2SARP.getValue(x[i][j][k]) > 0.5){
		// 				auxPair.first = i;
		// 				auxPair.second = j;
		// 				fipStat->solvec[k].push_back(auxPair);
		// 				cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
		// 				// getchar();
		// 			}
        //         }
        //     }   
        // }

        // for (int i = 0; i < setPD; i++){
		// 	// cout << "i: " << endl;
        //     for(int j = 0; j < bStat->bundleVec.size(); j++){
		// 		// cout << "j: " << j << endl;             
        //         if (bStat->bArcs[i][j] == true){
        //             for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
        //                 int k = bStat->arcV[i][j][k1];
		// 				// cout << "k: " << k << endl;
        //                 if (b2SARP.getValue(x[i][j][k]) > 0.5){
		// 					// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
		// 					fipStat->solBegin[i] = b2SARP.getValue(b[k][i]);
		// 					cout << "\nb(" << k << ", " << i << "): " << b2SARP.getValue(b[k][i]);
		// 					// getchar();
        //                     cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
        //                     // getchar();
        //                 }
        //             }
        //         }
        //     }   
        // }

		//for (int k = 0; k < inst->K; k++){
		//	int i = fDepot+k;
        //    for(int j = setPD; j < bStat->bundleVec.size(); j++){                
        //        if (bStat->bArcs[i][j] == true){
		//			if (b2SARP.getValue(x[i][j][k]) > 0.5){
		//				// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
		//				fipStat->solBegin[i] = b2SARP.getValue(b[k][i]);
		//				cout << "\nb(" << k << ", " << i << "): " << b2SARP.getValue(b[k][i]);
		//				// getchar();
		//				cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
		//				// getchar();
		//			}
        //        }
        //    }   
        //}

	//	// for (int k = 0; k < inst->K; k++){
	//	// 	for (int i = 0; i < 2*inst->n; i++){
	//	// 		// fipStat->solBegin.push_back(nSARP.getValue(b[k][i]));
	//	// 		cout << "\nb(" << k << ", " << i << "): " << nSARP.getValue(b[k][i]);
	//	// 		// getchar();
	//	// 		// cout << i << " " << j << " " << k << ": " << nSARP.getValue(x[i][j][k]) << endl;
	//	// 		// getchar();
	//	// 	}
	//	// }


		//for (int j = setPD; j < bStat->bundleVec.size(); j++){
		//	// cout << "i: " << endl;
        //    for(int i = 0; i < setPD; i++){
		//		// cout << "j: " << j << endl;             
        //        if (bStat->bArcs[i][j] == true){
        //            for (int k = 0; k < inst->K; k++){
		//				// cout << "k: " << k << endl;
        //                if (b2SARP.getValue(x[i][j][k]) > 0.5){
		//					fipStat->solBeginParcel.push_back(b2SARP.getValue(s[j]));
		//					cout << "\ns(" << j << "): " << b2SARP.getValue(s[j]) << " - ";
		//					// getchar();
        //                    cout << i << " " << j << " " << k << ": " << b2SARP.getValue(x[i][j][k]) << endl;
        //                    // getchar();
        //                }
        //            }
        //        }
        //    }   
        //}
		//cout << endl;
        // printResults(inst, mdist, sStat, nodeVec);

	}
	else{
		// cout << "\n\nServed parcels: " << 0 << endl;
	}
	env.end();
    // startVal.end();
    // startVar.end();
	// startVal2.end();
    // startVar2.end();
}

void mfipbundle(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, bundleStat *bStat, clSt *cStat, probStat* problem, solStats *sStat, fipBundleStats *fipStat){

	char var[100];
	IloEnv env;
	IloModel model(env, "b2SARP");
	int currSP;
	//// long M = 2*inst->T;
	//// long M2 = 2*(inst->n + inst->m + 1);
	//// long W = inst->m + 1;
	//// int Q = 5;
	int Q = 4; //q4 fip (only 1 parcel between passengers)
	////int Q = inst->m + 3; //qm fip (multiple parcels between passengers)
    int ref = 3*inst->m;

    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int setPD; //last index of bundles before parcel only
    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    
    int fDummy;
    int M = 2*inst->T;

    setPD = bStat->bundleVec.size() - 3*inst->m;
    setN = setPD - (2*inst->K);
    fDummy = setPD - inst->K;

    int fDepot = setN;

    // int csetP = cStat->clusterVec.size() - (inst->K*2);
    // int csetN = csetP;
	//int decimalPlaces = 4;
    //double multiplier = std::pow(10, decimalPlaces);
	
	//vector< pair<int, int> > auxPairVec;
	//pair<int, int> auxPair;



	//Creating variable (i in set of assigned passengers)
    //Reduce number of variables
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

	IloNumVarArray b(env, bStat->bundleVec.size(), 9, inst->T);
	// cout << "Size of solpass: " << fipStat->solPass.size() << endl;
    // cout << "Size of solpass K: " << fipStat->solPass[k].size() << endl;

    // for (int k = 0; k < fipStat->solPass.size(); k++) {
    //     for (int i = 0; i < fipStat->solPass[k].size(); i++){
    //         int u = fipStat->solPass[k][i];
    //         sprintf(var, "b(%d)", u);
    //         b[u].setName(var);
    //         model.add(b[u]);
    //     }
    // }

    for (int k = 0; k < fipStat->solPass.size(); k++) {
        for (int i = 1; i < fipStat->solPass[k].size() - 1; i++) {
            int u = fipStat->solPass[k][i];

        	sprintf(var, "b(%d)", u);
            b[i].setName(var);
            model.add(b[u]);
        }
    }

    for (int i = setN; i < bStat->bundleVec.size(); i++){
        sprintf(var, "b(%d)", i);
        b[i].setName(var);
        model.add(b[i]);
    }

	//Creating objective function
	
	IloExpr objFunction(env);

    objFunction -= fipStat->solprofit;

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

    //Constraint 1 - all solution bundles should be used
    for (int k = 0; k < inst->K; k++) {
        // cout << k << endl;
        // cout << inst->K << endl;
        // getchar();

        if (fipStat->solPass[k].size() < 3) {
            IloExpr exp(env);

            exp += x[fipStat->solPass[k][0]][fipStat->solPass[k][1]][k];

            sprintf (var, "Constraint1_%d_%d", k, 0);
            IloRange cons = (exp == 1);
            cons.setName(var);
		    model.add(cons);
        }
        else
        {
            for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
                IloExpr exp(env);

                int node = fipStat->solPass[k][i];

                for (int a = 0; a < bStat->vArcPlus[node][k].size(); a++) {
                    int u = bStat->vArcPlus[node][k][a].first;
                    int v = bStat->vArcPlus[node][k][a].second;

                    exp += x[u][v][k];
                }

                sprintf (var, "Constraint1_%d_%d", k, i);
                IloRange cons = (exp == 1);
                cons.setName(var);
                model.add(cons);
            }
        }
    }

    //Constraint 2 - flow conservation for bundles
    for (int i = 0; i < bStat->bundleVec.size(); i++) {
        if (i >= setN && i < setPD) {
            continue;
        }
        for (int k = 0; k < inst->K; k++) {
            IloExpr exp1(env);
            IloExpr exp2(env);

            for (int a = 0; a < bStat->vArcPlus[i][k].size(); a++) {
                int u = bStat->vArcPlus[i][k][a].first;
                int v = bStat->vArcPlus[i][k][a].second;

                exp1 += x[u][v][k];
            }

            for (int a = 0; a < bStat->vArcMinus[i][k].size(); a++) {
                int u = bStat->vArcMinus[i][k][a].first;
                int v = bStat->vArcMinus[i][k][a].second;

                exp2 += x[u][v][k];
            }

            sprintf (var, "Constraint2_%d_%d", i, k);
            IloRange cons = (exp1 - exp2 == 0);
            cons.setName(var);
		    model.add(cons);
        }
    }
  
    //Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

	for (int i = 0; i < fipStat->bundlesPonly.size(); i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);

            int u1 = fipStat->bundlesPonly[i];
            int v1 = fipStat->bundlesDonly[i];

			//Left side: arc leaves i
			for (int a = 0; a < bStat->vArcPlus[u1][k].size(); a++){
                int u = bStat->vArcPlus[u1][k][a].first;
                int v = bStat->vArcPlus[u1][k][a].second;

				exp1 += x[u][v][k];
			}
			//Right side: arc leaves i + m
			for (int a = 0; a < bStat->vArcPlus[v1][k].size(); a++){
				int u = bStat->vArcPlus[v1][k][a].first;
                int v = bStat->vArcPlus[v1][k][a].second;

                exp2 += x[u][v][k];
			}
			sprintf (var, "Constraint3_%d_%d_%d", u1, v1, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	//Constraint 4 - Max of one arc coming out of any of the bundles related to a certain parcel (23)
	for(int i = 0; i < bStat->parcelBundleVecWithP.size(); i++){
		IloExpr exp(env);
        for (int a = 0; a < bStat->parcelBundleVecWithP[i].size(); a++){
            int p = bStat->parcelBundleVecWithP[i][a];
            
            for (int k = 0; k < inst->K; k++) {
                for (int b = 0; b < bStat->vArcPlus[p][k].size(); b++) {
                    int u = bStat->vArcPlus[p][k][b].first;
                    int v = bStat->vArcPlus[p][k][b].second;

                    if (!bStat->bArcs[u][v]) {
                        continue;
                    }

                    exp += x[u][v][k];
                }
            }
        }

        for (int a = 0; a < bStat->parcelBundleVecWithD[i].size(); a++){
            int p = bStat->parcelBundleVecWithD[i][a];
            
            for (int k = 0; k < inst->K; k++) {
                for (int b = 0; b < bStat->vArcPlus[p][k].size(); b++) {
                    int u = bStat->vArcPlus[p][k][b].first;
                    int v = bStat->vArcPlus[p][k][b].second;

                    if (!bStat->bArcs[u][v]) {
                        continue;
                    }

                    exp += x[u][v][k];
                }
            }
        }
		sprintf (var, "Constraint4_%d", i + inst->n);

        IloRange cons = (exp <= 2);
		
		cons.setName(var);
		model.add(cons);
	}

    // cout << inst->K << endl;
    // for (int i = 0; i < bStat->cBundles.size(); i++) {
    //     cout << "cluster " << i << ":" << endl;
    //     for (int b = 0; b < bStat->cBundles[i].size(); b++) {
    //         cout << bStat->cBundles[i][b] << " ";
    //     }
    //     cout << endl;
    // }
    // getchar();

    // Constraint 5 - only one arc leaves the cluster
    for (int i = 0; i < inst->n + inst->K; i++){
		IloExpr exp(env);
        for (int b = 0; b < bStat->cBundles[i].size(); b++) {
            int b1 = bStat->cBundles[i][b];

            for (int k = 0; k < inst->K; k++){
                for (int a = 0; a < bStat->vArcPlus[b1][k].size(); a++){
                    // cout << "aqui" << endl;
                    int u = bStat->vArcPlus[b1][k][a].first;
                    int v = bStat->vArcPlus[b1][k][a].second;
                    // cout << "aqu2" << endl;

                    // if (!bStat->bArcs[u][v]) {
                    //     continue;
                    // }

                    exp += x[u][v][k];
                }
            }
        }
		sprintf (var, "Constraint5_%d", i);
		IloRange cons1 = (exp == 1);
		cons1.setName(var);
		model.add(cons1);
	}

    for (int i = 0; i < inst->n + 2*inst->K; i++){
        if (i >= inst->n && i < inst->n + inst->K) {
            continue;
        }
        
        IloExpr exp(env);
        for (int b = 0; b < bStat->cBundles[i].size(); b++) {
            int b1 = bStat->cBundles[i][b];

            for (int k = 0; k < inst->K; k++){
                for (int a = 0; a < bStat->vArcMinus[b1][k].size(); a++){
                    int u = bStat->vArcMinus[b1][k][a].first;
                    int v = bStat->vArcMinus[b1][k][a].second;

                    exp += x[u][v][k];
                }
            }
        }
        sprintf (var, "Constraint5_%d", i);
        IloRange cons = (exp == 1);
        cons.setName(var);
        model.add(cons);
    }

	//Constraint 6 - service of pickup must come before the delivery

	for (int i = 0; i < fipStat->bundlesPonly.size(); i++){
        
        int u = fipStat->bundlesPonly[i];
        int v = fipStat->bundlesDonly[i];

		IloExpr exp(env);
		exp = b[u] - b[v];

		sprintf (var, "Constraint6_%d", i);
		IloRange cons = (exp <= 0);
		cons.setName(var);
		model.add(cons);
	}

	//Constraints 7 - TW 

    // cout << M << endl;
    // getchar();

    // for (int i = 0; i < bStat->bundleVec.size(); i++) {
    //     cout << i << " " << bStat->bundleServVec[i] << endl;
    // }
    // getchar();

    // for (int i = 0; i < setN; i++) {
    //     if (bStat->bundleStart[i] + bStat->bundleServVec[i] != bStat->bundleEnd[i])
    //         cout << bStat->bundleStart[i] + bStat->bundleServVec[i] << " VS " << bStat->bundleEnd[i] << endl;
    // }
    // getchar();

	for (int a = 0; a < bStat->bArcVec.size(); a++){
		IloExpr exp(env);
		IloExpr sumX(env);
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;

        int u = bStat->lastElement[i];
        int v = bStat->firstElement[j];

        if (!bStat->bArcs[i][j]) {
            continue;
        }

        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
            sumX += x[i][j][k];
		}

        double cvalue = mdist[u][v]/inst->vmed;
        //cvalue = std::round(cvalue * multiplier) / multiplier;
        //cvalue = timeRound(cvalue);
        exp = b[i] - b[j] + bStat->bundleServVec[i] + (cvalue) - M * (1 - sumX);
        sprintf (var, "Constraint7_%d_%d", i, j);
        IloRange cons = (exp <= 0);
        cons.setName(var);
        model.add(cons);			

	}

	// Constraint 8 - ensure solution sequence
	for (int k = 0; k < fipStat->solPass.size(); k++) {
		for (int i = 0; i < fipStat->solPass[k].size() - 1; i++) {
			int u = fipStat->solPass[k][i];
			int v = fipStat->solPass[k][i + 1];
			IloExpr exp(env);

            int i1 = bStat->lastElement[u];
            int j1 = bStat->firstElement[v];

			exp += b[v] - b[u] - bStat->bundleServVec[u] - mdist[i1][j1]/inst->vmed;

			sprintf (var, "Constraint8_%d_%d", k, i);
			IloRange cons = (exp >= 0);
			cons.setName(var);
			model.add(cons);
		}
	}

    // // Constraint 9 - time service only when bundle is used
    // for (int i = 0; i < bStat->bundleVec.size(); i++) {
    //     IloExpr exp(env);

    //     exp += b[i];

    //     for (int k = 0; k < inst->K; k++) {
    //         for (int a = 0; a < bStat->vArcPlus[i][k].size(); a++) {
    //             int u = bStat->vArcPlus[i][k][a].first;
    //             int v = bStat->vArcPlus[i][k][a].second;

    //             if (!bStat->bArcs[u][v]) {
    //                 continue;
    //             }

    //             exp -= M*x[u][v][k];
    //         }
    //     }

    //     sprintf (var, "Constraint9_%d", i);
    //     IloRange cons = (exp <= bStat->bundleStart[i]);
    //     cons.setName(var);
    //     model.add(cons);
    // }

	// // // //Constraints 10 - load constraints

	// // for (int a = 0; a < nas->allArcs.size(); a++){
		
	// // 	IloExpr exp(env);
	// // 	IloExpr exp2(env);
	// // 	IloExpr sumX(env);
    // //     int i = nas->allArcs[a].first;
    // //     int j = nas->allArcs[a].second;

    // //     for (int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
    // //         int k = nas->arcV[i][j][k1];
	// // 		sumX += x[i][j][k];
	// // 	}

	// // 	exp = w[i] + nodeVec[j].load - W*(1 - sumX);
	// // 	exp2 = w[j];
		
    // //     sprintf (var, "Constraint10_%d_%d", i, j);
		
    // //     IloRange cons1 = (exp2 - exp >= 0);
	// // 	cons1.setName(var);
	// // 	model.add(cons1);
	// // }

	//Constraints 11, 12 and 13 - bound the service beginning time by the earlier and later service times for each node

	// for (int i = 0; i < setN; i++){
	// 	IloExpr exp(env);
	// 	exp = b[i];

    //     // cout << i << " " << bStat->bundleStart[i] << endl;

	// 	sprintf (var, "Constraint11_%d", i);
	// 	IloRange cons1 = (exp == bStat->bundleStart[i]);
	// 	cons1.setName(var);
	// 	model.add(cons1);			
	// }
    // getchar();

    for (int k = 0; k < fipStat->solPass.size(); k++) {
        for (int i = 1; i < fipStat->solPass[k].size() - 1; i++) {
            IloExpr exp(env);

            int u = fipStat->solPass[k][i];

        	exp = b[u];

            // cout << i << " " << bStat->bundleStart[i] << endl;

        	sprintf (var, "Constraint11_%d", u);
        	IloRange cons1 = (exp == bStat->bundleStart[u]);
        	cons1.setName(var);
        	model.add(cons1);	
        }
    }

    for (int i = setN; i < bStat->bundleVec.size(); i++){
		IloExpr exp(env);
		exp = b[i];

		sprintf (var, "Constraint12_%d", i);
		IloRange cons1 = (exp <= bStat->bundleEnd[i]);
		cons1.setName(var);
		model.add(cons1);
		
		sprintf (var, "Constraint13_%d", i);
		IloRange cons2 = (bStat->bundleStart[i] <= exp);
		cons2.setName(var);
		model.add(cons2);			
	}
    //////////////////////////////////////

    int threads = 1;
	IloCplex bSARP(model);
	bSARP.exportModel("bSARP.lp");
	bSARP.setParam(IloCplex::Threads, threads);

    bSARP.setParam(IloCplex::Param::TimeLimit, 7200);
    // bSARP.setParam(IloCplex::Param::TimeLimit, 10);

    IloNum start;
    IloNum time;
    start = bSARP.getTime();
    bSARP.setOut(env.getNullStream());
	bSARP.solve();
    time = (bSARP.getTime() - start)/threads;

	// cout << "\nSol status: " << bSARP.getStatus() << endl;
	sStat->feasible = bSARP.isPrimalFeasible();

    vector< pair<int, int> > auxPairVec;
    pair<int, int> auxPair;

    // cout << "here" << endl;
    // getchar();
    // cout << " Tree_Size: " <<  bSARP.getNnodes() + bSARP.getNnodesLeft() + 1 << endl;
    // cout << " Total Time: " << time << endl;

	if(sStat->feasible){

        // cout << " LB: " << bSARP.getObjValue() << endl;
        // cout << " UB: " << bSARP.getBestObjValue() << endl;

        solStatIni(sStat);
		// cout << "\nObj Val: " << setprecision(15) << bSARP.getObjValue() << endl;

		sStat->solprofit = bSARP.getObjValue();
        sStat->solDual = bSARP.getBestObjValue();
        sStat->time = time;

        if (((bSARP.getBestObjValue() - bSARP.getObjValue())/bSARP.getBestObjValue()) * 100 < 0.01) {
			sStat->status = "Optimal";
		} else {
			sStat->status = "Feasible";
		}

        // cout << "\nSolve Time: " << setprecision(15) << time << endl << endl;

        sStat->solvec.clear();

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
				// cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ") = " << bSARP.getValue(x[sStat->solvec[k][i].first][sStat->solvec[k][i].second][k]) << endl;
			}
		}		
    }
    
	env.end();
}