#include "SarpADS.h"
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
        if (sStat->solOrder[k].size() < 1){
            continue;
        }
        for (int i = 0; i < sStat->solOrder[k].size() - 2; i++){
            // dij = mdist[sStat->solOrder[k][i]][sStat->solOrder[k][i + 1]];
            currNode = sStat->solOrder[k][i];
            nextNode = sStat->solOrder[k][i + 1];
            dij = mdist[currNode][nextNode];
            tij = (mdist[currNode][nextNode])/(inst->vmed);

            // stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;
            
            // // cout << "b - " << currNode << ": " << sStat->solBegin[currNode] << "; b - " << nextNode << ": " << sStat->solBegin[nextNode] << "; delta: " << nodeVec[currNode].delta << endl;
            // // getchar();

            // sStat->tStillP += stop;

            // cout << "\nTesting idle still time: " << endl;
            // cout << "tij: " << tij << " || stop: " << stop << " || sStat->tStill: " << sStat->tStill << endl;
            // getchar();

            if(currNode < inst->n){//from passenger
                if(nextNode < inst->n){//to passenger
                    if (load > 0){//carrying parcels
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tBoth += nodeVec[nextNode].delta;   

                        sStat->dParcel += dij;
                        distPass = (nodeVec[nextNode].delta - (2 * inst->service))*inst->vmed;
                        sStat->dBoth += distPass;
                    }  
                    else{//not carrying parcels
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

                else if (nextNode < inst->n + inst->m){//from passenger to parcel PU
                    if (load > 0){//with load
                        sStat->tParcel += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dParcel += dij;
                    }  
                    else{//no load
                        sStat->tNone += dij/inst->vmed;
                        sStat->tParcel += inst->service;
                        load++;

                        sStat->dNone += dij;
                    }
                    stop = sStat->solBegin[nextNode] - sStat->solBegin[currNode] - tij - nodeVec[currNode].delta;

                    sStat->tStillG += stop;
                }

                else if (nextNode < inst->n + 2*inst->m){//from passenger to parcel DL
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

void printStructures(nodeArcsStruct *nas){

	cout<< "\nArcs:" << endl;
    for (int i = 0; i < nas->arcs.size(); i++){
        if (i == 0){
            cout << setw(3) << " ";
        }
        cout << setw(3) << std::right << i << " ";
    }
    cout << endl;
    for(int i = 0; i < nas->arcs.size(); i++){
        cout << setw(3) << std::right << i;
        for(int j = 0; j <  nas->arcs[i].size(); j++){
            cout << setw(3) <<   nas->arcs[i][j] << " ";
        }
        cout << endl;
    }
    cout << endl;
    // getchar();

    // cout << "\n\nAll Arcs: " << endl;
    // for (int i = 0; i < nas->allArcs.size(); i++){
    //     cout << nas->allArcs[i].first << "-" << nas->allArcs[i].second << " ==== ";
    // }
    
    // cout << "\n\nArcs Plus: " << endl;
    // for (int i = 0; i < nas->arcPlus.size(); i++){
    //     for (int j = 0; j < nas->arcPlus[i].size(); j++){
    //         cout << nas->arcPlus[i][j].first << "-" << nas->arcPlus[i][j].second << " ==== ";
    //     }
    // }

    // cout << "\n\nArcs Minus: " << endl;
    // for (int i = 0; i < nas->arcMinus.size(); i++){
    //     for (int j = 0; j < nas->arcMinus[i].size(); j++){
    //         cout << nas->arcMinus[i][j].first << "-" << nas->arcMinus[i][j].second << " ==== ";
    //     }
    // }

    // cout << "\n\nArc NN: " << endl;
    // for (int i = 0; i < nas->arcNN.size(); i++){
    //     cout << nas->arcNN[i].first << "-" << nas->arcNN[i].second << " ==== ";
    // }

    // cout << "\n\nArc Nplus: " << endl;
    // for (int i = 0; i < nas->arcNplus.size(); i++){
    //     cout << nas->arcNplus[i].first << "-" << nas->arcNplus[i].second << " ==== ";
    // }
    
    // cout << "\n\nArc PN: " << endl;
    // for (int i = 0; i < nas->arcPN.size(); i++){
    // cout << nas->arcPN[i].first << "-" << nas->arcPN[i].second << " ==== ";
    // }

    // cout << "\n\nArc nf: " << endl;
    // for (int i = 0; i < nas->arcnf.size(); i++){
    //     cout << nas->arcnf[i].first << "-" << nas->arcnf[i].second << " ==== ";
    // }

    // cout << "\n\nArc V Plus: " << endl;
    // for (int i = 0; i < nas->vArcPlus.size(); i++){
    //     cout << "i: " << i << endl;
    //     for (int j = 0; j < nas->vArcPlus[i].size(); j++){
    //         cout << "j(k): " << j << endl;
    //         for(int k = 0; k < nas->vArcPlus[i][j].size(); k++){
    //             cout << nas->vArcPlus[i][j][k].first << "-" << nas->vArcPlus[i][j][k].second << " ==== ";
    //         }
    //     }
    //     cout << endl;
    // } 

    // cout << "\n\nArc V Minus: " << endl;
    // for (int i = 0; i < nas->vArcMinus.size(); i++){
    //     cout << "i: " << i << endl;
    //     for (int j = 0; j < nas->vArcMinus[i].size(); j++){
    //         cout << "j(k): " << j << endl;
    //         for(int k = 0; k < nas->vArcMinus[i][j].size(); k++){
    //             cout << nas->vArcMinus[i][j][k].first << "-" << nas->vArcMinus[i][j][k].second << " ==== ";
    //         }
    //     }
    //     cout << endl;
    // }

    // cout << "\n\narc V:" << endl;
    // for (int i = 0; i < nas->arcV.size(); i++){
    //     cout << "i: " << i << endl;
    //     for (int j = 0; j < nas->arcV[i].size(); j++){
    //         cout << "j: " << j << endl;
    //         for(int k = 0; k < nas->arcV[i][j].size(); k++){
    //             cout << nas->arcV[i][j][k] << " ==== ";
    //         }
    //     }
    //     cout << endl;
    // }

}

void fipStruct(instanceStat *inst, solStats *sStat, fipStats *fipStat){

    vector<int> pulocations;
    pair <int, int> pairpuloc;
    vector< pair<int, int> > pupairs;

    for (int i = 0; i < sStat->solOrder.size(); i++){
        for (int j = 0; j < sStat->solOrder[i].size(); j++){
            if (sStat->solOrder[i][j] < 2*inst->n){
                pulocations.push_back(sStat->solOrder[i][j]);
            }
        }
        fipStat->solPass.push_back(pulocations);
        pulocations.clear();
    }
    pulocations.clear();


    for (int k = 0; k < inst->K; k++){
        for (int i = 0; i < fipStat->solPass[k].size(); i++){
            if(fipStat->solPass[k][i] < inst->n){
                pairpuloc.first = fipStat->solPass[k][i];
                pairpuloc.second = i;
                pupairs.push_back(pairpuloc);
            }
        }
        fipStat->solPassOrigins.push_back(pupairs);
        pupairs.clear();
    }

    for (int i = 0; i < 2*inst->n; i++){
        fipStat->solBegin.push_back(sStat->solBegin[i]);
    }


    // for (int i = 0; i < 2*inst->n; i++){
    //     fipStat->fullBegin.push_back(fipStat->solBegin[i]);
    // }

    // for (int i = 2*inst->n; i < 2*inst->n + 2*inst->m; i++){
    //     fipStat->fullBegin.push_back(0);
    // }

    // cout << "Beginning of service: " << endl;

    // for (int a = 0; a < fipStat->solBegin.size(); a++){
    //     cout << "b(" << a << "): " << fipStat->solBegin[a] << endl;
    // }

    // cout << "\nSolution part II: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     cout << "Vehicle " << k << ": ";
    //     for (int i = 0; i < fipStat->solPass[k].size(); i++){
    //         if (i < fipStat->solPass[k].size() - 1){
    //             cout << fipStat->solPass[k][i] << " - ";
    //         }
    //         else{
    //             cout << fipStat->solPass[k][i];
    //         }
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    // cout << "\nSolution part III - pickup locations: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     cout << "Vehicle " << k << ": ";
    //     for (int i = 0; i < fipStat->solPassOrigins[k].size(); i++){
    //         if (i < fipStat->solPassOrigins[k].size() - 1){
    //             cout << fipStat->solPassOrigins[k][i].first << " : " << fipStat->solPassOrigins[k][i].second <<  " - ";
    //         }
    //         else{
    //             cout << fipStat->solPassOrigins[k][i].first << " : " << fipStat->solPassOrigins[k][i].second;
    //         }
    //     }
    //     cout << endl;
    // }
    // cout << endl;

    // sStat->solvec.clear();

    // //test sf sarp 5-6-1
    // for(int k = 0; k < inst->K; k++){
    //     fipStat->solPass.push_back(pulocations);
    //     fipStat->solPassOrigins.push_back(pupairs);
    // }

    // fipStat->solPass[0].push_back(0);
    // fipStat->solPass[0].push_back(5);
    // fipStat->solPass[0].push_back(4);
    // fipStat->solPass[0].push_back(9);
    // fipStat->solPass[0].push_back(2);
    // fipStat->solPass[0].push_back(7);
    // fipStat->solPass[1].push_back(1);
    // fipStat->solPass[1].push_back(6);
    // fipStat->solPass[1].push_back(3);
    // fipStat->solPass[1].push_back(8);
    
    // pairpuloc.first = 0;
    // pairpuloc.second = 0;
    // fipStat->solPassOrigins[0].push_back(pairpuloc);

    // pairpuloc.first = 4;
    // pairpuloc.second = 2;
    // fipStat->solPassOrigins[0].push_back(pairpuloc);

    // pairpuloc.first = 2;
    // pairpuloc.second = 4;
    // fipStat->solPassOrigins[0].push_back(pairpuloc);

    // pairpuloc.first = 1;
    // pairpuloc.second = 0;
    // fipStat->solPassOrigins[1].push_back(pairpuloc);

    // pairpuloc.first = 3;
    // pairpuloc.second = 2;
    // fipStat->solPassOrigins[1].push_back(pairpuloc);
}

void mergeFipSol(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat, fipStats *fipStat){
    
    vector<int> auxVec;
    vector<double> timeVec;
    double profCustomer = 0;
    double profParcel = 0;
    double costs = 0;
    // //starting the full solution vector with only passengers (copying existing initial solution)
    // for (int k = 0; k < fipStat->solPass.size(); k++){
    //     for (int i = 0; i < fipStat->solPass[k].size(); i++){
    //         auxVec.push_back(fipStat->solPass[k][i]); 
    //     }
    //     fipStat->fullSol.push_back(auxVec);
    //     auxVec.clear();
    // }
    int parcelCount = 0;

    fipStat->solprofit = 0;
    for (int i = 0; i < inst->n; i++){
        fipStat->solprofit += nodeVec[i].profit;
        profCustomer += nodeVec[i].profit;
    }

    for (int k = 0; k < inst->K; k++){
        if (fipStat->solPass[k].size() < 1){
			continue;
		}
        for (int i = 0; i < fipStat->solPass[k].size(); i++){
            // fipStat->solBeginParcel.
            auxVec.push_back(fipStat->solPass[k][i]);
            cout << "k, i: " << fipStat->solPass[k][i] << endl;
            for (int j = 0; j < fipStat->solvec[k].size(); j++){
                if (fipStat->solPass[k][i] == fipStat->solvec[k][j].first){
                    cout << "k, j: " << fipStat->solPass[k][i] << endl;
                    auxVec.push_back(fipStat->solvec[k][j].second);
                    fipStat->solprofit += nodeVec[fipStat->solvec[k][j].second].profit;
                }
            }

        }

        fipStat->fullSol.push_back(auxVec);
        auxVec.clear();
    }

    cout<< "\n\nFull solution: " << endl;

    for (int k = 0; k < fipStat->fullSol.size(); k++){
        cout << "Vehicle: ";
        for (int j = 0; j < fipStat->fullSol[k].size(); j++){
            if (j == fipStat->fullSol[k].size() - 1){
                cout << fipStat->fullSol[k][j] << endl;
            }
            else{
                cout << fipStat->fullSol[k][j] << " - ";
            }
            if (fipStat->fullSol[k][j] >= 2*inst->n && fipStat->fullSol[k][j] < 2*inst->n+inst->m){
                parcelCount++;
            }
        }
        
    }

    for (int i = 0; i < 2*inst->n; i++){
        fipStat->beginPass.push_back(0);
    }

    //Calculate beginning times for each location

    double currentT = 0;
    for (int k = 0; k < fipStat->fullSol.size(); k++){
        int currentDepot = 2*inst->n + 2*inst->m + k;
        int u = fipStat->fullSol[k][0];

        fipStat->solprofit -= inst->costkm*mdist[currentDepot][u];
        costs += inst->costkm*mdist[currentDepot][u];

        currentT = nodeVec[u].e;
        timeVec.push_back(currentT);
        fipStat->beginPass[u] = currentT;

        for (int i = 0; i < fipStat->fullSol[k].size() - 1; i++){
            u = fipStat->fullSol[k][i];
            int v = fipStat->fullSol[k][i+1];
            currentT += mdist[u][v]/inst->vmed + inst->service;

            fipStat->solprofit -= inst->costkm*mdist[u][v];
            costs += inst->costkm*mdist[u][v];
            if (v < 2*inst->n){
                if (currentT < nodeVec[v].e){
                    currentT = nodeVec[v].e;
                }
                fipStat->beginPass[v] = currentT;
            }
            else if (v < 2*inst->n + inst->m){
                profParcel += nodeVec[v].profit;
            }
            timeVec.push_back(currentT);

        }

        fipStat->fullBegin.push_back(timeVec);
        timeVec.clear();
    }


    cout<< "\n\nFull solution with times: " << endl;

    for (int k = 0; k < fipStat->fullSol.size(); k++){
        cout << "Vehicle: ";
        for (int j = 0; j < fipStat->fullSol[k].size(); j++){
            if (j == fipStat->fullSol[k].size() - 1){
                cout << fipStat->fullSol[k][j] << "(" << fipStat->fullBegin[k][j] << ")"<< endl;
            }
            else{
                cout << fipStat->fullSol[k][j] << "(" << fipStat->fullBegin[k][j] << ")"<< " - ";
            }
        }
        cout << " --> Total travel time: " << fipStat->fullBegin[k][fipStat->fullSol[k].size()-1] - fipStat->fullBegin[k][0] << endl;
    }

    // cout << "\n\nbegin pass vector: "<< endl;
    // for(int i = 0; i < fipStat->beginPass.size(); i++){
    //     cout << i << ": " << fipStat->beginPass[i] << " - ";
    // }
    
    cout << "\n\nFull solution value: " << fipStat->solprofit << endl;

    cout << "\n\nServed parcels: " << parcelCount << endl;

    cout << "\n\nProfit of customers: " << profCustomer << endl;

    cout << "\n\nProfit of parcels: " << profParcel << endl;
    
    cout << "\n\nCosts: " << costs << endl;

    //calculating individual costs and profits:


}

void calcPassDetour(instanceStat *inst, vector<nodeStat> &nodeVec, fipStats *fipStat){

    double ntrip, dtrip;
    double detour;

    for (int i = inst->n; i < 2*inst->n; i++){
        ntrip = nodeVec[i].e - nodeVec[i - inst->n].e;
        dtrip = fipStat->beginPass[i] - fipStat->beginPass[i - inst->n];
        // cout << "dtrip: " << dtrip << " - ntrip" << ntrip << endl;
        // getchar();
        detour = (double)((dtrip - ntrip)/(ntrip))*(double)(100);
        fipStat->passDetour.push_back(detour);
    }

    cout << "\n\nPassenger detour (%): ";
    
    for (int i = 0; i < inst->n; i++){
        cout << i << ": " << fipStat->passDetour[i] << " ";
    }
    cout << endl << endl;


}
