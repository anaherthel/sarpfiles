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