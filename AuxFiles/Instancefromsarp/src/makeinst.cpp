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

struct OrStruct{
    double vxs;
    double vys;
    double vload;
    double ve;
    double vl;
    int index;
};

struct CandStruct{
    int index;
    int realInd;
    bool chosen;
    int label1;
    int label2;
    //l1: 1 - customer; 2 - parcel; 3 - depot
    //l2: 1 - pickup; 2 - delivery; 3 - depot
};

struct Info{
    long long seed;
    int n;
    int m;
    double speed = 41;
    int K;
    vector< pair<int, int> > dimVec;
    vector< pair<double, double> > tsVec;
    vector< pair <double, double> > coordVec;
    vector<int> loadVec;
    vector<double> delta;
};

// double CalcDistEuc (double X1, double Y1, double X2, double Y2);
// void genPoints (int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda, Info *info);
// double fRand(double fMin, double fMax);
// bool compareDist(const NodesStruct &a, const NodesStruct &b);
// void classNodes(Info *info, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec);
// void organizeNodes(Info *info, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes);
// bool compareIndex(const CandStruct &a, const CandStruct &b);
// void createTimesLoad(Info *info, vector<CandStruct> &orgNodes);
// void createDelta(Info *info, vector<CandStruct> &orgNodes);
void ReadData(int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, Info *info);
void extractData(Info *info, vector<OrStruct> &NdVec);
void output(Info *info, vector<OrStruct> &newvec, int p);

void ReadData(int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, Info *info)
{
    if (argc < 2) {
        cout << "\nMissing parameters\n";
        // cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]"<< endl;
        cout << " ./exe [Instance]"<< endl;
        exit(1);
    }
    
    if (argc > 2) {
        cout << "\nToo many parameters\n";
        cout << " ./exe [Instance]" << endl;
        exit(1);
    }

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    string file, ewf;
    int n;
    int m;
    int K;
    double service;
    int originalV;

    in >> K;
    in >> service;
    in >> n;
    in >> m;

    int V = n + m + 2;
    
    originalV = 2*n + 2*m + 2; 

    string strN, strM, strP;
    
    OrStruct nd;
    
    vector<OrStruct> Ndvec;

    int tempNode;

    for (int i = 0; i < originalV; i++){
        Ndvec.push_back(nd);
    }

    for (int i = 0; i < originalV; i++){
        in >> Ndvec[i].index >> Ndvec[i].vxs >> Ndvec[i].vys >> tempNode >> Ndvec[i].vload >> Ndvec[i].ve >> Ndvec[i].vl;
    }

    extractData(info, Ndvec);
}

void extractData(Info *info, vector<OrStruct> &Ndvec){

    double ninst = 5;

    vector<OrStruct> auxnp, auxnd, auxmp, auxmd, dp;

    vector<OrStruct> newvec;

    for (int i = 0; i < Ndvec.size() - 1; i++){
        if (Ndvec[i].vload == 3){
            auxnp.push_back(Ndvec[i]);
        }
        else if (Ndvec[i].vload == 1){
            auxmp.push_back(Ndvec[i]);
        }
        else if (Ndvec[i].vload == -3){
            auxnd.push_back(Ndvec[i]);

        }
        else if (Ndvec[i].vload == -1){
            auxmd.push_back(Ndvec[i]);
        }
        else{
            dp.push_back(Ndvec[i]);
        }
    }
    // cout << "\nAux ND: " << endl;

    // for (int i = 0; i < auxnd.size(); i++){
    //     cout << "i: " << auxnd[i].index << endl;
    // }

    // getchar();
    for (int i = 0; i < info->dimVec.size(); i++){
        for (int p = 1; p < ninst+1; p++){
            info->n = info->dimVec[i].first;
            info->m = info->dimVec[i].second;
            long power = pow(2, info->m);

            info->seed = (info->n*power)*(p);
            srand(info->seed);

            int newV = info->n + info->m + 1;
            int c;
            int counter = 0;

            for (int j = 0; j < info->n; j++){
                c = rand() % auxnp.size();
                // cout << "c: " << c;
                newvec.insert(newvec.begin() + counter, auxnp[c]);
                newvec.push_back(auxnd[c]);
                counter++;
                // cout << "vector populating n: " << endl;
                // for (int v = 0; v < newvec.size(); v++){
                //     cout << newvec[v].index << " " << newvec[v].vload << endl;
                // }
                // getchar();
            }
            counter = 0;
            for (int j = 0; j < info->m; j++){
                c = rand() % auxmp.size();
                newvec.insert(newvec.begin() + info->n + counter, auxmp[c]);
                newvec.push_back(auxmd[c]);
                counter++;
                // cout << "vector populating m: " << endl;
                // for (int v = 0; v < newvec.size(); v++){
                //     cout << newvec[v].index << " " << newvec[v].vload << endl;
                // }
                // getchar();
            }

            newvec.insert(newvec.begin(), dp[0]);

            // for (int i = 0; i < newvec.size(); i++){
            //     cout << "i: " << newvec[i].index << " - " << newvec[i].vload << " - " << newvec[i].ve << endl;
            // }
            // getchar();

            output(info, newvec, p);
        }

    }
}

