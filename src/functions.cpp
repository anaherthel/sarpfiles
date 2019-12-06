#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist){
    
    if (argc < 2) {
        cout << "\nMissing parameters\n";
        cout << " ./exeSARP [Instance] "<< endl;
        exit(1);
    }
    
    if (argc > 2) {
        cout << "\nToo many parameters\n";
        cout << " ./exeSARP [Instance] " << endl;
        exit(1);
    }  

    string file;
    int n;
    int m;
    int K;
    int service;
    int T;
    int V;

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    if( !in ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    in >> K;
    in >> service;
    in >> n;
    in >> m;
    in >> T;

    V = n + 2*m + K;

    double *xs = new double[V];
    double *ys = new double[V];
    char *label = new char[V];
    int *load = new int[V];
    int *e = new int[V];
    int *l = new int[V];
    double *xf = new double[V];
    double *yf = new double[V];
    double *delta = new double[V];

    double **dist = new double*[V + 1];
    for (int i= 0; i < V + 1; i++){
        dist[i] = new double [V + 1];
    }

    int tempNode;
    // for (int i = 0; i < V; i++){
    //     nodeVec.push_back(*node);
    // }

    for (int i = 0; i < V; i++){
        in >> tempNode >> xs[i] >> ys[i] >> label[i] >> load[i] >> e[i] >> l[i] >> xf[i] >> yf[i];
    }

    // Calculate distance matrix (Euclidian)

    for (int i = 0; i < V + 1; i++){
        if (i < n){ 
           delta[i] = 2 * service + sqrt(pow(xs[i] - xf[i], 2) + pow(ys[i] - yf[i], 2))/inst->vmed;
        }
        else if (i < V){ 
           delta[i] = service;
        }
        for (int j = 0; j < V + 1; j++){
            if (i < V){
                if (j < V){
                    dist[i][j] = floor(calcEucDist(xs, ys, i, j) + 0.5) * 5;
                }
                else if (j == V){
                    dist[i][j] = 0;
                }
            }
            else{
                dist[i][j] = 0;
            }
        }
    }

    for (int i = 0; i < V; i++){
        node->xs = xs[i];
        node->ys = ys[i];
        node->label = label[i];
        node->load = load[i];
        node->e = e[i];
        node->l = l[i];
        node->xf = xf[i];
        node->yf = yf[i];
        node->delta = delta[i];
        nodeVec.push_back(*node);
    }

    //Adding dummy node
    
    node->xs = 0;
    node->ys = 0;
    node->label = 'f';
    node->load = 0;
    node->e = 0;
    node->l = 240;
    node->xf = 0;
    node->yf = 0;
    node->delta = 0;
    nodeVec.push_back(*node);

    *Mdist = dist;
    inst->K = K;
    inst->n = n;
    inst->m = m;
    inst->T = T;
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

double calcEucDist (double *X, double *Y, int I, int J){
    return sqrt(pow(X[I] - X[J], 2) + pow(Y[I] - Y[J], 2));
}

void feasibleArcs (instanceStat *inst, vector<nodeStat> &nodeVec, vector< vector<bool> > &arcs, pair<int, int> &fArc, vector< vector< pair<int,int> > > &arcPlus, vector< vector< pair<int,int> > > &arcMinus){

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
            arcs[i][inst->V] = true;
            fArc.first = i;
            fArc.second = inst->V;
            arcMinus[inst->V].push_back(fArc);
            arcPlus[i].push_back(fArc);
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
                    arcs[i][inst->V] = true;
                    fArc.first = i;
                    fArc.second = inst->V;
                    arcMinus[inst->V].push_back(fArc);
                    arcPlus[i].push_back(fArc);                   
                }

                else if (i > inst->n + 2* inst->m - 1){
                    for (int j = 0; j < inst->n + inst->m; j++){
                        arcs[i][j] = true;
                        fArc.first = i;
                        fArc.second = j;
                        arcMinus[j].push_back(fArc);
                        arcPlus[i].push_back(fArc);
                    } 
                    arcs[i][inst->V] = true;
                    fArc.first = i;
                    fArc.second = inst->V;
                    arcMinus[inst->V].push_back(fArc);
                    arcPlus[i].push_back(fArc);             
                }
            }
        }
    }
}