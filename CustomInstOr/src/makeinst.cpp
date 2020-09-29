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

struct CandStruct{
    int index;
    int realInd;
    bool chosen;
    int label1;
    int label2;
    //l1: 1 - customer; 2 - parcel; 3 - depot
    //l2: 1 - pickup; 2 - delivery; 3 - depot
};

double CalcDistEuc (double X1, double Y1, double X2, double Y2);
void genPoints (int argc, char** argv, int K, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda);
double fRand(double fMin, double fMax);
bool compareDist(const NodesStruct &a, const NodesStruct &b);
void classNodes(int n, int m, int K, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec);
void organizeNodes(int n, int m, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes);
bool compareIndex(const CandStruct &a, const CandStruct &b);
void createTimesLoad(int n, int m, vector< pair<double, double> > &tsVec, vector<CandStruct> &orgNodes, vector<int> &loadVec);

double CalcDistEuc (double X1, double Y1, double X2, double Y2){
    return sqrt ( pow ( X1 - X2, 2 ) + pow ( Y1 - Y2, 2 ) );
}

void genPoints (int argc, char** argv, int K, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda){

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

    vector<int> loadVec;

    vector<NodesStruct> closeVec;
    NodesStruct nodeVar;
    CandStruct candidate;
    
    pair<double, double> timeStamps;
    vector< pair<double, double> > tsVec;

    vector<int> unclassVec;
    vector<CandStruct> nodeVec;
    vector<CandStruct> orgNodes;
    
    int seed = 1234;
    srand(seed);

    vector< vector<double> > dist;
    vector<double> rowvec;

    string strN, strM, strP;

    //Generate random points:

    candidate.index = 999;
    candidate.realInd = 999;
    candidate.chosen = 0;
    candidate.label1 = 0;
    candidate.label2 = 0;

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
            // dimensions.second = 4;

            dimVec.push_back(dimensions);
        }
    }


    int totalPoints;
    double lb = 0;
    double ub = 10;
    for (int p = 1; p < 5; p++){
        for (int i = 0; i < dimVec.size(); i++){
            K = dimVec[i].first - 1;
            totalPoints = (dimVec[i].first + dimVec[i].second)*2 + K;

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

            // cout << "Coordinates: " << endl;

            // for (int j = 0; j < coordVec.size(); j++){
            //     cout << j << "\t" << setw(5) << coordVec[j].first << " " << setw(5) << coordVec[j].second << endl;
            // }

            // cout << endl;

            // getchar();

            // cout<< "Distance Matrix: " << endl;

            // for (int j = 0; j < dist.size(); j++){
            //     for(int k = 0; k < dist[j].size(); k++){
            //         cout << setw(6) << dist[j][k] << " ";
            //     }
            //     cout << endl;
            // }

            // cout << endl;
            // getchar();

            //Get closest nodes
            int lambda;

            for (int l = 0; l < vecOfLambda.size(); l++){

                for (int j = 0; j < totalPoints; j++){
                    unclassVec.push_back(j);
                }

                for (int j = 0; j < totalPoints; j++){
                    nodeVec.push_back(candidate);
                }

                lambda = vecOfLambda[l];
                classNodes(dimVec[i].first, dimVec[i].second, K, &lambda, nodeVar, closeVec, dist, unclassVec, nodeVec);
                
                organizeNodes(dimVec[i].first, dimVec[i].second, nodeVec, orgNodes);

                for (int j = 0; j < orgNodes.size(); j++){
                    tsVec.push_back(timeStamps);
                    loadVec.push_back(999);
                }

                createTimesLoad(dimVec[i].first, dimVec[i].second, tsVec, orgNodes, loadVec);

                //output
                string outputname;

                string instclass;

                if (lambda == 2){
                    instclass = 'A';
                }
                else if (lambda == 5){
                    instclass = 'B';
                }
                else if (lambda == 0){
                    instclass = 'C';
                }

                strN = to_string(dimVec[i].first);
                strM = to_string(dimVec[i].second);
                strP = to_string(p);

                outputname = "sarp-" + strN + "-" + strM + "-" + instclass + "-" + strP + ".txt";
                cout << "output: " << outputname << endl;
                getchar();

                ofstream ofile;

                ofile.open(outputname);
                
                ofile << K << "\t" << 5 << "\t" << dimVec[i].first << "\t" << dimVec[i].second << endl;

                for (int i = 0; i < orgNodes.size(); i++){
                    ofile << i << "\t" << setw(9) << fixed << setprecision(4) << coordVec[orgNodes[i].realInd].first << "\t" << setw(9) << coordVec[orgNodes[i].realInd].second << "\t" << loadVec[i] << "\t" << fixed << setprecision(0) << tsVec[i].first << "\t" << tsVec[i].second << endl;
                }

                unclassVec.clear();
                nodeVec.clear();
                tsVec.clear();
                orgNodes.clear();
            }

            coordVec.clear();
            dist.clear();
        }        
    }

}

