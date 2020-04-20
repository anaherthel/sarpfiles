//Old code pieces for GOMDVSP

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<int> &clusters, vector< vector<int> > &clusterVec, vector< vector<int> > &clsParcel, probStat* problem){

//For 1A:
    // int counter = 0;
    // for (int i = 0; i < inst->n; i++){
    //     bStat->bundle.push_back(i);
    //     bStat->bundleVec.push_back(bStat->bundle);
    //     // clusters.push_back(bStat->bundle);
    //     clusters.push_back(bStat->bundleVec.size()-1);
    //     bStat->bundle.clear();
    //     for (int j = inst->n; j < inst->m + inst->n; j++){
    //         bStat->bundle.push_back(j);
    //         bStat->bundle.push_back(i);
    //         bStat->bundle.push_back(j+inst->m);
    //         bStat->bundleTimes.push_back(nodeVec[j].delta + nodeVec[i].delta + nodeVec[j+inst->m].delta);
    //         bStat->bundleVec.push_back(bStat->bundle);
    //         // clusters.push_back(bStat->bundle);
    //         clusters.push_back(bStat->bundleVec.size()-1);
    //         bStat->bundle.clear();
    //     }
    //     clusterVec.push_back(clusters);
    //     clusters.clear();
    // }

    // for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
    //     bStat->bundle.push_back(i);
    //     bStat->bundleVec.push_back(bStat->bundle);
    //     // clusters.push_back(bStat->bundle);
    //     clusters.push_back(bStat->bundleVec.size()-1);
    //     bStat->bundle.clear();
        
    //     clusterVec.push_back(clusters);
    //     clusters.clear();
    // }

 }

