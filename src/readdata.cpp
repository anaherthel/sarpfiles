#include "readdata.h"
#include "functions.h"
#include "modelnode.h"
#include "modeltwostage.h"
#include <cstdlib>
#include <stdio.h>

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem, int trialK, double trialMulti){
    
    if (argc < 4) {
        cout << "\nMissing parameters\n";
        cout << " ./exeSARP [Instance] [Scenario] [model type]"<< endl;
        exit(1);
    }
    
    if (argc > 4) {
        cout << "\nToo many parameters\n";
        cout << " ./exeSARP [Instance] [Scenario] [model type]" << endl;
        exit(1);
    }  

    cout << "\nNameOfInst: " << argv[1] << endl;
    cout << "RunningScen: " << argv[2] << endl;
    inst->preInst = 0;
    inst->InstName = getInstName(argv);

    cout << "Instance Name: " << inst->InstName;
    // getchar();
    // if (argv[2] == "sim"){
    //     problem->sim = true;
    // }
    // else if (argv[2] == "seq"){
    //     problem->seq = true;
    // }

    problem->scen = argv[2];
    problem->model = argv[3];

    if (problem->scen == "1A"){
        if (problem->model == "math"){
            cout << "\n\nRun this scenario with 'bundle' model.\n";
            cout << " ./exeSARP [Instance] [Scenario] bundle"<< endl;
            exit(1);
        }
        else{
            problem->p1 = 0; //1 is multi, 0 is single; p1 refers to customer
            problem->p2 = 0; //p2 refers to parcel
            problem->dParcel = 0;//1 allows for direct parcel delivery
        }
    }
    else if (problem->scen == "1AD"){//1A with direct parcel delivery

        problem->p1 = 0; //1 is multi, 0 is single; p1 refers to customer
        problem->p2 = 0; //p2 refers to parcel
        problem->dParcel = 1;//1 allows for direct parcel delivery
        
    }
    else if (problem->scen == "2A"){
        problem->p1 = 1; //1 is multi, 0 is single; p1 refers to customer
        problem->p2 = 0; //p2 refers to parcel
        problem->dParcel = 0;//1 allows for direct parcel delivery
    }
    else if (problem->scen == "1B"){
        problem->p1 = 0;
        problem->p2 = 1;
        problem->dParcel = 0;
    }
    else if (problem->scen == "2B"){
        problem->p1 = 1;
        problem->p2 = 1;
        problem->dParcel = 0;        
    }
    else if (problem->scen == "2MM"){
        problem->p1 = 1;
        problem->p2 = 1;
        problem->dParcel = 1;        
    }
    else if (problem->scen == "PC" || problem->scen == "BL2"){ //PC: dedicated vehicles for each service; BL2: same car for both services, no shared trips
        problem->p2 = -1;
        problem->dParcel = 1;    
    }
    //condition for osarp and fip later 
    //osarp: original sarp with detours;
    //fip: original freight insertion problem


    string file, ewf;
    int n;
    int m;
    int K;
    double service;
    double T;
    int V;
    int originalV;
    int dummy;
    // string instType;

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);
    
    if( !in ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    inst->instType = getInstanceType(argv);
    cout << "\nInstance type: " << inst->instType << endl;
    
    if (inst->instType == "sf_data"){
        int tempNode;

        in >> K;
        // in >> tempNode;
        in >> service;
        in >> n;
        in >> m;
        // K = n - 1;

        // if (n <= 10){
        //     K = n-1;
        // }
        // else{
        //     K = ceil(0.6*n);
        // }

        // if (problem->model == "ils"){
        //     K = n - 1;
        // }
        // K = 11;

        // if (trialK <= K){
        //     K = trialK;
        // }
        // else{
        //     trialK = K;
        // }

        if (K + trialK <= n - 1){
            K += trialK;
        }
        else{
            cout << "\nExceeded max number of vehicles\n";            
            exit(1);
        }
        
        cout << "K: " << K << endl;
        // getchar();

        service = service/60;

        cout << "Service: " << service << endl;



        originalV = 2*n + 2*m + 1;

        if (problem->model != "osarp" && problem->model != "fip"){
            V = n + 2*m + K;
            inst->sigma = n;
        }
        else{
            V = 2*n + 2*m + K;
            inst->sigma = 2*n;
        }

        // inst->vmed = 19.3;
        inst->dummy = K;

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
        if (problem->model != "osarp" && problem->model != "fip"){
            for (int i = 0; i < vxs.size(); i++){
                vxf.push_back(vxs[i]);
                vyf.push_back(vys[i]);

                if (vload[i] < -2.0){//if i is a parcel or a depot
                    vxf[i - m - n] = vxs[i];
                    vyf[i - m - n] = vys[i];
                }
            }
        }
        else{
            for (int i = 0; i < vxs.size(); i++){
                vxf.push_back(vxs[i]);
                vyf.push_back(vys[i]);
            }
        }

        vxs.push_back(vxs[0]);
        vys.push_back(vys[0]);
        vload.push_back(vload[0]);
        vxf.push_back(vxf[0]);
        vyf.push_back(vyf[0]);
        ve.push_back(ve[0]);
        vl.push_back(vl[0]);

        vxs.erase(vxs.begin());
        vys.erase(vys.begin());
        vload.erase(vload.begin());
        vxf.erase(vxf.begin());
        vyf.erase(vyf.begin());
        ve.erase(ve.begin());
        vl.erase(vl.begin());

        if (problem->model != "osarp" && problem->model != "fip"){
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

            for (int i = n; i < originalV; i++){
                ve[i] = 0;
                vl[i] = 1440;
            }

            for (int i = 0; i < n; i++){
                vload[i] = 0;
            }
        }
        else{
            // for (int i = 0; i < n; i++){
            //     vl[i] = ve[i] + 10;
            // }
            for (int i = n; i < n + m; i++){
                ve[i] = 0;
                vl[i] = 1440;
            }
            for (int i = 2*n + m; i < 2*n + 2*m; i++){
                ve[i] = 0;
                vl[i] = 1440;
            }
            for (int i =  2*n + 2*m; i <  originalV; i++){
                ve[i] = 0;
                vl[i] = 1440;
            }
            //Fixing ordering of requests
            for (int i = n; i < n + m; i++){
                vxs.insert(vxs.begin() + i + n + m, vxs[i]);
                vys.insert(vys.begin() + i + n + m, vys[i]);
                vload.insert(vload.begin() + i + n + m, vload[i]);
                ve.insert(ve.begin() + i + n + m, ve[i]);
                vl.insert(vl.begin() + i + n + m, vl[i]);
                vxf.insert(vxf.begin() + i + n + m, vxf[i]);
                vyf.insert(vyf.begin() + i + n + m, vyf[i]);
            }           
            for (int i = n; i < n + m; i++){
                vxs.erase(vxs.begin() + n);
                vys.erase(vys.begin() + n);
                vload.erase(vload.begin() + n);
                ve.erase(ve.begin() + n);
                vl.erase(vl.begin() + n);
                vxf.erase(vxf.begin() + n);
                vyf.erase(vyf.begin() + n);
            } 
        }

        // Calculate distance matrix (Geolocation)

        CalcLatLong ( vxs, vys, vxf, vyf, V, slatitude, slongitude, flatitude, flongitude );

        double singleProfit;
        // double manhattan;
        double geodist;
        if  (problem->model != "osarp" && problem->model != "fip"){
            for (int i = 0; i < V + inst->dummy; i++){
                if (i < n){ 
                    // manhattan = CalcMan(vxs, vys, vxf, vyf, i, i);
                    geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i);
                    delta[i] = (2 * (service)) + (geodist)/inst->vmed;
                    profit[i] = inst->minpas + inst->paskm*geodist - inst->costkm*geodist;
                }
                else if (i < V - K){ 
                    delta[i] = service;
                    if (i < n + m){
                        geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i+m);
                        // manhattan = CalcMan(vxs, vys, vxf, vyf, i, i+m);
                        profit[i] = inst->minpar + inst->parkm*geodist;
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
                                // manhattan = CalcMan(vxs, vys, vxf, vyf, i, j);
                                geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, j);
                                dist[i][j] = geodist;
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
        }
        else{
            for (int i = 0; i < V + inst->dummy; i++){
                delta[i] = service;
                if (i < n){ 
                    geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i+n);  
                    // manhattan = CalcMan(vxs, vys, vxf, vyf, i, i+n);      
                    profit[i] = inst->minpas + inst->paskm*geodist;
                }
                else if (i < V - K){ 
                    if (i < 2*n){
                        profit[i] = 0;
                    }
                    else if (i < 2*n + m){
                        geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i+m);
                        // manhattan = CalcMan(vxs, vys, vxf, vyf, i, i+m); 
                        profit[i] =  inst->minpar + inst->parkm*geodist;
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
                                geodist = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, j);
                                // manhattan = CalcMan(vxs, vys, vxf, vyf, i, j);                      
                                dist[i][j] = dist[i][j] = geodist;
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


            // //fixing passenger pu tw (with detours)
            // for (int i = 0; i < n; i++){
            //     vl[i] = ve[i] + 10;//10 minutes of tw
            // }

            // //fixing passenger dl tw
            // for (int i = n; i < 2*n; i++){
            //     double vmed2 = 0.683333;
            //     ve[i] = ve[i-n] + dist[i-n][i]/vmed2 + max(double(5), ((dist[i-n][i]*0.5)/vmed2));
            //     vl[i] = ve[i] + 10;//10 minutes of tw
            // }

            //fixing passenger pu tw (NO detours)
            for (int i = 0; i < n; i++){
                vl[i] = ve[i];//10 minutes of tw
            }

            //fixing passenger dl tw
            for (int i = n; i < 2*n; i++){
                double vmed2 = 0.683333;
                ve[i] = ve[i-n] + dist[i-n][i]/vmed2 + double(5);
                vl[i] = ve[i];
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
            node->e = 0;
            // node->l = 14*60;
            node->l = 24;
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
        // inst->T = nodeVec[V + inst->dummy - 1].l;
        // inst->T = 1440/60;

        inst->totalCustomProfit = 0;

        // if (problem->model == "node"){
        //     for (int i = 0; i < n; i++){
        //         inst->totalCustomProfit += nodeVec[i].profit;
        //     }
        // }


        for (int i = 0; i < n; i++){
            inst->totalCustomProfit += nodeVec[i].profit;
        }

        // cout << "\ntotal profit: " << inst->totalCustomProfit;

        // getchar();

        delete[] profit;
        delete[] delta;
        delete[] slatitude;
        delete[] slongitude;
        delete[] flatitude;
        delete[] flongitude;   
        delete[] trip;
    }
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    else if (inst->instType == "csarp" || inst->instType == "ghsarp"){
        int originalK = 0;
    
        in >> K;
        in >> service;
        in >> n;
        in >> m;
        // K = 6;
        // K = n-1;
        
        if (inst->instType == "ghsarp"){
            originalK = 1;
        }
        else{
            originalK = K;
            // originalK = 1;
        }

        // if (n <= 10){
        //     K = n-1;
        // }
        // else{
        //     K = ceil(0.6*n);
        // }

        // if (trialK >= n){
        //     cout << "Exceeded K size" << endl;
            
        //     exit(1);
        // }

        if (K + trialK <= n - 1){
            K += trialK;
        }
        else{
            cout << "\nExceeded max number of vehicles\n";            
            exit(1);
        }
        
        inst->preInst = 1;

        cout << "\nn: " << n;
        cout << "\nm: " << m;
        cout << "\nK: " << K << endl;
        // getchar();
        service = service/60;

        if (problem->model != "osarp" && problem->model != "fip"){
            V = n + 2*m + K;
        }
        else{
            V = 2*n + 2*m + K;
        }

        originalV = 2*n + 2*m + originalK;

        // inst->vmed = 19.3;
        inst->dummy = K;

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
        if (problem->model != "osarp" && problem->model != "fip"){
            for (int i = 0; i < vxs.size(); i++){
                vxf.push_back(vxs[i]);
                vyf.push_back(vys[i]);

                if (vload[i] < -2.0){
                    vxf[i - n] = vxs[i];
                    vyf[i - n] = vys[i];
                }
            }
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
                vload[i] = 0;
            }            
        }

        else{
            for (int i = 0; i < vxs.size(); i++){
                vxf.push_back(vxs[i]);
                vyf.push_back(vys[i]);
            }

            // for (int i = n; i < 2*n; i++){
                
            //     ve[i] = ve[i-n] + 18 + rand() % 14;//using data from Uber to obtain an average trip duration.

            //     vl[i] = ve[i];
            // }
        }

        if (inst->instType == "ghsarp"){ //multiplying depots
            for (int i = 1; i < K; i++){
                vxs.push_back(vxs[vxs.size()-1]);
                vys.push_back(vys[vys.size()-1]);
                vload.push_back(vload[vload.size()-1]);
                ve.push_back(ve[ve.size()-1]);
                vl.push_back(vl[vl.size()-1]);
                vxf.push_back(vxf[vxf.size()-1]);
                vyf.push_back(vyf[vyf.size()-1]);
            }
        }


        // cout << "Vector of final points:" << endl;

        // for (int i = 0; i < vxs.size(); i++){
        //     cout << i << ": " << vxs[i] << " " << vys[i] << endl;
        // }
        // cout << endl;     

        // getchar();

        // cout << "\nve: " << endl;
        // for (int i = 0; i < ve.size(); i++){
        //     cout << ve[i] << endl;
        // }
        // getchar();

        // Calculate distance matrix (Euclidian)

        double singleProfit;
        double euclidean;
        double scalingfactor = 50; //to scale grubhub (ghsarp) distance values

        if (problem->model != "osarp" && problem->model != "fip"){
            for (int i = 0; i < V + inst->dummy; i++){
                if (i < n){
                    euclidean = calcEucDist(vxs, vys, vxf, vyf, i, i);
                    if (inst->instType == "ghsarp"){
                        euclidean = euclidean/scalingfactor;
                    }
                    delta[i] = (2 * (service)) + (euclidean)/inst->vmed;
                    profit[i] = inst->minpas + inst->paskm*euclidean - inst->costkm*euclidean;
                }
                else if (i < V - K){ 
                    delta[i] = service;
                    if (i < n + m){
                        euclidean = calcEucDist(vxs, vys, vxf, vyf, i, i+m);
                        if (inst->instType == "ghsarp"){
                            euclidean = euclidean/scalingfactor;
                        }
                        profit[i] = inst->minpar + inst->parkm*euclidean;
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
                                euclidean = calcEucDist(vxs, vys, vxf, vyf, i, j);
                                if (inst->instType == "ghsarp"){
                                    euclidean = euclidean/scalingfactor;
                                }                           
                                dist[i][j] = euclidean;

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
        }

        else{
            for (int i = 0; i < V + inst->dummy; i++){
                delta[i] = service;
                if (i < n){
                    euclidean = calcEucDist(vxs, vys, vxf, vyf, i, i+n);
                    
                    if (inst->instType == "ghsarp"){
                        euclidean = euclidean/scalingfactor;
                    }
                    profit[i] = inst->minpas + inst->paskm*euclidean;
                }
                else if (i < V - K){ 
                    if (i < 2*n){
                        profit[i] = 0;
                    }
                    else if (i < 2*n + m){
                        euclidean = calcEucDist(vxs, vys, vxf, vyf, i, i+m);
                        if (inst->instType == "ghsarp"){
                            euclidean = euclidean/scalingfactor;
                        }
                        profit[i] = inst->minpar + inst->parkm*euclidean;
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
                                euclidean = calcEucDist(vxs, vys, vxf, vyf, i, j);
                                if (inst->instType == "ghsarp"){
                                    euclidean = euclidean/scalingfactor;
                                }                           
                                dist[i][j] = euclidean;

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


            // //fixing passenger pu tw
            // for (int i = 0; i < n; i++){
            //     vl[i] = ve[i] + 10;//10 minutes of tw
            // }

            // //fixing passenger dl tw
            // for (int i = n; i < 2*n; i++){
            //     double vmed2 = 0.683333;
            //     ve[i] = ve[i-n] + dist[i-n][i]/vmed2 + max(double(5), ((dist[i-n][i]*0.5)/vmed2));
            //     vl[i] = ve[i] + 10;//10 minutes of tw
            // }

            //fixing passenger pu tw (NO detours)
            for (int i = 0; i < n; i++){
                vl[i] = ve[i];//10 minutes of tw
            }

            //fixing passenger dl tw
            for (int i = n; i < 2*n; i++){
                double vmed2 = 0.683333;
                ve[i] = ve[i-n] + dist[i-n][i]/vmed2 + double(5);
                vl[i] = ve[i];
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
            node->e = 0;
            node->l = 24;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            node->profit = 0;
            node->index = V + i;
            nodeVec.push_back(*node);
        }
        
        // cout << "Earlier // Later: " << endl;

        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": {" << nodeVec[i].e << "}-{" << nodeVec[i].l << "}" << endl;
        // }
        // cout << endl;
        // getchar();

        // cout << "Service times: " << endl;
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].delta << endl;
        // }

        // getchar();

        // cout << "Profits: " << endl;
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].profit << endl;
        // }

        // getchar();

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        // inst->T = nodeVec[V + inst->dummy - 1].l;
        // inst->T = 1020/60;

        inst->totalCustomProfit = 0;

        // if (problem->model == "node"){
        //     for (int i = 0; i < n; i++){
        //         inst->totalCustomProfit += nodeVec[i].profit;
        //     }
        // }

        for (int i = 0; i < n; i++){
            inst->totalCustomProfit += nodeVec[i].profit;
        }
        // cout << "\ntotal profit: " << inst->totalCustomProfit;

        // getchar();

        delete[] profit;
        delete[] delta;
        delete[] trip;

    }

    // cout << "Coordinates: " << endl;
    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << i << ": " << nodeVec[i].xs << " - " << nodeVec[i].ys << endl;
    // }
    // getchar();

    // cout << "\nTime windows: " << endl;

    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << i << ": " << nodeVec[i].e << " - " <<  nodeVec[i].l << endl;
    // }

    // // cout << "\nDist Multiplier: " << trialMulti << endl;
    // getchar();

    // cout << "Service times: " << endl;
    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << i << ": " << nodeVec[i].delta << endl;
    // }

    // getchar();

    // cout << "Profits: " << endl;
    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << i << ": " << nodeVec[i].profit << endl;
    // }

    // // getchar();

    // // cout << "\nEarlier times: " << endl;

    // // for (int i = 0; i < inst->n; i++){
    // //     cout << i << ": " << nodeVec[i].e << endl;
    // // }

    // cout << "\nLoads: " << endl;

    // for (int i = 0; i < nodeVec.size(); i++){
    //     cout << i << ": " << nodeVec[i].load << endl;
    // }

    // cout << "\nDeltas: " << endl;

    // for (int i = 0; i < inst->n; i++){
    //     cout << i << ": " << nodeVec[i].delta << endl;
    // }
}