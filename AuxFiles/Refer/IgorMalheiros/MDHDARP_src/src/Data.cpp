#include "Data.h"
#include <stdio.h>
#include <string>
#include <cmath>
#include <iostream>
#include <climits>
#include <algorithm>
#include <exception>
#include <string>

using namespace std;

Data::Data(int problem, const char* filePath){
	FILE *input_file = fopen(filePath , "r" );

	fscanf( input_file, "%d %d", &v, &n); //Requestes, number of cars

	double Xd[4] = {-5.0,5.0,-5.0,5.0}; //Depot locations
	double Yd[4] = {-5.0,5.0,5.0,-5.0}; //Depot locations
	m = min(4, v); // Number of depots

	if(!problem){
		Xd[0] = 0; //Depot locations
		Yd[0] = 0; //Depot locations
		m = 1; 	// Number of depots
	}

	P.resize(n); //Request vertices
	D.resize(n); //Delivery vertices

	M_o.resize(v); //Set of dummy origin depots
	M_e.resize(v); //Set of dummy end depots

	id.resize(2*n+2*v);  //Id of each vertex
	
	N.resize(2*n); //Set of Pickup and Delivery vertices P U D
	V.resize(2*n+2*v); //All vertices

	K.resize(v); //Type of vehicles

	d.resize(2*n+2*v); //Service time of each vertex, depots must have value 0
	L.resize(2*n+v); //Max ride time
	e.resize( (2*n)+(2*v) ); //Earliest time arrived
	l.resize( (2*n)+(2*v) ); //Earliest time arrived

	q.resize(4, vector<int> (2*n+2*v) ); //Resources of a request
	C.resize(4, vector<int> (v) ); //Resource capacity of type of each vehicle

	U.resize(v); // Depot associated to each vehicle
	
	t.resize(V.size(), vector<double>(V.size()));
	c.resize(V.size(), vector<double>(V.size()));
	A_.resize(V.size(), vector<bool>(V.size(), false)); //Init with all arcs invalid

	//Each point in map
	X.resize(V.size()); 
	Y.resize(V.size());

	//Aux sets
	//arcs that come from i and can be crossed by vehicle k
	delta_plus.resize(  V.size(), vector<vector<arc> >(K.size(), vector<arc>()) );
	//arcs that reach i and can be crossed by vehicle k
	delta_minus.resize( V.size(), vector<vector<arc> >(K.size(), vector<arc>()) );
	//vehicles that can cross the arc
	K_aux.resize(V.size(), vector<vector<int> >(V.size(), vector<int>()) );

	//Vehicle information
	for(int k = 0; k < v; ++k){
		fscanf(input_file, "%lf %d %d %d %d", &L[ (2*n) +k], &C[0][k], &C[1][k], &C[2][k], &C[3][k]);
		q[0][ (2*n) + v + k] = q[0][(2*n) + k] = 0;
		q[1][ (2*n) + v + k] = q[1][(2*n) + k] = 0;
		q[2][ (2*n) + v + k] = q[2][(2*n) + k] = 0;
		q[3][ (2*n) + v + k] = q[3][(2*n) + k] = 0;
		e[ (2*n) + k] = e[ ((2*n) + v + k)] = 0;
		l[ (2*n) + v + k] = l[(2*n) + k] = L[ (2*n) +k];
		d[(2*n) + v + k]  = d[(2*n) + k] = 0;
		M_o[k] = k;
		M_e[k] = k+v;
		V[(2*n) + k] = k;
		V[(2*n) + k+v] = k+v;

		if(problem)
			U[k] = k%4;
		else
			U[k] = 0;
	}

	double notUsed[11];
	fscanf(input_file, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &notUsed[0], &notUsed[1], &notUsed[2], &notUsed[3], 
			&notUsed[4], &notUsed[5], &notUsed[6], &notUsed[7], &notUsed[8], &notUsed[9], &notUsed[10]);

	//N information
	for (int i = 0; i < (2*n); ++i){
		fscanf(input_file, "%d %lf %lf %lf %lf %d %d %d %d %lf %lf", &id[i], &X[i], &Y[i], &d[i], 
			&L[i], &q[0][i], &q[1][i], &q[2][i], &q[3][i], &e[i], &l[i]);

		if(i < n) P[i]   = i;
		else      D[i-n] = i;

		id[i]--;
		V[i] = N[i] = id[i];
	}


	fscanf(input_file, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &notUsed[0], &notUsed[1], &notUsed[2], &notUsed[3], 
			&notUsed[4], &notUsed[5], &notUsed[6], &notUsed[7], &notUsed[8], &notUsed[9], &notUsed[10]);	

	for(int k = 0; k < m; ++k){
		X[(2*n) + k + v] = X[(2*n) + k] = Xd[k];
		Y[(2*n) + k + v] = Y[(2*n) + k] = Yd[k];
		V[(2*n) + k] = (2*n) + k;
		V[(2*n) + k + v] = (2*n) + k + v;
	}

	for(int i = 0; i < V.size(); ++i){
		for(int j = 0; j < V.size(); ++j)
			t[i][j] = c[i][j] = hypot( (X[i]-X[j]), (Y[i]-Y[j]));
	}


	for(int i = 0; i < n; i++){
		if(e[i+n] <= e[i]){
	 		e[i+n] = (min(l[i+n], e[i] + d[i] +t[i][i+n])); //e[i+n] = (min(e[i+n], e[i] + d[i] +t[i][i+n]));
	 		l[i+n] = (min(l[i+n], l[i] + d[i] + L[i]));		
	 	}else if(l[i] >= l[i+n]){
	 		l[i] = (max( e[i], l[i+n] - ( t[i][i+n] + d[i] ))); //l[i] = (max( l[i], l[i+n] - ( t[i][i+n] + d[i] )));
	 		e[i] = (max( e[i], e[i+n] - ( L[i] + d[i] )));
	 	}
	}

	printf("1-PASSEI\n");

	for (int k1 = 0; k1 < (2*v); ++k1){
		for(int k2 = 0; k2 < (2*v); ++k2){
			if( (k1+v) != k2 ){
				A_[2*n+k1][2*n+k2] = true;
			}	
		}
	}

	for (int i = 0; i < n; ++i){
		A_[n+i][i] = A_[i][i] = A_[n+i][n+i] = true;
		for (int k = 0; k < v; ++k){
			A_[2*n+k][i+n] = A_[i][2*n+k] = A_[i][2*n+v+k] = A_[i+n][2*n+k] = A_[2*n+v+k][i] = A_[2*n+v+k][n+i] = true;
		}
	}

	for (int i = 0; i < V.size(); ++i){
		for (int j = 0; j < V.size(); ++j){
			if((l[i] + d[i] + t[i][j]) < e[j]){
				t[i][j] = e[j] - l[i] - d[i];
			}
		}
	}

	for (int i = 0; i < n; ++i){
		for (int j = 0; j < V.size(); ++j){
			if ( (t[i][j] + d[j] + t[j][i+n]) > L[i] )  {
				A_[i][j] = A_[j][i+n] = true;
			}
		}
	}

	printf("2-PASSEI\n");

	for (int i = 0; i < V.size(); ++i){
		for (int j = 0; j < V.size(); ++j){
			if( (e[i] + d[i] + t[i][j]) > l[j] ) 
				A_[i][j] = true;
		}
	}

	printf("3-PASSEI\n");

	double d1, d2, d3, d4;
	for (int i = 0; i < n; ++i){
		for (int j = 0; j < n; ++j){
			if(i != j){
				//j-i-n+j-n+i
				d1 = max(e[i], e[j] + t[j][i] + d[j]); //et at i
				d2 = max(e[j+n], d1 + t[i][j+n] + d[j+n]); //et at n+j
				if( (d2 + t[j+n][i+n] + d[j+n]) > l[i+n]){
					A_[i][j+n] = true;
				}

				//i-n+i-j-n+j
				d1 = max(e[i+n], e[i] + t[i][i+n] + d[i]); //et at n+i
				d2 = max(e[j], d1 + t[i+n][j] + d[i+n]);	//et at j
				if( (d2 + t[j][j+n] + d[j]) > l[j+n]){
					A_[i+n][j] = true;
				}

				//i-j-n+i-n+j and i-j-n+j-n+i
				d1 = max(e[j], e[i] + t[i][j] + d[i]); //et at j
				d2 = max(e[i+n], d1 + t[j][i+n] + d[j]); //et at n+i
				d3 = max(e[j+n], d1 + t[j][j+n] + d[j]); //et at n+j
				if( ((d2 + t[i+n][j+n] + d[i+n]) > l[j+n]) && ((d2 + t[j+n][i+n] + d[j+n]) > l[i+n]) ){
						A_[i][j] = true;
				}

				//i-j-n+i-n+j
				d1 = max(e[j], e[i] + t[i][j] + d[i]); //et at j
				d2 = max(e[i+n], d1 + t[j][i+n] + d[j]);	//et at n+i
				//j-i-n+i-n+j
				d3 = max(e[i], e[j] + t[j][i] + d[j]); //et at i
				d4 = max(e[i+n], d3 + t[i][i+n] + d[i]);	//et at n+i 
				if( ((d2 + t[i+n][j+n] + d[i+n]) > l[j+n]) && ( (d4 + t[i+n][j+n] + d[i+n]) > l[j+n]) ){
					A_[i+n][j+n] = true;
				}
			}
		}
	}

	for (int i = 0; i < n; ++i){
		for (int j = i+1; j < n; ++j){
			if(check_RT(j, i, j+n, i+n) && check_RT(i, i+n, j, j+n) && check_RT(i, j, i+n, j+n) &&
			   check_RT(i, j, j+n, i+n) && check_RT(j, i, i+n, j+n) && check_RT(j, j+n, i, i+n) ){

				A_[i][j] = A_[i][j+n] = A_[i+n][j] = A_[i+n][j+n] = A_[j][i] = true;
				A_[j][i+n] = A_[j+n][i] = A_[j+n][i+n] = true;

			}
		}
	}

	printf("TW Arc preproc\n");

	for(int i = 0; i < n; ++i){
		for (int k = 0; k < v; ++k){
			if(!A_[2*n+k][i]){
				Amp.push_back(make_pair((2*n)+k, i));
				A.push_back(make_pair((2*n)+k, i));
			}
		}
	}

	printf("4-PASSEI\n");

	for(int i = 0; i < (2*n); ++i){
		for (int j = 0; j < (2*n); ++j){
			// Arcs where i == j or from delivery i to pickup i or
			//If an arc from pickup to any node has time greater than L or T
			// if ( (i == j) || (j == (i-n) ) 
			// || ( (i < n) && ( (t[i][j] > L[i]) || (t[i][j] > l[i+n]) )) ) continue; 
			if(!A_[i][j]){
				Apd.push_back(make_pair(i, j));
				A.push_back(make_pair(i, j));
			}
		}
	}

	printf("5-PASSEI\n");

	for(int i = 0; i < n; ++i){
		for (int k = 0; k < v; ++k){
			if(!A_[i+n][2*n+v+k]){
				Adm.push_back(make_pair(i+n, (2*n+v)+k));
				A.push_back(make_pair(i+n, (2*n+v)+k));
			}
		}
	}

	printf("6-PASSEI\n");

	//	Set K_aux
	for (int a = 0; a < A.size(); ++a){
		int i = A[a].first;
		int j = A[a].second;

		for (int k = 0; k < v; ++k){
			if( (i < 2*n) && (j < 2*n) ) 
				K_aux[i][j].push_back(k);
			else if ( (i == 2*n+k) || (j == 2*n+v+k) )
				K_aux[i][j].push_back(k);
			
		}
	}


	// 	Set deltas
	for (int a = 0; a < A.size(); ++a){
		int i = A[a].first;
		int j = A[a].second;
		for(int k = 0; k < K_aux[i][j].size(); ++k){
			delta_plus[i][K_aux[i][j][k]].push_back(A[a]);
			delta_minus[j][K_aux[i][j][k]].push_back(A[a]);
		}
	}

}