int main (int argc, char *argv[]) {

    vector<int> vecOfn;
    vector<int> vecOfm;
    vector<int> vecOfLambda;
    int K;

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


    // vecOfn.push_back(5);
    
    // vecOfm.push_back(3);

    // vecOfn.push_back(6);
    // vecOfm.push_back(4);

    vecOfLambda.push_back(2);
    vecOfLambda.push_back(5);
    vecOfLambda.push_back(0);

	genPoints(argc, argv, K, vecOfn, vecOfm, vecOfLambda);

    return 0;

}

double fRand(double fMin, double fMax){
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

bool compareDist(const NodesStruct &a, const NodesStruct &b){
    return a.dist < b.dist;
}

bool compareIndex(const CandStruct &a, const CandStruct &b){
    if(a.index == b.index){
        return a.label2 < b.label2;    
    }
    else{
        return a.index < b.index; 
    }

}
void classNodes(int n, int m, int K, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec){
    // vector<int> candidateVec;
    int number;
    int chosenDel;

    int candPU;

    int limit;

    // cout << "Testing the sizes: n: " << n << " - m: " << m << endl;
    // getchar();

    for (int i = 0; i < K; i++){
        number = rand() % unclassVec.size();
        candPU = unclassVec[number];
        nodeVec[candPU].index = n + m + i;
        nodeVec[candPU].realInd = candPU;
        nodeVec[candPU].chosen = 1;
        nodeVec[candPU].label1 = 3;
        nodeVec[candPU].label2 = 3;

        unclassVec.erase(unclassVec.begin() + number);
    }

    for (int i = 0; i < n; i++){

        candPU = unclassVec[0];

        nodeVec[candPU].index = i;
        nodeVec[candPU].realInd = candPU;
        nodeVec[candPU].chosen = 1;
        nodeVec[candPU].label1 = 1;
        nodeVec[candPU].label2 = 1;

        unclassVec.erase(unclassVec.begin());

        closeVec.clear();

        for (int k = 0; k < unclassVec.size(); k++){
            if (dist[candPU][unclassVec[k]] > 0){
                nodeVar.index = unclassVec[k];
                nodeVar.dist = dist[candPU][unclassVec[k]];
                closeVec.push_back(nodeVar);                               
            }
        }

        sort(closeVec.begin(), closeVec.end(), compareDist);

        if (*lambda > 0){
            limit = *lambda;
        }
        else{
            limit = unclassVec.size();
        }

        number = rand() % limit;

        chosenDel = closeVec[number].index;
        nodeVec[chosenDel].index = i;
        nodeVec[chosenDel].realInd = chosenDel;
        nodeVec[chosenDel].chosen = 1;
        nodeVec[chosenDel].label1 = 1;
        nodeVec[chosenDel].label2 = 2;

        for (int j = 0; j < unclassVec.size(); j++){
            if (unclassVec[j] == chosenDel){
                unclassVec.erase(unclassVec.begin() + j);
            }
        }
    }
    
    for (int i = n; i < n + m; i++){

        candPU = unclassVec[0];

        nodeVec[candPU].index = i;
        nodeVec[candPU].realInd = candPU;
        nodeVec[candPU].chosen = 1;
        nodeVec[candPU].label1 = 2;
        nodeVec[candPU].label2 = 1;

        unclassVec.erase(unclassVec.begin());

        closeVec.clear();

        for (int k = 0; k < unclassVec.size(); k++){
            if (dist[candPU][unclassVec[k]] > 0){
                nodeVar.index = unclassVec[k];
                nodeVar.dist = dist[candPU][unclassVec[k]];
                closeVec.push_back(nodeVar);                               
            }
        }

        sort(closeVec.begin(), closeVec.end(), compareDist);

        if (*lambda > 0){
            limit = *lambda;
        }
        else{
            limit = unclassVec.size();
        }

        number = rand() % limit;
        
        chosenDel = closeVec[number].index;
        nodeVec[chosenDel].index = i;
        nodeVec[chosenDel].realInd = chosenDel;
        nodeVec[chosenDel].chosen = 1;
        nodeVec[chosenDel].label1 = 2;
        nodeVec[chosenDel].label2 = 2;

        for (int j = 0; j < unclassVec.size(); j++){
            if (unclassVec[j] == chosenDel){
                unclassVec.erase(unclassVec.begin() + j);
            }
        }
    }

    // for (int i = 0; i < K; i++){
    //     number = rand() % unclassVec.size();
    //     candPU = unclassVec[number];
    //     nodeVec[candPU].index = n + m + i;
    //     nodeVec[candPU].realInd = candPU;
    //     nodeVec[candPU].chosen = 1;
    //     nodeVec[candPU].label1 = 3;
    //     nodeVec[candPU].label2 = 3;

    //     unclassVec.erase(unclassVec.begin() + number);
    // }
}

void organizeNodes(int n, int m, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes){

    vector<CandStruct> auxVec;
    CandStruct lastNode;

    for (int i = 0; i < nodeVec.size(); i++){
        auxVec.push_back(nodeVec[i]);
    }

    sort(auxVec.begin(), auxVec.end(), compareIndex);

    // cout << "Ordered auxVec: " << endl;

    // for (int i = 0; i < auxVec.size(); i++){
    //     cout << i << ": " << auxVec[i].index << " - " << auxVec[i].label1 << " - " << auxVec[i].label2 << endl;
    // }
    // getchar();

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 1){
            if(auxVec[i].label2 == 1){
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 1){
            if(auxVec[i].label2 == 2){
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 2){
            if(auxVec[i].label2 == 1){
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 2){
            if(auxVec[i].label2 == 2){
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 3){
            orgNodes.push_back(auxVec[i]);
        }
    }
    
    // cout << "FINAL ORDERED : " << endl;
    // for (int i = 0; i < orgNodes.size(); i++){
    //     cout << i << ": " << orgNodes[i].index << " - " << orgNodes[i].realInd << " - " << orgNodes[i].label1 << " - " << orgNodes[i].label2 << endl;
    // }  
    // getchar();
}

void createTimesLoad(int n, int m, vector< pair<double, double> > &tsVec, vector<CandStruct> &orgNodes, vector<int> &loadVec)
{

    vector<double> auxts;
    double initts;

    for (int i = 0; i < n; i++){
        initts = 560 + rand() % 480;
        auxts.push_back(initts);
    }

    for (int i = 0; i < orgNodes.size(); i++){
        if (orgNodes[i].label1 == 3){
            tsVec[i].first = 540;
            tsVec[i].second = 1140;
            continue;
        }
        else if(orgNodes[i].label1 == 1){
            if (orgNodes[i].label2 == 1){
                tsVec[i].first = 560 + rand() % 480;
                tsVec[i].second = tsVec[i].first;
                continue;               
            }
            else if (orgNodes[i].label2 == 2){
                tsVec[i].first = tsVec[i - n].first + rand() % 480;
                tsVec[i].second = tsVec[i].first;
                continue;                   
            }

        }
        else if (orgNodes[i].label1 == 2){
            tsVec[i].first = 540;
            tsVec[i].second = 1020;
            continue;                          
        }
    }

    for (int i = 0; i < orgNodes.size(); i++){
        if(orgNodes[i].label1 == 1){
            loadVec[i] = 3;
            if (orgNodes[i].label2 == 2){
                loadVec[i] = loadVec[i]*(-1);
            }
        }
        else if(orgNodes[i].label1 == 2){
            loadVec[i] = 1;
            if (orgNodes[i].label2 == 2){
                loadVec[i] = loadVec[i]*(-1);
            }           
        }
        else{
            loadVec[i] = 0;
        }
    }

}
