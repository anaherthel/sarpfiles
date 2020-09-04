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

struct NodesStruct{
    double dist;
    int index;
};

double CalcDistEuc (double X1, double Y1, double X2, double Y2);
void genPoints (int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda);
double fRand(double fMin, double fMax);
bool compareDist(const NodesStruct &a, const NodesStruct &b);




double CalcDistEuc (double X1, double Y1, double X2, double Y2){
    return sqrt ( pow ( X1 - X2, 2 ) + pow ( Y1 - Y2, 2 ) );
}

void genPoints (int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda){

    if (argc < 1) {
        cout << "\nMissing parameters\n";
        cout << " ./exeCustInst"<< endl;
        exit(1);
    }
    
    if (argc > 1) {
        cout << "\nToo many parameters\n";
        cout << " ./exeCustInst" << endl;
        exit(1);
    }  

    double maxX, maxY;

    vector< pair<int, int> > dimVec;
    pair<int, int> dimensions;

    pair<double, double> coordinate;

    vector< pair <double, double> > coordVec;

    vector<NodesStruct> closeVec;
    NodesStruct nodeVar;
    
    vector<int> unclassVec;
    vector<int> passVec;
    vector<int> parcVec;
    vector<int> depotNode;

    int seed = 1234;
    srand(seed);

    vector< vector<double> > dist;
    vector<double> rowvec;

    //Generate random points:

    for (int i = 0; i < vecOfn.size(); i++){
        for (int j = 0; j < vecOfm.size(); j++){

            if (vecOfn[i] + vecOfm[j] > 15){
                if (vecOfn[i] + vecOfm[j] < 35){
                    dimensions.first = vecOfn[i];
                    dimensions.second = vecOfm[j];

                    dimVec.push_back(dimensions);   
                }             
            }
        }
        if (vecOfn[i] < 15){
            dimensions.first = vecOfn[i];
            dimensions.second = 20;

            dimVec.push_back(dimensions);
        }
    }

    // cout << "Test if dimensions vec is ok: " << endl;

    // for (int i = 0; i < dimVec.size(); i++){
    //     cout << dimVec[i].first << " + " << dimVec[i].second << endl;
    // }

    // cout << endl;
    // getchar();

    int totalPoints;
    double lb = 0;
    double ub = 20;

    for (int i = 0; i < dimVec.size(); i++){
        totalPoints = (dimVec[i].first + dimVec[i].second)*2 + 1;

        for (int j = 0; j < totalPoints; j++){
            coordinate.first = fRand(lb, ub);
            coordinate.second = fRand(lb, ub);

            coordVec.push_back(coordinate);           
        }
    
        for (int j = 0; j < coordVec.size(); j++){
            for (int k = 0; k < coordVec.size(); k++){
                rowvec.push_back(0);
            }
            dist.push_back(rowvec);
            rowvec.clear();
        }


        for (int j = 0; j < coordVec.size(); j++){
            for (int k = 0; k < coordVec.size(); k++){
                if(j == k){
                   dist[j][k] = 0;
                }
                else{
                    dist[j][k] = CalcDistEuc(coordVec[j].first, coordVec[j].second, coordVec[k].first, coordVec[k].second);
                }
            }
        }


        cout << "Coordinates: " << endl;

        for (int j = 0; j < coordVec.size(); j++){
            cout << setw(5) << coordVec[j].first << " " << setw(5) << coordVec[j].second << endl;
        }

        cout << endl;

        getchar();

        cout<< "Distance Matrix: " << endl;

        for (int j = 0; j < dist.size(); j++){
            for(int k = 0; k < dist[j].size(); k++){
                cout << setw(6) << dist[j][k] << " ";
            }
            cout << endl;
        }

        cout << endl;
        getchar();

        //Get closest nodes
        
        for (int j = 0 ; j < totalPoints; j++){
            unclassVec.push_back(j);
        }
        
        for (int j = 0; j < dist.size(); j++){
            for (int k = 0; k < dist[j].size(); k++){
                if (dist[j][k] > 0){
                    nodeVar.index = k;
                    nodeVar.dist = dist[j][k];
                    closeVec.push_back(nodeVar);                               
                }

            }
        
            sort(closeVec.begin(), closeVec.end(), compareDist);



            cout << "Ordered vec: " << endl;

            for(int l = 0; l < closeVec.size(); l++){
                cout << closeVec[l].index << " - " <<  closeVec[l].dist << " // ";
            }


            cout << endl;
            getchar();


        }

        //Sort 



    }


    //     //output
    //     string outputname;

    //     outputname = "grubhub-" + it1 + "-" + it2 + ".tsp";
    //     // cout << "output: " << outputname << endl;

    //     ofstream ofile;

    //     ofile.open(outputname);
        
    //     ofile << "DIMENSION: " << n + 2*m + 2;
    //     ofile << "\nN: " << n;
    //     ofile << "\nM: " << m;
    //     ofile << "\nK: " << 2;

    //     ofile << endl;

    //     ofile << "\nSERVICE: " << endl;
    //     for (int i = 0; i < service.size(); i++){
    //         ofile << service[i] << " ";
    //     }
    //     ofile << endl;

    //     ofile << "\nDIST MATRIX: " << endl;

    //     for (int i = 0; i < realdist.size(); i++){
    //         for(int j = 0; j < realdist[i].size(); j++){
    //             ofile << setw(5) << realdist[i][j] << " "; 
    //         }
    //         ofile << endl;
    //     }

    // }


}

int main (int argc, char *argv[]) {

    vector<int> vecOfn;
    vector<int> vecOfm;
    vector<int> vecOfLambda;

    for (int i = 10; i < 25; i++){
        if (i % 5 == 0){
            vecOfn.push_back(i);
        }
    }
	
    for (int i = 5; i < 20; i++){
        if (i % 5 == 0){
            vecOfm.push_back(i);
        }
    }

    vecOfLambda.push_back(2);
    vecOfLambda.push_back(5);

    // cout << "Vec of M: " << endl;

    // for (int i = 0; i < vecOfm.size(); i++){
    //     cout << vecOfm[i] << endl;
    // }
    // cout << endl;

    // getchar();
	genPoints(argc, argv, vecOfn, vecOfm, vecOfLambda);

    return 0;

}

double fRand(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

bool compareDist(const NodesStruct &a, const NodesStruct &b){
    return a.dist < b.dist;
}