void Data::print(){
	printf("Number of Requests: %d\nNumber of Vehicles: %d\nNumber of Depots: %d\n\n", n, v, m);
	for(int i = 0; i < v; ++i){
		printf("The vehicle %d starts in vertex %d: (%.2lf, %.2lf) and ends in vertex %d: (%.2lf, %.2lf). His capacity is (%d, %d, %d, %d), is TW is: [%.2lf-%.2lf], his duration is %.2lf\n", 
		i, V[2*n+i], X[2*n+i], Y[2*n+i], V[2*n+i+v], X[2*n+i], Y[2*n+i],C[0][i], C[1][i], C[2][i], C[3][i], e[2*n+i], l[2*n+i], L[2*n+i]);
	}

	for (int i = 0; i < n; ++i){
		printf("\nRequest %d denote:\n", i);
		printf("The   pickup: %d with demand: (%d, %d, %d, %d) passengers, with service time of %.2lf and must arrived in: [%.2lf-%.2lf] (%.2lf, %.2lf)\n",
		 V[i], q[0][i], q[1][i], q[2][i], q[3][i], d[i], e[i], l[i], X[i], Y[i]);
		printf("The delivery: %d with demand: (%d, %d, %d, %d) passengers, with service time of %.2lf and must arrived in: [%.2lf-%.2lf] (%.2lf, %.2lf)\n",
		 V[i+n], q[0][i+n], q[1][i+n], q[2][i+n], q[3][i+n], d[i+n], e[i+n], l[i+n], X[i+n], Y[i+n]);
		printf("The maximum ride time is: %.2lf\n", L[i]);
	}

	printf("\nCost Matrix\n");
	for(int i = 0; i < V.size(); ++i){
		for(int j = 0; j < V.size(); ++j)
			printf("%lf ", c[i][j]);
		printf("\n");
	}

	printf("\nTime Matrix\n");
	for(int i = 0; i < V.size(); ++i){
		for(int j = 0; j < V.size(); ++j)
			printf("%lf ", t[i][j]);
		printf("\n");
	}

	printf("\nArcs A\n");
	for (int i = 0; i < A.size(); ++i){
		printf("(%d, %d)", A[i].first, A[i].second);
	}

	printf("\nArcs Amp\n");
	for (int i = 0; i < Amp.size(); ++i){
		printf("(%d, %d)", Amp[i].first, Amp[i].second);
	}

	printf("\nArcs Apd\n");
	for (int i = 0; i < Apd.size(); ++i){
		printf("(%d, %d)", Apd[i].first, Apd[i].second);
	}

	printf("\nArcs Adm\n");
	for (int i = 0; i < Adm.size(); ++i){
		printf("(%d, %d)", Adm[i].first, Adm[i].second);
	}

	printf("\nArcs Adm\n");
	for (int i = 0; i < V.size(); ++i){
		for(int j = 0; j < V.size(); ++j)
			printf("A_[%d][%d]: %s ", i, j, A_[i][j] ? "true" : "false");
		printf("\n");
	}
	printf("\n");

	int cnt = 0;
	for (int i = 0; i < V.size(); ++i){
		for(int j = 0; j < V.size(); ++j){
			if(A_[i][j]){
				printf("A[%d][%d] ", i, j);
				cnt++;
			}		
		}
	}
	printf("\n# Forbidden arcs %d/%lu \n", cnt, (V.size()*V.size()));
	printf("\n");
}