//For simple nodes (before bundles)
 void feasibleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, pair<int, int> &fArc, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus, probStat* problem, vector< pair<int,int> > &arcNN){

    if (problem->scen == "1A" || problem->scen == "2A"){
           for (int i = 0; i < inst->V; i++){
            if (i < inst->n){
                for(int j = 0; j < inst->n + 2*inst->m; j++){
                    if(i != j){
                        arcs[i][j] = true;
                        fArc.first = i;
                        fArc.second = j;
                        arcMinus[j].push_back(fArc);
                        arcPlus[i].push_back(fArc);
                        if (j < inst->n){
                            arcNN.push_back(fArc);
                        }
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){
                    arcs[i][j] = true;
                    fArc.first = i;
                    fArc.second = j;
                    arcMinus[j].push_back(fArc);
                    arcPlus[i].push_back(fArc);
                }
            }

            else if (i > inst->n - 1){
                if (i < inst->n + inst->m){
                    for (int j = 0; j < inst->n + 2*inst->m; j++){
                        if (i != j){
                            if (j != i + inst->m){
                                arcs[i][j] = true;
                                fArc.first = i;
                                fArc.second = j;
                                arcMinus[j].push_back(fArc);
                                arcPlus[i].push_back(fArc);
                            }
                        }
                    }
                }
                else if (i > inst->n + inst->m - 1){
                    if (i < inst->n + 2*inst->m){
                        for (int j = 0; j < inst->n + 2*inst->m; j++){
                            if (i != j){
                                if (j + inst->m != i){
                                    arcs[i][j] = true;
                                    fArc.first = i;
                                    fArc.second = j;
                                    arcMinus[j].push_back(fArc);
                                    arcPlus[i].push_back(fArc);
                                }
                            }
                        }
                        for (int j = inst->V; j < inst->V + inst->dummy; j++){
                            arcs[i][j] = true;
                            fArc.first = i;
                            fArc.second = j;
                            arcMinus[j].push_back(fArc);
                            arcPlus[i].push_back(fArc);
                        }
                    }

                    else if (i > inst->n + 2* inst->m - 1){
                        for (int j = 0; j < inst->n + inst->m; j++){
                            arcs[i][j] = true;
                            fArc.first = i;
                            fArc.second = j;
                            arcMinus[j].push_back(fArc);
                            arcPlus[i].push_back(fArc);
                        }
                        // for (int j = inst->V; j < inst->V + inst->dummy; j++){
                        //     arcs[i][j] = true;
                        //     fArc.first = i;
                        //     fArc.second = j;
                        //     arcMinus[j].push_back(fArc);
                        //     arcPlus[i].push_back(fArc);
                        // }
                    }
                }
            }
        }
    }
    if (problem->scen == "1B" || problem->scen == "2B"){
        for (int i = 0; i < inst->V; i++){
            if (i < inst->n){
                for(int j = 0; j < inst->n + 2*inst->m; j++){
                    if(i != j){
                        arcs[i][j] = true;
                        fArc.first = i;
                        fArc.second = j;
                        arcMinus[j].push_back(fArc);
                        arcPlus[i].push_back(fArc);
                    }
                }
                for (int j = inst->V; j < inst->V + inst->dummy; j++){
                    arcs[i][j] = true;
                    fArc.first = i;
                    fArc.second = j;
                    arcMinus[j].push_back(fArc);
                    arcPlus[i].push_back(fArc);
                }
            }

            else if (i > inst->n - 1){
                if (i < inst->n + inst->m){
                    for (int j = 0; j < inst->n + inst->m; j++){
                        if (i != j){
                            if (j != i + inst->m){
                                arcs[i][j] = true;
                                fArc.first = i;
                                fArc.second = j;
                                arcMinus[j].push_back(fArc);
                                arcPlus[i].push_back(fArc);
                            }
                        }
                    }
                }
                else if (i > inst->n + inst->m - 1){
                    if (i < inst->n + 2*inst->m){
                        for (int j = 0; j < inst->n + 2*inst->m; j++){
                            if (i != j){
                                if (j + inst->m != i){
                                    arcs[i][j] = true;
                                    fArc.first = i;
                                    fArc.second = j;
                                    arcMinus[j].push_back(fArc);
                                    arcPlus[i].push_back(fArc);
                                }
                            }
                        }
                        for (int j = inst->V; j < inst->V + inst->dummy; j++){
                            arcs[i][j] = true;
                            fArc.first = i;
                            fArc.second = j;
                            arcMinus[j].push_back(fArc);
                            arcPlus[i].push_back(fArc);
                        }
                    }

                    else if (i > inst->n + 2* inst->m - 1){
                        for (int j = 0; j < inst->n + inst->m; j++){
                            arcs[i][j] = true;
                            fArc.first = i;
                            fArc.second = j;
                            arcMinus[j].push_back(fArc);
                            arcPlus[i].push_back(fArc);
                        }
                        // for (int j = inst->V; j < inst->V + inst->dummy; j++){
                        //     arcs[i][j] = true;
                        //     fArc.first = i;
                        //     fArc.second = j;
                        //     arcMinus[j].push_back(fArc);
                        //     arcPlus[i].push_back(fArc);
                        // }
                    }
                }
            }
        }
    }
}

void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, vector<double> &passProfit){
    double cost;
    double service;

    // for (int i = 0; i < bStat->bundleVec.size(); i++){
    //     if (bStat->bundleVec[i].size() <= 1){
    //         cost = 0;
    //         bStat->bundleProfVec.push_back(cost);
    //     }
    //     else{
    //         for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
    //             cost = 0;
    //             if (bStat->bundleVec[i][j] > inst->n - 1 && bStat->bundleVec[i][j] < inst->n + inst->m){
    //                 cost += inst->gamma + inst->mu*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j] + inst->m];
    //                 for (int k = j + 1; k < bStat->bundleVec[i].size(); k++){
    //                     if (bStat->bundleVec[i][k] == bStat->bundleVec[i][j] + inst->m){
    //                         break;
    //                     }
    //                     else if (bStat->bundleVec[i][k] < inst->n + inst->m){
    //                         cost += - mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][k]] - mdist[bStat->bundleVec[i][k]][bStat->bundleVec[i][j] + inst->m];
                            
    //                     }
    //                 }
    //                 bStat->bundleProfVec.push_back(cost);
    //             }
    //         }            
    //     }
    // }
   }

   // void feasibleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, pair<int, int> &fArc, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus){

