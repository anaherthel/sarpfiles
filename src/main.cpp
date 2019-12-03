#include <ilcplex/ilocplex.h>     
#include <cstdio>
#include <iostream>
#include <iomanip> 
#include <vector>
#include <stack>
#include <algorithm>
#include <list>
#include <utility>
#include <cmath>
#include <ctime>
#include "functions.h"

using namespace std;

int main (int argc, char *argv[]) {

	double **distMatrix;
	
	nodeStat node;
	instanceStat inst;
	std::vector<nodeStat> nodeVec;
	vector< vector<bool> > arcs;
	// int vertices;

	readData(argc, argv, &node, &inst, nodeVec, &distMatrix);


	cout << "Distance Matrix: " << endl;

	for (int i = 0; i < inst.V + 1; i++){
		for (int j = 0; j < inst.V + 1; j++){
			cout << setw(5) << distMatrix[i][j] << " ";
		}
		cout << endl;
	}

	vector<bool> arcRow;
	
	for (int i = 0; i < inst.V + 1; i++){
		for (int j = 0; j < inst.V + 1; j++){
			arcRow.push_back(false);
		}
		arcs.push_back(arcRow);
	}
	cout << "Feasible arcs: " << endl;

	feasibleArcs(&inst, nodeVec, arcs);
	for (int i = 0; i < inst.V + 1; i++){
		if (i == 0){
			cout << setw(3) << " ";
		}
		cout << setw(3) << std::right << i;
	}
	cout << endl;
	for (int i = 0; i < inst.V + 1; i++){
		cout << setw(3) << std::right << i;
		for (int j = 0; j < inst.V + 1; j++){
			cout << setw(3) << std:: right << arcs[i][j];
		}
		cout << endl;
	}

	cout << "Instance stats: " << inst.K  << " " << inst.delta << " " <<
	inst.n << " " << inst.m << " " << inst.T << " " << inst.V << endl;

	//MIP

	//Creating environment and model 
	// char var[100];
	IloEnv env;
	IloModel model(env, "SARP");

	//Creating variables
	IloArray <IloArray <IloBoolVarArray> > x(env, nodeVec.size());

	for (int i = 0; i < nodeVec.size(); i++){
		x[i] = IloArray <IloBoolVarArray> (env, nodeVec.size());
		// for (int j = i + 1; j < nodeVec.size(); j++){
		// 	IloArray <IloBoolVarArray> array2D(env, shifts.size());

		// 	x[i][j] = IloBoolVarArray (env, );
		// 	for (int k = 0; k < shifts.size(); k++){
		// 		IloBoolVarArray array(env, shifts.size());
		// 		x[i][j][k] = array;
		// 	}
		// }	
	}

	// for (int i = 0; i < occRows.size(); i++){
	// 	for (int j = i + 1; j < occRows.size(); j++){
	// 		for (int k = 0; k < shifts.size(); k++){
	// 			for (int l = 0; l < shifts.size(); l++){
	// 				sprintf(var, "x_%d_%d_%d_%d", i, j, k, l);
	// 				x[i][j][k][l].setName(var);
	// 				model.add(x[i][j][k][l]);
	// 			}
 // 			}
	// 	}
	// }

	// IloArray <IloBoolVarArray> y(env, shifts.size());

	// for (int i = 0; i < occRows.size(); i++){
	// 	y[i] = IloBoolVarArray (env, shifts.size());
	// }
	 
	// for (int i = 0; i < occRows.size(); i++){
	// 	for (int k = 0; k < shifts.size(); k++){
	// 		sprintf(var, "y_%d_%d", i, k);
	// 		y[i][k].setName(var);
	// 		model.add(y[i][k]);
	// 	}
	// }

	// //Creating objective function
	
	// IloExpr objFunction(env);

	// for (int i = 0; i < occRows.size(); i++){
	// 	for (int j = i + 1; j < occRows.size(); j++){
	// 		twoRowsCost = 0;
	// 		for (int k = 0; k < shifts.size(); k++){
	// 			for (int l = 0; l < shifts.size(); l++){
	// 				for (int m = 0; m < vecShSol[i].shiftRow.size(); m++){
	// 					for (int n = 0; n < vecShSol[j].shiftRow.size(); n++){
	// 						twoRowsCost += (data.freq_matrix[vecShSol[i].shiftRow[m]][vecShSol[j].shiftRow[n]]*newBMatrix[vecShSol[i].posRow[m]][vecShSol[j].posRow[n]][k][l]);
	// 					}
	// 				}
	// 				objFunction += (double)twoRowsCost*x[i][j][k][l];
	// 			}
	// 		}
	// 	}
	// }

	// for (int i = 0; i < occRows.size(); i++){
	// 	objFunction += (double)((vecShSol[i].rowCost)/2);
	// }

	// model.add(IloMinimize(env, objFunction));

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

	// env.end();
	// /*------------*/
	//_______________________________end comment________________________________

	//infile.close();


	// for (int i = 0; i < keyb.ksize; i++){
	// 	delete [] data.b_matrix[i];
	// }
	// for (int i = 0; i < data.size; i++){
	// 	delete [] data.freq_matrix[i];
	// }
	// delete [] data.b_matrix;
	// delete [] data.freq_matrix;
	// delete [] data.char_list;
	// delete [] keyb.grid;
	// delete [] keyb.assignment;
	
	// delete[] xs;
 //    delete[] ys;
 //    delete[] label;
 //    delete[] load;
 //    delete[] e;
 //    delete[] l;
 //    delete[] xf;
 //    delete[] yf;

	for ( int i = 0; i < inst.V + 1; i++ ) {
		delete[] distMatrix[i];
	}

	delete[] distMatrix;

	return 0;
}