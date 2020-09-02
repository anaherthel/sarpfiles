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


        cout << "\nve: " << endl;
        for (int i = 0; i < ve.size(); i++){
            cout << ve[i] << endl;
        }
        getchar();
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
        
        cout << "Earlier: \n{";

        for (int i = 0; i < nodeVec.size(); i++){
            cout << nodeVec[i].e << " } {";
        }
        cout << endl;
        getchar();

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

    else if (instType == "grubhub"){

        K = 1;

        // bool increaseK = false;
        // K = 2;
        service = 5; //for some reason, service = 5/60 did not work
        service = service/60;
        int refpoint = K + 1;
        int instV;
        dummy = 1;
        inst->dummy = dummy;
        inst->vmed = 19.3;

        int seed = 1234;
        srand(seed);

        vector <vector <double> > tempData;
        vector<double> auxtempdata;

        vector <vector <double> > realData;

        while ( file.compare("DIMENSION:") != 0 && file.compare("DIMENSION") != 0 ){
            in >> file;
        }
        
        in >> instV;

        m = floor(instV * parcelP);
        
        if (m % 2 != 0){
            m--;
        }

        n = (instV - refpoint - m)/2;

        m /= 2; 

        V = n + 2*m + K;
        // cout << "V: " << V << endl;
        cout << "\nn: " << n << " m: " << m << endl; 


        while ( file.compare("EDGE_WEIGHT_FORMAT") != 0 && file.compare("EDGE_WEIGHT_FORMAT") != 0 ){
            in >> file;
        }

        in >> file;
        in >> ewf;

        while (file.compare("EDGE_WEIGHT_SECTION") != 0){
            in >> file;
        }
        
        for (int i = 0; i < instV + refpoint; i++){
            for(int j = 0; j < instV + refpoint; j++){
                auxtempdata.push_back(0);
            }
            tempData.push_back(auxtempdata);
            auxtempdata.clear();
        }

        for (int i = 0; i < V + inst->dummy; i++){
            for(int j = 0; j < V + inst->dummy; j++){
                auxtempdata.push_back(0);
            }
            realData.push_back(auxtempdata);
            auxtempdata.clear();
        }

        if (ewf == "LOWER_DIAG_ROW"){
           for (int i = 0; i < instV; i++) {
                for (int j = 0; j < i + 1; j++) {
                    in >> tempData[i][j];
                    if (i > 0){
                        tempData[j][i] = tempData[i][j];                        
                    }
                }
            }
        }
        
        //adjusting rows
        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[instV + j][i] = tempData[j][i];
            }
        }

        //adjusting columns
        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[i][instV + j] = tempData[i][j];
            }
        }

        // //maybe we needed to adjust the corner (relying on the -0 being f)

        //erase unused
        for (int j = 0; j < refpoint; j++){
            tempData.erase(tempData.begin());
        }

        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[i].erase(tempData[i].begin());
            }   
        }

        // cout << "\nDistance Matrix BEFORE: " << endl;
        // getchar();
        // for (int i = 0; i < tempData.size(); i++){
        //     for (int j = 0; j < tempData[i].size(); j++){
        //         cout << setw(5) << tempData[i][j] << " ";
        //     }
        //     cout << endl;
        // }
        // getchar();

        vector<double> dRow;
        vector<double> pRow;

        vector< vector<double> > deliveries;
        vector< vector<double> > pickups;

        for (int i = 0; i < 2*m; i++){
            for (int j = 0; j < tempData.size(); j++){
                if (i % 2 != 0){
                    dRow.push_back(tempData[2*n + i][j]);
                }
                else{
                    pRow.push_back(tempData[2*n + i][j]);
                }
            }
            if (i % 2 != 0){
                deliveries.push_back(dRow);
                dRow.clear();
            }
            else{
                pickups.push_back(pRow);
                pRow.clear();
            }
        }

        //Organize columns in dettached rows
        int count;
        double chosen;

        for (int j = 0; j < pickups.size(); j++){
            count = 1;
            for (int i = 2; i < 2*m; i++){
                if (i % 2 == 0){
                    // cout << "First i: " << i << "- j: " << j << endl;
                    // getchar();
                    chosen = pickups[j][2*n + i];
                    // getchar();
                    // cout << "chosen " << chosen << " i: " << i << "- j: " << j << endl;
                    // getchar();
                    pickups[j].erase(pickups[j].begin() + 2*n + i);
                    pickups[j].insert(pickups[j].begin() + 2*n + count, chosen);
                    // cout << "\nPickups verify: ";
                    // for (int k = 0; k < pickups[j].size(); k++){
                    //     cout << pickups[j][k] << " "; 
                    // }
                    // cout << endl;
                    // getchar();
                    count++;
                }
            }
        }


        for (int j = 0; j < deliveries.size(); j++){
            count = 1;
            for (int i = 2; i < 2*m; i++){
                if (i % 2 == 0){
                    chosen = deliveries[j][2*n + i];
                    deliveries[j].erase(deliveries[j].begin() + 2*n + i);
                    deliveries[j].insert(deliveries[j].begin() + 2*n + count, chosen);
                    count++;
                }
            }
        }

        for (int j = 0; j < tempData.size(); j++){
            for (int i = 0; i < 2*m; i++){
                tempData[j].erase(tempData[j].begin()+2*n); 
            }
        }

        tempData.erase(tempData.begin()+2*n, tempData.begin()+2*n+2*m);  

        for (int i = deliveries.size() - 1; i >= 0; i--){
            tempData.insert(tempData.begin()+2*n, deliveries[i]);
        }

        for (int i = pickups.size() - 1; i >= 0; i--){
            tempData.insert(tempData.begin()+2*n, pickups[i]);
        }

        for (int k = deliveries.size() - 1; k >= 0; k--){
            for (int j = 0; j < deliveries[k].size(); j++){
                if (j < 2*n){
                    tempData[j].insert(tempData[j].begin()+2*n, deliveries[k][j]);
                }
                else if (j < 2*n+2*m){
                    continue;
                }
                else{
                    tempData[j].insert(tempData[j].begin()+2*n, deliveries[k][j]);
                }
            }
        }

        for (int k = pickups.size() - 1; k >= 0; k--){
            for (int j = 0; j < pickups[k].size(); j++){
                if (j < 2*n){
                    tempData[j].insert(tempData[j].begin()+2*n, pickups[k][j]);
                }
                else if (j < 2*n+2*m){
                    continue;
                }
                else{
                    tempData[j].insert(tempData[j].begin()+2*n, pickups[k][j]);
                }
            }
        }

        //collapsing passenger nodes

        for (int i = 0; i < V + inst->dummy; i++){
            for (int j = 0; j < V + inst->dummy; j++){
                if (i == j){
                    realData[i][j] = 0;
                }
                else{
                    if (i < n){
                        if (j < n){
                            realData[i][j] = (tempData[2*i+1][2*j])/1000;
                        }
                        else{
                            realData[i][j] = (tempData[2*i+1][n+j])/1000;
                        }
                    }
                    else{
                        if (j < n){
                            realData[i][j] = (tempData[n+i][2*j])/1000;
                        }
                        else{
                            realData[i][j] = (tempData[n+i][n+j])/1000;
                        }
                    }
                }
            }
        }

        if (trialK > 1){
            if (trialK >= K){
                K = trialK;
            }
            else{
                trialK = K;
            } 
            for (int l = 0; l < K -1; l++){
                vector<double> distRow;
                vector<double> dummyRow;

                double valueDist;
                
                for (int i = 0; i < V + inst->dummy; i++){
                    valueDist = realData[i][realData[i].size() - 2];
                    realData[i].insert(realData[i].begin() + realData[i].size() - 1, valueDist);
                }

                for (int i = 0; i < V + inst->dummy; i++){
                    distRow.push_back(realData[V - 1][i]);
                    dummyRow.push_back(realData[V][i]);
                }
                distRow.push_back(0);
                dummyRow.push_back(0); 

                realData.pop_back();

                realData.push_back(distRow);
                realData.push_back(dummyRow);
                V++;
            }
        }

        double *delta = new double[V + inst->dummy];
        double *profit = new double[V + inst->dummy];
        double *e = new double[V + inst->dummy];
        double *l = new double[V + inst->dummy];
        int *w = new int[V + inst->dummy];      

        int reference = n;
        //calculate deltas
        for(int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                // cout << i << ": " << (tempData[2*i][2*i+1]);
                delta[i] = 2 * service + (((tempData[2*i][2*i+1])/1000)/inst->vmed);
                // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
                profit[i] = inst->gamma2 + inst->mu2*(tempData[2*i][2*i+1]/1000) - (tempData[2*i][2*i+1]/1000);    
                w[i] = 0;
            }
            else if (i < V - K){
                delta[i] = service;
                if (i < n + m){
                    profit[i] = inst->gamma + inst->mu*(tempData[i + n][i + n + m]/1000);
                    w[i] = 1;
                }
                else if (i < n + 2*m){
                    profit[i] = 0;
                    w[i] = -1;
                }
                else{
                   profit[i] = 0;
                   w[i] = 0;
                }
                
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
                w[i] = 0;
            }
        }

        for (int i = 0; i < V + inst->dummy; i++){
            if(i < n){
                e[i] = 540 + rand() % 480;
                l[i] = e[i];
            }
            else if (i < inst->n + 2*inst->m){
                e[i] = 0;
                l[i] = 1020;                
            }
            else if (i < V + inst->dummy - 1){
                e[i] = 540;
                l[i] = 1020;
            }
            else{
                e[i] = 540;
                l[i] = 1440;
            } 
        }

        for (int i = 0; i < V + inst->dummy; i++){
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->delta = delta[i];
            node->profit = profit[i];
            node->load = w[i];
            node->index = i;
            nodeVec.push_back(*node);
        }

        // cout << "\nLoad vector: " << endl;

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].load << " ||| " << endl;
        // }
        // getchar();

        // cout << "\nDelta vector: " << endl;

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].delta << " || ";
        // }
        // cout << endl;
        // getchar();

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }

        for(int i = 0; i < V + inst->dummy; i++){
            for (int j = 0; j < V + inst->dummy; j++){
                dist[i][j] = realData[i][j];
            }
        }

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        inst->T = nodeVec[V + inst->dummy - 1].l;

        delete[] profit;
        delete[] delta;
        
        cout << "\nStarting times: " << endl;

        for (int i = 0; i < nodeVec.size(); i++){
            cout << i << ": " << nodeVec[i].e << " || ";
        }
    }

    else if (instType == "grubhub2"){

        K = 1;

        // bool increaseK = false;
        // K = 2;
        service = 5; //for some reason, service = 5/60 did not work
        service = service/60;
        int refpoint = K + 1;
        int instV;
        dummy = 1;
        inst->dummy = dummy;
        inst->vmed = 19.3;

        int seed = 1234;
        srand(seed);

        vector <vector <double> > tempData;
        vector<double> auxtempdata;

        vector <vector <double> > realData;

        while ( file.compare("DIMENSION:") != 0 && file.compare("DIMENSION") != 0 ){
            in >> file;
        }
        
        in >> instV;

        m = floor(instV * parcelP);
        
        if (m % 2 != 0){
            m--;
        }

        n = (instV - refpoint - m)/2;

        m /= 2; 

        V = n + 2*m + K;
        // cout << "V: " << V << endl;
        // cout << "\n" << n << " " << m << endl;
        // getchar();

        while ( file.compare("EDGE_WEIGHT_FORMAT") != 0 && file.compare("EDGE_WEIGHT_FORMAT") != 0 ){
            in >> file;
        }

        in >> file;
        in >> ewf;

        while (file.compare("EDGE_WEIGHT_SECTION") != 0){
            in >> file;
        }
        
        for (int i = 0; i < instV + refpoint; i++){
            for(int j = 0; j < instV + refpoint; j++){
                auxtempdata.push_back(0);
            }
            tempData.push_back(auxtempdata);
            auxtempdata.clear();
        }

        for (int i = 0; i < V + inst->dummy; i++){
            for(int j = 0; j < V + inst->dummy; j++){
                auxtempdata.push_back(0);
            }
            realData.push_back(auxtempdata);
            auxtempdata.clear();
        }

        if (ewf == "LOWER_DIAG_ROW"){
           for (int i = 0; i < instV; i++) {
                for (int j = 0; j < i + 1; j++) {
                    in >> tempData[i][j];
                    if (i > 0){
                        tempData[j][i] = tempData[i][j];                        
                    }
                }
            }
        }
        
        //adjusting rows
        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[instV + j][i] = tempData[j][i];
            }
        }

        //adjusting columns
        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[i][instV + j] = tempData[i][j];
            }
        }

        // //maybe we needed to adjust the corner (relying on the -0 being f)

        //erase unused
        for (int j = 0; j < refpoint; j++){
            tempData.erase(tempData.begin());
        }

        for (int i = 0; i < instV; i++){
            for (int j = 0; j < refpoint; j++){
                tempData[i].erase(tempData[i].begin());
            }   
        }

        vector<double> dRow;
        vector<double> pRow;

        vector< vector<double> > deliveries;
        vector< vector<double> > pickups;

        for (int i = 0; i < 2*m; i++){
            for (int j = 0; j < tempData.size(); j++){
                if (i % 2 != 0){
                    dRow.push_back(tempData[2*n + i][j]);
                }
                else{
                    pRow.push_back(tempData[2*n + i][j]);
                }
            }
            if (i % 2 != 0){
                deliveries.push_back(dRow);
                dRow.clear();
            }
            else{
                pickups.push_back(pRow);
                pRow.clear();
            }
        }
        
        //Organize columns in dettached rows
        int count;
        double chosen;

        for (int j = 0; j < pickups.size(); j++){
            count = 1;
            for (int i = 2; i < 2*m; i++){
                if (i % 2 == 0){
                    // cout << "First i: " << i << "- j: " << j << endl;
                    // getchar();
                    chosen = pickups[j][2*n + i];
                    // getchar();
                    // cout << "chosen " << chosen << " i: " << i << "- j: " << j << endl;
                    // getchar();
                    pickups[j].erase(pickups[j].begin() + 2*n + i);
                    pickups[j].insert(pickups[j].begin() + 2*n + count, chosen);
                    // cout << "\nPickups verify: ";
                    // for (int k = 0; k < pickups[j].size(); k++){
                    //     cout << pickups[j][k] << " "; 
                    // }
                    // cout << endl;
                    // getchar();
                    count++;
                }
            }
        }


        for (int j = 0; j < deliveries.size(); j++){
            count = 1;
            for (int i = 2; i < 2*m; i++){
                if (i % 2 == 0){
                    chosen = deliveries[j][2*n + i];
                    deliveries[j].erase(deliveries[j].begin() + 2*n + i);
                    deliveries[j].insert(deliveries[j].begin() + 2*n + count, chosen);
                    count++;
                }
            }
        }

        for (int j = 0; j < tempData.size(); j++){
            for (int i = 0; i < 2*m; i++){
                tempData[j].erase(tempData[j].begin()+2*n); 
            }
        }

        tempData.erase(tempData.begin()+2*n, tempData.begin()+2*n+2*m);  

        for (int i = deliveries.size() - 1; i >= 0; i--){
            tempData.insert(tempData.begin()+2*n, deliveries[i]);
        }

        for (int i = pickups.size() - 1; i >= 0; i--){
            tempData.insert(tempData.begin()+2*n, pickups[i]);
        }

        for (int k = deliveries.size() - 1; k >= 0; k--){
            for (int j = 0; j < deliveries[k].size(); j++){
                if (j < 2*n){
                    tempData[j].insert(tempData[j].begin()+2*n, deliveries[k][j]);
                }
                else if (j < 2*n+2*m){
                    continue;
                }
                else{
                    tempData[j].insert(tempData[j].begin()+2*n, deliveries[k][j]);
                }
            }
        }

        for (int k = pickups.size() - 1; k >= 0; k--){
            for (int j = 0; j < pickups[k].size(); j++){
                if (j < 2*n){
                    tempData[j].insert(tempData[j].begin()+2*n, pickups[k][j]);
                }
                else if (j < 2*n+2*m){
                    continue;
                }
                else{
                    tempData[j].insert(tempData[j].begin()+2*n, pickups[k][j]);
                }
            }
        }

        //collapsing passenger nodes

        for (int i = 0; i < V + inst->dummy; i++){
            for (int j = 0; j < V + inst->dummy; j++){
                if (i == j){
                    realData[i][j] = 0;
                }
                else{
                    if (i < n){
                        if (j < n){
                            realData[i][j] = (tempData[2*i+1][2*j])/1000;
                        }
                        else{
                            realData[i][j] = (tempData[2*i+1][n+j])/1000;
                        }
                    }
                    else{
                        if (j < n){
                            realData[i][j] = (tempData[n+i][2*j])/1000;
                        }
                        else{
                            realData[i][j] = (tempData[n+i][n+j])/1000;
                        }
                    }
                }
            }
        }

        if (trialK > 1){
            if (trialK >= K){
                K = trialK;
            }
            else{
                trialK = K;
            } 
            for (int l = 0; l < K -1; l++){
                vector<double> distRow;
                vector<double> dummyRow;

                double valueDist;
                
                for (int i = 0; i < V + inst->dummy; i++){
                    valueDist = realData[i][realData[i].size() - 2];
                    realData[i].insert(realData[i].begin() + realData[i].size() - 1, valueDist);
                }

                for (int i = 0; i < V + inst->dummy; i++){
                    distRow.push_back(realData[V - 1][i]);
                    dummyRow.push_back(realData[V][i]);
                }
                distRow.push_back(0);
                dummyRow.push_back(0); 

                realData.pop_back();

                realData.push_back(distRow);
                realData.push_back(dummyRow);
                V++;
            }
        }
        cout << "temp date size: " << tempData.size();
        cout << "aux data size: " << auxtempdata.size();

        double *delta = new double[V + inst->dummy];
        double *profit = new double[V + inst->dummy];
        double *e = new double[V + inst->dummy];
        double *l = new double[V + inst->dummy];
        int *w = new int[V + inst->dummy];      

        int reference = n;
        //calculate deltas
        for(int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                // cout << i << ": " << (tempData[2*i][2*i+1]);
                delta[i] = 2 * service + (((tempData[2*i][2*i+1])/1000)/inst->vmed);
                // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
                profit[i] = inst->gamma2 + inst->mu2*(tempData[2*i][2*i+1]/1000) - (tempData[2*i][2*i+1]/1000);    
                w[i] = 0;
            }
            else if (i < V - K){
                delta[i] = service;
                if (i < n + m){
                    profit[i] = inst->gamma + inst->mu*(tempData[i + n][i + n + m]/1000);
                    w[i] = 1;
                }
                else if (i < n + 2*m){
                    profit[i] = 0;
                    w[i] = -1;
                }
                else{
                   profit[i] = 0;
                   w[i] = 0;
                }
                
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
                w[i] = 0;
            }
        }

        for (int i = 0; i < V + inst->dummy; i++){
            if(i < n){
                e[i] = 540 + rand() % 480;
                l[i] = e[i];
            }
            else if (i < inst->n + 2*inst->m){
                e[i] = 0;
                l[i] = 1020;                
            }
            else if (i < V + inst->dummy - 1){
                e[i] = 540;
                l[i] = 1020;
            }
            else{
                e[i] = 540;
                l[i] = 1440;
            } 
        }

        for (int i = 0; i < V + inst->dummy; i++){
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->delta = delta[i];
            node->profit = profit[i];
            node->load = w[i];
            node->index = i;
            nodeVec.push_back(*node);
        }

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }

        for(int i = 0; i < V + inst->dummy; i++){
            for (int j = 0; j < V + inst->dummy; j++){
                dist[i][j] = realData[i][j];
            }
        }
        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        inst->T = nodeVec[V + inst->dummy - 1].l;

        delete[] profit;
        delete[] delta;

    }

    else if (instType == "hybgrub" || instType == "hyb20" || instType == "hyb25" || instType == "hyb30"){

        // bool increaseK = false;
        // K = 2;
        service = 5; //for some reason, service = 5/60 did not work
        service = service/60;
        int refpoint = K + 1;
        int V;
        dummy = 1;
        inst->dummy = dummy;
        inst->vmed = 19.3;

        int seed = 1234;
        srand(seed);

        vector <vector <double> > realData;
        vector<double> auxvec;

        vector<double> serviceVec;

        in >> file;
        in >> V;
        in >> file;
        in >> n;
        in >> file;
        in >> m;
        in >> file;
        in >> K;
        in >> file;

        for (int i = 0; i < n; i++){
            serviceVec.push_back(0);
        }

        for (int i = 0; i < n; i++){
            in >> serviceVec[i];
            serviceVec[i] = serviceVec[i]/1000;
        }
        
       
        for (int i = 0; i < V + inst->dummy; i++){
            for(int j = 0; j < V + inst->dummy; j++){
                auxvec.push_back(0);
            }
            realData.push_back(auxvec);
            auxvec.clear();
        }

        in >> file >> file;
        
        for (int i = 0; i < V + inst->dummy; i++) {
            for (int j = 0; j < V + inst->dummy; j++) {
                in >> realData[i][j];
                realData[i][j] = realData[i][j]/1000;
            }
        }

        int posK1;
        int posK2;

        posK1 = realData[0].size() - 3;
        posK2 = realData[0
            ].size() - 2;

        if (trialK > 2){
            if (trialK >= K){
                K = trialK;
            }
            else{
                trialK = K;
            }

            for (int valK = 3; valK <= K; valK++){

                if (valK % 2 != 0){
                     

                    vector<double> distRow;
                    vector<double> dummyRow;

                    double valueDist;

                    for (int i = 0; i < V + inst->dummy; i++){
                        valueDist = realData[i][posK1];
                        realData[i].insert(realData[i].begin() + realData[i].size() - 1, valueDist);
                    }

                    // cout << "\nSize of real data: " << realData.size() << endl;
                    // getchar();
                    // cout << "\nNumber 1: Distance Matrix (inside read data): " << endl;

                    // for (int i = 0; i < realData.size(); i++){
                    //     for (int j = 0; j < realData[i].size(); j++){
                    //         cout << setw(5) << realData[i][j] << " ";
                    //     }
                    //     cout << endl;
                    // }
                    // getchar();

                    for (int i = 0; i < V + inst->dummy; i++){
                        distRow.push_back(realData[posK1][i]);
                        dummyRow.push_back(realData[V][i]);
                    }
                    distRow.push_back(0);
                    dummyRow.push_back(0); 

                    realData.pop_back();

                    realData.push_back(distRow);
                    realData.push_back(dummyRow);
                    V++;
                }
                else{
                    vector<double> distRow;
                    vector<double> dummyRow;

                    double valueDist;

                    for (int i = 0; i < V + inst->dummy; i++){
                        valueDist = realData[i][realData[i].size() - 2];
                        realData[i].insert(realData[i].begin() + realData[i].size() - 1, valueDist);
                    }

                    // cout << "\nSize of real data: " << realData.size() << endl;
                    // getchar();
                    // cout << "\nNumber 2: Distance Matrix (inside read data): " << endl;

                    // for (int i = 0; i < realData.size(); i++){
                    //     for (int j = 0; j < realData[i].size(); j++){
                    //         cout << setw(5) << realData[i][j] << " ";
                    //     }
                    //     cout << endl;
                    // }
                    // getchar();

                    for (int i = 0; i < V + inst->dummy; i++){
                        distRow.push_back(realData[V - 1][i]);
                        dummyRow.push_back(realData[V][i]);
                    }
                    distRow.push_back(0);
                    dummyRow.push_back(0); 

                    realData.pop_back();

                    realData.push_back(distRow);
                    realData.push_back(dummyRow);
                    V++;
                }
            }
            

            // cout << "\nDistance Matrix (inside read data): " << endl;

            // for (int i = 0; i < realData.size(); i++){
            //     for (int j = 0; j < realData[i].size(); j++){
            //         cout << setw(5) << realData[i][j] << " ";
            //     }
            //     cout << endl;
            // }
            // getchar();
        }

        double *delta = new double[V + inst->dummy];
        double *profit = new double[V + inst->dummy];
        double *e = new double[V + inst->dummy];
        double *l = new double[V + inst->dummy];
        int *w = new int[V + inst->dummy]; 

        for(int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                // cout << i << ": " << (tempData[2*i][2*i+1]);
                delta[i] = 2 * service + (serviceVec[i]/inst->vmed);

                // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
                profit[i] = inst->gamma2 + inst->mu2*(serviceVec[i]) - (serviceVec[i]);    
                w[i] = 0;
            }
            else if (i < V - K){
                delta[i] = service;

                if (i < n + m){
                    profit[i] = inst->gamma + inst->mu*(realData[i][i + m]);
                    w[i] = 1;
                }
                else if (i < n + 2*m){
                    profit[i] = 0;
                    w[i] = -1;
                }
                else{
                   profit[i] = 0;
                   w[i] = 0;
                }
                
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
                w[i] = 0;
            }
        }
 
        for (int i = 0; i < V + inst->dummy; i++){
            if(i < n){
                e[i] = 540 + rand() % 480;
                l[i] = e[i];
            }
            else if (i < inst->n + 2*inst->m){
                e[i] = 0;
                l[i] = 1020;                
            }
            else if (i < V + inst->dummy - 1){
                e[i] = 540;
                l[i] = 1020;
            }
            else{
                e[i] = 540;
                l[i] = 1440;
            } 
        }
        
        // for (int i = 0; i < V + inst->dummy; i++){
        //     cout << "\nearlier times: " << i << ": "<< e[i] << endl;    
        // }

        // getchar();
        
        for (int i = 0; i < V + inst->dummy; i++){
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->delta = delta[i];
            node->profit = profit[i];
            node->load = w[i];
            node->index = i;
            nodeVec.push_back(*node);
        }

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }


        for(int i = 0; i < V + inst->dummy; i++){
            for (int j = 0; j < V + inst->dummy; j++){
                dist[i][j] = realData[i][j];
            }
        }

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        inst->T = nodeVec[V + inst->dummy - 1].l;


        // cout << "\nhyb instance: K: " << inst->K << " -n: " << inst->n << " -m: " << inst->m << " -V: " << inst->V << " -T: " << endl;
        // getchar();

        delete[] profit;
        delete[] delta;
        delete[] w;
        delete[] e;
        delete[] l;

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