//     for (int i = 0; i < inst->V; i++){
//         if (i < inst->n){
//             for(int j = 0; j < inst->n + 2*inst->m; j++){
//                 if(i != j){
//                     arcs[i][j] = true;
//                     fArc.first = i;
//                     fArc.second = j;
//                     arcMinus[j].push_back(fArc);
//                     arcPlus[i].push_back(fArc);
//                 }
//             }
//             arcs[i][inst->V] = true;
//             fArc.first = i;
//             fArc.second = inst->V;
//             arcMinus[inst->V].push_back(fArc);
//             arcPlus[i].push_back(fArc);
//         }

//         else if (i > inst->n - 1){
//             if (i < inst->n + inst->m){
//                 for (int j = 0; j < inst->n + 2*inst->m; j++){
//                     if (i != j){
//                         if (j != i + inst->m){
//                             arcs[i][j] = true;
//                             fArc.first = i;
//                             fArc.second = j;
//                             arcMinus[j].push_back(fArc);
//                             arcPlus[i].push_back(fArc);
//                         }
//                     }
//                 }
//             }
//             else if (i > inst->n + inst->m - 1){
//                 if (i < inst->n + 2*inst->m){
//                     for (int j = 0; j < inst->n + 2*inst->m; j++){
//                         if (i != j){
//                             if (j + inst->m != i){
//                                 arcs[i][j] = true;
//                                 fArc.first = i;
//                                 fArc.second = j;
//                                 arcMinus[j].push_back(fArc);
//                                 arcPlus[i].push_back(fArc);
//                             }
//                         }
//                     }
//                     arcs[i][inst->V] = true;
//                     fArc.first = i;
//                     fArc.second = inst->V;
//                     arcMinus[inst->V].push_back(fArc);
//                     arcPlus[i].push_back(fArc);                   
//                 }

//                 else if (i > inst->n + 2* inst->m - 1){
//                     for (int j = 0; j < inst->n + inst->m; j++){
//                         arcs[i][j] = true;
//                         fArc.first = i;
//                         fArc.second = j;
//                         arcMinus[j].push_back(fArc);
//                         arcPlus[i].push_back(fArc);
//                     } 
//                     arcs[i][inst->V] = true;
//                     fArc.first = i;
//                     fArc.second = inst->V;
//                     arcMinus[inst->V].push_back(fArc);
//                     arcPlus[i].push_back(fArc);             
//                 }
//             }
//         }
//     }
// }

