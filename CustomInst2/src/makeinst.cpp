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
    int index; //index the random location will have in the final instance
    int realInd; //index of the random location in the list it was created
    bool chosen; //if this location is already assigned to a pass/parc/depot
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
    vector< vector< pair<int, int> > > dimVec;//vector of scale, with every combination of n and m
    vector< pair<double, double> > tsVec;
    vector< pair <double, double> > coordVec;
    vector<int> loadVec;
    vector<double> delta;

    vector<int> scaleVec;
    string scale;
    vector<int> vecOfn;
    vector<int> vecOfm;
    vector<int> vecOfLambda;
};

double CalcDistEuc (double X1, double Y1, double X2, double Y2);
void genPoints (int argc, char** argv, Info *info);
double fRand(double fMin, double fMax);
bool compareDist(const NodesStruct &a, const NodesStruct &b);
void classNodes(Info *info, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec);
void organizeNodes(Info *info, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes);
bool compareIndex(const CandStruct &a, const CandStruct &b);
void createTimesLoad(Info *info, vector<CandStruct> &orgNodes);
void createDelta(Info *info, vector<CandStruct> &orgNodes);
void getInstanceScale(int argc, char** argv, Info *info);

double CalcDistEuc (double X1, double Y1, double X2, double Y2){
    return sqrt ( pow ( X1 - X2, 2 ) + pow ( Y1 - Y2, 2 ) );
}

