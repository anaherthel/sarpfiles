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
#include "Data.h"
#include "MyLazyCallback.h"
#include "MyCutCallback.h"
#include "MyBranchCallback.h"

#define PROBLEM 0 //HDARP
// #define PROBLEM 1 //MDHDARP

using namespace std;

int upperBoundCapacity(Data& data){
	int value = -1;
	for(int r = 0; r < 4; ++r){
		value = max(value, *max_element(data.C[r].begin(), data.C[r].end()));
	}	
	return value;
}

double upperBoundTime(Data& data){
	double value = -1;
	value = (*max_element (begin(data.l)+ 2*data.n, end(data.l) ) + *max_element (begin(data.d), end(data.d)));
	return value;
}

void solve(Data& data){

	int BigB, BigQ;
	double upperBound;

	IloEnv env;
	IloModel model(env, "MDHDARP");

	// **** VARIABLES ****

	//	Decision variable
	IloArray <IloArray < IloBoolVarArray > > X (env, data.V.size()); //Vertices

	for(int i = 0; i < data.V.size(); ++i){
		X[i] = IloArray <IloBoolVarArray> (env, data.V.size()); //Vertices
		for(int j = 0; j < data.V.size(); ++j){

			if( data.A_[i][j]) continue; // If arc i to j is invalid

			X[i][j] = IloBoolVarArray (env, data.v); //Number of Vehicles

			for(int k_ = 0; k_ < data.K_aux[i][j].size(); ++k_){
				int k = data.K_aux[i][j][k_];
				char name[20];
				sprintf(name, "X(%d,%d,%d)", i, j, k);
				X[i][j][k].setName(name);
				model.add(X[i][j][k]);
			}
		}
	}

	//	Time Variable for each vertex v in V
	upperBound = upperBoundTime(data);
	BigB = upperBound + 1;
	printf("upperboundtime: %.2lf\n\n", upperBound);

	IloNumVarArray b(env, data.V.size(), 0, upperBound);

	for (int i = 0; i < data.V.size(); ++i){
		char name[20];
		sprintf(name, "b(%d)", i);
		b[i].setName(name);
		model.add(b[i]);
	}

	//  Load Variable
	upperBound = (double) upperBoundCapacity(data);
	BigQ = upperBound + 1;
	printf("upperboundcapacity: %.2lf\n\n", upperBound);

	IloArray <IloNumVarArray> Q(env, 4);

	for(int r = 0; r < 4; ++ r){
		Q[r] = IloNumVarArray(env, data.V.size(), 0, upperBound);
		for(int i = 0; i < data.V.size(); ++i){
			char name[20];
			sprintf(name, "Q(%d,%d)", r, i);
			Q[r][i].setName(name);
			model.add(Q[r][i]);
		}
	}

	// **** OBJECTIVE FUNCTION ****
	IloExpr OBJ(env);
	for (int a = 0; a < data.A.size(); ++a){
		int i = data.A[a].first;
		int j = data.A[a].second;

		for(int k_ = 0; k_ < data.K_aux[i][j].size(); ++k_){
			int k = data.K_aux[i][j][k_];

			OBJ += data.c[i][j] * X[i][j][k];
		}
	}
	model.add(IloMinimize(env, OBJ));

	// ***** CONSTRAINTS *****

	//Every request is served once (2)
	for(int i = 0; i < data.n; ++i){
		IloExpr expr(env);
		for(int k = 0; k < data.v; ++k){
			for(int a = 0; a < data.delta_plus[i][k].size(); ++a){
				int u = data.delta_plus[i][k][a].first;
				int v = data.delta_plus[i][k][a].second;

				expr += X[u][v][k];
			}
		}

		IloRange r = (expr == 1);
		char c[100];
		sprintf(c, "c1_%d", i);
		r.setName(c);
		model.add(r);
	}

	//Same vehicle serves the pickup and delivery (3)
	for(int i = 0; i < data.n; ++i){
		for(int k = 0; k < data.v; ++k){

			IloExpr expr1(env);
			IloExpr expr2(env);

			//if an arc comes from i
			for(int a = 0; a < data.delta_plus[i][k].size(); ++a){

				int u = data.delta_plus[i][k][a].first;
				int v = data.delta_plus[i][k][a].second;

				expr1 += X[u][v][k];
			}

			//if an arc comes from n+i
			for(int a = 0; a < data.delta_plus[i+data.n][k].size(); ++a){
				int u = data.delta_plus[i+data.n][k][a].first;
				int v = data.delta_plus[i+data.n][k][a].second;		
				expr2 += X[u][v][k];

			}

			IloRange r = ( (expr1-expr2) == 0);
			char c[100];
			sprintf(c, "c2_%d,%d", i, k);
			r.setName(c);
			model.add(r);
		}
	}
	
	//The same vehicle that enters a node leaves the node (4)
	for(int i = 0; i < data.N.size(); ++i){
		for(int k = 0; k < data.v; ++k){

			IloExpr expr1(env); //Enters in i
			IloExpr expr2(env); //Leaves from i

			//	if an arc enters i
			for(int a = 0; a < data.delta_plus[i][k].size(); ++a){
				int u = data.delta_plus[i][k][a].first;
				int v = data.delta_plus[i][k][a].second;
				expr1 += X[u][v][k];
			}

			//	if an arc leaves i
			for(int a = 0; a < data.delta_minus[i][k].size(); ++a){
				int u = data.delta_minus[i][k][a].first;
				int v = data.delta_minus[i][k][a].second;
				expr2 += X[u][v][k];
			}

			IloRange r = ( (expr1-expr2) == 0);
			char c[100];
			sprintf(c, "c3_%d,%d", i, k);
			r.setName(c);
			model.add(r);
		}
	}

	//The number of vehicles k that leaves depotd i s at most 1 (5)
	for(int k = 0; k < data.v; ++k){
		IloExpr expr1(env);

		for(int a = 0; a < data.delta_plus[2*data.n+k][k].size(); ++a){
			int u = data.delta_plus[2*data.n+k][k][a].first;
			int v = data.delta_plus[2*data.n+k][k][a].second;
			expr1 += X[u][v][k];
		}

		IloRange r1 = (expr1 <= 1);

		char c1[100];
		sprintf(c1, "c4_%d", k);
		r1.setName(c1);
		model.add(r1);
	}

	//Remove subtour size 2 (6)
	for(int a = 0; a < data.Apd.size(); ++a){
		IloExpr expr1(env);
		IloExpr expr2(env);

		int u = data.Apd[a].first;
		int v = data.Apd[a].second;

		for(int k_ = 0; k_ < data.K_aux[u][v].size(); ++k_){
			int k = data.K_aux[u][v][k_];
			expr1 += X[u][v][k];
		}

		for(int k_ = 0; k_ < data.K_aux[v][u].size(); ++k_){
			int k = data.K_aux[v][u][k_];
			expr2 += X[v][u][k];
		}

		IloRange r = ( (expr1 + expr2) <= 1);
		char c[100];
		sprintf(c, "c5_%d,%d", u, (u+data.n));
		r.setName(c);
		model.add(r);	
	}

	//	Setting and checking visit time (7)
	for(int a = 0; a < data.A.size(); ++a){
		IloExpr expr(env);
		IloExpr sumX(env);

		int u = data.A[a].first;
		int v = data.A[a].second;

		for(int k_ = 0; k_ < data.K_aux[u][v].size(); ++k_){
			int k = data.K_aux[u][v][k_];
			sumX += X[u][v][k];
		}

		expr = b[u] + data.t[u][v] + data.d[u] - BigB * (1 - sumX);

		IloRange r = ( (b[v] - expr) >= 0);
		char c[100];
		sprintf(c, "c6_%d,%d", u, v);
		r.setName(c);
		model.add(r);
	}

	//	Setting TW (8)
	for(int i = 0; i < data.V.size(); ++i){
		IloExpr expr(env);
		expr = b[i];

		IloRange r1 = (expr <= data.l[i]);
		char c1[100];
		sprintf(c1, "c7_%d", i);
		r1.setName(c1);
		model.add(r1);

		IloRange r2 = (data.e[i] <= expr);
		char c2[100];
		sprintf(c2, "c8_%d", i);
		r2.setName(c2);
		model.add(r2);
	}

	//Setting and checking ride time: (9)
	for (int i = 0; i < data.n; ++i){
		IloExpr expr1(env);
		IloExpr expr2(env);
		expr1 = b[(data.n+i)] - (b[i] + data.d[i]);


		IloRange range1 = (data.t[i][(data.n+i)] <= expr1);
		char c1[100];
		sprintf(c1, "c9_%d", i);
		range1.setName(c1);
		model.add(range1);

		IloRange range2 = (expr1 <= data.L[i]);
		char c2[100];
		sprintf(c2, "c10_%d", i);
		range2.setName(c2);
		model.add(range2);
	}

	//Setting and checking route duration: (10)
	for (int k = 0; k < data.v; ++k){
		IloExpr expr1(env);
		expr1 = b[(2*data.n+data.v)+k] - (b[(2*data.n)+k] + data.d[(2*data.n)+k]);

		IloRange range1 = (expr1 <= data.L[(2*data.n)+k]);
		char c1[100];
		sprintf(c1, "c10_%d", 2*data.n+k);
		range1.setName(c1);
		model.add(range1);
	}

	//Setting and checking the load in each vertex (11)
	for(int r = 0; r < 4; ++r){
		for(int a = 0; a < data.A.size(); ++a){
			IloExpr expr(env);
			IloExpr sumX(env);

			int u = data.A[a].first;
			int v = data.A[a].second;

			for (int k_ = 0; k_ < data.K_aux[u][v].size(); ++k_){
				int k = data.K_aux[u][v][k_];
				sumX += X[u][v][k];
			}

			expr = Q[r][u] + data.q[r][v] - BigQ * (1 - sumX);

			IloRange range = ( (Q[r][v] - expr) >= 0);
			char c[100];
			sprintf(c, "c11_%d,%d,%d", r, u, v);
			range.setName(c);
			model.add(range);
		}
	}

	//(12)
	for(int r = 0; r < 3; ++r){
		for(int i = 0; i < data.V.size(); ++i){
			IloExpr expr(env);
			IloExpr expr2(env);

			for(int r_ = r; r_ < 3; ++r_){
				expr2 += Q[r_][i];
			}
			for(int r_ = r; r_ < 3; ++r_){
				for(int k = 0; k < data.v; ++k){
					for(int a = 0; a < data.delta_minus[i][k].size(); ++a){
						int j = data.delta_minus[i][k][a].first;	
						expr += (data.C[r_][k] * X[j][i][k]);
					}
				}
			}
			
			IloRange range = ( (expr2 - expr) <= 0);
			char c[100];
			sprintf(c, "c12_%d,%d", r, i);
			range.setName(c);
			model.add(range);
		}
	}

	// (13)
	for(int i = 0; i < data.V.size(); ++i){
		IloExpr expr(env);
		for(int k = 0; k < data.v; ++k){
			for(int a = 0; a < data.delta_minus[i][k].size(); ++a){
				int j = data.delta_minus[i][k][a].first;
				expr += (data.C[3][k] * X[j][i][k]);
			}
		}
		
		IloRange range = ( (Q[3][i] - expr) <= 0);
		char c[100];
		sprintf(c, "c13_%d", i);
		range.setName(c);
		model.add(range);
	}

	//Setting lower bounds (14)
	for(int r = 0; r < 4; ++r){
		for(int i = 0; i < data.N.size(); ++i){			
			IloRange range = ( (Q[r][i])  >= data.q[r][i]); //(14)
			char c[100];
			sprintf(c, "c13_%d,%d", r, i);
			range.setName(c);
			model.add(range);
		
		}
	}

	// cout << "Constraints OK\n\n";

	IloCplex mdhdarp(model);
	mdhdarp.exportModel("mdhdarp.lp");
	mdhdarp.setParam(IloCplex::Threads, 1);
	const IloAlgorithm::Status solStatus = mdhdarp.getStatus();

	// cout << "++++++++++++++++++++++++++++++++++++++++" << endl;

	// this print the "Optimal" on the screen
	// cout << ">>>>>> " << solStatus << " <<<<<" << endl; 

	// ***** CALLBACKS *****
	const IloArray< IloArray <IloBoolVarArray > >& x_ref = X;
	MyLazyCallback* lazyCbk = new (env) MyLazyCallback(env, x_ref, &data); 
    mdhdarp.use(lazyCbk);
    MyCutCallback* cutCbk = new (env) MyCutCallback(env, x_ref, &data); 
    mdhdarp.use(cutCbk);
    MyBranchCallback* branchCbk = new (env) MyBranchCallback(env);
    mdhdarp.use(branchCbk);

	// cout << "Callbacks OK\n\n";
	
	mdhdarp.solve();

	double value = mdhdarp.getObjValue();

	stack<int> s;
	bool visit[data.V.size()];
	double totalTime = 0;
	int u, v;

	for(int k = 0; k < data.v; ++k){
		totalTime += (mdhdarp.getValue(b[2*data.n+data.v+k]) + data.d[2*data.n+data.v+k]);

		printf("\nVehicle %d from depot %d, capacity (0,0,0,0)/(%d,%d,%d,%d)", k, data.U[k], data.C[0][k], data.C[1][k], data.C[2][k], data.C[3][k]);
		printf(", departure from %.2lf\n\n", mdhdarp.getValue(b[k + (2*data.n)]));
		memset(visit, false, sizeof (visit));
		s.push(k + 2*data.n);
		while(!s.empty()){
			u = s.top();
			s.pop();
			for(int a = 0; a < data.delta_plus[u][k].size(); ++a){
				v = data.delta_plus[u][k][a].second;
				if(mdhdarp.getValue(X[u][v][k]) > 0.0001){

					cout << "Leaving from: " << u << setw(5);
					printf(" Time necessary from %d to %d: %.2lf", u, v, data.t[u][v]);
					printf("\tVehicle capacity: (%d,%d,%d,%d)/(%d,%d,%d,%d)\t",(int)mdhdarp.getValue(Q[0][u]), (int)mdhdarp.getValue(Q[1][u]),
					(int)mdhdarp.getValue(Q[2][u]), (int)mdhdarp.getValue(Q[3][u]), data.C[0][k], data.C[1][k], data.C[2][k], data.C[3][k]);
					printf("\tActual time %.2lf\n", (mdhdarp.getValue(b[u]) + data.d[u]) );
					s.push(v);
				}else if(v == (2*data.n+k+v)){
					cout << "Leaving from: " << u << setw(5);
					printf(" Time necessary from %d to %d: %.2lf", u, v, data.t[u][v]);
					printf("\tVehicle capacity: (%d,%d,%d,%d)/(%d,%d,%d,%d)\t",(int)mdhdarp.getValue(Q[0][u]), (int)mdhdarp.getValue(Q[1][u]),
					(int)mdhdarp.getValue(Q[2][u]), (int)mdhdarp.getValue(Q[3][u]), data.C[0][k], data.C[1][k], data.C[2][k], data.C[3][k]);
					printf("\tActual time %.2lf\n", (mdhdarp.getValue(b[u]) + data.d[u]) );
				}
			}
		}			
	}

	for(int i = 0; i < data.n; ++i){
		printf("\nPassenger %d: Collected at %.2lf/%.2lf-%.2lf + service time of %.2lf " , i, mdhdarp.getValue(b[i]), data.e[i], data.l[i], data.d[i]);
		printf("and delivered at %.2lf/%.2lf-%.2lf + service time of %.2lf, ", mdhdarp.getValue(b[data.n+i]), data.e[i+data.n], data.l[i+data.n], data.d[i]);
		printf("He waited %.2lf/%.2lf", (mdhdarp.getValue(b[data.n+i]) - (mdhdarp.getValue(b[i]) + data.d[i])) , data.L[i]);
	}

	printf("\nTotal Time: %.4lf minutes\n", totalTime);
	printf("\nTotal Km: %.4lf\n", value);

}

int main( int argc, char **argv ) {
	time_t start_d, start_s, start_t, end;
	start_t = start_d = clock();

	Data data(PROBLEM, argv[1]);
	end = clock();
	double time_data = (end-start_d)/ double(CLOCKS_PER_SEC);
	
	data.print();

	start_s = clock();
	solve(data);
	end = clock();

	double time_solver = (end-start_s)/ double(CLOCKS_PER_SEC);

	printf("\nData processing Running time %lf\n", time_data);
	printf("Solver Running time %lf\n", time_solver);
	printf("Total Running time %lf\n", (end-start_t)/ double(CLOCKS_PER_SEC));
}