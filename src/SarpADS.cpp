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

void fipStatIni(fipStats *fipStat){

    fipStat->solprofit = 0;

    fipStat->tParcel = 0;
    fipStat->tPass = 0;
    fipStat->tBoth = 0;
    fipStat->tNone = 0;

    fipStat->dParcel = 0;
    fipStat->dPass = 0;
    fipStat->dBoth = 0;
    fipStat->dNone = 0;
}

void fipStatIniBundle(fipBundleStats *fipStat){

    fipStat->solprofit = 0;

    fipStat->tParcel = 0;
    fipStat->tPass = 0;
    fipStat->tBoth = 0;
    fipStat->tNone = 0;

    fipStat->dParcel = 0;
    fipStat->dPass = 0;
    fipStat->dBoth = 0;
    fipStat->dNone = 0;
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
            
            // // // TODO UNCOMMENT //  << "b - " << currNode << ": " << sStat->solBegin[currNode] << "; b - " << nextNode << ": " << sStat->solBegin[nextNode] << "; delta: " << nodeVec[currNode].delta << endl;
            // // getchar();

            // sStat->tStillP += stop;

            // // TODO UNCOMMENT //  << "\nTesting idle still time: " << endl;
            // // TODO UNCOMMENT //  << "tij: " << tij << " || stop: " << stop << " || sStat->tStill: " << sStat->tStill << endl;
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


            // // TODO UNCOMMENT //  << "\nTotal passenger time: " << sStat->tPass << endl;
            // // TODO UNCOMMENT //  << "\nTotal parcel time: " << sStat->tParcel << endl;
            // // TODO UNCOMMENT //  << "\nTotal combined transportation time: " << sStat->tBoth << endl;
            // // TODO UNCOMMENT //  << "\nTotal idle time: " << sStat->tNone << endl;

            // // TODO UNCOMMENT //  << "\nTotal passenger distance: " << sStat->dPass << endl;
            // // TODO UNCOMMENT //  << "\nTotal parcel distance: " << sStat->dParcel << endl;
            // // TODO UNCOMMENT //  << "\nTotal combined transportation distance: " << sStat->dBoth << endl;
            // // TODO UNCOMMENT //  << "\nTotal idle distance: " << sStat->dNone << endl;
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

	// TODO UNCOMMENT // << "\nArcs:" << endl;
    for (int i = 0; i < nas->arcs.size(); i++){
        if (i == 0){
            // TODO UNCOMMENT //  << setw(3) << " ";
        }
        // TODO UNCOMMENT //  << setw(3) << std::right << i << " ";
    }
    // TODO UNCOMMENT //  << endl;
    for(int i = 0; i < nas->arcs.size(); i++){
        // TODO UNCOMMENT //  << setw(3) << std::right << i;
        for(int j = 0; j <  nas->arcs[i].size(); j++){
            // TODO UNCOMMENT //  << setw(3) <<   nas->arcs[i][j] << " ";
        }
        // TODO UNCOMMENT //  << endl;
    }
    // TODO UNCOMMENT //  << endl;
    // getchar();

    // // TODO UNCOMMENT //  << "\n\nAll Arcs: " << endl;
    // for (int i = 0; i < nas->allArcs.size(); i++){
    //     // TODO UNCOMMENT //  << nas->allArcs[i].first << "-" << nas->allArcs[i].second << " ==== ";
    // }
    
    // // TODO UNCOMMENT //  << "\n\nArcs Plus: " << endl;
    // for (int i = 0; i < nas->arcPlus.size(); i++){
    //     for (int j = 0; j < nas->arcPlus[i].size(); j++){
    //         // TODO UNCOMMENT //  << nas->arcPlus[i][j].first << "-" << nas->arcPlus[i][j].second << " ==== ";
    //     }
    // }

    // // TODO UNCOMMENT //  << "\n\nArcs Minus: " << endl;
    // for (int i = 0; i < nas->arcMinus.size(); i++){
    //     for (int j = 0; j < nas->arcMinus[i].size(); j++){
    //         // TODO UNCOMMENT //  << nas->arcMinus[i][j].first << "-" << nas->arcMinus[i][j].second << " ==== ";
    //     }
    // }

    // // TODO UNCOMMENT //  << "\n\nArc NN: " << endl;
    // for (int i = 0; i < nas->arcNN.size(); i++){
    //     // TODO UNCOMMENT //  << nas->arcNN[i].first << "-" << nas->arcNN[i].second << " ==== ";
    // }

    // // TODO UNCOMMENT //  << "\n\nArc Nplus: " << endl;
    // for (int i = 0; i < nas->arcNplus.size(); i++){
    //     // TODO UNCOMMENT //  << nas->arcNplus[i].first << "-" << nas->arcNplus[i].second << " ==== ";
    // }
    
    // // TODO UNCOMMENT //  << "\n\nArc PN: " << endl;
    // for (int i = 0; i < nas->arcPN.size(); i++){
    // // TODO UNCOMMENT //  << nas->arcPN[i].first << "-" << nas->arcPN[i].second << " ==== ";
    // }

    // // TODO UNCOMMENT //  << "\n\nArc nf: " << endl;
    // for (int i = 0; i < nas->arcnf.size(); i++){
    //     // TODO UNCOMMENT //  << nas->arcnf[i].first << "-" << nas->arcnf[i].second << " ==== ";
    // }

    // // TODO UNCOMMENT //  << "\n\nArc V Plus: " << endl;
    // for (int i = 0; i < nas->vArcPlus.size(); i++){
    //     // TODO UNCOMMENT //  << "i: " << i << endl;
    //     for (int j = 0; j < nas->vArcPlus[i].size(); j++){
    //         // TODO UNCOMMENT //  << "j(k): " << j << endl;
    //         for(int k = 0; k < nas->vArcPlus[i][j].size(); k++){
    //             // TODO UNCOMMENT //  << nas->vArcPlus[i][j][k].first << "-" << nas->vArcPlus[i][j][k].second << " ==== ";
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // } 

    // // TODO UNCOMMENT //  << "\n\nArc V Minus: " << endl;
    // for (int i = 0; i < nas->vArcMinus.size(); i++){
    //     // TODO UNCOMMENT //  << "i: " << i << endl;
    //     for (int j = 0; j < nas->vArcMinus[i].size(); j++){
    //         // TODO UNCOMMENT //  << "j(k): " << j << endl;
    //         for(int k = 0; k < nas->vArcMinus[i][j].size(); k++){
    //             // TODO UNCOMMENT //  << nas->vArcMinus[i][j][k].first << "-" << nas->vArcMinus[i][j][k].second << " ==== ";
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }

    // // TODO UNCOMMENT //  << "\n\narc V:" << endl;
    // for (int i = 0; i < nas->arcV.size(); i++){
    //     // TODO UNCOMMENT //  << "i: " << i << endl;
    //     for (int j = 0; j < nas->arcV[i].size(); j++){
    //         // TODO UNCOMMENT //  << "j: " << j << endl;
    //         for(int k = 0; k < nas->arcV[i][j].size(); k++){
    //             // TODO UNCOMMENT //  << nas->arcV[i][j][k] << " ==== ";
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }

}