int main (int argc, char *argv[]) {

    vector<int> vecOfn;
    vector<int> vecOfm;

    Info info;
    // for (int i = 10; i < 25; i++){
    //     if (i % 5 == 0){
    //         vecOfn.push_back(i);
    //     }
    // }
	
    // for (int i = 5; i < 20; i++){
    //     if (i % 5 == 0){
    //         vecOfm.push_back(i);
    //     }
    // }
 

    // vecOfn.push_back(5);
    
    // vecOfm.push_back(10);
    
    pair<int, int> dimensions;

    // for (int i = 0; i < vecOfn.size(); i++){
    //     for (int j = 0; j < vecOfm.size(); j++){

    //         if (vecOfn[i] + vecOfm[j] > 15){
    //             if (vecOfn[i] + vecOfm[j] < 35){
    //                 dimensions.first = vecOfn[i];
    //                 dimensions.second = vecOfm[j];

    //                 info.dimVec.push_back(dimensions);   
    //             }             
    //         }

    //         // dimensions.first = vecOfn[i];
    //         // dimensions.second = vecOfm[j];  
    //         // info.dimVec.push_back(dimensions);

    //     }
    //     // if (vecOfn[i] < 15){
    //     //     dimensions.first = vecOfn[i];
    //     //     // dimensions.second = 20;
    //     //     dimensions.second = 10;

    //     //     info.dimVec.push_back(dimensions);
    //     // }
    // }

//creating smaller instances <20

    // for (int i = 7; i < 11; i++){

    //     vecOfn.push_back(i);
    // }
  
    // for (int i = 5; i < 8; i++){
    //     vecOfm.push_back(i);
    // } 

    // for (int i = 0; i < vecOfn.size(); i++){
    //     for (int j = 0; j < vecOfm.size(); j++){

    //         if (vecOfn[i] + vecOfm[j] < 20){

    //             dimensions.first = vecOfn[i];
    //             dimensions.second = vecOfm[j];

    //             info.dimVec.push_back(dimensions);   
       
    //         }

    //         // dimensions.first = vecOfn[i];
    //         // dimensions.second = vecOfm[j];  
    //         // info.dimVec.push_back(dimensions);

    //     }
    //     // if (vecOfn[i] < 15){
    //     //     dimensions.first = vecOfn[i];
    //     //     // dimensions.second = 20;
    //     //     dimensions.second = 10;

    //     //     info.dimVec.push_back(dimensions);
    //     // }
    // }
//creating larger instances >30

    // for (int i = 20; i < 31; i++){
    //     if (i % 5 == 0){
    //         vecOfn.push_back(i);
    //     }
    // }
  
    // for (int i = 15; i < 26; i++){
    //     if (i % 5 == 0){
    //         vecOfm.push_back(i);
    //     }
    // } 

    // for (int i = 0; i < vecOfn.size(); i++){
    //     for (int j = 0; j < vecOfm.size(); j++){

    //         if (vecOfn[i] + vecOfm[j] > 30){

    //             dimensions.first = vecOfn[i];
    //             dimensions.second = vecOfm[j];

    //             info.dimVec.push_back(dimensions);   
       
    //         }

    //         // dimensions.first = vecOfn[i];
    //         // dimensions.second = vecOfm[j];  
    //         // info.dimVec.push_back(dimensions);

    //     }
    //     // if (vecOfn[i] < 15){
    //     //     dimensions.first = vecOfn[i];
    //     //     // dimensions.second = 20;
    //     //     dimensions.second = 10;

    //     //     info.dimVec.push_back(dimensions);
    //     // }
    // }

//create threshold

   for (int i = 4; i < 7; i++){
        // if (i % 5 == 0){
            vecOfn.push_back(i*10);
        // }
    }
  
    for (int i = 4; i < 7; i++){
        // if (i % 5 == 0){
            vecOfm.push_back(i*10);
        // }
    } 

    for (int i = 0; i < vecOfn.size(); i++){
        for (int j = 0; j < vecOfm.size(); j++){

            if (vecOfn[i] >= vecOfm[j]){

                if (vecOfn[j] == 40 && vecOfn[i] == 60){
                    continue;
                }
                else{
                    dimensions.first = vecOfn[i];
                    dimensions.second = vecOfm[j];

                    info.dimVec.push_back(dimensions);   
                }
            }

            // dimensions.first = vecOfn[i];
            // dimensions.second = vecOfm[j];  
            // info.dimVec.push_back(dimensions);

        }
        // if (vecOfn[i] < 15){
        //     dimensions.first = vecOfn[i];
        //     // dimensions.second = 20;
        //     dimensions.second = 10;

        //     info.dimVec.push_back(dimensions);
        // }
    }

    ReadData(argc, argv, vecOfn, vecOfm,  &info);

	// genPoints(argc, argv, vecOfn, vecOfm, vecOfLambda, &info);

    return 0;

}

void output(Info *info, vector<OrStruct> &newvec, int p)
{
    string outputname;
    string instclass;
    string strN, strM, strP;

    strN = to_string(info->n);
    strM = to_string(info->m);
    strP = to_string(p);
    info->K = floor(info->n/2);

    outputname = "sfsarp-" + strN + "-" + strM + "-" + strP + ".txt";
    cout << "output: " << outputname << endl;
    getchar();

    ofstream ofile;

    ofile.open(outputname);
    
    ofile << info->K << "\t" << 5 << "\t" << info->n << "\t" << info->m << endl;

    for (int i = 0; i < newvec.size(); i++){
        ofile << i << "\t" << setw(9) << fixed << setprecision(4) << newvec[i].vxs << "\t" << setw(9) << newvec[i].vys << "\t" << 0 << "\t" << fixed << setprecision(0) << newvec[i].vload << "\t" << fixed << setprecision(0) << newvec[i].ve << "\t" << newvec[i].vl << endl;
    }

    newvec.clear();
}