//FTS ALGORITHM FOR ROUTE LEN = 4
bool Data::check_RT(int n0, int n1, int n2, int n3){
	bool infeasibility = false;
	int len = 4;
	int path[len];
	int pair_pos[len];
	bool visit_pickup[len];
	double arrival[len];
	double beginning[len];
	double departure[len];
	double waiting[len];
	double ridetime[len];
	double fts_min = __DBL_MAX__;
	double fts = 0;
	double p = 0;
	double cumulative_waiting = 0;
	
	path[0] = n0; path[1] = n1; path[2] = n2; path[3] = n3;

	if(!A_[n0][n1] && !A_[n1][n2] && !A_[n2][n3]){

		// 	UPDATE STRUCTURES
		for (int i = 0; i < len; ++i){
			if(path[i] < n) pair_pos[i] = -1;
			else			pair_pos[i] = 0;
		}
		
		for (int i = 0; i < len; ++i){
			visit_pickup[i] = true;
			if(pair_pos[i] == 0){
				for (int j = 0; j < len; ++j){
					if(path[i] == path[j] + n){
						pair_pos[i] = j;
						visit_pickup[i] = false;
					}
				}
			}
		}

		// 	STEP 1 AND 2
		beginning[0] = arrival[0] = e[path[0]];
		departure[0] = beginning[0] + d[path[0]];

		ridetime[0] = waiting[0] = 0;

		for(int i = 1; i < len; ++i){
			int before = path[i-1];
			int curr   = path[i];

			arrival[i]   = departure[i-1] + t[before][curr];
			beginning[i] = max(e[curr], arrival[i]);
			departure[i] = beginning[i] + d[curr];
			waiting[i]   = beginning[i] - arrival[i];
		}

		// // 	STEP 3
		fts_min = __DBL_MAX__;
		cumulative_waiting = 0;

		for (int i = 0; i < len; ++i){
			int curr = path[i];

			if(i != 0) cumulative_waiting += waiting[i];

			fts = cumulative_waiting;

			if(pair_pos[i] != -1 && visit_pickup[i])
				p = beginning[i] - departure[pair_pos[i]];
			else
				p = -100000000;

			p = max(0.0, L[curr]-p);
			fts += min(l[curr] - beginning[i], p);

			fts_min = min(fts_min, fts);
		}

		//	STEP 4
		beginning[0] += min(fts_min, cumulative_waiting);
		departure[0] = beginning[0] + d[path[0]];

		// 	STEP 5 AND 6
		for (int i = 1; i < len; ++i){
			int before = path[i-1];
			int curr   = path[i];

			arrival[i]   = departure[i-1] + t[before][curr];
			beginning[i] = max(e[curr], arrival[i]);
			departure[i] = beginning[i] + d[curr];
			waiting[i]   = beginning[i] - arrival[i];

			if(pair_pos[i] != -1){
				ridetime[i] = beginning[i] - departure[pair_pos[i]];
			}
		}

		// STEP 7
		for (int i = 1; i < len; ++i){
			int node_u = path[i];
			if(node_u < n){
				fts_min = __DBL_MAX__;
				cumulative_waiting = 0;
				for (int j = i; j < len; ++j){
					int node_v = path[j];

					if(i != j)
						cumulative_waiting += waiting[j];

					fts = cumulative_waiting;

					if(pair_pos[j] != -1 && visit_pickup[j])
						p = beginning[j] - departure[pair_pos[j]];
					else
						p = -100000000;

					p = max(0.0, L[node_v]-p);
					fts += min(l[node_v] - beginning[j], p);

					fts_min = min(fts_min, fts);
				}

				beginning[i] += min(fts_min, cumulative_waiting);
				departure[i] = beginning[i] + d[node_u];

				for (int j = i+1; j < len; ++j){
					int aux = path[j-1];
					int node_v = path[j];
					arrival[j] = departure[j-1] + t[aux][node_v];
					beginning[j] = max(e[node_v], arrival[j]);
					departure[j] = beginning[j] + d[node_v];
					waiting[j] = beginning[j] - arrival[j];

					if(pair_pos[j] != -1)
						ridetime[j] = beginning[j] - departure[pair_pos[j]];

					if(pair_pos[j] == i)
						visit_pickup[j] = 1;
				}
			}
		}

		infeasibility = false;

		for (int i = 0; i < len; ++i){
			if(pair_pos[i] != -1 && ridetime[i] > L[path[pair_pos[i]]]){
				infeasibility = true;
			}
		}

	}else{
		infeasibility = true;
	}

	return infeasibility;
}