void fipStruct(instanceStat *inst, solStats *sStat, fipStats *fipStat){

    vector<int> pulocations;
    pair <int, int> pairpuloc;
    vector< pair<int, int> > pupairs;
    int fdepot = 2*inst->n + 2*inst->m;
    int fdummy = fdepot + inst->K;

    for (int i = 0; i < inst->n; i++){
        fipStat->vehicleVec.push_back(-1);
    }

    for (int i = 0; i < sStat->solOrder.size(); i++){
        int depot = fdepot + i;
        int dummy = fdummy + i;
        pulocations.push_back(depot);
        for (int j = 0; j < sStat->solOrder[i].size(); j++){
            if (sStat->solOrder[i][j] < 2*inst->n){
                pulocations.push_back(sStat->solOrder[i][j]);
            }
        }
        pulocations.push_back(dummy);
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
                fipStat->vehicleVec[fipStat->solPass[k][i]] = k;
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

    // // TODO UNCOMMENT //  << "Beginning of service: " << endl;

    // for (int a = 0; a < fipStat->solBegin.size(); a++){
    //     // TODO UNCOMMENT //  << "b(" << a << "): " << fipStat->solBegin[a] << endl;
    // }

    // // TODO UNCOMMENT //  << "\nSolution part II: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < fipStat->solPass[k].size(); i++){
    //         if (i < fipStat->solPass[k].size() - 1){
    //             // TODO UNCOMMENT //  << fipStat->solPass[k][i] << " - ";
    //         }
    //         else{
    //             // TODO UNCOMMENT //  << fipStat->solPass[k][i];
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }
    // // TODO UNCOMMENT //  << endl;

    // // TODO UNCOMMENT //  << "\nSolution part III - pickup locations: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < fipStat->solPassOrigins[k].size(); i++){
    //         if (i < fipStat->solPassOrigins[k].size() - 1){
    //             // TODO UNCOMMENT //  << fipStat->solPassOrigins[k][i].first << " : " << fipStat->solPassOrigins[k][i].second <<  " - ";
    //         }
    //         else{
    //             // TODO UNCOMMENT //  << fipStat->solPassOrigins[k][i].first << " : " << fipStat->solPassOrigins[k][i].second;
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }
    // // TODO UNCOMMENT //  << endl;

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



