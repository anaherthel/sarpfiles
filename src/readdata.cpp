#include "readdata.h"
#include "functions.h"
#include "modelnode.h"
#include "modeltwostage.h"
#include <cstdlib>
#include <stdio.h>
//maybe later it is necessary to add the maximum driving time.

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem, int trialK){
    
    if (argc < 5) {
        cout << "\nMissing parameters\n";
        // cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]"<< endl;
        cout << " ./exeSARP [Instance] [Scenario] [Parcel percentage] [model type]"<< endl;
        exit(1);
    }
    
    if (argc > 5) {
        cout << "\nToo many parameters\n";
        cout << " ./exeSARP [Instance] [Scenario] [Parcel percentage] [model type]" << endl;
        exit(1);
    }  

    // if (argv[2] == "sim"){
    //     problem->sim = true;
    // }
    // else if (argv[2] == "seq"){
    //     problem->seq = true;
    // }

    problem->scen = argv[2];
    problem->model = argv[4];

    string file, ewf;
    int n;
    int m;
    int K;
    double service;
    double T;
    int V;
    int originalV;
    int dummy;
    string parcelArg;
    double parcelP;

    string instType;

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    
    if( !in ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    instType = getInstanceType(argv);
    parcelArg = argv[3];
    parcelP = stod(parcelArg)/100;
    
    if (instType == "sf_data"){

        in >> K;
        in >> service;
        in >> n;
        in >> m;

        // K = 11;

        if (trialK >= K){
            K = trialK;
        }
        else{
            trialK = K;
        }
        
        cout << "K: " << K << endl;
        // getchar();

        service = service/60;
        V = n + 2*m + K;

        originalV = 2*n + 2*m + 2; 
        inst->vmed = 19.3;
        inst->dummy = 1;

        double *delta = new double[V + inst->dummy];
        double *slatitude = new double [V + inst->dummy];
        double *slongitude = new double [V + inst->dummy];
        double *flatitude = new double [V + inst->dummy];
        double *flongitude = new double [V + inst->dummy];
        double *profit = new double[V+inst->dummy];
        double *trip = new double[V+inst->dummy];

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }

        vector<double> vxs;
        vector<double> vys;
        vector<double> vload;
        vector<double> ve;
        vector<double> vxf;
        vector<double> vyf;
        vector<double> vl;

        int tempNode;

        for (int i = 0; i < originalV; i++){
            vxs.push_back(0);
            vys.push_back(0);
            vload.push_back(0);
            ve.push_back(0);
            vl.push_back(0);
        }

        for (int i = 0; i < originalV; i++){
            in >> tempNode >> vxs[i] >> vys[i] >> tempNode >> vload[i] >> ve[i] >> vl[i];
        }

        // ve[ve.size()-1] = 0;

        for (int i = 0; i < vxs.size(); i++){
            vxf.push_back(vxs[i]);
            vyf.push_back(vys[i]);

            if (vload[i] < -2.0){
                vxf[i - m - n] = vxs[i];
                vyf[i - m - n] = vys[i];
            }
        }

        vxs.erase(vxs.begin());
        vys.erase(vys.begin());
        vload.erase(vload.begin());
        vxf.erase(vxf.begin());
        vyf.erase(vyf.begin());
        ve.erase(ve.begin());
        vl.erase(vl.begin());

        for (int i = 0; i < n; i++){
            vxs.erase(vxs.begin() + n + m);
            vys.erase(vys.begin() + n + m);
            vload.erase(vload.begin() + n + m);
            ve.erase(ve.begin() + n + m);
            vl.erase(vl.begin() + n + m);
            vxf.erase(vxf.begin() + n + m);
            vyf.erase(vyf.begin() + n + m);
        }

        for (int i = 0; i < n; i++){
            vl[i] = ve[i];
        }

        for (int i = n; i < n + 2*m; i++){
            ve[i] = 0;
   
        }

        for (int i = 1; i < K; i++){
            vxs.push_back(vxs[vxs.size()-1]);
            vys.push_back(vys[vys.size()-1]);
            vload.push_back(vload[vload.size()-1]);
            ve.push_back(ve[ve.size()-1]);
            vl.push_back(vl[vl.size()-1]);
            vxf.push_back(vxf[vxf.size()-1]);
            vyf.push_back(vyf[vyf.size()-1]);
        }

        for (int i = 0; i < n; i++){
            vload[i] = 0;
        }


        // cout << "\nve: " << endl;
        // for (int i = 0; i < ve.size(); i++){
        //     cout << ve[i] << endl;
        // }
        // getchar();
        // Calculate distance matrix (Geolocation)

        // CalcLatLong ( vxs, vys, vxf, vyf, V, slatitude, slongitude, flatitude, flongitude );

        double singleProfit;
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
                // delta[i] = (2 * (service)) + (CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i))/inst->vmed;
                // trip[i] = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i);
               
                delta[i] = (2 * (service)) + (calcEucDist(vxs, vys, vxf, vyf, i, i))/inst->vmed;
                trip[i] = calcEucDist(vxs, vys, vxf, vyf, i, i);
                profit[i] = inst->gamma2 + inst->mu2*calcEucDist(vxs, vys, vxf, vyf, i, i) - calcEucDist(vxs, vys, vxf, vyf, i, i);
                // profit[i] = inst->gamma2 + inst->mu2*CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i) - CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i);
            }
            else if (i < V - K){ 
                delta[i] = service;
                if (i < n + m){
                    // profit[i] = inst->gamma + inst->mu*CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i+m);
                    profit[i] = inst->gamma + inst->mu*calcEucDist(vxs, vys, vxf, vyf, i, i+m);
                }
                else{
                    profit[i] = 0;
                }
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
            }
            for (int j = 0; j < V + inst->dummy; j++){
                if(i == j){
                   dist[i][j] = 0;
                }
                else{
                    if (i < V){
                        if (j < V){
                            // dist[i][j] = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, j);
                            dist[i][j] = calcEucDist(vxs, vys, vxf, vyf, i, j);
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
            node->xs = vxs[i];
            node->ys = vys[i];
            node->load = vload[i];
            node->e = ve[i]/60;
            node->l = vl[i]/60;
            node->xf = vxf[i];
            node->yf = vyf[i];
            node->delta = delta[i];
            node->profit = profit[i];
            node->index = i;
            nodeVec.push_back(*node);
        }

        // Adding dummy nodes
        for (int i = 0; i < inst->dummy; i++){
            node->xs = 0;
            node->ys = 0;
            node->load = 0;
            node->e = 9;
            // node->l = 14*60;
            node->l = 17;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            node->profit = 0;
            node->index = V + i;
            nodeVec.push_back(*node);
        }
        
        // cout << "Earlier: \n{";

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << nodeVec[i].e << " } {";
        // }
        // cout << endl;
        // getchar();

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        inst->T = nodeVec[V + inst->dummy - 1].l;

        delete[] profit;
        delete[] delta;
        delete[] slatitude;
        delete[] slongitude;
        delete[] flatitude;
        delete[] flongitude;   
    }

    else if (instType == "csarp"){
        in >> K;
        in >> service;
        in >> n;
        in >> m;

        // K = 11;

        if (trialK < K){
            K = trialK;
        }
        else{
            trialK = K;
        }
        
        cout << "K: " << K << endl;
        // getchar();

        service = service/60;
        V = n + 2*m + K;

        originalV = 2*n + 2*m + K; 
        inst->vmed = 19.3;
        inst->dummy = 1;

        double *delta = new double[V + inst->dummy];
        double *profit = new double[V+inst->dummy];
        double *trip = new double[V+inst->dummy];

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }

        vector<double> vxs;
        vector<double> vys;
        vector<double> vload;
        vector<double> ve;
        vector<double> vxf;
        vector<double> vyf;
        vector<double> vl;

        int tempNode;

        for (int i = 0; i < originalV; i++){
            vxs.push_back(0);
            vys.push_back(0);
            vload.push_back(0);
            ve.push_back(0);
            vl.push_back(0);
        }

        for (int i = 0; i < originalV; i++){
            in >> tempNode >> vxs[i] >> vys[i] >> vload[i] >> ve[i] >> vl[i];
        }

        // ve[ve.size()-1] = 0;

        for (int i = 0; i < vxs.size(); i++){
            vxf.push_back(vxs[i]);
            vyf.push_back(vys[i]);

            if (vload[i] < -2.0){
                vxf[i - n] = vxs[i];
                vyf[i - n] = vys[i];
            }
        }

        // cout << "Vector of vxf:" << endl;

        // for (int i = 0; i < vxf.size(); i++){
        //     cout << i << ": " << vxf[i] << endl;
        // }
        // cout << endl;

        for (int i = 0; i < n; i++){
            vxs.erase(vxs.begin() + n);
            vys.erase(vys.begin() + n);
            vload.erase(vload.begin() + n);
            ve.erase(ve.begin() + n);
            vl.erase(vl.begin() + n);
            vxf.erase(vxf.begin() + n);
            vyf.erase(vyf.begin() + n);
        }


        for (int i = 0; i < n; i++){
            vl[i] = ve[i];
        }

        for (int i = n; i < n + 2*m; i++){
            ve[i] = 0;
   
        }

        // for (int i = 1; i < K; i++){
        //     vxs.push_back(vxs[vxs.size()-1]);
        //     vys.push_back(vys[vys.size()-1]);
        //     vload.push_back(vload[vload.size()-1]);
        //     ve.push_back(ve[ve.size()-1]);
        //     vl.push_back(vl[vl.size()-1]);
        //     vxf.push_back(vxf[vxf.size()-1]);
        //     vyf.push_back(vyf[vyf.size()-1]);
        // }

        for (int i = 0; i < n; i++){
            vload[i] = 0;
        }


        // cout << "\nve: " << endl;
        // for (int i = 0; i < ve.size(); i++){
        //     cout << ve[i] << endl;
        // }
        // getchar();

        // Calculate distance matrix (Euclidian)

        double singleProfit;
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
                delta[i] = (2 * (service)) + (calcEucDist(vxs, vys, vxf, vyf, i, i))/inst->vmed;
                trip[i] = calcEucDist(vxs, vys, vxf, vyf, i, i);
                profit[i] = inst->gamma2 + inst->mu2*calcEucDist(vxs, vys, vxf, vyf, i, i) - calcEucDist(vxs, vys, vxf, vyf, i, i);
            }
            else if (i < V - K){ 
                delta[i] = service;
                if (i < n + m){
                    profit[i] = inst->gamma + inst->mu*calcEucDist(vxs, vys, vxf, vyf, i, i+m);
                }
                else{
                    profit[i] = 0;
                }
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
            }
            for (int j = 0; j < V + inst->dummy; j++){
                if(i == j){
                   dist[i][j] = 0;
                }
                else{
                    if (i < V){
                        if (j < V){
                            dist[i][j] = calcEucDist(vxs, vys, vxf, vyf, i, j);
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
            node->xs = vxs[i];
            node->ys = vys[i];
            node->load = vload[i];
            node->e = ve[i]/60;
            node->l = vl[i]/60;
            node->xf = vxf[i];
            node->yf = vyf[i];
            node->delta = delta[i];
            node->profit = profit[i];
            node->index = i;
            nodeVec.push_back(*node);
        }

        // Adding dummy nodes
        for (int i = 0; i < inst->dummy; i++){
            node->xs = 0;
            node->ys = 0;
            node->load = 0;
            node->e = 9;
            // node->l = 14*60;
            node->l = 17;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            node->profit = 0;
            node->index = V + i;
            nodeVec.push_back(*node);
        }
        
        // cout << "Earlier: \n{";

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << nodeVec[i].e << " } {";
        // }
        // cout << endl;
        // getchar();

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        inst->T = nodeVec[V + inst->dummy - 1].l;

        delete[] profit;
        delete[] delta;
        delete[] trip;

    }

    if(problem->scen == "1A" || "1B"){
        inst->nCluster = inst->n + inst->K + inst->dummy;
    }
    else if(problem->scen == "2A" || "2B"){
        inst->nCluster = inst->n + inst->K + inst->dummy;
    }

    cout << "\nsize of n: " << inst->n << endl;
    cout << "\nsize of m: " << inst->m << endl;
}

double calcEucDist (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    return sqrt(pow(Xf[I] - Xs[J], 2) + pow(Yf[I] - Ys[J], 2));
}

double CalcMan (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    return abs(Xf[I] - Xs[J]) + abs(Yf[I] - Ys[J]);
}

double CalcLatLong (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int n, double *slatit, double* slongit, double *flatit, double* flongit){
    double PI = 3.141592, min;
    int deg;
    
    for (int i = 0; i < n; i++) {
        deg = (int) Xs[i];
        min = Xs[i] - deg;
        // slatit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
        slatit[i] = Xs[i] * PI / 180.0;
      
        deg = (int) Xf[i];
        min = Xf[i] - deg;
        // flatit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
        flatit[i] = Xf[i] * PI/ 180.0;
    }
    
    for (int i = 0; i < n; i++) {
        deg = (int) Ys[i];
        min = Ys[i] - deg;
        // slongit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
        slongit[i] = Ys[i] * PI / 180.0;

        deg = (int) Yf[i];
        min = Yf[i] - deg;
        // flongit[i] = PI * (deg + 5.0 * min / 3.0) / 180.0;
        flongit[i] = Yf[i] * PI / 180;
    }
    return 0;
}


double CalcDistGeo (double *slatit, double* slongit, double *flatit, double* flongit, int I, int J){
    double q1, q2, q3, RRR = 6378.388;
    
    q1 = cos(flongit[I] - slongit[J]);
    q2 = cos(flatit[I] - slatit[J]);
    q3 = cos(flatit[I] + slatit[J]);

    return (RRR * acos( 0.5*((1.0+q1)*q2 - (1.0-q1)*q3)));
    // (int) (RRR * acos( 0.5*((1.0+q1)*q2 - (1.0-q1)*q3)));
}

string getInstanceType (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_first_of("/");
    string::size_type loc2 = filename.find_last_of("/", filename.size());
    string InstanceType;

    InstanceType.append(filename, loc+1, loc2-loc-1 );

    return InstanceType;
}