//For real instances
// Data::Data(const char* filePath){
// 	FILE *input_file = fopen(filePath , "r" );

// 	fscanf( input_file, "%d %d %d", &n, &v, &m); //Requestes, number of cars, depots

// 	P.resize(n); //Request vertices
// 	D.resize(n); //Delivery vertices
// 	M.resize(m); //Depot vertices

// 	N.resize(2*n); //Set of Pickup and Delivery vertices P U D
// 	M_o.resize(v); //Set of dummy origin depots
// 	M_e.resize(v); //Set of dummy end depots
// 	N_.resize( (2*n) + (2*v)); //Set of Pickup and Delivery vertices and dummy depots N U M_o U M_e

// 	V.resize(2*n+m); //All vertices
// 	K.resize(v); //Type of vehicles
// 	d.resize(2*n+m); //Service time of each vertex, depots must have value 0
// 	L.resize(n); //Max ride time
// 	e.resize( (2*n)+(2*v) ); //Earliest time arrived
// 	l.resize( (2*n)+(2*v) ); //Earliest time arrived

// 	U.resize(v); // Depot associated to each vehicle

// 	q.resize(4, vector<int> (n) ); //Resources of a request
// 	C.resize(4, vector<int> (v) ); //Resource capacity of type of each vehicle
	
