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


double CalcDistEuc (double X1, double Y1, double X2, double Y2);
void genPoints (int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda);
double fRand(double fMin, double fMax);

double CalcDistEuc (double X1, double Y1, double X2, double Y2){
    return sqrt ( pow ( X1 - X2, 2 ) + pow ( Y1 - Y2, 2 ) );
}

struct nodeArcsStruct{
    vector<double> closestDist;
    vector<int> closestNode;
};

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

    int seed = 1234;
    srand(seed);

    // vector <vector <double> > tempData;
    // vector<double> auxtempdata;

    // vector< pair <double, double> > start;

    // vector <vector <double> > realData;

    // vector<double> service;

    vector< vector<double> > dist;
    // vector< vector<double> > realdist;

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
    double ub = 10;

    for (int i = 0; i < dimVec.size(); i++){
        totalPoints = dimVec[i].first + dimVec[i].second + 1;

        coordinate.first = fRand(lb, ub);
        coordinate.second = fRand(lb, ub);

        cout <<  "coordinates: " << std::fixed << std::setprecision(3) << coordinate.first << ", " << std::fixed << setprecision(3) << coordinate.second << endl;
        getchar();
    }


    // if (it1 != it2){
    //     ifstream in1(instance1, ios::in);

        
    //     if( !in1 ) {
    //         cout << "the file could not be opened\n";
    //         exit (1);
    //     }

    //     while ( file1.compare("DIMENSION:") != 0 && file1.compare("DIMENSION") != 0 ){
    //         in1 >> file1;
    //     }

    //     in1 >> s1;

    //     n = (s1 - 2)/2;

    //     while (file1.compare("NODE_COORD_SECTION") != 0){
    //         in1 >> file1;
    //     }

    //     for (int i = 0; i < s1 - 2; i++){
    //         coordVec.push_back(coordinate);
    //     }


    //     for (int i = 0; i < s1; i++){

    //         if (i == 0){
    //             in1 >> file1;
    //             in1 >> coordinate.first;
    //             in1 >> coordinate.second;
    //             start.push_back(coordinate);
    //         }

    //         else if (i == 1){
    //             in1 >> file1;
    //             in1 >> file1;
    //             in1 >> file1;
    //         }
            
    //         else{
    //             in1 >> file1;
    //             in1 >> coordVec[i - 2].first;
    //             in1 >> coordVec[i - 2].second;

    //         }

    //     }

    //     ifstream in2(instance2, ios::in);

        
    //     if( !in2 ) {
    //         cout << "the file could not be opened\n";
    //         exit (1);
    //     }

    //     while ( file2.compare("DIMENSION:") != 0 && file2.compare("DIMENSION") != 0 ){
    //         in2 >> file2;
    //     }

    //     in2 >> s2;

    //     // m = (s2 - 2)/2;
    //     m = 3;
    //     s2 = m*2 + 2;

    //     while (file2.compare("NODE_COORD_SECTION") != 0){
    //         in2 >> file2;
    //     }

    //     for (int i = 0; i < s2 - 2; i++){
    //         coordVec.push_back(coordinate);
    //     }

    //     for (int i = s1 - 2; i < s1+s2 - 2; i++){

    //         if (i == s1 - 2){
    //             in2 >> file2;
    //             in2 >> coordinate.first;
    //             in2 >> coordinate.second;
    //             start.push_back(coordinate);
    //         }
    //         else if (i == s1 - 1){
    //             in2 >> file2;
    //             in2 >> file2;
    //             in2 >> file2;
    //         }

    //         else{
    //             in2 >> file2;
    //             in2 >> coordVec[i - 2].first;
    //             in2 >> coordVec[i - 2].second;            
    //         }
    //     }

    //     int count;
    //     pair<double, double> chosen;

    //     //rearranging pickup and delivery
    //     count = 1;
    //     for (int i = 2; i < 2*m; i++){
    //         if (i % 2 == 0){
    //             chosen = coordVec[2*n + i];
    //             coordVec.erase(coordVec.begin() + 2*n + i);
    //             coordVec.insert(coordVec.begin() + 2*n + count, chosen);
    //             count++;
    //         }
    //     }

    //     for(int i = 0; i < start.size(); i++){
    //         coordVec.push_back(start[i]);
    //     }

    //     for (int i = 0; i < coordVec.size(); i++){
    //         if (i < n){
    //             service.push_back(0);
    //         }
    //         for (int j = 0; j < coordVec.size(); j++){
    //             rowvec.push_back(0);
    //         }
    //         dist.push_back(rowvec);
    //         rowvec.clear();
    //     }

    //     for (int i = 0; i < n + 2*m + 2; i++){
    //         for (int j = 0; j < n + 2*m + 2; j++){
    //             rowvec.push_back(0);
    //         }
    //         realdist.push_back(rowvec);
    //         rowvec.clear();
    //     }


    //     for (int i = 0; i < 2*n; i++){
    //         if(i % 2 == 0){
    //             service[i/2] = floor( CalcDistEuc ( coordVec[i].first, coordVec[i].second, coordVec[i+1].first, coordVec[i+1].second ) + 0.5 );
    //         }
    //     }

    //     for (int i = 0; i < coordVec.size(); i++){
    //         for (int j = 0; j < coordVec.size(); j++){
    //             dist[i][j] = floor ( CalcDistEuc (coordVec[i].first, coordVec[i].second, coordVec[j].first, coordVec[j].second) + 0.5 );                   
    //         }
    //     }
    //     cout << endl;

    //     //Shrink matrix

    //     for (int i = 0; i < n + 2*m + 2; i++){
    //         for (int j = 0; j < n + 2*m + 2; j++){
    //             if (i == j){
    //                 realdist[i][j] = 0;
    //             }
    //             else{
    //                 if (i < n){
    //                     if (j < n){
    //                         realdist[i][j] = (dist[2*i+1][2*j]);
    //                     }
    //                     else{
    //                         realdist[i][j] = (dist[2*i+1][n+j]);
    //                     }
    //                 }
    //                 else{
    //                     if (j < n){
    //                         realdist[i][j] = (dist[n+i][2*j]);
    //                     }
    //                     else{
    //                         realdist[i][j] = (dist[n+i][n+j]);
    //                     }
    //                 }
    //             }
    //         }
    //     }


    //     //adding dummy node

    //     for (int i = 0; i < realdist.size(); i++){
    //         realdist[i].push_back(0);
    //         rowvec.push_back(0);
    //     }

    //     rowvec.push_back(0);
    //     realdist.push_back(rowvec);

    //     // //to screen

    //     // cout << "DIMENSION: " << n + 2*m + 2;
    //     // cout << "\nN: " << n;
    //     // cout << "\nM: " << m;
    //     // cout << "\nK: " << 2;

    //     // cout << endl;

    //     // cout << "\nSERVICE: " << endl;
    //     // for (int i = 0; i < service.size(); i++){
    //     //     cout << service[i] << " ";
    //     // }
    //     // cout << endl;

    //     // cout << "\nDIST MATRIX: " << endl;

    //     // for (int i = 0; i < realdist.size(); i++){
    //     //     for(int j = 0; j < realdist[i].size(); j++){
    //     //         cout << setw(5) << realdist[i][j] << " "; 
    //     //     }
    //     //     cout << endl;
    //     // }
    //     // getchar();

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

