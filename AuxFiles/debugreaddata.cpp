   if (inst->instType == "debug"){

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
        
        // cout << "K: " << K << endl;
        // getchar();

        service = service/60;
        V = n + 2*m + K;

        originalV = 2*n + 2*m + 2; 
        // inst->vmed = 19.3;
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

        // cout << "Vector of vxf:" << endl;

        // for (int i = 0; i < vxf.size(); i++){
        //     cout << i << ": " << vxf[i] << endl;
        // }
        // cout << endl;

        // getchar();

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
                delta[i] = (2 * (service)) + (calcEucDist(vxs, vys, vxf, vyf, i, i))/inst->vmed;

                trip[i] = calcEucDist(vxs, vys, vxf, vyf, i, i);
                // cout << "Trip:" << endl;
                // cout << i << ": " << trip[i] << endl;
                profit[i] = inst->minpas + inst->paskm*calcEucDist(vxs, vys, vxf, vyf, i, i) - inst->costkm*calcEucDist(vxs, vys, vxf, vyf, i, i);
                // cout << "Profit: " << endl;
                // cout << i << ": " << profit[i] << endl;
            }
            else if (i < V - K){ 
                delta[i] = service;
                if (i < n + m){
                    profit[i] = inst->minpar + inst->parkm*calcEucDist(vxs, vys, vxf, vyf, i, i+m);
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
        // getchar();
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