// 	t.resize(V.size(), vector<double>(V.size()));
// 	c.resize(V.size(), vector<double>(V.size()));
// 	A_.resize(V.size(), vector<bool>(V.size(), true)); //Init with all arcs invalid

// 	//Each point in map
// 	X.resize(V.size()); 
// 	Y.resize(V.size());

// 	for(int k = 0; k < v; ++k)
// 		fscanf(input_file, "%d %d %d %d %d", &U[k], &C[0][k], &C[1][k], &C[2][k], &C[3][k]);
	
// 	for (int i = 0; i < n; ++i){
// 		for(int r = 0; r < 4; ++r)		
// 			fscanf(input_file, "%d", &q[r][i]);
// 	}
	
// 	for(int i = 0; i < V.size(); ++i)
// 		fscanf(input_file, "%lf", &d[i]);

// 	for (int i = 0; i < n; ++i)
// 		fscanf(input_file, "%lf", &L[i]);

// 	for (int i = 0; i < ( (2*n)+(2*v) ); ++i)
// 		fscanf(input_file, "%lf", &e[i]);

// 	for (int i = 0; i < ( (2*n)+(2*v) ); ++i)
// 		fscanf(input_file, "%lf", &l[i]);

// 	for (int i = 0; i < V.size(); ++i)
// 		fscanf(input_file, "%lf" "%lf", &X[i], &Y[i]);

