#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>	
#include <iterator>
#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>

using namespace std;


double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J);

void readData (int argc, char** argv, double ***Mdist);

double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J){
    return sqrt(pow(Xf[I] - Xs[J], 2) + pow(Yf[I] - Ys[J], 2));
}

void readData (int argc, char** argv, double ***Mdist){

    if (argc < 3) {
        cout << "\nMissing parameters\n";
        cout << " ./exeHybIns [Instance1] [Instance2]" << endl;
        exit(1);
    }
    
    if (argc > 3) {
        cout << "\nToo many parameters\n";
        cout << " ./exeHybIns [Instance1] [Instance2]" << endl;
        exit(1);
    }  

 //   	vector<coordSt> coordVec1;
	// vector<coordSt> coordVec2;

	string file1;
	string file2;

    int s1;
    int s2;

    pair<int, int> coordinate;

    vector< pair <int, int> > coordVec;

    int refpoint = 2;

    int n, m;

    int seed = 1234;
    srand(seed);

    vector <vector <double> > tempData;
    vector<double> auxtempdata;

    vector< pair <int, int> > start;

    vector <vector <double> > realData;

    vector<double> service;

    vector< vector<double> > dist;
    char *instance1; 
    char *instance2; 

    instance1 = argv[1];
    instance2 = argv[2];

    ifstream in1(instance1, ios::in);

    
    if( !in1 ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    while ( file1.compare("DIMENSION:") != 0 && file1.compare("DIMENSION") != 0 ){
        in1 >> file1;
    }

    in1 >> s1;

    n = (s1 - 2)/2;

    cout << "n: " << n << endl;
    getchar();

    while (file1.compare("NODE_COORD_SECTION") != 0){
        in1 >> file1;
    }

    for (int i = 0; i < s1 - 2; i++){
    	coordVec.push_back(coordinate);
    }


    for (int i = 0; i < s1; i++){

        if (i == 0){
            in1 >> file1;
            in1 >> coordinate.first;
            in1 >> coordinate.second;
            start.push_back(coordinate);
        }

        else if (i == 1){
            in1 >> file1;
            in1 >> file1;
            in1 >> file1;
        }
        
        else{
            in1 >> file1;
            in1 >> coordVec[i - 2].first;
            in1 >> coordVec[i - 2].second;

        }

    }


    ifstream in2(instance2, ios::in);

    
    if( !in2 ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    while ( file2.compare("DIMENSION:") != 0 && file2.compare("DIMENSION") != 0 ){
        in2 >> file2;
    }

    in2 >> s2;

    cout << "s2: " << s2 << endl;
    getchar();

    m = (s2 - 2)/2;
    cout << "m: " << m << endl;
    getchar();

    while (file2.compare("NODE_COORD_SECTION") != 0){
        in2 >> file2;
    }

    for (int i = 0; i < s2 - 2; i++){
    	coordVec.push_back(coordinate);
    }

    for (int i = s1 - 2; i < s1+s2 - 2; i++){

        if (i == s1 - 2){
            in2 >> file2;
            in2 >> coordinate.first;
            in2 >> coordinate.second;
            start.push_back(coordinate);
        }
        else if (i == s1 - 1){
            in2 >> file2;
            in2 >> file2;
            in2 >> file2;
        }

        else{
            in2 >> file2;
            in2 >> coordVec[i - 2].first;
            in2 >> coordVec[i - 2].second;            
        }
    }

    cout << "Coords of both: " << endl;
   	for (int i = 0; i < coordVec.size(); i++){
    	cout << coordVec[i].first << " " << coordVec[i].second << endl;
    }
    getchar();

    cout << "Starting points: " << endl;
    for(int i = 0; i < start.size(); i++){
        cout << start[i].first << " " << start[i].second << endl;
    }

    //Adjust coordinates

    for (int i = 0; i < coordVec.size(); i++){
        for (int j = 0; j < coordVec.size(); j++){
            dist[i][j] = floor ( CalcDistEuc ( x, y, i, j ) + 0.5 );
        }
    }



    // for (int i = 0; i < s1; i++) {
    //     for (int j = 0; j < i + 1; j++) {
    //         in1 >> tempData[i][j];
    //         // cout << tempData[i][j];
    //         // getchar();
    //         if (i > 0){
    //             tempData[j][i] = tempData[i][j];                        
    //         }
    //     }
    // }



 //    // //adjusting rows
 //    // for (int i = 0; i < s1; i++){
 //    //     for (int j = 0; j < refpoint; j++){
 //    //         tempData[s1 + j][i] = tempData[j][i];
 //    //     }
 //    // }

 //    // //adjusting columns
 //    // for (int i = 0; i < s1; i++){
 //    //     for (int j = 0; j < refpoint; j++){
 //    //         tempData[i][s1 + j] = tempData[i][j];
 //    //     }
 //    // }

 //    // //maybe we needed to adjust the corner (relying on the -0 being f)

 //    cout << "Matrix1:" << endl;
 //    for (int i = 0; i < s1; i++){
 //   		for(int j = 0; j < s1; j++){
 //   			cout << setw(5) << tempData[i][j] << " ";
 //   		}
 //   		cout << endl;
 //    }

	// cout << endl;

	// getchar();

	// //save starting point

	// for(int i = 0; i < tempData[0].size(); i++){
	// 	if(i % 2 == 0){
	// 		start.push_back(tempData[0][i]);
	// 	}
	// }

	// //fix starting point

	// start.push_back(start[0]);
	// start.erase(start.begin());

 //    //erase unused
 //    for (int j = 0; j < refpoint; j++){
 //        tempData.erase(tempData.begin());
 //    }

 //    for (int i = 0; i < s1; i++){
 //        for (int j = 0; j < refpoint; j++){
 //            tempData[i].erase(tempData[i].begin());
 //        }   
 //    }

 //    cout << "Matrix2:" << endl;
 //    for (int i = 0; i < tempData.size(); i++){
 //   		for(int j = 0; j < tempData[i].size(); j++){
 //   			cout << setw(5) << tempData[i][j] << " ";
 //   		}
 //   		cout << endl;
 //    }

	// cout << endl;

	// getchar();

 //    // collapsing passenger nodes

 //    for (int i = 0; i < n; i++){
 //        for (int j = 0; j < n; j++){
 //            if (i == j){
 //                realData[i][j] = 0;
 //            }
 //            else{
 //              realData[i][j] = (tempData[2*i+1][2*j])/1000;
 //            }
 //        }
 //    }

 //    cout << "Matrix3:" << endl;
 //    for (int i = 0; i < realData.size(); i++){
 //   		for(int j = 0; j < realData[i].size(); j++){
 //   			cout << setw(5) << realData[i][j] << " ";
 //   		}
 //   		cout << endl;
 //    }

	// cout << endl;

	// getchar();


 //    double delta;

 //    for(int i = 0; i < n; i++){
 //        // cout << i << ": " << (tempData[2*i][2*i+1]);
 //        delta = (tempData[2*i][2*i+1])/1000;
 //        service.push_back(delta);
 //        // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
 //    }



    // if (trialK > 1){
    //     if (trialK >= K){
    //         K = trialK;
    //     }
    //     else{
    //         trialK = K;
    //     } 
    //     for (int l = 0; l < K -1; l++){
    //         vector<double> distRow;
    //         vector<double> dummyRow;

    //         double valueDist;
            
    //         for (int i = 0; i < V + inst->dummy; i++){
    //             valueDist = realData[i][realData[i].size() - 2];
    //             realData[i].insert(realData[i].begin() + realData[i].size() - 1, valueDist);
    //         }

    //         for (int i = 0; i < V + inst->dummy; i++){
    //             distRow.push_back(realData[V - 1][i]);
    //             dummyRow.push_back(realData[V][i]);
    //         }
    //         distRow.push_back(0);
    //         dummyRow.push_back(0); 

    //         realData.pop_back();

    //         realData.push_back(distRow);
    //         realData.push_back(dummyRow);
    //         V++;
    //     }
    // }


    // double *delta = new double[V + inst->dummy];
    // double *profit = new double[V + inst->dummy];
    // double *e = new double[V + inst->dummy];
    // double *l = new double[V + inst->dummy];
    // int *w = new int[V + inst->dummy];      

    // int reference = n;
    // //calculate deltas
    // for(int i = 0; i < V + inst->dummy; i++){
    //     if (i < n){
    //         // cout << i << ": " << (tempData[2*i][2*i+1]);
    //         delta[i] = 2 * service + (((tempData[2*i][2*i+1])/1000)/inst->vmed);
    //         // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
    //         profit[i] = inst->gamma2 + inst->mu2*(tempData[2*i][2*i+1]/1000) - (tempData[2*i][2*i+1]/1000);    
    //         w[i] = 0;
    //     }
    //     else if (i < V - K){
    //         delta[i] = service;
    //         if (i < n + m){
    //             profit[i] = inst->gamma + inst->mu*(tempData[i + n][i + n + m]/1000);
    //             w[i] = 1;
    //         }
    //         else if (i < n + 2*m){
    //             profit[i] = 0;
    //             w[i] = -1;
    //         }
    //         else{
    //            profit[i] = 0;
    //            w[i] = 0;
    //         }
            
    //     }
    //     else if (i >= V - K){
    //         delta[i] = 0;
    //         profit[i] = 0;
    //         w[i] = 0;
    //     }
    // }

    // for (int i = 0; i < V + inst->dummy; i++){
    //     if(i < n){
    //         e[i] = 540 + rand() % 480;
    //         l[i] = e[i];
    //     }
    //     else if (i < V + inst->dummy - 1){
    //         e[i] = 540;
    //         l[i] = 1020;
    //     }
    //     else{
    //         e[i] = 0;
    //         l[i] = 1440;
    //     }
    // }

    // for (int i = 0; i < V + inst->dummy; i++){
    //     node->e = e[i]/60;
    //     node->l = l[i]/60;
    //     node->delta = delta[i];
    //     node->profit = profit[i];
    //     node->load = w[i];
    //     nodeVec.push_back(*node);
    // }

    // double **dist = new double*[V + inst->dummy];
    // for (int i= 0; i < V + inst->dummy; i++){
    //     dist[i] = new double [V + inst->dummy];
    // }

    // for(int i = 0; i < V + inst->dummy; i++){
    //     for (int j = 0; j < V + inst->dummy; j++){
    //         dist[i][j] = realData[i][j];
    //     }
    // }
    // *Mdist = dist;
    // inst->K = K;
    // inst->n = n;
    // inst->m = m;
    // inst->V = V;
    // inst->service = service;
    // inst->T = nodeVec[V + inst->dummy - 1].l;

    // delete[] profit;
    // delete[] delta;

    // ifstream in2(instance2, ios::in);   


}

int main (int argc, char *argv[]) {

	double **distMatrix;
	double **auxdist;
	double cost;

	
	readData(argc, argv, &distMatrix);
    cout << "\n After function: " << endl;


return 0;

}

