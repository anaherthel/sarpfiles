#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem){
    
    if (argc < 4) {
        cout << "\nMissing parameters\n";
        cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]"<< endl;
        exit(1);
    }
    
    if (argc > 4) {
        cout << "\nToo many parameters\n";
        cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]" << endl;
        exit(1);
    }  

    if (argv[2] == "sim"){
        problem->sim = true;
    }
    else if (argv[2] == "seq"){
        problem->seq = true;
    }

    problem->scen = argv[3];

    string file;
    int n;
    int m;
    int K;
    double service;
    double T;
    int V;
    int dummy;

    string instType;

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    if( !in ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    instType = getInstanceType(argv);

    if (instType == "myinstances"){

        in >> K;
        in >> service;
        in >> n;
        in >> m;
        in >> T;

        V = n + 2*m + K;
        // inst->dummy = K;
        inst->dummy = 1;

        double *xs = new double[V];
        double *ys = new double[V];
        char *label = new char[V];
        int *load = new int[V];
        double *e = new double[V];
        double *l = new double[V];
        double *xf = new double[V];
        double *yf = new double[V];
        double *delta = new double[V];

        double **dist = new double*[V + K];
        for (int i= 0; i < V + K; i++){
            dist[i] = new double [V + K];
        }

        int tempNode;
        // for (int i = 0; i < V; i++){
        //     nodeVec.push_back(*node);
        // }

        for (int i = 0; i < V; i++){
            in >> tempNode >> xs[i] >> ys[i] >> label[i] >> load[i] >> e[i] >> l[i] >> xf[i] >> yf[i];
        }

        // Calculate distance matrix (Euclidian)

        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
               delta[i] = (2 * (service/60)) + (floor(calcEucDist(xs, ys, xf, yf, i, i) + 0.5))/inst->vmed;
               cout << "delta " << i << ": " << delta[i] << endl;
            }
            else if (i < V){ 
               delta[i] = service/60;
            }
            for (int j = 0; j < V + inst->dummy; j++){
                if(i == j){
                   dist[i][j] = 0;
                }
                else{
                    if (i < V){
                        if (j < V){
                            dist[i][j] = floor(calcEucDist(xs, ys, xf, yf, i, j) + 0.5);
                        }
                        else if (j >= V){
                            dist[i][j] = 0;
                        }
                    }
                    else{
                        dist[i][j] = 0;
                    }
                }
            }
        }

        for (int i = 0; i < V; i++){
            node->xs = xs[i];
            node->ys = ys[i];
            node->label = label[i];
            node->load = load[i];
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->xf = xf[i];
            node->yf = yf[i];
            node->delta = delta[i];
            nodeVec.push_back(*node);
        }

        //Adding dummy nodes
        for (int i = 0; i < inst->dummy; i++){
            node->xs = 0;
            node->ys = 0;
            node->label = 'f';
            node->load = 0;
            node->e = 0;
            node->l = 240/60;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            nodeVec.push_back(*node);
        }

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->T = T/60;
        inst->V = V;
        // cout << "\nNode vec: ";
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << nodeVec[i].label << " ";
        // }
        // cout << endl;

        delete[] xs;
        delete[] ys;
        delete[] label;
        delete[] load;
        delete[] e;
        delete[] l;
        delete[] xf;
        delete[] yf;
    }
    else if (instType == "sarpdata"){
        
    }
 
}