// 	for(int i = 0; i < V.size(); ++i){
// 		for(int j = 0; j < V.size(); ++j){
// 			t[i][j] = c[i][j] = hypot( (X[i]-X[j]), (Y[i]-Y[j]));
// 			//fscanf(input_file, "%lf", &c[i][j]);
// 		}
// 	}

// 	for(int i = 0; i < V.size(); ++i){
// 		for(int j = 0; j < V.size(); ++j){
// 			//t[i][j] = c[i][j] = hypot( (X[i]-X[j]), (Y[i]-Y[j]));
// 			//fscanf(input_file, "%lf", &t[i][j]);
// 		}
// 	}

// 	for(int i = 0; i < n; ++i){
// 		for (int d = 0; d < m; ++d){
// 			Amp.push_back(make_pair((2*n)+d, i));
// 			A.push_back(make_pair((2*n)+d, i));
// 			A_[(2*n)+d][i] = false;
// 		}
// 	}

// 	for(int i = 0; i < (2*n); ++i){
// 		for (int j = 0; j < (2*n); ++j){
// 			// Arcs where i == j or from delivery i to pickup i or
// 			//If an arc from pickup to any node has time greater than L or T
// 			if ( (i == j) || (j == (i-n) ) 
// 			|| ( (i < n) && ( (t[i][j] > L[i]) || (t[i][j] > l[i]) )) ) continue; 
// 			Apd.push_back(make_pair(i, j));
// 			A.push_back(make_pair(i, j));
// 			A_[i][j] = false;
// 		}
// 	}

// 	for(int i = 0; i < n; ++i){
// 		for (int d = 0; d < m; ++d){
// 			Adm.push_back(make_pair(i+n, (2*n)+d));
// 			A.push_back(make_pair(i+n, (2*n)+d));
// 			A_[i+n][(2*n)+d] = false;
// 		}
// 	}
// }
