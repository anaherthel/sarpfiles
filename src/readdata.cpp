#include "readdata.h"
#include "functions.h"
#include "modelnode.h"
#include "modeltwostage.h"
#include <cstdlib>
#include <stdio.h>

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem, int trialK, double trialMulti){
    
    if (argc < 4) {
        // TODO UNCOMMENT //  << "\nMissing parameters\n";
        // TODO UNCOMMENT //  << " ./exeSARP [Instance] [Scenario] [model type]"<< endl;
        exit(1);
    }
    
    if (argc > 4) {
        // TODO UNCOMMENT //  << "\nToo many parameters\n";
        // TODO UNCOMMENT //  << " ./exeSARP [Instance] [Scenario] [model type]" << endl;
        exit(1);
    }  

    // TODO UNCOMMENT //  << "\nNameOfInst: " << argv[1] << endl;
    // TODO UNCOMMENT //  << "RunningScen: " << argv[2] << endl;
    inst->preInst = 0;
    inst->InstName = getInstName(argv);

    cout << "Instance Name: " << inst->InstName << endl;
    // getchar();
    // if (argv[2] == "sim"){
    //     problem->sim = true;
    // }
    // else if (argv[2] == "seq"){
    //     problem->seq = true;
    // }


    //// TODO UNCOMMENT //  << "multiplier: " << multiplier << endl;
    //getchar();
    //double vmed = 41;
    double vmed2 = double(41)/double(60);
    ////vmed2 = timeRound(vmed2);
    // TODO UNCOMMENT //  << "\nvmed: " << std::fixed << std::setprecision(5) << vmed2;
    //getchar();
    problem->scen = argv[2];
    problem->model = argv[3];

    vector <double> passDistances;

    if (problem->scen == "1A"){
        if (problem->model == "math"){
            // TODO UNCOMMENT //  << "\n\nRun this scenario with 'bundle' model.\n";
            // TODO UNCOMMENT //  << " ./exeSARP [Instance] [Scenario] bundle"<< endl;
            exit(1);
        }
        else{
            problem->p1 = 0; //1 is multi, 0 is single; p1 refers to customer
            problem->p2 = 0; //p2 refers to parcel
            problem->p3 = 0; //p3 refers to baseline scenarios
            problem->dParcel = 0;//1 allows for direct parcel delivery
        }
    }
    else if (problem->scen == "1AD"){//1A with direct parcel delivery

        problem->p1 = 0; //1 is multi, 0 is single; p1 refers to customer
        problem->p2 = 0; //p2 refers to parcel
        problem->p3 = 0;
        problem->dParcel = 1;//1 allows for direct parcel delivery
        
    }
    else if (problem->scen == "2A"){
        problem->p1 = 1; //1 is multi, 0 is single; p1 refers to customer
        problem->p2 = 0; //p2 refers to parcel; -1: No P-d arcs
        problem->p3 = 0;
        problem->dParcel = 0;//1 allows for direct parcel delivery
    }
    else if (problem->scen == "1B"){
        problem->p1 = 0;
        problem->p2 = 1;
        problem->p3 = 0;
        problem->dParcel = 0;
    }
    else if (problem->scen == "2B"){
        problem->p1 = 1;
        problem->p2 = 1;
        problem->p3 = 0;
        problem->dParcel = 0;        
    }
    else if (problem->scen == "2MM"){
        problem->p1 = 1;
        problem->p2 = 1;
        problem->p3 = 0;
        problem->dParcel = 1;        
    }
    else if (problem->scen == "PC" ){ //PC: dedicated vehicles for each service (basically, passenger only);
        problem->p1 = -1;
        problem->p2 = -1;
        problem->p3 = -1;
        problem->dParcel = 1;
    }   
    else if (problem->scen == "BL2" ){ //BL2: same car for both services, no shared trips
        problem->p1 = 1;
        problem->p2 = 1;
        problem->p3 = 1;
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
        // TODO UNCOMMENT //  << "the file could not be opened\n";
        exit (1);
    }

    inst->instType = getInstanceType(argv);
    // TODO UNCOMMENT //  << "\nInstance type: " << inst->instType << endl;
    
    //else if (inst->instType == "csarp" || inst->instType == "ghsarp"){
    int originalK = 0;

    in >> K;
    in >> service;
    in >> n;
    in >> m;
    //if (n > 40){
    //    K = floor((n+m)*0.1);
    //}

    
    if (inst->instType == "ghsarp" || inst->instType == "sf_data"){
        originalK = 1;
    }
    else{
        originalK = K;
        // originalK = 1;
    }

    //&&&&&&&&&&&&&&&& Uncomment to scale K &&&&&&&&&&&&&&&&
    ////if (n <= 10){
    ////    K = n-1;
    ////}
    ////else{
    ////    K = ceil(0.6*n);
    ////}

    //if (trialK <= K){
    //    K = trialK;
    //}
    //else{
    //    trialK = K;
    //}

    //if (trialK >= n){
    //    cout << "Exceeded K size" << endl;
    
    //    exit(1);
    //}
    ////if (n > 40){
    ////    if (K + trialK <= n - 1){
    ////        K += trialK;
    ////    }
    ////    else{
    ////        cout << "\nExceeded max number of vehicles\n";            
    ////        exit(1);
    ////    }
    ////}
    //// getchar();
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

    inst->preInst = 1;

    // TODO UNCOMMENT //  << "\nn: " << n;
    // TODO UNCOMMENT //  << "\nm: " << m;
    cout << "\nK: " << K << endl;
    // getchar();
    service = service/60;
    //service = valRound(service);
    //service = 0.1;
    //service = 0.083;

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
    if (problem->model != "osarp" && problem->model != "fip"){
        for (int i = n + 2*m; i < vl.size(); i++){
            //vl[i] = 1440;
            vl[i] = 1140;
            //vl[i] = 750;
            //vl[i] = 480;
        }
    }

    if (inst->instType == "ghsarp" || inst->instType == "sf_data"){ //multiplying depots
        //if (problem->model == "fip"){ 
        //    for (int i = n; i < 2*n; i++){
        //        vl[i] = 1140;
        //    }
        //}
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

    // Calculate distance matrix (Euclidian)

    //cout << "Coordinates: " << endl;
    //for (int i = 0; i < vxs.size(); i++){
    //    cout << i << ": " << vxs[i] << " " << vys[i] << " --- " << vxf[i] << " " << vyf[i] << endl;
    //}
    //getchar();

    //cout << "some distances: " << endl;
    //cout << "Dist of end of 0 to start of 1: " << CalcManKm(vxs, vys, vxf, vyf, 0, 1) << endl;

    double singleProfit;
    double euclidean;
    double calcdist;
    double scalingfactor = 50; //to scale grubhub (ghsarp) distance values
    //double scalingfactor = 35; //to scale grubhub (ghsarp) distance values

    if (problem->model != "osarp" && problem->model != "fip"){
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                    calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, i);
                    if (inst->instType == "ghsarp"){
                        calcdist = calcdist/scalingfactor;
                        //euclidean = valRound(euclidean);
                    }
                }
                else if (inst->instType == "sf_data"){
                    calcdist = CalcManKm(vxs, vys, vxf, vyf, i, i); 
                }

                delta[i] = (2 * (service)) + (calcdist)/inst->vmed;
                profit[i] = inst->minpas + inst->paskm*calcdist;
                profit[i] =  profit[i]- inst->costkm*calcdist;
            }
            else if (i < V - K){ 
                delta[i] = service;
                if (i < n + m){
                    if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                        calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, i+m);
                        if (inst->instType == "ghsarp"){
                            calcdist = calcdist/scalingfactor;
                        }
                    }
                    else if (inst->instType == "sf_data"){
                        calcdist = CalcManKm(vxs, vys, vxf, vyf, i, i+m); 
                    }
                    profit[i] = inst->minpar + inst->parkm*calcdist;
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
                            if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                                calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, j);
                                if (inst->instType == "ghsarp"){
                                    calcdist = calcdist/scalingfactor;
                                }
                            }
                            else if (inst->instType == "sf_data"){
                                calcdist = CalcManKm(vxs, vys, vxf, vyf, i, j); 
                            }                                
                            dist[i][j] = calcdist;
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
                if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                    calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, i+n);
                    if (inst->instType == "ghsarp"){
                        calcdist = calcdist/scalingfactor;
                    }
                }
                else if (inst->instType == "sf_data"){
                    calcdist = CalcManKm(vxs, vys, vxf, vyf, i, i+n); 
                }
                passDistances.push_back(calcdist);
                profit[i] = inst->minpas + inst->paskm*calcdist;
            }
            else if (i < V - K){ 
                if (i < 2*n){
                    profit[i] = 0;
                }
                else if (i < 2*n + m){
                    if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                        calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, i+m);
                        if (inst->instType == "ghsarp"){
                            calcdist = calcdist/scalingfactor;
                        }
                    }
                    else if (inst->instType == "sf_data"){
                        calcdist = CalcManKm(vxs, vys, vxf, vyf, i, i+m); 
                    }
                    profit[i] = inst->minpar + inst->parkm*calcdist;
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
                            if (inst->instType == "ghsarp" || inst->instType == "csarp"){
                                calcdist = calcEucDist2(vxs, vys, vxf, vyf, i, j);
                                if (inst->instType == "ghsarp"){
                                    calcdist = calcdist/scalingfactor;
                                }
                            }
                            else if (inst->instType == "sf_data"){
                                calcdist = CalcManKm(vxs, vys, vxf, vyf, i, j); 
                            }
                            dist[i][j] = calcdist;
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

            //fixing passenger dl tw
        for (int i = n; i < 2*n; i++){
            double vmed2 = 0.683333;
            ve[i] = ve[i-n] + dist[i-n][i]/vmed2 + max(double(5), ((dist[i-n][i]*0.5)/vmed2));
            //vl[i] = ve[i] + 10;//10 minutes of tw
            vl[i] = ve[i] + 10;
            //vl[i] = 1140;
        }

        //fixing passenger pu tw (NO detours)
        for (int i = 0; i < n; i++){
            vl[i] = ve[i];
        }

    }        

    for (int i = 0; i < V; i++){
        node->xs = vxs[i];
        node->ys = vys[i];
        node->load = vload[i];

        if (i < n){
            node->load2 = -1;
        }
        else if (i < n + m){
            node->load2 = 1;
        }
        else{
            node->load2 = 0;
        }    
                    
        node->e = ve[i]/60;
        //node->e =  timeRound(node->e);

        //node->e = static_cast<int>(node->e * multiplier) / multiplier;

        node->l = vl[i]/60;
        //node->l = timeRound(node->l); 
        //node->l = static_cast<int>(node->l * multiplier) / multiplier;

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
        //node->e = 0;
        //node->l = 24;
        node->e = 9;
        node->l = 19;           
        node->xf = 0;
        node->yf = 0;
        node->delta = 0;
        node->profit = 0;
        node->index = V + i;
        nodeVec.push_back(*node);
    }

    
    // TODO UNCOMMENT 
    //cout << "Earlier // Later: " << endl;

    //for (int i = 0; i < nodeVec.size(); i++){
    ////    // TODO UNCOMMENT // 
    //    cout << i << ": {" << nodeVec[i].e << "}-{" << nodeVec[i].l << "}" << endl;
    //}
    ////// TODO UNCOMMENT // 
    //cout << endl;
    //getchar();

    //// TODO UNCOMMENT //  << "Service times: " << endl;
    //for (int i = 0; i < nodeVec.size(); i++){
    //    // TODO UNCOMMENT //  << i << ": " << nodeVec[i].delta << endl;
    //}

    // getchar();

    //// TODO UNCOMMENT //  << "Profits: " << endl;
    //for (int i = 0; i < nodeVec.size(); i++){
    //    // TODO UNCOMMENT //  << i << ": " << nodeVec[i].profit << endl;
    //}

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
    // // TODO UNCOMMENT //  << "\ntotal profit: " << inst->totalCustomProfit;

    // getchar();

    delete[] profit;
    delete[] delta;
    delete[] trip;

    //}

    // // TODO UNCOMMENT //  << "Coordinates: " << endl;
    // for (int i = 0; i < nodeVec.size(); i++){
    //     // TODO UNCOMMENT //  << i << ": " << nodeVec[i].xs << " - " << nodeVec[i].ys << endl;
    // }
    // getchar();

    //cout << "\nTime windows: " << endl;

    //for (int i = 0; i < nodeVec.size(); i++){
    //    cout << i << ": " << nodeVec[i].e << " - " <<  nodeVec[i].l << endl;
    //}
    
    //cout << "\nProfits: " << endl;
    //for (int i = 0; i < nodeVec.size(); i++){
    //    cout << i << ": " << nodeVec[i].profit << endl;
    //}

    vector < pair <int, double> > orderedPassengers;
    pair <int, double> p;

    for (int i = 0; i < inst->n; i++){
        p.first = i;
        p.second = nodeVec[i].e;
        orderedPassengers.push_back(p);
    } 

    std::sort(orderedPassengers.begin(), orderedPassengers.end(), [](const std::pair<int, double>& a, const std::pair<int, double>& b) {
        return a.second < b.second;
    });

    //cout << "Ordered passengers: " << endl;
    //for (int i = 0; i < orderedPassengers.size(); i++){
    //    cout << orderedPassengers[i].first << " - " << orderedPassengers[i].second << endl;
    //}
    //getchar();
    // // // TODO UNCOMMENT //  << "\nDist Multiplier: " << trialMulti << endl;
    // getchar();

    //cout  << "Service times: " << endl;
    //for (int i = 0; i < nodeVec.size(); i++){
    //    cout  << i << ": " << nodeVec[i].delta << endl;
    //}

    //cout << "Check feasibility of passengers: " << endl;
    //for (int i = 0; i < inst->n; i++){
    //    int j = i + inst->n;
    //    double diff = nodeVec[j].e - nodeVec[i].l;
    //    double distance = passDistances[i];
    //    double traveltime = distance/inst->vmed;

    //    cout << i << " to "  << j << " // travel time: " << traveltime << " // diff: " << diff << " possible? " << (diff >= traveltime) << endl;
    //}

    //getchar();

    ////////passDistances.clear();
    ////////// TODO UNCOMMENT //  
    //cout << "Profits: " << endl;
    //for (int i = 0; i < nodeVec.size(); i++){
    //    cout  << i << ": " << nodeVec[i].profit << endl;
    //}

    ////////// // getchar();

    ////////// TODO UNCOMMENT //  << "\nEarlier times: " << endl;
    //cout << "\nEarlier times: " << endl;
    //for (int i = 0; i < inst->n; i++){
    //    // TODO UNCOMMENT 
    //    cout << i << ": " << nodeVec[i].e << endl;
    //}

    ////////// // TODO UNCOMMENT //  << "\nLoads: " << endl;

    ////////// for (int i = 0; i < nodeVec.size(); i++){
    //////////     // TODO UNCOMMENT //  << i << ": " << nodeVec[i].load << endl;
    ////////// }

    //////// TODO UNCOMMENT //  
    //cout << "\nDeltas: " << endl;

    //for (int i = 0; i < inst->n; i++){
    //    cout << i << ": " << nodeVec[i].delta << endl;
    //}
}