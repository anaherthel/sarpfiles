#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void solStatIni(solStats *sStat){

    sStat->tParcel = 0;
    sStat->tPass = 0;
    sStat->tBoth = 0;
    sStat->tNone = 0;

    sStat->tStillP = 0;
    sStat->tStillG = 0;
    sStat->tStill = 0;


    sStat->dParcel = 0;
    sStat->dPass = 0;
    sStat->dBoth = 0;
    sStat->dNone = 0;

    sStat->solOrder.clear();

    sStat->servedParcels = 0;

    sStat->pProfit = 0;
    sStat->costs = 0;

    // sStat->solOrder.clear();
    // sStat->solvec.clear();
}

void mipSolStats (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat){

    int load;
    double distPass;
    // load = 0;
    double dij;
    double stop;
    double tij;
    int currNode;
    int nextNode;
    
    for (int k = 0; k < inst->K; k++){
        load = 0;
        for (int i = 0; i < sStat->solOrder[k].size() - 2; i++){
            // dij = mdist[sStat->solOrder[k][i]][sStat->solOrder[k][i + 1]];
            currNode = sStat->solOrder[k][i];
            nextNode = sStat->solOrder[k][i + 1];
            dij = mdist[currNode][nextNode];
            tij = mdist[currNode][nextNode]/inst->vmed;

            // stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;
            
            // // cout << "b - " << currNode << ": " << sStat->solBegin[currNode] << "; b - " << nextNode << ": " << sStat->solBegin[nextNode] << "; delta: " << nodeVec[currNode].delta << endl;
            // // getchar();

            // sStat->tStillP += stop;

            // cout << "\nTesting idle still time: " << endl;
            // cout << "tij: " << tij << " || stop: " << stop << " || sStat->tStill: " << sStat->tStill << endl;
            // getchar();

            if(currNode < inst->n){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta;   

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        // sStat->tNoneP += dij/inst->vmed;

                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;

                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                    }

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillP += stop;
                }

                else if (nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }
                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop;
                }

                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop;
                }
            }
            else if (currNode < inst->n + inst->m){
                if (nextNode < inst->n){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tBoth += nodeVec[nextNode].delta;

                    sStat->dParcel += dij;
                    distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                    sStat->dBoth += distPass;
                    
                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillP += stop;  
                }
                else if(nextNode < inst->n + inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load++;

                    sStat->dParcel += dij;         

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop; 
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop; 
                }
            }
            else if (currNode < inst->n + 2*inst->m){
                if(nextNode < inst->n){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta; 

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;                           
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;
                                               
                    }

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillP += stop; 

                }
                else if(nextNode < inst->n + inst->m){
                    if (load > 0){
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop;                                       
                }
                else if (nextNode < inst->n + 2*inst->m){
                    sStat->tParcel += dij/inst->vmed;
                    sStat->tParcel += inst->service;
                    load--;

                    sStat->dParcel += dij;

                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop; 
                }
            }
            else{
                if (nextNode < inst->n + 2*inst->m + inst->K){
                    if(nextNode < inst->n){
                        sStat->tNone += dij/inst->vmed;
                        sStat->tPass += nodeVec[nextNode].delta;
                        load = 0;

                        sStat->dNone += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dPass += distPass;  

                        stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                        sStat->tStillP += stop; 
                    }
                    else if(nextNode < inst->n + inst->m){
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;

                        stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                        sStat->tStillG += stop;
                    }  
                }
            }


            // cout << "\nTotal passenger time: " << sStat->tPass << endl;
            // cout << "\nTotal parcel time: " << sStat->tParcel << endl;
            // cout << "\nTotal combined transportation time: " << sStat->tBoth << endl;
            // cout << "\nTotal idle time: " << sStat->tNone << endl;

            // cout << "\nTotal passenger distance: " << sStat->dPass << endl;
            // cout << "\nTotal parcel distance: " << sStat->dParcel << endl;
            // cout << "\nTotal combined transportation distance: " << sStat->dBoth << endl;
            // cout << "\nTotal idle distance: " << sStat->dNone << endl;
            // getchar();

        }
    }
}

void printStats(instanceStat *inst, solStats *sStat){
    // for (int i = 0; i < inst->K; i++){


        cout << "\nsize of n: " << inst->n << endl;
        cout << "\nsize of m: " << inst->m << endl;
        
        cout << "\n*************" << endl;

        cout << "\n\nServed parcels: " << sStat->servedParcels << endl;
        cout << "\nUnserved parcels: " << inst->m - sStat->servedParcels << endl;


        cout << "\n*************" << endl;

        cout << "\nTotal time: " << sStat->tPass + sStat->tParcel + sStat->tBoth + sStat->tNone << endl;
        cout << "\nTotal passenger time: " << sStat->tPass << endl;
        cout << "\nTotal parcel time: " << sStat->tParcel << endl;
        cout << "\nTotal combined transportation time: " << sStat->tBoth << endl;
        cout << "\nTotal idle time: " << sStat->tNone << endl;

        cout << "\n*************" << endl;

        cout << "\nTotal distance: " << sStat->dPass + sStat->dParcel + sStat->dBoth + sStat->dNone << endl;
        cout << "\nTotal passenger distance: " << sStat->dPass << endl;
        cout << "\nTotal parcel distance: " << sStat->dParcel << endl;
        cout << "\nTotal combined transportation distance: " << sStat->dBoth << endl;
        cout << "\nTotal idle distance: " << sStat->dNone << endl;

        cout << "\n*************" << endl;

        cout << "\nWaiting time passenger: " << sStat->tStillP << endl;
        cout << "\nWaiting time goods: " << sStat->tStillG << endl;
        cout << "\nTotal waiting time: " << sStat->tStillG + sStat->tStillP << endl;
    // }

}

void distScale(instanceStat *inst, int *instV, vector <vector <double> > &tempData, double *curAvg, int *scale){
    double distSum, avgDist, totalAvg;

    vector<double> avgVec;

    distSum = 0;
    // cout << "\nNumber of nodes: " << tempData.size() << " and " << tempData[0].size() << endl;

    cout << "\ncurrent scale: " << *scale << endl;
    cout << "\ncurrent average: " << *curAvg << endl;
    getchar();

    // cout << "\nDistance Matrix (Pre-adapting): " << endl;

    // for (int i = 0; i < tempData.size() - 1; i++){
    //     for (int j = 0; j < tempData[i].size() - 1; j++){
    //         cout << setw(5) << setprecision(5) << tempData[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // getchar();

    for (int i = 0; i < tempData.size() - 1; i++){
        distSum = 0;
        for (int j = 0; j < tempData.size() - 1; j++){
            distSum += (double)(tempData[i][j]/ *scale);
        }
        avgDist = distSum/(tempData.size() - 1);
        avgVec.push_back(avgDist);
    }

    cout << "\nVector of average distances: " << endl; 
    for(int i = 0; i < avgVec.size(); i++){
        cout << i << ": " << avgVec[i] << endl;
    }
    getchar();

    distSum = 0;

    for(int i = 0; i < avgVec.size(); i++){
        distSum += avgVec[i];
    }

    cout << "\nDistSum: " << distSum << endl;
    totalAvg = distSum/avgVec.size();


    cout << totalAvg << endl;
    *curAvg = totalAvg;
}