void feasibleBundleArcs (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, int p, probStat* problem){
    //*******
    //For 1A:
    //*******
    if (problem->scen == "1A"){
        int setN = bStat->bundleVec.size() - inst->K - 1;
        int currentCluster = 0;
        int ref;
        if (p < 0){
            ref = inst->m;
        }

        else{
            ref = p;
        }

        for(int i = 0; i < bStat->bundleVec.size() - 1; i++){
            if (i > currentCluster*(ref + 1) + ref){
                currentCluster++;
            }
            if(i < setN){
                for (int j = 0; j < setN; j++){
                    if (i != j){
                        if (j > currentCluster*(ref + 1) + ref || j < currentCluster*(ref + 1)){
                            if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                                bStat->bArcs[i][j] = true;
                                // bStat->bFArc.first = i;
                                // bStat->bFArc.second = j;
                                // bStat->bArcMinus[j].push_back(bStat->bFArc);
                                // bStat->bArcPlus[i].push_back(bStat->bFArc);                            
                            }
                        }
                    } 
                }
                bStat->bArcs[i][bStat->bundleVec.size()-1] = true;
                // bStat->bFArc.first = i;
                // bStat->bFArc.second = bStat->bundleVec.size()-1;
                // bStat->bArcMinus[bStat->bundleVec.size()-1].push_back(bStat->bFArc);
                // bStat->bArcPlus[i].push_back(bStat->bFArc);
            }
            else if (i >= setN){
                for (int j = 0; j < setN; j++){
                    if (bStat->bundleStart[i] + bStat->bundleServVec[i] + (mdist[bStat->lastElement[i]][bStat->firstElement[j]]/inst->vmed) < bStat->bundleStart[j]){
                        bStat->bArcs[i][j] = true;
                        // bStat->bFArc.first = i;
                        // bStat->bFArc.second = j;
                        // bStat->bArcMinus[j].push_back(bStat->bFArc);
                        // bStat->bArcPlus[i].push_back(bStat->bFArc);
                    }
                }
            }
        }

        //remove arcs to bundles with same parcel requests
        for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
            for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
                for (int k = 0; k < bStat->parcelBundleVec[i].size(); k++){
                    if (bStat->parcelBundleVec[i][j] != bStat->parcelBundleVec[i][k]){
                        bStat->bArcs[bStat->parcelBundleVec[i][j]][bStat->parcelBundleVec[i][k]] = false;
                    }
                }
            }
        }

        // for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
        //     for (int j = 0; j < bStat->parcelBundleVec[i].size() - 1; j++){
        //         if (bStat->parcelBundleVec[i][j] != bStat->parcelBundleVec[i][j+1]){
        //             bStat->bArcs[bStat->parcelBundleVec[i][j]][bStat->parcelBundleVec[i][j+1]] = false;
        //             bStat->bArcs[bStat->parcelBundleVec[i][j+1]][bStat->parcelBundleVec[i][j]] = false;
        //         }
        //     }
        // }

        //remove arcs from/to bundles with negative start times
        for (int i = 0; i < bStat->bundleStart.size() - 1; i++){
            if (bStat->bundleStart[i] < 0){
                for (int j = 0; j < bStat->bundleVec.size() - 1; j++){
                    bStat->bArcs[j][i] = false;
                    bStat->bArcs[i][j] = false;
                }
            }
        }



        for (int i = 0; i < bStat->bundleVec.size(); i++){
            for (int j = 0; j < bStat->bundleVec.size(); j++){
                if (bStat->bArcs[i][j]){
                    bStat->bFArc.first = i;
                    bStat->bFArc.second = j;
                    bStat->bArcMinus[j].push_back(bStat->bFArc);
                    bStat->bArcPlus[i].push_back(bStat->bFArc); 
                }
            }
        }        
    }
 }

 // void increaseK (instanceStat *inst, vector<nodeStat> &nodeVec, double ***mdist, bundleStat *bStat, vector< vector<int> > &clusterVec, vector< pair<int,int> > &cArcVec, vector< vector< pair<int,int> > > &cArcPlus, vector< vector< pair<int,int> > > &cArcMinus, probStat* problem, solStats *sStat, double **auxdist){

//     inst->K++;
//     vector< vector<double> > distVec;
//     vector<double> distRow;
//     vector<double> dummyRow;

//     for (int i = 0; i < inst->V; i++){
//         for (int j = 0; j < inst->V; j++){

//             distRow.push_back(auxdist[i][j]);
//             if (j == inst->V - 1){
//                 distRow.push_back(auxdist[i][j]);
//                 distRow.push_back(0);
//             }
//         }
//         distVec.push_back(distRow);
//         if (i == inst->V - 1){
//             distVec.push_back(distRow);
//         }
//         distRow.clear();
//     }

//     for (int i = 0; i < distVec[0].size(); i++){
//         distRow.push_back(0);
//     }
//     distVec.push_back(distRow);

//     inst->V = inst->n + 2*inst->m + inst->K;

//     double **dist = new double*[inst->V + inst->dummy];
//     for (int i = 0; i < inst->V + inst->dummy; i++){
//         dist[i] = new double [inst->V + inst->dummy];
//     }

//     for (int i = 0; i < distVec.size(); i++){
//         for (int j = 0; j < distVec[i].size(); j++){
//             dist[i][j] = distVec[i][j];
//         }
//     }

//     *mdist = dist;
//     //update Node Vector
//     nodeStat newNode = nodeVec[nodeVec.size() - 2];
//     nodeVec.insert(nodeVec.begin() + nodeVec.size() - 1, newNode);
    
//     //Update Bundle Vector
//     vector<int> auxVec;
//     auxVec.push_back(nodeVec.size() - 2);
//     bStat->bundleVec.insert(bStat->bundleVec.begin() + bStat->bundleVec.size() - 1, auxVec);
//     bStat->bundleVec.back()[0]++;
//     auxVec.clear();

