#include "modeltwostage.h"
#include <cstdlib>
#include <stdio.h>

void tsInitArcs (instanceStat *inst, tsArcsStruct *tsas){
    vector<bool> auxVec;
    vector< pair<int,int> > auxPairVec;

    for(int i = 0; i < inst->n + inst->K + inst->dummy; i++){
        for(int j = 0; j < inst->n + inst->K + inst->dummy; j++){
            auxVec.push_back(false);
        }
        tsas->tsarcs.push_back(auxVec);
        tsas->tsarcPlus.push_back(auxPairVec);
        tsas->tsarcMinus.push_back(auxPairVec);
        auxVec.clear();
    }
}

void prepVecs(instanceStat *inst, tsArcsStruct *tsas, vector<nodeStat> &p1vec, vector<nodeStat> &nodeVec, probStat* problem){

	for (int i = 0; i < nodeVec.size(); i++){
		if (i < inst->n){
			p1vec.push_back(nodeVec[i]);
		}
		else if (i >= 2*inst->m + inst->n){
			p1vec.push_back(nodeVec[i]);			
		}
	}

	cout << endl;

    for (int i = 0; i < p1vec.size(); i++){//i is a passenger req
        if (i < inst->n){//i is a passenger req
            for(int j = 0; j < inst->n; j++){// j is a passenger
                if(i != j){
                    tsas->tsarcs[i][j] = true;
                    tsas->tsfArc.first = i;
                    tsas->tsfArc.second = j;
                    tsas->tsarcMinus[j].push_back(tsas->tsfArc);
                    tsas->tsarcPlus[i].push_back(tsas->tsfArc);
                    tsas->tsarcNN.push_back(tsas->tsfArc);
                    tsas->tsarcNplus.push_back(tsas->tsfArc);
                    tsas->tsallArcs.push_back(tsas->tsfArc);
                    tsas->tsarcnf.push_back(tsas->tsfArc);
                }
            }

            int dummyindex = p1vec.size() - 1;//j is the dummy node
                tsas->tsarcs[i][dummyindex] = true;
                tsas->tsfArc.first = i;
                tsas->tsfArc.second = dummyindex;
                tsas->tsarcMinus[dummyindex].push_back(tsas->tsfArc);
                tsas->tsarcPlus[i].push_back(tsas->tsfArc);
                tsas->tsarcNplus.push_back(tsas->tsfArc);
                tsas->tsallArcs.push_back(tsas->tsfArc);
            
        }

        else if (i < inst->n + inst->K){ // i is a starting node
            for (int j = 0; j < inst->n; j++){//j is a passenger node
                tsas->tsarcs[i][j] = true;
                tsas->tsfArc.first = i;
                tsas->tsfArc.second = j;
                tsas->tsarcMinus[j].push_back(tsas->tsfArc);
                tsas->tsarcPlus[i].push_back(tsas->tsfArc);
                tsas->tsallArcs.push_back(tsas->tsfArc);
                tsas->tsarcnf.push_back(tsas->tsfArc);
            }
        }          
    }

	cout<< "\nFeasible arcs between nodes (two stage):" << endl;
    for (int i = 0; i < tsas->tsarcs.size(); i++){
        if (i == 0){
            cout << setw(3) << " ";
        }
        cout << setw(3) << std::right << i << " ";
    }
    cout << endl;
    for(int i = 0; i < tsas->tsarcs.size(); i++){
        cout << setw(3) << std::right << i;
        for(int j = 0; j < tsas->tsarcs[i].size(); j++){
            cout << setw(3) <<  tsas->tsarcs[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;

    getchar();

}

void mipts(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, probStat* problem, nodeArcsStruct *nas, solStats *sStat){
	//MIP
	//Creating environment and model 
	char var[100];
	IloEnv env;
	IloModel model(env, "tsSARP");
	int currSP;
	// long M = 2*inst->T;
	// long M2 = 2*(inst->n + inst->m + 1);
	long M2 = 2*(inst->n + 1);
	// long W = inst->m + 1;
	// int Q;
 	


	vector< pair<int, int> > auxPairVec;
	pair<int, int> auxPair;

	// if (problem->scen == "1A" || problem->scen == "2A"){
	// 	Q = 1;
	// }
	// else if (problem->scen == "1B" || problem->scen == "2B"){
	// 	Q = inst->m;
	// }


	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, inst->n);

	for (int i = 0; i < inst->n; i++){
		x[i] = IloArray <IloBoolVarArray> (env, inst->n);
		for(int j = 0; j < inst->n; ++j){
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
	IloBoolVarArray y(env, inst->n); 

	for (int i = 0; i < inst->n; i++){
		sprintf(var, "y(%d)", i);
		y[i].setName(var);
		model.add(y[i]);
	}

		// Variable start of service time
	IloNumVarArray b(env, inst->n, 0, inst->T);
	for (int i = 0; i < inst->n; i++){
		sprintf(var, "b(%d)", i);
		b[i].setName(var);
		model.add(b[i]);
	}

	// Load variable
	// IloNumVarArray w(env, nodeVec.size(), 0, Q);

	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "w(%d)", i);
	// 	w[i].setName(var);
	// 	model.add(w[i]);
		
	// }

	//Position variable
	// IloNumVarArray P(env, nodeVec.size(), 0, nodeVec.size());
	// for (int i = 0; i < nodeVec.size(); i++){
	// 	sprintf(var, "P(%d)", i);
	// 	P[i].setName(var);
	// 	model.add(P[i]);	
	// }	
//**
	IloExpr objFunction(env);

	for (int i = 0; i < nas->arcNN.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction += nodeVec[nas->arcNN[i].first].profit * x[nas->arcNN[i].first][nas->arcNN[i].second][k];
		}
	}

	// for (int i = 0; i < nas->arcPP.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		objFunction += nodeVec[nas->arcPP[i].first].profit * x[nas->arcPP[i].first][nas->arcPP[i].second][k];
	// 	}
	// }

	for (int i = 0; i < nas->arcNN.size(); i++){
		for (int k = 0; k < inst->K; k++){
			objFunction -= (double)mdist[nas->arcNN[i].first][nas->arcNN[i].second] * x[nas->arcNN[i].first][nas->arcNN[i].second][k];
		}
	}

	model.add(IloMaximize(env, objFunction));


	// //Creating constraints

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

	// for (int i = inst->n; i < inst->n + inst->m; i++){

	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	for (int k = 0; k < inst->K; k++){
	// 		for (int j = 0; j < nas->arcPlus[i].size(); j++){
	// 			exp += x[i][nas->arcPlus[i][j].second][k];
	// 		}
	// 	}

 //        exp2 += y[i];
	// 	sprintf (var, "Constraint2_%d", i);
	// 	IloRange cons = (exp - exp2 == 0);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }


	// // //Constraint 3 - parcel that is picked up, has to be delivered by the same vehicle

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

	// // //Constraint 4 - Flow conservation

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
	// // //Constraint 5 - The route of every vehicle has to start at its starting position

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

	// // //Constraint 6 - The route of every vehicle has to end at dummy node f

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

	// // //Constraint 7 - tie service begining to node visit

	// // for (int i = 0; i < inst->V; i++){
	// // 	for (int k = 0; k < inst->K; k++){
	// // 		IloExpr exp(env);
	// // 		exp = b[i] - M * y[i][k]; 
	// // 		sprintf (var, "Constraint7_%d_%d", i, k);
	// // 		IloRange cons = (exp <= 0);
	// // 		cons.setName(var);
	// // 		model.add(cons);
	// // 	}
	// // }

 //    for (int i = 0; i < inst->V; i++){
 //        IloExpr exp(env);
 //        exp = b[i] - M * y[i]; 
 //        sprintf (var, "Constraint7_%d", i);
 //        IloRange cons = (exp <= 0);
 //        cons.setName(var);
 //        model.add(cons);
 //    }


	// // //Constraint 8 - service of pickup must come before the delivery

	// for (int i = inst->n; i < inst->n + inst->m; i++){
	// 	IloExpr exp(env);
	// 	exp = b[i] - b[i + inst->m];

	// 	sprintf (var, "Constraint8_%d", i);
	// 	IloRange cons = (exp <= 0);
	// 	cons.setName(var);
	// 	model.add(cons);
	// }

	// // //Constraints 9 - TW 

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

	// //Constraints 10 - load constraints

	// for (int i = 0; i < nas->allArcs.size(); i++){
		
	// 	IloExpr exp(env);
	// 	IloExpr exp2(env);
	// 	IloExpr sumX(env);

	// 	for (int k = 0; k < inst->K; k++){
	// 		sumX += x[nas->allArcs[i].first][nas->allArcs[i].second][k];
	// 	}

	// 	exp = w[nas->allArcs[i].first] + nodeVec[nas->allArcs[i].second].load - W*(1 - sumX);
	// 	exp2 = w[nas->allArcs[i].second];
		
 //        sprintf (var, "Constraint10_%d_%d", nas->allArcs[i].first, nas->allArcs[i].second);
		
 //        IloRange cons1 = (exp2 - exp >= 0);
	// 	cons1.setName(var);
	// 	model.add(cons1);
	// }



	// //Constraints 11 and 12 - bound the service beginning time by the earlier and later service times for each node

	// for (int i = 0; i < nodeVec.size(); i++){
	// 	for (int k = 0; k < inst->K; k++){
	// 		IloExpr exp(env);
	// 		exp = b[i];

	// 		sprintf (var, "Constraint11_%d_%d", i, k);
	// 		IloRange cons1 = (exp <= nodeVec[i].l);
	// 		cons1.setName(var);
	// 		model.add(cons1);
			
	// 		sprintf (var, "Constraint12_%d_%d", i, k);
	// 		IloRange cons2 = (nodeVec[i].e <= exp);
	// 		cons2.setName(var);
	// 		model.add(cons2);			
	// 	}

	// }
	
	// //Constraint 13  - bound number of passenger visits transporting parcel
	// if (problem->scen == "1A" || problem->scen == "1B"){
	// 	for (int i = 0; i < nas->arcNN.size(); i++){
	// 		IloExpr exp(env);
	// 		IloExpr sumX(env);

	// 		for (int k = 0; k < inst->K; k++){
	// 			sumX += x[nas->arcNN[i].first][nas->arcNN[i].second][k];
	// 		}
	// 		exp = 1 - (w[nas->arcNN[i].first]/W);

	// 		sprintf (var, "Constraint13_%d", i);
	// 		IloRange cons1 = (sumX - exp <= 0);
	// 		cons1.setName(var);
	// 		model.add(cons1);

	// 	}
	// }

 //    //test constraint

 //    IloExpr exp(env);
 //    exp = w[nodeVec.size()-1];

 //    sprintf (var, "Constraint14");
 //    IloRange cons1 = (exp == 0);
 //    cons1.setName(var);
 //    model.add(cons1);

	// // // //Constraint 13 and 14 - sequence constraints

	// // // for (int i = 0; i < nas->allArcs.size(); i++){
	// // // 	IloExpr exp(env);
	// // // 	IloExpr exp2(env);
	// // // 	IloExpr exp3(env);
	// // // 	IloExpr sumX(env);

	// // // 	for (int k = 0; k < inst->K; k++){
	// // // 		sumX += x[nas->allArcs[i].first][nas->allArcs[i].second][k];
	// // // 	}

	// // // 	exp = P[nas->allArcs[i].first];
	// // // 	exp2 = M2*(sumX - 1) + P[nas->allArcs[i].second] - 1;
	// // // 	exp3 = M2*(1 - sumX) + P[nas->allArcs[i].second] - 1;
		
	// // // 	sprintf (var, "Constraint13_%d", i);
	// // // 	IloRange cons1 = (exp  - exp2 >= 0);
	// // // 	cons1.setName(var);
	// // // 	model.add(cons1);

	// // // 	sprintf (var, "Constraint14_%d", i);
	// // // 	IloRange cons2 = (exp - exp3 <= 0);
	// // // 	cons2.setName(var);
	// // // 	model.add(cons2);	
	// // // }

	// // // Constraint 15 - bound number of passenger visits transporting parcel

	// // // if (problem->scen == "1A"){
		
	// // // 	for (int i = inst->n; i < inst->n + inst->m; i++){
	// // // 		IloExpr exp(env);
	// // // 		exp = P[i + inst->m] - P[i] - 1;

	// // // 		sprintf (var, "Constraint15_%d", i);
	// // // 		IloRange cons1 = (exp == 1);
	// // // 		cons1.setName(var);
	// // // 		model.add(cons1);					
	// // // 	}
	// // // }


	IloCplex tsSARP(model);
	tsSARP.exportModel("tsSARP.lp");
	// tsSARP.setParam(IloCplex::Threads, 8);
    
	// tsSARP.setParam(IloCplex::Param::TimeLimit, 7200);

// 	tsSARP.solve();
// 	cout << "\nSol status: " << tsSARP.getStatus() << endl;
// 	sStat->feasible = tsSARP.isPrimalFeasible();

// 	if (sStat->feasible){
// 		cout << "\nObj Val: " << setprecision(15) << tsSARP.getObjValue() << endl;

// 		sStat->solprofit = tsSARP.getObjValue();
		
// 		for (int k = 0; k < inst->K; k++){
// 	 		sStat->solvec.push_back(auxPairVec);
// 		}

// 		for (int i = 0; i < nodeVec.size(); i++){
// 			for(int j = 0; j < nodeVec.size(); ++j){
// 				for (int k = 0; k < inst->K; k++){
// 					if (nas->arcs[i][j] == true){
// 						if (tsSARP.getValue(x[i][j][k]) > 0.5){
// 							auxPair.first = i;
// 							auxPair.second = j;
// 							sStat->solvec[k].push_back(auxPair);
// 							// cout << i << " " << j << " " << k << ": " << bSARP.getValue(x[i][j][k]) << endl;
// 							// getchar();
// 						}
// 					}
// 				}
// 			}	
// 		}

// 		for (int k = 0; k < inst->K; k++){
// 			for (int i = 0; i < sStat->solvec[k].size(); i++){
// 				cout << "x(" << sStat->solvec[k][i].first << ", " << sStat->solvec[k][i].second << ", " << k << ")" << endl;
// 			}
// 		}


//         // for (int i = 0; i < inst->V + inst->dummy; i++){
//         //     sStat->solBegin.push_back(0);
//         // }

//         for (int i = 0; i < nodeVec.size(); i++){
//             if (tsSARP.getValue(b[i]) > 0.5){
//                 sStat->solBegin.push_back(tsSARP.getValue(b[i]));
//             }
//             else {
//                 sStat->solBegin.push_back(0);
//             }
//         }

//         cout << endl;

// //        for (int i = 0; i < nodeVec.size(); i++){
//   //          cout << "b(" << i << "): " << sStat->solBegin[i] << endl;
//     //    }


//         for (int i = 0; i < nodeVec.size(); i++){
//             if (tsSARP.getValue(w[i]) > 0.5){
//                 sStat->solLoad.push_back(tsSARP.getValue(w[i]));
//             }
//             else {
//                 sStat->solLoad.push_back(0);
//             }
//         }

//         // for (int i = 0; i < nodeVec.size(); i++){
//         //     cout << "w(" << i << "): " << sStat->solLoad[i] << endl;
//         // }

// 	}

	env.end();
}

void twoStageMethod(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
	
	nodeArcsStruct nas;
	tsArcsStruct tsas;
	vector<nodeStat> p1vec;
	
	// for (int i = 0; i < inst->V; i++){
	// 	cout << "load " << i << ": " << nodeVec[i].load << endl;
	// }
	
	// for (int i = 0; i < inst->n; i++){
	// 	cout << "delta " << i << ": " << nodeVec[i].delta << endl;
	// }

//	cout << "\nDistance Matrix: " << endl;

//	for (int i = 0; i < inst->V + inst->dummy; i++){
//		for (int j = 0; j < inst->V + inst->dummy; j++){
//			cout << setw(5) << mdist[i][j] << " ";
//		}
//		cout << endl;
//	}
	// getchar();
	initArcs(inst, &nas);
	feasibleArcs (inst, &nas, problem);

	tsInitArcs(inst, &tsas);

	prepVecs(inst, &tsas, p1vec, nodeVec, problem);



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

 //    cout << "arcs NN: " << endl;
	// for (int i = 0; i < nas.arcNN.size(); i++){
	// 	cout << nas.arcNN[i].first << " - " << nas.arcNN[i].second << " | | ";
	// }
	// getchar();
	// cout << "Arcs that leave a pickup: " << endl;

	// for (int i = 0; i < nas.arcPP.size(); i++){
	// 	cout<< nas.arcPP[i].first << "-" << nas.arcPP[i].second << "  |  ";
	// }
	// cout << endl;


	mipts(inst, nodeVec, mdist, problem, &nas, sStat);

	// if(sStat->feasible){
	// 	viewSol (inst, mdist, nodeVec, sStat);

	// 	mipSolStats (inst, mdist, nodeVec, sStat);

	// 	printStats(inst, sStat);
	// }		

	for ( int i = 0; i < inst->V + inst->dummy; i++) {
		delete[] mdist[i];
	}
	delete[] mdist;
}
