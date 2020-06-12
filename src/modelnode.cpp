#include "modelnode.h"
#include <cstdlib>
#include <stdio.h>

void initArcs (instanceStat *inst, nodeArcsStruct *nas){
    vector<bool> auxVec;
    vector< pair<int,int> > auxPairVec;

    for(int i = 0; i < inst->V + inst->dummy; i++){
        for(int j = 0; j < inst->V + inst->dummy; j++){
            auxVec.push_back(false);
        }
        nas->arcs.push_back(auxVec);
        nas->arcPlus.push_back(auxPairVec);
        nas->arcMinus.push_back(auxPairVec);
        auxVec.clear();
    }


    // cout << "Node arcs all false: " << endl;
    // for (int i = 0; i < nas->arcs.size(); i++){
    //     if (i == 0){
    //         cout << setw(3) << " ";
    //     }
    //     cout << setw(3) << std::right << i << " ";
    // }
    // cout << endl;
    // for(int i = 0; i < nas->arcs.size(); i++){
    //     cout << setw(3) << std::right << i;
    //     for(int j = 0; j < nas->arcs[i].size(); j++){
    //         cout << setw(3) << nas->arcs[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // cout << endl;
    // getchar();
}

void feasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem){

    if (problem->scen == "1A" || problem->scen == "2A"){
        for (int i = 0; i < inst->V; i++){
            if (i < inst->n){//i is a passenger req
                for(int j = 0; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
                    if(i != j){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        if (j < inst->n){
                            nas->arcNN.push_back(nas->fArc);
                        }
                        nas->arcNplus.push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->n + inst->m){//i is a parcel pickup node
                for (int j = 0; j < inst->n; j++){ //j is a passenger node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcPP.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }
            else if (i < inst->n + 2*inst->m){// i is a parcel delivery node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger node or parcel pickup node
                    if (j + inst->m != i){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }                    
                }

                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->V + inst->dummy){ // i is a starting node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }          
        }
    }
    else if (problem->scen == "1B" || problem->scen == "2B"){
        for (int i = 0; i < inst->V; i++){
            if (i < inst->n){//i is a passenger req
                for(int j = 0; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
                    if(i != j){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        if (j < inst->n){
                            nas->arcNN.push_back(nas->fArc);
                        }
                        nas->arcNplus.push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->n + inst->m){//i is a parcel pickup node
                for (int j = 0; j < inst->n + inst->m; j++){ //j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->arcPP.push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }
            else if (i < inst->n + 2*inst->m){// i is a parcel delivery node
                for (int j = 0; j < inst->n + 2*inst->m; j++){//j is a passenger node or parcel node (pu or del)
                    if (j + inst->m != i){
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                    }                    
                }

                for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }

            else if (i < inst->V + inst->dummy){ // i is a starting node
                for (int j = 0; j < inst->n + inst->m; j++){//j is a passenger or parcel pickup node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                }
            }          
        }
    }
}

void viewSol (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat){
    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    vector<int> auxSolOrder;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int currSP;
    vector<int> orderVec;

	solStatIni(sStat);

    for (int k = 0; k < inst->K; k++){
        sStat->solOrder.push_back(auxSolOrder);
    }

    for (int k = 0; k < inst->K; k++){
        currSP = inst->V - inst->K + k;

        for (int i = 0; i < sStat->solvec[k].size(); i++){
            auxPair.first = sStat->solvec[k][i].first;
            auxPair.second = sStat->solvec[k][i].second;            
            auxVec.push_back(auxPair);
        }
        // cout<< "here1";
        // getchar();
        // cout << "auxVec: " << endl;
        // for (int i = 0; i < auxVec.size(); i++){
        //     cout << auxVec[i].first << " " << auxVec[i].second << endl;
        // }

        while(!auxVec.empty()){
            if (sStat->solOrder[k].empty()){

                for (int i = 0; i < auxVec.size(); i++){
                    if (auxVec[i].first == currSP){
                        sStat->solOrder[k].push_back(auxVec[i].first);
                        sStat->solOrder[k].push_back(auxVec[i].second);

                        auxVec.erase(auxVec.begin()+i);

                    }
                }
            }
            else{

                for (int j = 0; j < auxVec.size(); j++){
                    if(auxVec[j].first == sStat->solOrder[k].back()){
                        sStat->solOrder[k].push_back(auxVec[j].second);

                        auxVec.erase(auxVec.begin()+j);
                    }
                }
            }       
        // cout<< "auxvec size: " << auxVec.size();
        // getchar();
        }
        // cout<< "here3";
        // getchar();
    }

    cout << "\nNumber of Vehicles: " << inst->K << endl;

    cout << "\nSolution: " << endl;
    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solOrder[k].size(); i++){
            if (i < sStat->solOrder[k].size() - 1){
                cout << sStat->solOrder[k][i] << " - ";
            }
            else{
                cout << sStat->solOrder[k][i];
            }
        }
        cout << endl;
    }
    cout << endl;

    cout << "\nSolution structure: " << endl;
    for (int k = 0; k < inst->K; k++){
        cout << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solOrder[k].size(); i++){
            if (i < sStat->solOrder[k].size() - 1){
                if (sStat->solOrder[k][i] < inst->n){
                    cout << "d" << " - ";
                }
                else if (sStat->solOrder[k][i] < inst->n + inst->m){
                    cout << "P" << " - ";
                }
                else if (sStat->solOrder[k][i] < inst->n + 2*inst->m){
                    cout << "D" << " - ";
                }
                else if (sStat->solOrder[k][i] < inst->n + 2*inst->m + inst->K){
                    cout << "S" << " - ";
                }                                      
            }
            else{

                cout << "f";
            }
        }
        cout << endl;
    }
    cout << endl;   
    // getchar();    
}


void mipnode(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "nSARP");
	int currSP;
	long M = 2*inst->T;
	long M2 = 2*(inst->n + inst->m + 1);
	long W = 2*inst->m + 1;
	int Q;
 	
	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	if (problem->scen == "1A" || problem->scen == "2A"){
		Q = 1;
	}
	else if (problem->scen == "1B" || problem->scen == "2B"){
		Q = inst->m;
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
				// cout << "x: [" << i << "][" << j << "][" << k << "]" << endl;

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

	//Position variable
	// IloNumVarArray P(env, nodeVec.size(), 0, nodeVec.size());
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "P(%d)", i);
	// 	P[i].setName(var);
	// 	model.add(P[i]);	
	// }	

	IloExpr objFunction(env);

	for (int i = 0; i < nas->arcNplus.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += nodeVec[nas->arcNplus[i].first].profit * x[nas->arcNplus[i].first][nas->arcNplus[i].second][k];
		}
	}

	for (int i = 0; i < nas->arcPP.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += nodeVec[nas->arcPP[i].first].profit * x[nas->arcPP[i].first][nas->arcPP[i].second][k];
		}
	}

	for (int i = 0; i < nas->allArcs.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction -= (double)mdist[nas->allArcs[i].first][nas->allArcs[i].second] * x[nas->allArcs[i].first][nas->allArcs[i].second][k];
		}
	}

	model.add(IloMaximize(env, objFunction));


	//Creating constraints

	//Constraint 1 - All passenger nodes must be visited

	for (int i = 0; i < inst->n; i++){
		IloExpr exp(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < nas->arcPlus[i].size(); j++){
				exp += x[i][nas->arcPlus[i][j].second][k];
			}
		}
		sprintf (var, "Constraint1_%d", i);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 2 - Relationship between visit variable and parcel node arcs
	// //A parcel request can be denied

	for (int i = inst->n; i < inst->n + inst->m; i++){

		IloExpr exp(env);
		IloExpr exp2(env);
		for (int k = 0; k < inst->K; k++){
			for (int j = 0; j < nas->arcPlus[i].size(); j++){
				exp += x[i][nas->arcPlus[i][j].second][k];
			}
			exp2 += y[i][k];
		}

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
			for (int j = 0; j < nas->arcPlus[i].size(); j++){
				exp1 += x[i][nas->arcPlus[i][j].second][k];
			}
			//Right side: arc leaves i + m
			for (int j = 0; j < nas->arcPlus[i + inst->m].size(); j++){
				exp2 += x[i + inst->m][nas->arcPlus[i + inst->m][j].second][k];

			}
			sprintf (var, "Constraint3_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}

	// //Constraint 4 - Flow conservation

	for (int i = 0; i < inst->n + 2*inst->m; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp1(env);
			IloExpr exp2(env);
			//Left side: arc leaves i
			for (int j = 0; j < nas->arcPlus[i].size(); j++){
				exp1 += x[nas->arcPlus[i][j].first][nas->arcPlus[i][j].second][k];
			}
			//Right side: arc enters i
			for (int j = 0; j < nas->arcMinus[i].size(); j++){
				exp2 += x[nas->arcMinus[i][j].first][nas->arcMinus[i][j].second][k];

			}
			sprintf (var, "Constraint4_%d_%d", i, k);
			IloRange cons = ((exp1-exp2) == 0);
			cons.setName(var);
			model.add(cons);
		}
	}
	// //Constraint 5 - The route of every vehicle has to start at its starting position

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < nas->arcPlus[inst->V - inst->K + k].size(); i++){
			exp += x[nas->arcPlus[inst->V - inst->K + k][i].first][nas->arcPlus[inst->V - inst->K + k][i].second][k];
		}
		sprintf (var, "Constraint5_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 6 - The route of every vehicle has to end at dummy node f

	for (int k = 0; k < inst->K; k++){
		IloExpr exp(env);
		for (int i = 0; i < nas->arcMinus[inst->V].size(); i++){
			exp += x[nas->arcMinus[inst->V][i].first][nas->arcMinus[inst->V][i].second][k];
		}
		sprintf (var, "Constraint6_%d", k);
		IloRange cons = (exp == 1);
		cons.setName(var);
		model.add(cons);
	}

	// //Constraint 7 - tie service begining to node visit

	for (int i = 0; i < inst->V; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp(env);
			exp = b[i] - M * y[i][k]; 
			sprintf (var, "Constraint7_%d_%d", i, k);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);
		}
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

	for (int i = 0; i < nas->allArcs.size(); i++){
		IloExpr exp(env);
		IloExpr sumX(env);
		for (int k = 0; k < inst->K; k++){
			sumX += x[nas->allArcs[i].first][nas->allArcs[i].second][k];
		}
			exp = b[nas->allArcs[i].first] - b[nas->allArcs[i].second] + nodeVec[nas->allArcs[i].first].delta + (mdist[nas->allArcs[i].first][nas->allArcs[i].second]/inst->vmed) - M * (1 - sumX);
			sprintf (var, "Constraint9_%d_%d", nas->allArcs[i].first, nas->allArcs[i].second);
			IloRange cons = (exp <= 0);
			cons.setName(var);
			model.add(cons);			

	}

	// //Constraints 10 - load constraints

	for (int i = 0; i < nas->arcnf.size(); i++){
		
		IloExpr exp(env);
		IloExpr exp2(env);
		IloExpr sumX(env);
		for (int k = 0; k < inst->K; k++){
			sumX += x[nas->arcnf[i].first][nas->arcnf[i].second][k];
		}
		exp = w[nas->arcnf[i].first] + nodeVec[nas->arcnf[i].first].load - W*(1 - sumX);
		exp2 = w[nas->arcnf[i].second];
		sprintf (var, "Constraint10_%d", i);
		IloRange cons1 = (exp - exp2 <= 0);
		cons1.setName(var);
		model.add(cons1);
	}
	//Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	for (int i = 0; i < inst->n; i++){
		for (int k = 0; k < inst->K; k++){
			IloExpr exp(env);
			exp = b[i];

			sprintf (var, "Constraint11_%d_%d", i, k);
			IloRange cons1 = (exp <= nodeVec[i].l);
			cons1.setName(var);
			model.add(cons1);
			
			sprintf (var, "Constraint12_%d_%d", i, k);
			IloRange cons2 = (nodeVec[i].e <= exp);
			cons2.setName(var);
			model.add(cons2);			
		}

	}
	
	//Constraint 13  - bound number of passenger visits transporting parcel
	if (problem->scen == "1A" || problem->scen == "1B"){
		for (int i = 0; i < nas->arcNN.size(); i++){
			IloExpr exp(env);
			IloExpr sumX(env);

			for (int k = 0; k < inst->K; k++){
				sumX += x[nas->arcNN[i].first][nas->arcNN[i].second][k];
			}
			exp = 1 - (w[nas->arcNN[i].first]/W);

			sprintf (var, "Constraint13_%d", i);
			IloRange cons1 = (sumX - exp <= 0);
			cons1.setName(var);
			model.add(cons1);

		}
	}

	// //Constraint 13 and 14 - sequence constraints

	// for (int i = 0; i < nas->allArcs.size(); i++){
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	IloExpr exp3(env);
	// 	IloExpr sumX(env);

	// 	for (int k = 0; k < inst->K; k++){
	// 		sumX += x[nas->allArcs[i].first][nas->allArcs[i].second][k];
	// 	}

	// 	exp = P[nas->allArcs[i].first];
	// 	exp2 = M2*(sumX - 1) + P[nas->allArcs[i].second] - 1;
	// 	exp3 = M2*(1 - sumX) + P[nas->allArcs[i].second] - 1;
		
	// 	sprintf (var, "Constraint13_%d", i);
	// 	IloRange cons1 = (exp  - exp2 >= 0);
	// 	cons1.setName(var);
	// 	model.add(cons1);

	// 	sprintf (var, "Constraint14_%d", i);
	// 	IloRange cons2 = (exp - exp3 <= 0);
	// 	cons2.setName(var);
	// 	model.add(cons2);	
	// }

	// Constraint 15 - bound number of passenger visits transporting parcel

	// if (problem->scen == "1A"){
		
	// 	for (int i = inst->n; i < inst->n + inst->m; i++){
	// 		IloExpr exp(env);
	// 		exp = P[i + inst->m] - P[i] - 1;

	// 		sprintf (var, "Constraint15_%d", i);
	// 		IloRange cons1 = (exp == 1);
	// 		cons1.setName(var);
	// 		model.add(cons1);					
	// 	}
	// }


	IloCplex nSARP(model);
	nSARP.exportModel("nSARP.lp");
	// nSARP.setParam(IloCplex::Threads, 8);
	
	nSARP.solve();
	cout << "\nSol status: " << nSARP.getStatus() << endl;
	sStat->feasible = nSARP.isPrimalFeasible();

	if (sStat->feasible){
		cout << "\nObj Val: " << setprecision(15) << nSARP.getObjValue() << endl;

		sStat->solprofit = nSARP.getObjValue();
		
		for (int k = 0; k < inst->K; k++){
	 		sStat->solvec.push_back(auxPairVec);
		}

		for (int i = 0; i < nodeVec.size(); i++){
			for(int j = 0; j < nodeVec.size(); ++j){
				for (int k = 0; k < inst->K; k++){
					if (nas->arcs[i][j] == true){
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

		for (int k = 0; k < inst->K; k++){
			for (int i = 0; i < sStat->solvec[k].size(); i++){
				cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
			}
		}

	}

	env.end();
}

void nodeMethod (nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
	
	nodeArcsStruct nas;
	
	// for (int i = 0; i < inst->V; i++){
	// 	cout << "load " << i << ": " << nodeVec[i].load << endl;
	// }
	
	// for (int i = 0; i < inst->n; i++){
	// 	cout << "delta " << i << ": " << nodeVec[i].delta << endl;
	// }

	// cout << "\nDistance Matrix: " << endl;

	// for (int i = 0; i < inst->V + inst->dummy; i++){
	// 	for (int j = 0; j < inst->V + inst->dummy; j++){
	// 		cout << setw(5) << mdist[i][j] << " ";
	// 	}
	// 	cout << endl;
	// }
	// getchar();

	initArcs(inst, &nas);
	feasibleArcs (inst, &nas, problem);

	// cout<< "\nFeasible arcs between nodes:" << endl;
 //    for (int i = 0; i < nas.arcs.size(); i++){
 //        if (i == 0){
 //            cout << setw(3) << " ";
 //        }
 //        cout << setw(3) << std::right << i << " ";
 //    }
 //    cout << endl;
 //    for(int i = 0; i < nas.arcs.size(); i++){
 //        cout << setw(3) << std::right << i;
 //        for(int j = 0; j < nas.arcs[i].size(); j++){
 //            cout << setw(3) <<  nas.arcs[i][j] << " ";
 //        }
 //        cout << endl;
 //    }
 //    cout << endl;

    cout << "arcs NN: " << endl;
	for (int i = 0; i < nas.arcNN.size(); i++){
		cout << nas.arcNN[i].first << " - " << nas.arcNN[i].second << " | | ";
	}
	getchar();
	// cout << "Arcs that leave a pickup: " << endl;

	// for (int i = 0; i < nas.arcPP.size(); i++){
	// 	cout<< nas.arcPP[i].first << "-" << nas.arcPP[i].second << "  |  ";
	// }
	// cout << endl;

	mipnode(inst, nodeVec, mdist, problem, &nas, sStat);
	
	
	if(sStat->feasible){
		viewSol (inst, mdist, nodeVec, sStat);

		mipSolStats (inst, mdist, nodeVec, sStat);

		printStats(inst, sStat);
	}		


	for ( int i = 0; i < inst->V + inst->dummy; i++) {
		delete[] mdist[i];
	}
	delete[] mdist;
}