//     //Update Bundle Profit Vector
//     double newProfit = bStat->bundleProfVec[bStat->bundleProfVec.size() - 2];
//     bStat->bundleProfVec.insert(bStat->bundleProfVec.begin() + bStat->bundleProfVec.size() - 1, newProfit);

//     //Update Cluster Vector

//     auxVec.push_back(bStat->bundleVec.size() - 2);
    
//     clusterVec.pop_back();

//     clusterVec.push_back(auxVec);
//     auxVec.clear();
    
//     auxVec.push_back(bStat->bundleVec.size() - 1);

//     clusterVec.push_back(auxVec);
//     auxVec.clear();

//     //Update Bundle Arc Vector
//     for (int i = 0; i < bStat->bundleVec.size() - 1; i++){
//         cout << setw(3) << std::right << i;
//         for (int j = 0; j < bStat->bundleVec.size() - 1; j++){
//             cout << setw(3) << std:: right << bStat->bArcs[i][j];
//         }
//         cout << endl;
//     }

//     for (int i = 0; i < bStat->bundleVec.size() - 1; i++){
//         bStat->bArcs[i].insert(bStat->bArcs[i].begin() + bStat->bArcs[i].size() - 1, false);
//     }

//     for (int j = 0; j < bStat->bundleVec.size(); j++){
//         bStat->bArcRow.push_back(bStat->bArcs[bStat->bundleVec.size() - 3][j]);
//     }

//     bStat->bArcRow.clear();


//     //Update Bundle Plus and Minus Vector

//     // pair<int,int> newArc;
//     // vector< pair<int, int> > vecNewArcPlus;
//     // vector< pair<int, int> > vecNewArcMinus;

//     // cout << "Bundle Arcs Plus (before): " << endl;

//     // for (int i = 0; i < bStat->bArcPlus.size(); i++){
//     //     cout << i << ": ";
//     //     for (int j = 0; j < bStat->bArcPlus[i].size(); j++){
//     //         cout << "(" << bStat->bArcPlus[i][j].first << ", " << bStat->bArcPlus[i][j].second << "), ";
//     //     }
//     //     cout << endl;
//     // }
//     // getchar();

//     // for (int i = 0; i < bStat->bArcs[bStat->bundleVec.size() - 2].size(); i++){
//     //     if(bStat->bArcs[bStat->bundleVec.size() - 2][i] == true){
//     //         newArc.first = bStat->bArcPlus[bStat->bundleVec.size() - 2][i].first;
//     //         newArc.second = bStat->bArcPlus[bStat->bundleVec.size() - 2][i].second;
//     //         vecNewArcPlus.push_back(newArc);
//     //     }
//     //     // newArc.first = bArcMinus[bStat->bundleVec.size() - 3][i].first;
//     //     // newArc.second = bArcMinus[bStat->bundleVec.size() - 3][i].second;
//     //     // vecNewArcMinus.push_back(newArc);
//     // }

//     // bStat->bArcPlus.insert(bStat->bArcPlus.begin() + bStat->bArcPlus.size() - 1, vecNewArcPlus);

//     // // bStat->bArcMinus.insert(bStat->bArcMinus.begin() + bStat->bArcMinus.size() - 1, vecNewArcMinus);
    
//     // cout << "Bundle Arcs Plus (after): " << endl;

//     // for (int i = 0; i < bStat->bArcPlus.size(); i++){
//     //     cout << i << ": ";
//     //     for (int j = 0; j < bStat->bArcPlus[i].size(); j++){
//     //         cout << "(" << bStat->bArcPlus[i][j].first << ", " << bStat->bArcPlus[i][j].second << "), ";
//     //     }
//     //     cout << endl;
//     // }
//     // getchar();

//     bStat->bArcPlus.clear();
//     bStat->bArcMinus.clear();



//     for (int i = 0; i < bStat->bArcs.size(); i++){
//         for (int j = 0; j < bStat->bArcs[i].size(); j++){
//             if (bStat->bArcs[i][j] == true);

//         }
//     }
// }