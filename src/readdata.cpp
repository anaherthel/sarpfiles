#include "readdata.h"
#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void readData (int argc, char** argv, nodeStat *node, instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist, probStat* problem, vector<double> &passProfit){
    
    if (argc < 3) {
        cout << "\nMissing parameters\n";
        // cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]"<< endl;
        cout << " ./exeSARP [Instance] [Scenario]"<< endl;
        exit(1);
    }
    
    if (argc > 3) {
        cout << "\nToo many parameters\n";
        cout << " ./exeSARP [Instance] [Scenario]" << endl;
        exit(1);
    }  

    // if (argv[2] == "sim"){
    //     problem->sim = true;
    // }
    // else if (argv[2] == "seq"){
    //     problem->seq = true;
    // }

    problem->scen = argv[2];

    string file, ewf;
    int n;
    int m;
    int K;
    double service;
    double T;
    int V;
    int originalV;
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
        inst->vmed = 19.3;
        service = service/60;
        inst->service = service;
        V = n + 2*m + K;
        // inst->dummy = K;
        inst->dummy = 1;

        double *xs = new double[V + inst->dummy];
        double *ys = new double[V + inst->dummy];
        char *label = new char[V + inst->dummy];
        double *load = new double[V + inst->dummy];
        double *e = new double[V + inst->dummy];
        double *l = new double[V + inst->dummy];
        double *xf = new double[V + inst->dummy];
        double *yf = new double[V + inst->dummy];
        double *delta = new double[V + inst->dummy];
        double *profit = new double[V+inst->dummy];

        double **dist = new double*[V + inst->dummy];
        for (int i= 0; i < V + inst->dummy; i++){
            dist[i] = new double [V + inst->dummy];
        }

        int tempNode;
        // for (int i = 0; i < V; i++){
        //     nodeVec.push_back(*node);
        // }

        for (int i = 0; i < V; i++){
            in >> tempNode >> xs[i] >> ys[i] >> label[i] >> load[i] >> e[i] >> l[i] >> xf[i] >> yf[i];
        }

        // Calculate distance matrix (Euclidian)
        double singleProfit;
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
                // cout << "\nDist " << i << " :" << floor(calcEucDist(xs, ys, xf, yf, i, i) + 0.5); 
                delta[i] = (2 * service) + (floor(calcEucDist(xs, ys, xf, yf, i, i) + 0.5))/inst->vmed;
                profit[i] = inst->gamma2 + inst->mu2*floor(calcEucDist(xs, ys, xf, yf, i, i) + 0.5) - floor(calcEucDist(xs, ys, xf, yf, i, i) + 0.5);
               // cout << "delta " << i << ": " << delta[i] << endl;
               // cout << "profit " << i << ": " << profit[i] << endl;
            }
            else if (i < V - K){ 
                delta[i] = service;
                profit[i] = 0;
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
            node->profit = profit[i];
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
            node->profit = 0;
            nodeVec.push_back(*node);
        }

        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->T = T/60;
        inst->V = V;
        inst->service = service;
        // cout << "\nNode vec: ";
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << nodeVec[i].label << " ";
        // }
        // cout << endl;

        delete[] profit;
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

        string filename(argv[1]);
        string InstanceName;

        string::size_type loc = filename.find_last_of("r", filename.size());
        string::size_type loc2 = filename.find_last_of(".", filename.size());
       
        InstanceName.append(filename, loc+1, loc2-loc-1);
        std::replace(InstanceName.begin(), InstanceName.end(), '_', ' ');
        vector<int> instanceData;
        stringstream ss(InstanceName);
        int temp;
        while (ss >> temp){
            instanceData.push_back(temp);
        }

        K = instanceData[0];
        n = instanceData[1]/2;
        m = instanceData[2]/2;
        service = 5;
        service = service/60;
        V = n + 2*m + K;
        originalV = 2*n + 2*m + 2*K;

        inst->vmed = 19.3;
        inst->dummy = 1;


        double *delta = new double[V + inst->dummy];
        double *slatitude = new double [V + inst->dummy];
        double *slongitude = new double [V + inst->dummy];
        double *flatitude = new double [V + inst->dummy];
        double *flongitude = new double [V + inst->dummy];
        double *profit = new double[V+inst->dummy];


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
        }

        for (int i = 0; i < originalV; i++){
            in >> vxs[i] >> vys[i] >> vload[i] >> ve[i];
        }

        ve[ve.size()-1] = 0;

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

        for (int i = 0; i < n; i++){
            vxs.erase(vxs.begin() + n + m);
            vys.erase(vys.begin() + n + m);
            vload.erase(vload.begin() + n + m);
            ve.erase(ve.begin() + n + m);
            vxf.erase(vxf.begin() + n + m);
            vyf.erase(vyf.begin() + n + m);
        }

        for (int i = 0; i < ve.size(); i++){
            vl.push_back(ve[i]);
        }

        for (int i = n; i < vl.size(); i++){
            vl[i] = 14*60;
        }

        // cout << "vl: " << endl;
        // for(int i = 0; i < vl.size(); i++){
        //     cout << vl[i] << " ";
        // }

        CalcLatLong ( vxs, vys, vxf, vyf, V, slatitude, slongitude, flatitude, flongitude );
        
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
               delta[i] = (2 * (service)) + (CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i))/inst->vmed;
               profit[i] = inst->gamma2 + inst->mu2*CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i) - CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i);

               // cout << "delta " << i << ": " << delta[i] << endl;
                // delta[i] = (2 * (service/60)) + (CalcMan(vxs, vys, vxf, vys, i, i))/inst->vmed;

            }
            else if (i < V - K){ 
               delta[i] = service;
               profit[i] = 0;
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
                            dist[i][j] = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, j);
                            // dist[i][j] = CalcMan(vxs, vys, vxf, vys, i, i);
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


        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;
        
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
            nodeVec.push_back(*node);
        }

        for (int i = 0; i < inst->dummy; i++){
            node->xs = 0;
            node->ys = 0;
            node->load = 0;
            node->e = 0;
            node->l = 14*60;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            node->profit = 0;
            nodeVec.push_back(*node);
        }

        delete[] profit;
        delete[] delta;
        delete[] slatitude;
        delete[] slongitude;
        delete[] flatitude;
        delete[] flongitude;
    }
 
    else if (instType == "sf_data"){

        in >> K;
        in >> service;
        in >> n;
        in >> m;

        K = 9;

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

        ve[ve.size()-1] = 0;

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

        for (int i = 0; i < n; i++){
            vxs.erase(vxs.begin() + n + m);
            vys.erase(vys.begin() + n + m);
            vload.erase(vload.begin() + n + m);
            ve.erase(ve.begin() + n + m);
            vxf.erase(vxf.begin() + n + m);
            vyf.erase(vyf.begin() + n + m);
        }

        for (int i = 0; i < n; i++){
            vl[i] = ve[i];
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
        // Calculate distance matrix (Geolocation)

        CalcLatLong ( vxs, vys, vxf, vyf, V, slatitude, slongitude, flatitude, flongitude );
        
        double singleProfit;
        for (int i = 0; i < V + inst->dummy; i++){
            if (i < n){ 
                delta[i] = (2 * (service)) + (CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i))/inst->vmed;
                profit[i] = inst->gamma2 + inst->mu2*CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i) - CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, i);
            }
            else if (i < V - K){ 
               delta[i] = service;
               profit[i] = 0;
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
                            dist[i][j] = CalcDistGeo(slatitude, slongitude, flatitude, flongitude, i, j);
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


        *Mdist = dist;
        inst->K = K;
        inst->n = n;
        inst->m = m;
        inst->V = V;
        inst->service = service;

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
            nodeVec.push_back(*node);
        }

        //Adding dummy nodes
        for (int i = 0; i < inst->dummy; i++){
            node->xs = 0;
            node->ys = 0;
            node->load = 0;
            node->e = 0;
            node->l = 14*60;
            node->xf = 0;
            node->yf = 0;
            node->delta = 0;
            node->profit = 0;
            nodeVec.push_back(*node);
        }

        delete[] profit;
        delete[] delta;
        delete[] slatitude;
        delete[] slongitude;
        delete[] flatitude;
        delete[] flongitude;
   
    }

    else if (instType == "grubhub"){

        K = 1;
        bool increaseK = true;
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

        m = floor(instV * 0.3);
        
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

        if (increaseK == true){
            K = 2;

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

        //calculate deltas
        for(int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                // cout << i << ": " << (tempData[2*i][2*i+1]);
                delta[i] = 2 * service + (((tempData[2*i][2*i+1])/1000)/inst->vmed);
                // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
                profit[i] = inst->gamma2 + inst->mu2*(tempData[2*i][2*i+1]/1000) - (tempData[2*i][2*i+1]/1000);    
            }
            else if (i < V - K){
                delta[i] = service;
                profit[i] = 0;
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
            }
        }

        for (int i = 0; i < V + inst->dummy; i++){
            if(i < n){
                e[i] = 540 + rand() % 480;
                l[i] = e[i];
            }
            else if (i < V + inst->dummy - 1){
                e[i] = 540;
                l[i] = 1020;
            }
            else{
                e[i] = 0;
                l[i] = 1440;
            }
        }
        // cout << "Earlier times: " << endl;
        // for(int i = 0; i < V + inst->dummy; i++){
        //     cout << e[i] << " ";
        // }
        // cout << endl;
        // getchar();

        // cout << "Deltas: " << endl;
        // for(int i = 0; i < V + inst->dummy; i++){
        //     cout << delta[i] << " ";
        // }
        // cout << endl;
        // getchar();

        for (int i = 0; i < V + inst->dummy; i++){
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->delta = delta[i];
            node->profit = profit[i];
            nodeVec.push_back(*node);
        }

        // cout << "\nNode Vec:" << endl;
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].e << " // ";
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

        delete[] profit;
        delete[] delta;
    }

    else if (instType == "grubhub_1"){

        K = 1;
        bool increaseK = false;
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

        m = floor(instV * 0.3);
        
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

        if (increaseK == true){
            K = 2;

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

        //calculate deltas
        for(int i = 0; i < V + inst->dummy; i++){
            if (i < n){
                // cout << i << ": " << (tempData[2*i][2*i+1]);
                delta[i] = 2 * service + (((tempData[2*i][2*i+1])/1000)/inst->vmed);
                // cout << "i: " << i << " - " << ((tempData[2*i][2*i+1])/1000)/inst->vmed << endl;
                profit[i] = inst->gamma2 + inst->mu2*(tempData[2*i][2*i+1]/1000) - (tempData[2*i][2*i+1]/1000);    
            }
            else if (i < V - K){
                delta[i] = service;
                profit[i] = 0;
            }
            else if (i >= V - K){
                delta[i] = 0;
                profit[i] = 0;
            }
        }

        for (int i = 0; i < V + inst->dummy; i++){
            if(i < n){
                e[i] = 540 + rand() % 480;
                l[i] = e[i];
            }
            else if (i < V + inst->dummy - 1){
                e[i] = 540;
                l[i] = 1020;
            }
            else{
                e[i] = 0;
                l[i] = 1440;
            }
        }
        // cout << "Earlier times: " << endl;
        // for(int i = 0; i < V + inst->dummy; i++){
        //     cout << e[i] << " ";
        // }
        // cout << endl;
        // getchar();

        // cout << "Deltas: " << endl;
        // for(int i = 0; i < V + inst->dummy; i++){
        //     cout << delta[i] << " ";
        // }
        // cout << endl;
        // getchar();

        for (int i = 0; i < V + inst->dummy; i++){
            node->e = e[i]/60;
            node->l = l[i]/60;
            node->delta = delta[i];
            node->profit = profit[i];
            nodeVec.push_back(*node);
        }

        // cout << "\nNode Vec:" << endl;
        // for (int i = 0; i < nodeVec.size(); i++){
        //     cout << i << ": " << nodeVec[i].e << " // ";
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

        delete[] profit;
        delete[] delta;
    }

    if(problem->scen == "1A" || "1B"){
        inst->nCluster = inst->n + inst->K + inst->dummy;
    }
    else if(problem->scen == "2A" || "2B"){
        inst->nCluster = inst->n + inst->K + inst->dummy;
    }


}

double calcEucDist (double *Xs, double *Ys, double *Xf, double *Yf, int I, int J){
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