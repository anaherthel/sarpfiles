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
    int delta;
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
    in >> delta;
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

    double **dist = new double*[V];
    for (int i= 0; i < V; i++){
        dist[i] = new double [V];
    }

    int tempNode;
    // for (int i = 0; i < V; i++){
    //     nodeVec.push_back(*node);
    // }

    for (int i = 0; i < V; i++){
        in >> tempNode >> xs[i] >> ys[i] >> label[i] >> load[i] >> e[i] >> l[i] >> xf[i] >> yf[i];
    }

    // Calculate distance matrix (Euclidian)

    for (int i = 0; i < V; i++){
        for (int j = 0; j < V; j++){
            dist[i][j] = floor(calcEucDist(xs, ys, i, j) + 0.5);
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
        nodeVec.push_back(*node);
    }


    *Mdist = dist;
    inst->K = K;
    inst->delta = delta;
    inst->n = n;
    inst->m = m;
    inst->T = T;
    inst->V = V;
    // cout << "\nNode vec: ";
    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << nodeVec[i].label << " ";
    // }
    // cout << endl;
}

double calcEucDist (double *X, double *Y, int I, int J){
    return sqrt(pow(X[I] - X[J], 2) + pow(Y[I] - Y[J], 2));
}