void genPoints (int argc, char** argv, Info *info){

    double maxX, maxY;

    pair<double, double> coordinate;

    vector<NodesStruct> closeVec;
    NodesStruct nodeVar;
    CandStruct candidate;
    
    pair<double, double> timeStamps;

    vector<int> unclassVec;
    vector<CandStruct> nodeVec;
    vector<CandStruct> orgNodes;
    
    // int seed = 1234;
    // srand(seed);

    vector< vector<double> > dist;
    vector<double> rowvec;

    string strN, strM, strP;

    //Generate random points:

    candidate.index = 999;
    candidate.realInd = 999;
    candidate.chosen = 0;
    candidate.label1 = 0;
    candidate.label2 = 0;

    int totalPoints, totalGenerated;
    double lb = 0;
    double ub = 25;
    int multiplier = 1;

    for (int p = 1; p < 3; p++){ // 2 of each lambda
        for (int i = 0; i < info->dimVec.size(); i++){
            for (int scd = 0; scd < info->dimVec[i].size(); scd++){
                info->n = info->dimVec[i][scd].first;
                info->m = info->dimVec[i][scd].second;
                info->seed = pow((info->n-1), 8) + pow((info->m-1), 7) + pow(p, 6);//changed (same as gh)
                       
                info->K = floor(info->n/2);
                totalPoints = (info->n + info->m)*2 + (info->K);
                totalGenerated = totalPoints + ceil(totalPoints/5);//generating more than needed because of bugs of missing coordinates

                for (int j = 0; j < totalGenerated; j++){
                    coordinate.first = fRand(lb, ub);
                    coordinate.second = fRand(lb, ub);

                    info->coordVec.push_back(coordinate);           
                }


                for (int j = 0; j < info->coordVec.size(); j++){
                    for (int k = 0; k < info->coordVec.size(); k++){
                        rowvec.push_back(0);
                    }
                    dist.push_back(rowvec);
                    rowvec.clear();
                }

                for (int j = 0; j < info->coordVec.size(); j++){
                    for (int k = 0; k < info->coordVec.size(); k++){
                        if(j == k){
                            dist[j][k] = 0;
                        }
                        else{
                            dist[j][k] = CalcDistEuc(info->coordVec[j].first, info->coordVec[j].second, info->coordVec[k].first, info->coordVec[k].second);
                        }
                    }
                }
                // cout << "Coordinates: " << endl;
 
                // for (int j = 0; j < info->coordVec.size(); j++){
                //     cout << j << "\t" << setw(5) << info->coordVec[j].first << " " << setw(5) << info->coordVec[j].second << endl;
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
                //getchar();

                double distSum, avgDist, totalAvg;

                vector<double> avgVec;

                for (int i = 0; i < dist.size(); i++){
                    distSum = 0;
                    for (int j = 0; j < dist[i].size(); j++){
                        distSum += (double)(dist[i][j]);
                    }
                    avgDist = distSum/(dist.size());
                    avgVec.push_back(avgDist);
                }

                // cout << "\nVector of average distances: " << endl; 
                // for(int i = 0; i < avgVec.size(); i++){
                //     cout << i << ": " << avgVec[i] << endl;
                // }
                // //getchar();

                // distSum = 0;

                // for(int i = 0; i < avgVec.size(); i++){
                //     distSum += avgVec[i];
                // }

                // // cout << "\nDistSum: " << distSum << endl;
                // totalAvg = distSum/avgVec.size();

                // // cout << "Total average: " << totalAvg << endl;
                // // //getchar();

                //Get closest nodes
                int lambda;

                for (int l = 0; l < info->vecOfLambda.size(); l++){

                    for (int j = 0; j < totalGenerated; j++){
                        unclassVec.push_back(j);
                    }

                    for (int j = 0; j < totalGenerated; j++){
                        nodeVec.push_back(candidate);
                    } 

                    lambda = info->vecOfLambda[l];


                    info->seed += pow(l + 1, 5); //different tw for different class instances (A, B and C)
                    srand(info->seed);
                    classNodes(info, &lambda, nodeVar, closeVec, dist, unclassVec, nodeVec);

                    organizeNodes(info, nodeVec, orgNodes);

                    for (int j = 0; j < orgNodes.size(); j++){
                        info->tsVec.push_back(timeStamps);
                        info->loadVec.push_back(999);
                    }

                    createDelta(info, orgNodes);
                    createTimesLoad(info, orgNodes);

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

                    strN = to_string(info->n);
                    strM = to_string(info->m);
                    strP = to_string(p);

                    outputname = "sarp-" + strN + "-" + strM + "-" + instclass + "-" + strP + ".txt";
                    cout << "output: " << outputname << endl;
                    //getchar();

                    ofstream ofile;

                    ofile.open(outputname);
                    
                    ofile << info->K << "\t" << 5 << "\t" << info->n << "\t" << info->m << endl;

                    for (int i = 0; i < orgNodes.size(); i++){
                        ofile << i << "\t" << setw(9) << fixed << setprecision(4) << info->coordVec[orgNodes[i].realInd].first << "\t" << setw(9) << info->coordVec[orgNodes[i].realInd].second << "\t" << info->loadVec[i] << "\t" << fixed << setprecision(0) << info->tsVec[i].first << "\t" << info->tsVec[i].second << endl;
                    }

                    unclassVec.clear();
                    nodeVec.clear();
                    info->tsVec.clear();
                    info->loadVec.clear();
                    orgNodes.clear();
                }

                info->coordVec.clear();
                dist.clear();
            }
        }  
    }
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
void classNodes(Info *info, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec){
    // vector<int> candidateVec;
    int number;
    int chosenDel;

    int candPU;

    int limit;

    srand(info->seed);

    cout << "Info seed: " << info->seed << endl;

    // cout << "Testing the sizes: n: " << n << " - m: " << m << endl;
    // //getchar();

    for (int i = 0; i < info->K; i++){
        number = rand() % unclassVec.size();
        candPU = unclassVec[number];
        nodeVec[candPU].index = info->n + info->m + i;
        nodeVec[candPU].realInd = candPU;
        nodeVec[candPU].chosen = 1;
        nodeVec[candPU].label1 = 3;
        nodeVec[candPU].label2 = 3;

        unclassVec.erase(unclassVec.begin() + number);
    }

    for (int i = 0; i < info->n; i++){

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


        // cout << "Close vec after sorting: " << endl;
        // for (int i = 0; i < closeVec.size(); i++){

        // }

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
        closeVec.clear();
    }

    for (int i = info->n; i < (info->n + info->m); i++){

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
    // vecOfn.push_back(5);
    // vecOfm.push_back(3);
        nodeVec[chosenDel].realInd = chosenDel;
        nodeVec[chosenDel].chosen = 1;
        nodeVec[chosenDel].label1 = 2;
        nodeVec[chosenDel].label2 = 2;

        for (int j = 0; j < unclassVec.size(); j++){
            if (unclassVec[j] == chosenDel){
                unclassVec.erase(unclassVec.begin() + j);
            }
        }
        closeVec.clear();
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
    // cout << "Final vector: " << unclassVec.size() << endl;
    // getchar();
    // cout << "Node  Vec so far: " << endl;
    // for (int i = 0; i < nodeVec.size(); i++){

    // }
}

void organizeNodes(Info *info, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes)
{

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
    // //getchar();

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 1){//customer
            if(auxVec[i].label2 == 1){//pickup
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 1){//customer
            if(auxVec[i].label2 == 2){//delivery
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 2){//parcel
            if(auxVec[i].label2 == 1){//pickup
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 2){//parcel
            if(auxVec[i].label2 == 2){//delivery
                orgNodes.push_back(auxVec[i]);
            }
        }
    }

    for (int i = 0; i < auxVec.size(); i++){
        if(auxVec[i].label1 == 3){//depot
            orgNodes.push_back(auxVec[i]);
        }
    }
    
    // cout << "FINAL ORDERED : " << endl;
    // for (int i = 0; i < orgNodes.size(); i++){
    //     cout << i << ": " << orgNodes[i].index << " - " << orgNodes[i].realInd << " - " << orgNodes[i].label1 << " - " << orgNodes[i].label2 << endl;
    // }  
    // //getchar();
}

void createDelta(Info *info, vector<CandStruct> &orgNodes)
{

    pair <double,double> p1, p2;

    double dist;
    double delta;

    for (int i = 0; i < info->n; i++){
        int j = info->n + i;
        p1.first = info->coordVec[orgNodes[i].realInd].first;
        p1.second = info->coordVec[orgNodes[i].realInd].second;
        p2.first = info->coordVec[orgNodes[j].realInd].first;
        p2.second = info->coordVec[orgNodes[j].realInd].second;

        dist = CalcDistEuc(p1.first, p1.second, p2.first, p2.second);
 
        delta = (double)dist/info->speed;

        info->delta.push_back(delta);
    }

    // cout <<"Delta vector: " << endl;

    // for (int i = 0; i < info->delta.size(); i++){
    //     cout << info->delta[i] << endl;
    // }
    // //getchar();

}

void createTimesLoad(Info *info, vector<CandStruct> &orgNodes)
{

    vector<double> auxts;
    double initts;

    srand(info->seed);

    // for (int i = 0; i < n; i++){
    //     initts = rand() % 1440;
    //     auxts.push_back(initts);
    // }
    //l1: 1 - customer; 2 - parcel; 3 - depot
    //l2: 1 - pickup; 2 - delivery; 3 - depot

    for (int i = 0; i < orgNodes.size(); i++){
        if (orgNodes[i].label1 == 3){//depot
            info->tsVec[i].first = 0;
            info->tsVec[i].second = 1440;
            continue;
        }
        else if(orgNodes[i].label1 == 1){//customer
            if (orgNodes[i].label2 == 1){//pickup
                // tsVec[i].first = 560 + rand() % 480;
                // tsVec[i].second = tsVec[i].first;
                info->tsVec[i].first = 30 + rand() % 1300;
                while (info->tsVec[i].first + info->delta[i] > 1440){
                    info->tsVec[i].first = 30 + rand() % 1300;
                    // cout << "Time point for node " << i << ": " << info->tsVec[i].first << endl; 
                    // //getchar();
                }
                info->tsVec[i].second = info->tsVec[i].first;               
                continue;               
            }
            else if (orgNodes[i].label2 == 2){//customer delivery
                // tsVec[i].first = tsVec[i - n].first + rand() % 480;
                // tsVec[i].second = tsVec[i].first;
                int pu = i - info->n;
                info->tsVec[i].first = info->tsVec[pu].first;
                info->tsVec[i].second = info->tsVec[pu].second;
                continue;                   
            }

        }
        else if (orgNodes[i].label1 == 2){//parcel
            info->tsVec[i].first = 0;
            info->tsVec[i].second = 1440;
            continue;                          
        }
    }

    for (int i = 0; i < orgNodes.size(); i++){
        if(orgNodes[i].label1 == 1){//customer
            info->loadVec[i] = 3;
            if (orgNodes[i].label2 == 2){//delivery
                info->loadVec[i] = info->loadVec[i]*(-1);
            }
        }
        else if(orgNodes[i].label1 == 2){//parcel
            info->loadVec[i] = 1;
            if (orgNodes[i].label2 == 2){//delivery
                info->loadVec[i] = info->loadVec[i]*(-1);
            }           
        }
        else{//depot
            info->loadVec[i] = 0;
        }
    }

}

void getInstanceScale(int argc, char** argv, Info *info){
    
    if (argc < 2) {
        cout << "\nMissing parameters\n";
        cout << " ./exeCustInst [Instance Scale]"<< endl;
        exit(1);
    }
    
    if (argc > 2) {
        cout << "\nToo many parameters\n";
        cout << " ./exeCustInst [Instance Scale]" << endl;
        exit(1);
    }

    pair<int, int> dimensions;
    vector< pair<int, int> > auxvec;

    info->vecOfLambda.push_back(2);
    info->vecOfLambda.push_back(5);
    info->vecOfLambda.push_back(0);

    info->scale = argv[1];

    if(info->scale == "S"){
        for (int i = 5; i < 11; i++){
            info->vecOfm.push_back(i);
            info->vecOfn.push_back(i);
        }
        for (int i = 10; i < 16; i++){
            info->scaleVec.push_back(i);
            info->dimVec.push_back(auxvec);
        }
        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }

    }
    else if(info->scale == "M"){
       for (int i = 5; i < 31; i++){
            if (i % 5 == 0 && i != 25){
                info->vecOfn.push_back(i);
            }
        }
        for (int i = 10; i < 31; i++){
            if (i % 5 == 0 && i != 25){
                info->vecOfm.push_back(i);
            }
        }

        for (int i = 20; i < 61; i++){
             if (i % 5 == 0 && i != 55){
                info->scaleVec.push_back(i);
                info->dimVec.push_back(auxvec);
            }           
        }

        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }


    }
    else if(info->scale == "L"){
       for (int i = 20; i < 51; i++){
            if (i % 10 == 0){
                info->vecOfn.push_back(i);
            }
        }
        for (int i = 40; i < 71; i++){
            if (i % 10 == 0){
                info->vecOfm.push_back(i);
            }
        }

        for (int i = 70; i < 121; i++){
             if (i % 10 == 0){
                info->scaleVec.push_back(i);
                info->dimVec.push_back(auxvec);
            }           
        }

        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }
    }
    else if(info->scale == "LL1"){
    //    for (int i = 60; i < 201; i++){
    //         if (i % 20 == 0){
    //             info->vecOfn.push_back(i);
    //         }
    //     }
    //     for (int i = 80; i < 201; i++){
    //         if (i % 30 == 0){
    //             info->vecOfm.push_back(i);
    //         }
    //     }

        for (int i = 150; i < 301; i++){
             if (i % 50 == 0){
                info->scaleVec.push_back(i);
                info->dimVec.push_back(auxvec);
            }           
        }

    //     for (int k = 0; k < info->scaleVec.size(); k++){
    //         for (int i = 0; i < info->vecOfn.size(); i++){
    //             for (int j = 0; j < info->vecOfm.size(); j++){
    //                 dimensions.first = info->vecOfn[i];
    //                 dimensions.second = info->vecOfm[j];
    //                 if (dimensions.first + dimensions.second < info->scaleVec[k]){
    //                     continue;
    //                 }
    //                 else if (dimensions.first + dimensions.second > info->scaleVec[k]){
    //                     break;
    //                 }
    //                 else{
    //                     info->dimVec[k].push_back(dimensions);
    //                 }
    //             }
    //         }
    //     }
        dimensions.first = 60;
        dimensions.second = 90;
        info->dimVec[0].push_back(dimensions);
        dimensions.first = 70;
        dimensions.second = 80;
        info->dimVec[0].push_back(dimensions);
        dimensions.first = 80;
        dimensions.second = 120;
        info->dimVec[1].push_back(dimensions);
        dimensions.first = 100;
        dimensions.second = 100;
        info->dimVec[1].push_back(dimensions);
        dimensions.first = 120;
        dimensions.second = 80;
        info->dimVec[1].push_back(dimensions);
        dimensions.first = 80;
        dimensions.second = 170;
        info->dimVec[2].push_back(dimensions);
        dimensions.first = 100;
        dimensions.second = 150;
        info->dimVec[2].push_back(dimensions);
        dimensions.first = 150;
        dimensions.second = 100;
        info->dimVec[2].push_back(dimensions);
        dimensions.first = 100;
        dimensions.second = 200;
        info->dimVec[3].push_back(dimensions);       
        dimensions.first = 200;
        dimensions.second = 100;
        info->dimVec[3].push_back(dimensions);       

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }
        getchar();
    }
    else{
        cout << "\nInstance scale should be:\n";
        cout << "1.Small (S)\n2.Medium (M)\n3.Large (L)\n4.Larger1 (LL1)\n" << endl;
        exit(1);
    }
}

int main (int argc, char *argv[]) {

    Info info;

    getInstanceScale(argc, argv, &info);

    genPoints(argc, argv, &info);

    return 0;

}