double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J){
    return sqrt(pow(Xf[I] - Xs[J], 2) + pow(Yf[I] - Ys[J], 2));
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

string getInstanceType (char **argv){
    // //sarp_car1_4_4_1.txt
    // string filename(argv[1]);
    // string::size_type loc = filename.find_last_of(".", filename.size() );
    // string::size_type loc2 = filename.find_last_of("/", filename.size() );
    // string InstanceName;
    // if (loc != string::npos) {
    // InstanceName.append(filename, loc2+1, loc-loc2-1 );
    // //cout << "\n1-" << nomeDaInstancia << endl;
    // }
    // else {
    // InstanceName.append(filename, loc2+1, filename.size() );
    // //cout << "\n2-" << nomeDaInstancia << endl;
    // }

    // return InstanceName;

    //sarp_car1_4_4_1.txt
    //sarpc_1.txt
    string filename(argv[1]);

    string::size_type loc = filename.find_first_of("/");
    string::size_type loc2 = filename.find_last_of("/", filename.size());
    // cout << loc << " " << loc2 << endl;
    // getchar();
    string InstanceType;


    InstanceType.append(filename, loc+1, loc2-loc-1 );
    // cout << "\nInstance type: " << InstanceType << endl;
    // getchar();

    return InstanceType;

}

void makeBundles (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, vector<int> &bundle, vector<double> &bundleTimes, vector< vector<int> > &clusters, vector< vector< vector<int> > > &clusterVec){

//For 1A:   
    for (int i = 0; i < inst->n; i++){
        bundle.push_back(i);
        bundleVec.push_back(bundle);
        clusters.push_back(bundle);
        bundle.clear();
        for (int j = inst->n; j < inst->m + inst->n; j++){
            bundle.push_back(j);
            bundle.push_back(i);
            bundle.push_back(j+inst->m);
            bundleTimes.push_back(nodeVec[j].delta + nodeVec[i].delta + nodeVec[j+inst->m].delta);

            // cout << "\nBundle: [";
            // for (int k = 0; k < bundle.size(); k++){
            //     cout << bundle[k];
            //     if (k < bundle.size() - 1){
            //         cout << ", ";
            //     }
            // }
            // cout << "]" << endl;

            bundleVec.push_back(bundle);
            clusters.push_back(bundle);
            bundle.clear();
        }
        clusterVec.push_back(clusters);
        clusters.clear();
    }

    for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){
        bundle.push_back(i);
        bundleVec.push_back(bundle);
        clusters.push_back(bundle);
        bundle.clear();
        
        clusterVec.push_back(clusters);
        clusters.clear();
    }


}

void bundleProfit(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, vector<double> &bundleProfVec){
    double cost;

    for (int i = 0; i < bundleVec.size(); i++){
        if (bundleVec[i].size() <= 1){
            cost = 0;
            bundleProfVec.push_back(cost);
        }
        else{
            for (int j = 0; j < bundleVec[i].size() - 1; j++){
                cost = 0;
                if (bundleVec[i][j] > inst->n - 1 && bundleVec[i][j] < inst->n + inst->m){
                    cost += inst->gamma + inst->mu*mdist[bundleVec[i][j]][bundleVec[i][j] + inst->m];
                    for (int k = j + 1; k < bundleVec[i].size(); k++){
                        if (bundleVec[i][k] == bundleVec[i][j] + inst->m){
                            break;
                        }
                        else if (bundleVec[i][k] < inst->n + inst->m){
                            cost += - mdist[bundleVec[i][j]][bundleVec[i][k]] - mdist[bundleVec[i][k]][bundleVec[i][j] + inst->m];
                            
                        }
                    }
                    bundleProfVec.push_back(cost);
                }
            }            
        }
    }   
}

void feasibleBundleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<int> > &bundleVec, pair<int, int> &bFArc, vector< vector<bool> > &bArcs, vector< vector< pair<int,int> > > &bArcPlus, vector< vector< pair<int,int> > > &bArcMinus){
    //1-A
    int setN = bundleVec.size() - inst->K - 1;
    int currentCluster = 0;

    for(int i = 0; i < bundleVec.size(); i++){
        if (i > currentCluster*(inst->m + 1) + inst->m){
            currentCluster++;
        }
        if(i < setN){
            for (int j = 0; j < setN; j++){
                if (i != j){
                    if (j > currentCluster*(inst->m + 1) + inst->m || j < currentCluster*(inst->m + 1)){
                        bArcs[i][j] = true;
                        bFArc.first = i;
                        bFArc.second = j;
                        bArcMinus[j].push_back(bFArc);
                        bArcPlus[i].push_back(bFArc);
                    }
                } 
            }
            bArcs[i][bundleVec.size()-1] = true;
            bArcMinus[bundleVec.size()-1].push_back(bFArc);
            bArcPlus[i].push_back(bFArc);
        }
        else if (i >= setN){
            if (i < bundleVec.size()-1){
                for (int j = 0; j < setN; j++){
                    bArcs[i][j] = true;
                    bArcMinus[j].push_back(bFArc);
                    bArcPlus[i].push_back(bFArc);
                }
            }
        }
    }
}