void mergeFipSol(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat, fipStats *fipStat, bool feasFlag){
    
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

    int currDepot;

    fipStat->solprofit = 0;
    for (int i = 0; i < inst->n; i++){
        fipStat->solprofit += nodeVec[i].profit;
        profCustomer += nodeVec[i].profit;
    }

    if (feasFlag){
        for (int k = 0; k < inst->K; k++){
            if (fipStat->solPass[k].size() >= 1){
                for (int i = 0; i < fipStat->solPass[k].size(); i++){
                    // fipStat->solBeginParcel.
                    auxVec.push_back(fipStat->solPass[k][i]);
                    // // TODO UNCOMMENT //  << "k, i: " << fipStat->solPass[k][i] << endl;
                    for (int j = 0; j < fipStat->solvec[k].size(); j++){
                        if (fipStat->solPass[k][i] == fipStat->solvec[k][j].first){
                            // // TODO UNCOMMENT //  << "k, j: " << fipStat->solPass[k][i] << endl;
                            auxVec.push_back(fipStat->solvec[k][j].second);
                            fipStat->solprofit += nodeVec[fipStat->solvec[k][j].second].profit;
                        }
                    }

                }
            }
            fipStat->fullSol.push_back(auxVec);
            auxVec.clear();
        }
    }
    else{
        for (int k = 0; k < inst->K; k++){
            if (fipStat->solPass[k].size() < 1){
                fipStat->fullSol.push_back(auxVec);
                continue;
            }
            for (int i = 0; i < fipStat->solPass[k].size(); i++){
                auxVec.push_back(fipStat->solPass[k][i]);
            }
            fipStat->fullSol.push_back(auxVec);
            auxVec.clear();
        }
    }

    // TODO UNCOMMENT // << "\n\nFull solution: " << endl;

    for (int k = 0; k < fipStat->fullSol.size(); k++){
        currDepot = 2*inst->n + 2*inst->m + k;
        // TODO UNCOMMENT //  << "Vehicle: " << currDepot << ": ";
        if(fipStat->fullSol[k].size() < 1){
            // TODO UNCOMMENT //  << endl;
        }
        else{
            for (int j = 0; j < fipStat->fullSol[k].size(); j++){
                if (j == fipStat->fullSol[k].size() - 1){
                    // TODO UNCOMMENT //  << fipStat->fullSol[k][j] << endl;
                }
                else{
                    // TODO UNCOMMENT //  << fipStat->fullSol[k][j] << " - ";
                }
                if (fipStat->fullSol[k][j] >= 2*inst->n && fipStat->fullSol[k][j] < 2*inst->n+inst->m){
                    parcelCount++;
                }
            }
        }        
    }
    for (int k = 0; k < fipStat->fullSol.size(); k++){
        currDepot = 2*inst->n + 2*inst->m + k;
        // TODO UNCOMMENT //  << "Vehicle: " << currDepot << ": ";
        if(fipStat->fullSol[k].size() < 1){
            // TODO UNCOMMENT //  << endl;
        }
        else{
            for (int j = 0; j < fipStat->fullSol[k].size(); j++){
                int node = fipStat->fullSol[k][j];
                char label = 'W';
                if(node < inst->n){
                    label = 'C';

                }
                else if(node < 2*inst->n){
                    label = 'c';
                }                
                
                else if(node < 2*inst->n + inst->m){
                    label = 'P';
                }
                
                else if(node < 2*inst->n + 2*inst->m){
                    label = 'D';
                }
                else if(node < 2*inst->n + 2*inst->m + inst->K){
                    label = 'S';

                }                
                else if(node < 2*inst->n + 2*inst->m + 2*inst->K){
                    label = 'f';

                }      
                if (j == fipStat->fullSol[k].size() - 1){
                    // TODO UNCOMMENT //  << label << endl;
                }
                else{
                    // TODO UNCOMMENT //  << label << " - ";
                }
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

        if(fipStat->fullSol[k].size() < 3){
            currentT = nodeVec[currentDepot].e;
            timeVec.push_back(currentT);
            fipStat->fullBegin.push_back(timeVec);
            continue;
        }
        
        if (fipStat->fullSol[k][1] < 2*inst->n){

            int u = fipStat->fullSol[k][1];

            fipStat->solprofit -= inst->costkm*mdist[currentDepot][u];
            costs += inst->costkm*mdist[currentDepot][u];
            
            currentT = nodeVec[u].e - mdist[currentDepot][u]/inst->vmed;
            timeVec.push_back(currentT);

            currentT = nodeVec[u].e;
            timeVec.push_back(currentT);
            fipStat->beginPass[u] = currentT;
        }

        else{
            currentT = fipStat->solBegin[currentDepot];

            timeVec.push_back(currentT);
        }

        int counter = 0;

        if(fipStat->fullSol[k][1] < 2*inst->n){
            counter = 1;
        }

        for (int i = counter; i < fipStat->fullSol[k].size() - 2; i++){

            int u = fipStat->fullSol[k][i];
            int v = fipStat->fullSol[k][i+1];

            if (i > 0){
                currentT += mdist[u][v]/inst->vmed + inst->service;
            }
            else{
                currentT += mdist[u][v]/inst->vmed;
            }

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
        currentT += inst->service;

        timeVec.push_back(currentT);

        fipStat->fullBegin.push_back(timeVec);
        timeVec.clear();
    }


    // TODO UNCOMMENT // << "\n\nFull solution with times: " << endl;

    for (int k = 0; k < fipStat->fullSol.size(); k++){
        currDepot = 2*inst->n + 2*inst->m + k;
        // TODO UNCOMMENT //  << "Vehicle: " << currDepot << ": ";

        if ( fipStat->fullSol[k].size() < 3){
            // TODO UNCOMMENT //  << " --> Total travel time: " << 0 << endl;
            continue;

        }
        for (int j = 0; j < fipStat->fullSol[k].size(); j++){
            if (j == fipStat->fullSol[k].size() - 1){
                // TODO UNCOMMENT //  << fipStat->fullSol[k][j] << "(" << fipStat->fullBegin[k][j] << ")"<< endl;
            }
            else{
                // TODO UNCOMMENT //  << fipStat->fullSol[k][j] << "(" << fipStat->fullBegin[k][j] << ")"<< " - ";
            }
        }
        // TODO UNCOMMENT //  << " --> Total travel time: " << fipStat->fullBegin[k][fipStat->fullSol[k].size()-1] - fipStat->fullBegin[k][0] << endl;
    }



    // TODO UNCOMMENT //  << "_______________________________________________________" << endl;
    for (int k = 0; k < fipStat->fullSol.size(); k++){
        for (int j = 0; j < fipStat->fullSol[k].size(); j++){
            int node = fipStat->fullSol[k][j];
            if(node < inst->n){
                if(fipStat->fullBegin[k][j] > nodeVec[node].l){
                    // TODO UNCOMMENT //  << "Error: Vehicle " << k << " arrives at node " << node << " after closing time" << endl;
                    
                }
            }

        }
    }
    // TODO UNCOMMENT //  << "_______________________________________________________" << endl;

    // // TODO UNCOMMENT //  << "\n\nbegin pass vector: "<< endl;
    // for(int i = 0; i < fipStat->beginPass.size(); i++){
    //     // TODO UNCOMMENT //  << i << ": " << fipStat->beginPass[i] << " - ";
    // }
    
    // TODO UNCOMMENT //  << "\n\nFull solution value: " << fipStat->solprofit << endl;

    // TODO UNCOMMENT //  << "\n\nServed parcels: " << parcelCount << endl;

    // TODO UNCOMMENT //  << "\n\nProfit of customers: " << profCustomer << endl;

    // TODO UNCOMMENT //  << "\n\nProfit of parcels: " << profParcel << endl;
    
    // TODO UNCOMMENT //  << "\n\nCosts: " << costs << endl;

    fipStatIni(fipStat);

    //calculating specific distances and times:
    int load;//load of parcel
    int load2;//load of passenger
    double distPass;
    // load = 0;
    double dij;
    int currNode;
    int nextNode;

    double stop;
    double tij;


    for (int k = 0; k < inst->K; k++){
        load = 0;
        load2 = 0;
        currDepot = 2*inst->n + 2*inst->m + k;

        if (fipStat->fullSol[k].size() < 3){
            continue;
        }

        dij = mdist[currDepot][fipStat->fullSol[k][1]];

        fipStat->tNone += dij/inst->vmed;
        fipStat->tNone += inst->service;
        fipStat->dNone += dij;
        if(fipStat->fullSol[k][1] < inst->n){
            load = 0;

            load2++;
        }
        else if(fipStat->fullSol[k][1] < 2*inst->n + inst->m){
            load++;

            load2 = 0;
        }

        for (int i = 1; i < fipStat->fullSol[k].size() - 1; i++){
            // dij = mdist[sStat->solInNode[k][i]][sStat->solInNode[k][i + 1]];
            currNode = fipStat->fullSol[k][i];
            nextNode = fipStat->fullSol[k][i + 1];
            dij = mdist[currNode][nextNode];            
            if(currNode < inst->n){ //pass PU

                if(nextNode < 2*inst->n){ //pass DL 
                    if (load > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;

                        fipStat->dBoth += dij;
                        load2--;
                    }  
                    else{
                        fipStat->tPass +=  dij/inst->vmed;
                        fipStat->tPass +=  inst->service;

                        fipStat->dPass += dij;
                        load2--;
                    }
                }

                else if (nextNode < 2*inst->n + inst->m){//parcel PU
                    if (load > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;

                        fipStat->dBoth += dij;
                        load++;
                    }  
                    else{
                        fipStat->tPass +=  dij/inst->vmed;
                        fipStat->tPass +=  inst->service;

                        fipStat->dPass += dij;
                        load++;
                    }
                }

                else if (nextNode < 2*inst->n + 2*inst->m){//parcel DL
                    fipStat->tBoth += dij/inst->vmed;
                    fipStat->tBoth += inst->service;

                    fipStat->dBoth += dij;
                    load--;
                }
            }

                

            else if (currNode < 2*inst->n){ //Passenger DL
                if(nextNode < inst->n){ //pass PU
                    if (load > 0){
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;

                        fipStat->dParcel += dij;
                        load2++;
                    }  
                    else{
                        fipStat->tNone +=  dij/inst->vmed;
                        fipStat->tNone +=  inst->service;

                        fipStat->dNone += dij;
                        load2++;
                    }
                }

                else if (nextNode < 2*inst->n + inst->m){//parcel PU
                    if (load > 0){
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;

                        fipStat->dParcel += dij;
                        load++;
                    }  
                    else{
                        fipStat->tNone +=  dij/inst->vmed;
                        fipStat->tNone +=  inst->service;

                        fipStat->dNone += dij;
                        load++;
                    }
                }

                else if (nextNode < 2*inst->n + 2*inst->m){//parcel DL
                    fipStat->tParcel += dij/inst->vmed;
                    fipStat->tParcel += inst->service;

                    fipStat->dParcel += dij;
                    load--;
                }
            }

            else if (currNode < 2*inst->n + inst->m){ //parcel PU
                if (nextNode < inst->n){//passenger PU
                    fipStat->tParcel += dij/inst->vmed;
                    fipStat->tParcel += inst->service;

                    fipStat->dParcel += dij;
                    load2++;
                }                
                else if (nextNode < 2*inst->n){//passenger DL
                    fipStat->tBoth += dij/inst->vmed;
                    fipStat->tBoth += inst->service;

                    fipStat->dBoth += dij;
                    load2--;
                }
                else if(nextNode < 2*inst->n + inst->m){//parcel PU
                    if (load2 > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;

                        fipStat->dBoth += dij;
                        load++;
                    }
                    else{
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;
                        load++;

                        fipStat->dParcel += dij;   
                    }
      
                }
                else if (nextNode < 2*inst->n + 2*inst->m){//parcel DL
                    if (load2 > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;

                        fipStat->dBoth += dij;
                        load--;                        
                    }
                    else{
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;
                        load--;
  
                        fipStat->dParcel += dij;                       
                    }

                }
            }


            else if (currNode < 2*inst->n + 2*inst->m){ //parcel DL
                if(nextNode < inst->n){ // if next is a Pass PU, there will not be a load2 > 0
                    if (load > 0){
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;

                        fipStat->dParcel += dij;
                        load2++;
                    } 
                    else{
                        fipStat->tNone += dij/inst->vmed;
                        fipStat->tNone += inst->service;

                        fipStat->dNone += dij;
                        load2++;
                    }
                }
                else if (nextNode < 2*inst->n){ //pass DL
                    if (load > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;

                        fipStat->dBoth += dij;
                        load2--;                             
                    } 
                    else{
                        fipStat->tPass += dij/inst->vmed;
                        fipStat->tPass += inst->service;

                        fipStat->dPass += dij;
                        load2--;                                                  
                    }                    
                }
                else if(nextNode < 2*inst->n + inst->m){//parcel PU
                    if (load > 0){
                        if (load2 > 0){
                            fipStat->tBoth += dij/inst->vmed;
                            fipStat->tBoth += inst->service;

                            fipStat->dBoth += dij;
                            load++;
                        }
                        else{
                            fipStat->tParcel += dij/inst->vmed;
                            fipStat->tParcel += inst->service;
                            load++;

                            fipStat->dParcel += dij;
                        }
                    } 
                    else{
                        if (load2 > 0){
                            fipStat->tPass += dij/inst->vmed;
                            fipStat->tPass += inst->service;

                            fipStat->dPass += dij;
                            load++;  
                        }
                        else{
                            fipStat->tNone += dij/inst->vmed;
                            fipStat->tNone += inst->service;
                            load++;

                            fipStat->dNone += dij;
                        }

                    } 
           
                }
                else if (nextNode < 2*inst->n + 2*inst->m){//parcel DL                
                    if (load2 > 0){
                        fipStat->tBoth += dij/inst->vmed;
                        fipStat->tBoth += inst->service;
                        load--;

                        fipStat->dBoth += dij;
                    }
                    else{
                        fipStat->tParcel += dij/inst->vmed;
                        fipStat->tParcel += inst->service;
                        load--;

                        fipStat->dParcel += dij;
                    }

                }
            }
        }
    }

    // TODO UNCOMMENT //  << "\n*************" << endl;

    // TODO UNCOMMENT //  << "\nTotal time: " << fipStat->tPass + fipStat->tParcel + fipStat->tBoth + fipStat->tNone << endl;
    // TODO UNCOMMENT //  << "\nTotal passenger time: " << fipStat->tPass << endl;
    // TODO UNCOMMENT //  << "\nTotal parcel time: " << fipStat->tParcel << endl;
    // TODO UNCOMMENT //  << "\nTotal combined transportation time: " << fipStat->tBoth << endl;
    // TODO UNCOMMENT //  << "\nTotal idle time: " << fipStat->tNone << endl;

    // TODO UNCOMMENT //  << "\n*************" << endl;

    // TODO UNCOMMENT //  << "\nTotal distance: " << fipStat->dPass + fipStat->dParcel + fipStat->dBoth + fipStat->dNone << endl;
    // TODO UNCOMMENT //  << "\nTotal passenger distance: " << fipStat->dPass << endl;
    // TODO UNCOMMENT //  << "\nTotal parcel distance: " << fipStat->dParcel << endl;
    // TODO UNCOMMENT //  << "\nTotal combined transportation distance: " << fipStat->dBoth << endl;
    // TODO UNCOMMENT //  << "\nTotal idle distance: " << fipStat->dNone << endl;


}

void calcPassDetour(instanceStat *inst, vector<nodeStat> &nodeVec, fipStats *fipStat){

    double ntrip, dtrip;
    double detour;

    for (int i = inst->n; i < 2*inst->n; i++){
        ntrip = nodeVec[i].e - nodeVec[i - inst->n].e;
        dtrip = fipStat->beginPass[i] - fipStat->beginPass[i - inst->n];
        // // TODO UNCOMMENT //  << "dtrip: " << dtrip << " - ntrip" << ntrip << endl;
        // getchar();
        detour = (double)((dtrip - ntrip)/(ntrip))*(double)(100);
        fipStat->passDetour.push_back(detour);
    }

    // TODO UNCOMMENT //  << "\n\nPassenger detour (%): ";
    
    for (int i = 0; i < inst->n; i++){
        // TODO UNCOMMENT //  << i << ": " << fipStat->passDetour[i] << " ";
    }
    // TODO UNCOMMENT //  << endl << endl;


}

void clearStats(solStats *sStat, fipStats *fipStat){

    sStat->solOrder.clear(); 
	sStat->solInNode.clear();
	sStat->solvec.clear();

	sStat->solBegin.clear();
	sStat->solLoad.clear();

    fipStat->solPass.clear(); 
	fipStat->solPassOrigins.clear(); 

	fipStat->solvec.clear();
	fipStat->solBegin.clear();
	fipStat->solBeginParcel.clear();

	fipStat->beginPass.clear();

	fipStat->fullSol.clear();
	fipStat->fullBegin.clear();

	fipStat->passDetour.clear();

}

void clearBndStats(solStats *sStat){

    sStat->solOrder.clear(); 
	sStat->solInNode.clear();
	sStat->solvec.clear();

	sStat->solBegin.clear();
	sStat->solLoad.clear();
}