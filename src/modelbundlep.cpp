#include "modelbundlep.h"
#include <cstdlib>
#include <stdio.h>

void makeBundlesP (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, vector< vector<bParcelStruct> > &clsParcel, probStat* problem){

    int counter = 0;
    for (int i = 0; i < inst->n; i++){
        bStat->bundle.push_back(i);
        bStat->bundleVec.push_back(bStat->bundle);
        // cStat->clusters.push_back(bStat->bundle);
        cStat->clusters.push_back(bStat->bundleVec.size()-1);
        bStat->bundle.clear();
        for (int j = 0; j < clsParcel[i].size(); j++){
            double bundledelta = nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta + nodeVec[clsParcel[i][j].parcelreq + inst->m].delta;

            //Bundles of P-D-d
            bStat->bundle.push_back(clsParcel[i][j].parcelreq);//P
            bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);//D
            bStat->bundle.push_back(i);//d


            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();


            //Bundles of d-P-D
            bStat->bundle.push_back(i);//d
            bStat->bundle.push_back(clsParcel[i][j].parcelreq);//P
            bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);//D

            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();

            bundledelta = nodeVec[clsParcel[i][j].parcelreq].delta + nodeVec[i].delta;

            //Bundles of d-P
            bStat->bundle.push_back(i);
            bStat->bundle.push_back(clsParcel[i][j].parcelreq);

            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();

            //Bundles of P-d
            bStat->bundle.push_back(clsParcel[i][j].parcelreq);
            bStat->bundle.push_back(i);

            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();

            bundledelta = nodeVec[i].delta + nodeVec[clsParcel[i][j].parcelreq + inst->m].delta;

            //Bundles of d-D
            bStat->bundle.push_back(i);
            bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);

            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();
            
            //Bundles of D-d
            bStat->bundle.push_back(clsParcel[i][j].parcelreq + inst->m);
            bStat->bundle.push_back(i);

            bStat->bundleTimes.push_back(bundledelta);
            bStat->bundleVec.push_back(bStat->bundle);
            cStat->clusters.push_back(bStat->bundleVec.size()-1);
            bStat->bundle.clear();


        }
        cStat->clusterVec.push_back(cStat->clusters);
        cStat->clusters.clear();
    }

    for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){ //start and end bundles.
        
        bStat->bundle.push_back(i);
        bStat->bundleVec.push_back(bStat->bundle);
        // cStat->clusters.push_back(bStat->bundle);
        cStat->clusters.push_back(bStat->bundleVec.size()-1);
        bStat->bundle.clear();
        
        cStat->clusterVec.push_back(cStat->clusters);
        cStat->clusters.clear();
    }

}

//void makeBundlesPartial (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, 
//                        clSt *cStat, vector< vector<bParcelStruct> > &clsParcel,
//                        probStat* problem, vector< vector<int> > &fullsorted){

//    int sizeofvec = ceil(inst->m*0.2);
//    cout << "Size of vec: " << sizeofvec << endl;
 
//    for (int i = 0; i < inst->n; i++){
//        bPStat->bundle.push_back(i);
//        bPStat->bundleVec.push_back(bPStat->bundle);
//        // cPStat->clusters.push_back(bPStat->bundle);
//        cPStat->clusters.push_back(bPStat->bundleVec.size()-1);
//        bPStat->bundle.clear();
//        for (int j = 0; j < sizeofvec; j++){
//            int currentBundle = fullsorted[i][j];
//            cout << "Current bundle: " << currentBundle << endl;
            
//            //copying bundles to the partial bundle structure

//            for (int k = 0; k < bStat->bundleVec[currentBundle].size(); k++){
//                bPStat->bundle.push_back(bStat->bundleVec[currentBundle][k]);

//            }
//            //Updating the bundle times, bundle profits, bundle service, bundle vector, and the cluster vector
//            bPStat->bundleTimes.push_back(bStat->bundleTimes[currentBundle]);
//            bPStat->bundleProfVec.push_back(bStat->bundleProfVec[currentBundle]);
//            bPStat->bundleServVec.push_back(bStat->bundleServVec[currentBundle]);
//            bPStat->bundleVec.push_back(bPStat->bundle);
//            cPStat->clusters.push_back(bPStat->bundleVec.size()-1);
//            bPStat->bundle.clear();




//        }
//        cPStat->clusterVec.push_back(cPStat->clusters);
//        cPStat->clusters.clear();
//    }



//    for (int i = 2*inst->m + inst->n; i < nodeVec.size(); i++){ //start and end bundles.
//        cout <<"Value of i: " << i << endl;

//        bPStat->bundle.push_back(i);
//        bPStat->bundleVec.push_back(bPStat->bundle);
//        // cPStat->clusters.push_back(bPStat->bundle);
//        cPStat->clusters.push_back(bPStat->bundleVec.size()-1);
//        bPStat->bundle.clear();
        
//        cPStat->clusterVec.push_back(cPStat->clusters);
//        cPStat->clusters.clear();
//    }
//}


void orderBundles(instanceStat *inst, bundleStat *bStat, clSt *cStat){
    //based on a bundles profit, order the bundles in decreasing order of profit.
    vector<double> sortedProf;

    //int refnumber = inst->m*3 + 1;
    int refnumber = inst->m*6 + 1;


    int counter = 0;
    for (int i = 0; i < inst->n; i++){
        //for(int k = 0; k < cStat->clusterVec[i].size() - 2*inst->K; k++){
            //bStat->bundleVec[cStat->clusterVec[i][k]];
            //for (int j = 0; j < bStat->bundleVec[cStat->clusterVec[i][k]].size(); j++){
            //bStat->bundleVec[cStat->clusterVec[i][k]];
            vector< pair<double, int> > paired;  // To store pairs

            // Pair each value with its corresponding vector
            for (int j = counter; j < counter + (refnumber); ++j) {
                if (j == (i)*refnumber){ // do not consider pass only bundles
                    continue;
                }
                paired.push_back(std::make_pair(bStat->bundleProfVec[j], j));

            }


            // Sort the paired vector based on the double values
            std::sort(paired.begin(), paired.end(), [](const std::pair<double, int>& a, const std::pair<double, int>& b) {
                return a.first > b.first;
            });        

            bStat->sortedBundles.push_back((i)*refnumber);//adding pass only bundle to the sorted bundles
            sortedProf.push_back(bStat->bundleProfVec[(i)*refnumber]);//adding pass only profit to the sorted profits

            // Extract the sorted vectors
            for (int j = 0; j < paired.size(); ++j) {                
                //bStat->bundleVec[cStat->clusterVec[i][j]]
                bStat->sortedBundles.push_back(paired[j].second);
                sortedProf.push_back(paired[j].first);
                //bStat->bundleVec[cStat->clusterVec[i][j]] = paired[j].second;
                //bStat->bundleProfVec[cStat->clusterVec[i][j]] = paired[j].first; // Optional: If you also want to sort the values array
            }
            //}
            paired.clear();
        //}
        
        //cout << "Sorted bundles:" << endl;

        //for (int j = 0; j < sortedBundles.size(); j++){
        //    cout << sortedBundles[j] << " - " << sortedProf[j] << endl;

        //}
        bStat->fullsorted.push_back(bStat->sortedBundles);
        bStat->sortedBundles.clear();
        sortedProf.clear();

        //cout << "End of loop" << endl;

        //cout << "\nCluster " << i << ": [";  
        //for(int k = 0; k < cStat->clusterVec[i].size(); k++){
        //    cout << "(" << cStat->clusterVec[i][k] << ") " << "[";
        //    for (int j = 0; j < bStat->bundleVec[cStat->clusterVec[i][k]].size(); j++){
        //        cout << setw(3) << std:: right << bStat->bundleVec[cStat->clusterVec[i][k]][j];
        //        if (j < bStat->bundleVec[cStat->clusterVec[i][k]].size() - 1){
        //            cout << ",";
        //        }
        //        else{
        //            cout << "] ";
                    
        //        }
        //    }
        //}
        //cout << "]" << endl;
        counter = (refnumber)*(i + 1);

    }
    //Print the sorted bundles
    //for (int i = 0; i < bStat->fullsorted.size(); i++){
    //    cout << "Cluster " << i << ": ";
    //    for (int j = 0; j < bStat->fullsorted[i].size(); j++){
    //        cout << bStat->fullsorted[i][j] << " ";
    //    }
    //    cout << endl;
    //}
}

void makeParcelBundlesP(instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){
    int parcelReq;
    bool foundparc;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        if (bStat->bundleVec[i].size() < 2){//single passenger; depot
            continue;
        }            
        for (int j = 0; j < bStat->bundleVec[i].size(); j++){
            foundparc = false;

            if (bStat->bundleVec[i][j] >= inst->n && bStat->bundleVec[i][j] < inst->n + inst->m){
                parcelReq = bStat->bundleVec[i][j];
                bStat->parcelBundleVec[parcelReq - inst->n].push_back(i);
                foundparc = true;
            }
            if (foundparc) {
                break;
            }
        }
    } 
}

void bundleProfitP(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){
    double cost;
    double service;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        cost = 0;
        service = 0;
        if (bStat->bundleVec[i].size() < 2){//bundles of single requests and for parcel direct delivery
            cost = nodeVec[bStat->bundleVec[i][0]].profit;
            bStat->bundleProfVec.push_back(cost);
            service = nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleServVec.push_back(service);
        }
        else{//bundles of size 2 or larger
            if (bStat->bundleVec[i].back() == nodeVec.size() - 1){//if it is a dummy node
                cost = nodeVec[bStat->bundleVec[i][0]].profit;
                bStat->bundleProfVec.push_back(cost);
                service = nodeVec[bStat->bundleVec[i][0]].delta;
                bStat->bundleServVec.push_back(service); 
            }
            else{
                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    cost += nodeVec[bStat->bundleVec[i][j]].profit;
                    cost += - (inst->costkm*mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]);
                    service += nodeVec[bStat->bundleVec[i][j]].delta + (mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]]/inst->vmed);           
                }
                cost += nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].profit;
                service += nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].delta;
                // cout << bStat->bundleVec[i][bStat->bundleVec[i].size() - 1] << " - delta: " << nodeVec[bStat->bundleVec[i][bStat->bundleVec[i].size() - 1]].delta;
                // getchar();
                // cout << "bundle: " << i << " - service(final): " << service << endl;
                // getchar(); 
                bStat->bundleProfVec.push_back(cost);
                bStat->bundleServVec.push_back(service);                
            }
        }
    } 
}

void makeStartTimesP (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, probStat* problem){

    double parcelTime;
    double bundleTime;
    double endTime;
    bool foundpass;//found passenger
    double tij;

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        parcelTime = 0;
        bundleTime = 0;
        endTime = 0;
        foundpass = false;

        if (bStat->bundleVec[i].size() > 1){
            if (bStat->bundleVec[i][0] < inst->n){//if the 1st element of the bundle is a passenger node(d-P-D, d-P or d-D)
                foundpass = true;
                bundleTime = nodeVec[bStat->bundleVec[i][0]].e;

                tij = ((mdist[bStat->bundleVec[i][0]][bStat->bundleVec[i][1]])/inst->vmed);
                endTime = bundleTime + nodeVec[bStat->bundleVec[i][0]].delta + tij;

                if (bStat->bundleVec[i].size() > 2){
                    for (int j = 1; j < bStat->bundleVec[i].size() - 1; j++){
                        endTime += nodeVec[bStat->bundleVec[i][j]].delta;
                        tij = ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                        endTime += tij;
                    }
                }
            }

            else if (bStat->bundleVec[i].back() < inst->n){//the last element is a passenger(P-D-d, P-d or D-d)
                endTime = nodeVec[bStat->bundleVec[i].back()].e;
                bundleTime = endTime;

                for (int j = 0; j < bStat->bundleVec[i].size() - 1; j++){
                    bundleTime -= nodeVec[bStat->bundleVec[i][j]].delta;
                    tij = ((mdist[bStat->bundleVec[i][j]][bStat->bundleVec[i][j + 1]])/inst->vmed);
                    bundleTime -= tij;
                }

            }
           
            endTime += nodeVec[bStat->bundleVec[i].back()].delta;
            bStat->bundleEnd.push_back(endTime);
            bStat->bundleStart.push_back(bundleTime);
            
        }

        else{
            foundpass = true;
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e;
            bStat->bundleStart.push_back(bundleTime);
            bundleTime = nodeVec[bStat->bundleVec[i][0]].e + nodeVec[bStat->bundleVec[i][0]].delta;
            bStat->bundleEnd.push_back(bundleTime);
            continue;
        }
    }

}

void makeBundleReferenceP(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat){

    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->lastElement.push_back(bStat->bundleVec[i][bStat->bundleVec[i].size()-1]);

    }
    for (int i = 0; i < bStat->bundleVec.size(); i++){

        bStat->firstElement.push_back(bStat->bundleVec[i][0]);

    }
}


void selectEligibleBundlesP(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat *problem, bundleStat *bStat) {
    //int clusterSize = 1 + 3*inst->m;
    int clusterSize = 1 + 6*inst->m;
    int nClusters = inst->n;
    int nBundles = bStat->bundleVec.size();

    int nRemoved = 0;
    vector<int> RemovedByTime;
    vector<int> RemovedByCriterion;

    bStat->eligibleBundleVec.clear();
    bStat->eligibleBundleVec.resize(nBundles);

    //inst->T = 24;

    // Removing bundles unfeasible to any route
    for (int i = 0; i < bStat->bundleVec.size(); i++) {

        int u = bStat->firstElement[i];

        for (int k = 0; k < inst->K; k++) {
            int sDepot = bStat->firstElement[nBundles - 2*inst->K + k];

            double earliestArrv = bStat->bundleStart[nBundles - 2*inst->K + k] + mdist[sDepot][u]/inst->vmed;
            double latestEnd = bStat->bundleEnd[i];

            if (earliestArrv <= bStat->bundleStart[i] && bStat->bundleEnd[i] <= inst->T) {
                bStat->eligibleBundleVec[i] = true;
            }
        }

        if (bStat->eligibleBundleVec[i] == false) {
            RemovedByTime.push_back(i);
        }
    }

    // // Remove bundle with negastive profit
    // if (problem->model != "bundle2") {
    //     for (int i = 0; i < nClusters; i++) {
    //         int index = clusterSize*i;

    //         for (int parcelIndex = index; parcelIndex < index + clusterSize; parcelIndex++) {
    //             bool isEligible = (bStat->bundleProfVec[parcelIndex] >= 0);
    //             bStat->eligibleBundleVec[parcelIndex] = isEligible & bStat->eligibleBundleVec[parcelIndex];

    //             if (bStat->eligibleBundleVec[parcelIndex] == false) {
    //                 RemovedByCriterion.push_back(parcelIndex);
    //             }
    //         }
    //     }
    // }

    // // Remove bundles cheaper than the customer
    // else if (problem->model != "bundle2") {
    //     for (int i = 0; i < nClusters; i++) {
    //         int index = clusterSize*i;
    //         int customerProfit = bStat->bundleProfVec[index];

    //         for (int parcelIndex = index; parcelIndex < index + clusterSize; parcelIndex++) {
    //             bool isEligible = (bStat->bundleProfVec[parcelIndex] >= customerProfit);
    //             bStat->eligibleBundleVec[parcelIndex] = isEligible & bStat->eligibleBundleVec[parcelIndex];

    //             if (bStat->eligibleBundleVec[parcelIndex] == false) {
    //                 RemovedByCriterion.push_back(parcelIndex);
    //             }
    //         }
    //     }
    // }
    
    // // selecting bundles by priority
    // else 
    if (problem->model == "bundle6") {
        // Para encontrar os bundles associados a um customer, faça o seguinte:
        // - para n customers e m parcels, os bundles associados ao bundle i < n são 
        //   indexados de [a -- z], onde 'a = i * (3*m + 1)' e 'z = (i+1)*(3*m + 1) - 1';
        // - os bundles associados a uma parcel i < m podem ser encontrados em bStat->parcelBundleVec[i];

        // 1 - criar matriz de prioridades

        int cur, prev, count;
        cur = 0;
        prev = -1;

        count = 0;

        // while (cur != prev) {
            vector<bool> priority(bStat->bundleVec.size(), false);
            // cout << cur << " " << prev << endl;
            // prev = cur;
            cur = 0;
            // cout << "Customer" << endl;
            // 2 - para cada customer, calcular a média dos profits de todos os seus bundles e marcar como prioritários os bundles acima da média
            for (int i = 0; i < inst->n; i++) {
                double media = 0;
                int qtd = 0;

                for (int a = i * (clusterSize) + 1; a < (i+1)*(clusterSize); a++) {
                    if (bStat->eligibleBundleVec[a])
                    {
                        media += bStat->bundleProfVec[a];
                        qtd++;
                    }
                }

                media /= qtd;

                for (int a = i * (clusterSize) + 1; a < (i+1)*(clusterSize); a++) {
                    if (bStat->bundleProfVec[a] >= media && bStat->eligibleBundleVec[a]) {
                        priority[a] = true;
                        cur++;
                    }

                    // if (bStat->eligibleBundleVec[a] && !priority[a])
                    // {
                    //     cout << endl;
                    //     cout << "Removed bundle: " << a << endl;
                    //     cout << "Type: Customer" << endl;
                    //     cout << "Profit: " << bStat->bundleProfVec[a] << endl;
                    //     cout << "Serve: " << bStat->bundleServVec[a] << endl;
                    //     cout << endl;
                    // } else if (priority[a]) {
                    //     cout << endl;
                    //     cout << "selected bundle: " << a << endl;
                    //     cout << "Type: Customer" << endl;
                    //     cout << "Profit: " << bStat->bundleProfVec[a] << endl;
                    //     cout << "Serve: " << bStat->bundleServVec[a] << endl;
                    //     cout << endl;
                    // }
                }
            }

            // cout << "Parcel" << endl;
            // 3 - para cada parcel, calcular a distância média percorrida nos bundles (tirando a distância interna do customer) e marcar como
            // prioritários os bundles cuja distância percorrida é menor do que a média (por hora usar o tempo de viagem em bStat->bundleServVec
            // como referência, mas tem que descobrir onde tá armazenada a informação da distância)
            for (int i = 0; i < inst->m; i++)
            {
                double media = 0;
                int qtd = 0;

                for (int a = 0; a < bStat->parcelBundleVec[i].size(); a++)
                {
                    int bundleIdx = bStat->parcelBundleVec[i][a];

                    if (bStat->eligibleBundleVec[ bundleIdx ])
                    {
                        int customerIdx = min(
                            bStat->bundleVec[bundleIdx][0], 
                            min(bStat->bundleVec[bundleIdx][1], bStat->bundleVec[bundleIdx][2])
                        );

                        media += bStat->bundleServVec[ bundleIdx ] - nodeVec[customerIdx].delta;
                        qtd++;
                    }
                }

                media /= qtd;

                for (int a = 0; a < bStat->parcelBundleVec[i].size(); a++)
                {
                    int bundleIdx = bStat->parcelBundleVec[i][a];
                    int customerIdx = min(
                            bStat->bundleVec[bundleIdx][0], 
                            min(bStat->bundleVec[bundleIdx][1], bStat->bundleVec[bundleIdx][2])
                        );

                    if (bStat->bundleServVec[ bundleIdx ] - nodeVec[customerIdx].delta <= media && bStat->eligibleBundleVec[ bundleIdx ])
                    {
                        priority[ bundleIdx ] = true;
                        cur++;
                    }
        
                    // if (bStat->eligibleBundleVec[bundleIdx] && bStat->bundleServVec[ bundleIdx ] > media && !priority[bundleIdx])
                    // {
                    //     cout << endl;
                    //     cout << "Removed bundle: " << bundleIdx << endl;
                    //     cout << "Type: Parcel" << endl;
                    //     cout << "Profit: " << bStat->bundleProfVec[ bundleIdx ] << endl;
                    //     cout << "Serve: " << bStat->bundleServVec[ bundleIdx ] - nodeVec[customerIdx].delta << endl;
                    //     cout << endl;
                    // } else if (priority[bundleIdx]) {
                    //     cout << endl;
                    //     cout << "selected bundle: " << bundleIdx << endl;
                    //     cout << "Type: Parcel" << endl;
                    //     cout << "Profit: " << bStat->bundleProfVec[ bundleIdx ] << endl;
                    //     cout << "Serve: " << bStat->bundleServVec[ bundleIdx ] - nodeVec[customerIdx].delta << endl;
                    //     cout << endl;
                    // }
                }
            }

            // 4 - remover da eligibilidade os bundles que não são prioritários para nenhum dos critérios
            for (int i = 0; i < bStat->bundleVec.size() - 2*inst->K; i++) {
                // cout << "1 aqui" << endl;

                // if (bStat->eligibleBundleVec[i])
                // {
                //     bStat->eligibleBundleVec[i] = (bStat->eligibleBundleVec[i] & priority[i]);
                //     if (!bStat->eligibleBundleVec[i])
                //     {
                //         cout << endl;
                //         cout << "Removed bundle: " << i << endl;
                //         cout << "Profit: " << bStat->bundleProfVec[i] << endl;
                //         cout << "Serve: " << bStat->bundleServVec[i] << endl;
                //         cout << endl;
                //     }
                // }
                bStat->eligibleBundleVec[i] = (bStat->eligibleBundleVec[i] & priority[i]);
            }
        // }

    }

    for (int i = 0; i < bStat->bundleVec.size(); i++) {
        if (bStat->eligibleBundleVec[i] == false) {
            nRemoved++;
        }
    }

    // cout << endl;
    // cout << "total bundles: " << bStat->bundleVec.size() << endl;
    // cout << "total bundles removed: " << nRemoved << endl;

    // cout << endl;
    // cout << "Bundles Removed By time horizon:" << endl;
    // for (int i = 0; i < RemovedByTime.size(); i++) {
    //     cout << RemovedByTime[i] << " ";
    // }
    // cout << endl;

    // cout << endl;
    // cout << "Bundles Removed By criterion:" << endl;
    // for (int i = 0; i < RemovedByCriterion.size(); i++) {
    //     cout << RemovedByCriterion[i] << " ";
    // }
    // cout << endl;
    // getchar();
}

void initArcsP (instanceStat *inst, bundleStat *bStat, clSt *cStat){

    vector< vector<int> > vec2d;
    vector<int> vec1d;
    vector<bool> arcRow;
    vector< pair<int,int> > auxVec;

    //init Bundle arcs

    for (int k = 0; k < inst->K; k++){
        bStat->bArcPlus.push_back(auxVec);
    }

    for(int i = 0; i < bStat->bundleVec.size(); i++){
        vec2d.push_back(vec1d);

        bStat->vArcPlus.push_back(bStat->bArcPlus);
        bStat->vArcMinus.push_back(bStat->bArcPlus);
    }

    bStat->bArcPlus.clear();

    for (int i = 0; i < bStat->bundleVec.size(); i++){
        for (int j = 0; j < bStat->bundleVec.size(); j++){
            arcRow.push_back(false);
        }
        bStat->bArcs.push_back(arcRow);
        bStat->bArcMinus.push_back(auxVec);
        bStat->bArcPlus.push_back(auxVec);


        bStat->arcV.push_back(vec2d);

        arcRow.clear();
    }

    auxVec.clear();
    vec2d.clear();
    vec1d.clear();

    //init Cluster arcs

    for (int k = 0; k < inst->K; k++){
        cStat->cArcPlus.push_back(auxVec);
    }

    for(int i = 0; i < cStat->clusterVec.size(); i++){
        vec2d.push_back(vec1d);

        cStat->vArcPlus.push_back(cStat->cArcPlus);
        cStat->vArcMinus.push_back(cStat->cArcPlus);
    }

    cStat->cArcPlus.clear();
   
    for (int i = 0; i < cStat->clusterVec.size(); i++){
        for (int j = 0; j < cStat->clusterVec.size(); j++){
            arcRow.push_back(false);
        }
        cStat->cArcs.push_back(arcRow);
        cStat->cArcMinus.push_back(auxVec);
        cStat->cArcPlus.push_back(auxVec);

        cStat->arcV.push_back(vec2d);
        
        cStat->cArcs.push_back(arcRow);
    }

}

void feasibleBundleArcsP (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, int p, probStat* problem){
    //cout << "Begin feasibleBundleArcs2" << endl;
    int setN; //last index of bundles before starting points
    int setP; //last index of bundles with only passengers
    int ref;
    //int bundlelimit = inst->m*0.2 + 1; // '+1' to add the pass only bundle to the list
    int bundlelimit = inst->n*0.2 + 1; // '+1' to add the pass only bundle to the list

    //int bundlelimit = 1;
    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    setN = bStat->bundleVec.size() - (2*inst->K);
    setP = setN;

    //ref = 3*inst->m;
    ref = 6*inst->m;

    //inst->T = 24;

    int currentCluster = 0;

    pair<int, int> bFArc;
    int auxK;
    int fDummy = bStat->bundleVec.size() - inst->K;
    int fDepot = bStat->bundleVec.size() - 2*inst->K;



    ////////////////////////////////////
    //Uncomment this for restricted bundle selection

    //remove non promising parcel-passenger bundles connections when comparing to passenger to passenger.
    //for (int i = 0; i < inst->n; i++){
    //    cout << "pass1: " << i << endl;
    //    int currentbundle = bStat->fullsorted[i][0];
    //    bStat->sortedBundles.push_back(currentbundle);
    //    for (int j = 1; j < bundlelimit + 1; j++){
    //        double bestprofit = bStat->bundleProfVec[currentbundle];
    //        bool bestbundle = true;
    //        cout << "Current bundle: " << currentbundle << " - ";
    //        for (int pos = 0; pos < bStat->bundleVec[currentbundle].size(); pos++){
    //            cout << bStat->bundleVec[currentbundle][pos] << " ";
    //        }
    //        cout << endl;
    //        cout << "Best profit: " << bestprofit << endl;
    //        cout << endl;

    //        if(bStat->bundleVec[currentbundle][0] == i){
    //            cout << "pass1-pass2 case" << endl;
    //            for (int b = 0; b < inst->n; b++){ //to test pass1-pass2 arcs
    //                cout << "pass2: " << b << endl;
    //                if (i == b){
    //                    continue;
    //                }
    //                if (nodeVec[b].e <= nodeVec[i].e){
    //                    continue;
    //                }
    //                double currentprofit = nodeVec[i].profit + nodeVec[b].profit - inst->costkm*mdist[i][b];
    //                if (currentprofit >= bestprofit){
    //                    cout << "best profit surpassed: " << currentprofit << endl;
    //                    bestbundle = false;            
    //                }
    //            }
    //        }
    //        else if(bStat->bundleVec[currentbundle][1] == i){
    //            cout << "pass2-pass1 case" << endl;
    //            for (int b = 0; b < inst->n; b++){ //To test pass2-pass1 arcs
    //                cout << "pass2: " << b << endl;
    //                if (i == b){
    //                    continue;
    //                }
    //                if (nodeVec[i].e <= nodeVec[b].e){
    //                    continue;
    //                }                
    //                double currentprofit = nodeVec[i].profit + nodeVec[b].profit - inst->costkm*mdist[b][i];
    //                if (currentprofit >= bestprofit){
    //                    cout << "best profit surpassed: " << currentprofit << endl;
    //                    bestbundle = false;            
    //                }
    //            }
    //        }
    //        if (bestbundle){
    //            cout << "Bundle " << currentbundle << " is a promising bundle" << endl;
    //            bStat->sortedBundles.push_back(currentbundle);
    //        }

    //    }
    //}
    //getchar();

    //end of restricted bundle selection
    ////////////////////////////////////

    //Uncomment this if partial bundle with no restriction

    for (int i = 0; i < bStat->fullsorted.size(); i++){
        for (int j = 0; j < bundlelimit + 1; j++){
            bStat->sortedBundles.push_back(bStat->fullsorted[i][j]);
        }
    }

    
    //////////end of partial bundle with no restriction
    for(int k = 0; k < inst->K; k++){ //depot bundles
        bStat->sortedBundles.push_back(fDepot + k);
    }
    for(int k = 0; k < inst->K; k++){ //dummy bundles
        bStat->sortedBundles.push_back(fDepot + inst->K + k);
    }

    for (int i = 0; i < bStat->bundleVec.size(); i++) {
        bStat->clofbundle.push_back(-1);
    }

    int setNsort = bStat->sortedBundles.size() - (inst->K*2);

    //for(int i = 0; i < bStat->bundleVec.size(); i++){
    for(int i = 0; i < bStat->sortedBundles.size(); i++){
        int bda = bStat->sortedBundles[i]; 

        if (bda < fDummy){
            if(bda < setP){//i is not depot or dummy or parcel only
                if (bda > currentCluster*(ref + 1) + ref){
                    currentCluster++;
                }
                bStat->clofbundle[bda] = currentCluster;
                //for (int j = 0; j < setN; j++){//j is not depot or dummy
                for (int j = 0; j < setNsort; j++){//j is not depot or dummy
                    int bdb = bStat->sortedBundles[j];
                    if (bda != bdb){
                        if (bdb > currentCluster*(ref + 1) + ref || bdb < currentCluster*(ref + 1)){
                            if (bStat->bundleEnd[bdb] <= inst->T){
                                if (bStat->bundleStart[bdb] > 0){
                                    if (bStat->bundleStart[bda] + bStat->bundleServVec[bda] + 
                                       (mdist[bStat->lastElement[bda]][bStat->firstElement[bdb]]/inst->vmed) <= bStat->bundleStart[bdb]){
                                        if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[bdb]) {
                                            bStat->bArcs[bda][bdb] = true;
                                        }
                                    } 
                                }
                                else{
                                    if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[bdb]) {
                                        bStat->bArcs[bda][bdb] = true;
                                    }
                                }
                            }
                        }
                    } 
                }

                for (int j = fDummy; j < bStat->bundleVec.size(); j++){//j is dummy node
                    if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[j]) {
                        bStat->bArcs[bda][j] = true;
                    }                                   
                }
            }

            else if (bda >= setNsort){//i is a starting point bundle

                currentCluster++;

                bStat->clofbundle[bda] = currentCluster;
                for (int j = 0; j < setNsort; j++){//j is a request bundle
                    int bdb = bStat->sortedBundles[j];
                    if (bStat->bundleEnd[bdb] <= inst->T){
                        if (bStat->bundleStart[bdb] > 0){
                            if (bStat->bundleStart[bda] + bStat->bundleServVec[bda] +
                               (mdist[bStat->lastElement[bda]][bStat->firstElement[bdb]]/inst->vmed) <= bStat->bundleStart[bdb]){
                                if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[bdb]) {
                                    bStat->bArcs[bda][bdb] = true;
                                }                      
                            }
                        }
                        else{
                            if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[bdb]) {
                                bStat->bArcs[bda][bdb] = true;
                            }
                        }
                    }
                }
                if (bStat->eligibleBundleVec[bda] && bStat->eligibleBundleVec[bda + inst->K]) {
                    bStat->bArcs[bda][bda + inst->K] = true; //direct trip to depot (empty vehicle)
                }
            }
        }
        else{
            currentCluster++;
            bStat->clofbundle[bda] = currentCluster;
        }
    }

    // cout << "list of cluster allocations: " <<endl;

    // for (int i = 0; i < bStat->clofbundle.size(); i++){
    //     cout << "Bundle: (" << i << ") - " << "cluster: " << bStat->clofbundle[i] << endl;
    // }
    // getchar();



    //remove arcs to bundles with same parcel requests
    for (int i = 0; i < bStat->parcelBundleVec.size(); i++){
        for (int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
            for (int k = 0; k < bStat->parcelBundleVec[i].size(); k++){
                if (bStat->parcelBundleVec[i][j] != bStat->parcelBundleVec[i][k]){
                    bStat->bArcs[bStat->parcelBundleVec[i][j]][bStat->parcelBundleVec[i][k]] = false;
                }
            }
        }
    }

    //remove arcs from/to bundles with negative start times
    for (int i = 0; i < bStat->bundleStart.size() - 1; i++){
        if (bStat->bundleStart[i] < 0){
            for (int j = 0; j < bStat->bundleVec.size() - 1; j++){
                bStat->bArcs[j][i] = false;
                bStat->bArcs[i][j] = false;           
            }
        }
    }

    for(int i = 0; i < bStat->bundleVec.size(); i++){
        for(int j = 0; j < bStat->bundleVec.size(); j++){
            if (bStat->bArcs[i][j]){
                bFArc.first = i;
                bFArc.second = j;                
                
                bStat->bArcMinus[j].push_back(bFArc);
                bStat->bArcPlus[i].push_back(bFArc);
                
                bStat->bArcVec.push_back(bFArc);

                if (i < setN){//i is a bundle of request
                    if (j < setN){ // j is a bundle of request
                        for (int k = 0; k < inst->K; k++){ // in this case, all vehicles are available
                            bStat->arcV[i][j].push_back(k);
                        }                        
                    }
                    else if (j >= fDummy){ //j is a dummy bundle
                        auxK = j - fDummy; //k is the vehicle associated with dummy bundle j
                        bStat->arcV[i][j].push_back(auxK);                         
                    }
                }
                else if (i < fDummy){//i is a starting bundle
                    if (j < setN){ //j is a request bundle
                        auxK = i - setN;
                        bStat->arcV[i][j].push_back(auxK);                         
                    }              
                }
            }
        }
        if (i >= setN && i < fDummy){//i is a starting bundle
            auxK = i - setN; //arc from starting node to end depot
            bStat->arcV[i][i + inst->K].push_back(auxK);
        }
    }

    //creating the vectors for leaving incoming arcs with available vehicles
    for (int a = 0; a < bStat->bArcVec.size(); a++){
        //cout << "\n\nIn Arcs: " << endl;
        //cout << "bundle arc: " << bStat->bArcVec[a].first << "-" << bStat->bArcVec[a].second << endl;

        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        //cout << "size of arcV: " << bStat->arcV[i][j].size();

        for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
            int k = bStat->arcV[i][j][k1];
            bStat->vArcPlus[i][k].push_back(bStat->bArcVec[a]);
            bStat->vArcMinus[j][k].push_back(bStat->bArcVec[a]);
        }
    }

    // Creating cluster of bundles
    bStat->cBundles.resize(currentCluster + 1);

    for (int i = 0; i < bStat->clofbundle.size(); i++) {
        if (bStat->clofbundle[i] != -1) {
            bStat->cBundles[bStat->clofbundle[i]].push_back(i);
        }
    }

    // for (int i = 0; i < bStat->cBundles.size(); i++) {
    //     cout << "cluster : " << i << endl;
    //     for (int j = 0; j < bStat->cBundles[i].size(); j++) {
    //         cout << bStat->cBundles[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // getchar();
    //cout << "End feasibleBundleArcs2" << endl;
}

void feasibleClusterArcsP (instanceStat *inst, vector<nodeStat> &nodeVec, bundleStat *bStat, clSt *cStat, probStat* problem){
    int reqClusters = cStat->clusterVec.size() - (inst->K*2);
    int clusterA = 0;
    int clusterB;
    int setN;
    int setP;//with passengers
    int setS;
    int cset;
    int csetP; //with passengers
    int ref;

    // setP = bStat->bundleVec.size() - (2*inst->K) - inst->m;
    setN = bStat->bundleVec.size() - (2*inst->K);
    setP = setN;

    cset = cStat->clusterVec.size() - (inst->K*2);
    csetP = cset;
    // csetP = cStat->clusterVec.size() - (inst->K*2) - inst->m;

    //ref = 3*inst->m;

    ref = 6*inst->m;
    // int setf;

    pair<int, int> cFArc;
    int fDummy = bStat->bundleVec.size() - inst->K;
    int fcDummy = cStat->clusterVec.size() - inst->K;

    int auxK;

    //for (int i = 0; i < bStat->bundleVec.size(); i++){
    for (int i = 0; i < bStat->sortedBundles.size(); i++){
        int bda = bStat->sortedBundles[i];
        if (bda < setP){//i is a request bundle
            if (bda > clusterA*(ref + 1) + ref){
                clusterA++;
            }           
        }
        else{
            if(problem->scen == "1A" || problem->scen == "1AD"){
                clusterA++;
            }
        }

        clusterB = 0;
        //for (int j = 0; j < bStat->bundleVec.size(); j++){
        for (int j = 0; j < bStat->sortedBundles.size(); j++){     
            int bdb = bStat->sortedBundles[j];   

            if (bdb < setP){//j is a request bundle
                if (bdb > clusterB*(ref + 1) + ref){
                    clusterB++;
                }                
            }
            else{
                if(problem->scen == "1A" || problem->scen == "1AD"){
                    clusterB++;
                }
            }
            if (clusterA == clusterB){
                continue;
            }
            else{
                if (bStat->bArcs[bda][bdb]){
                    // cout << i << " " << j << endl;
                    // cout << clusterA << " " << clusterB << endl;
                    // getchar();
                    cStat->cArcs[clusterA][clusterB] = true;
                    cFArc.first = bda;
                    cFArc.second = bdb;

                    cStat->cArcMinus[clusterB].push_back(cFArc);
                    cStat->cArcPlus[clusterA].push_back(cFArc);
                }
            }
        }
    }

    for (int a = 0; a < bStat->bArcVec.size(); a++){
        int i = bStat->bArcVec[a].first;
        int j = bStat->bArcVec[a].second;
        int cA = bStat->clofbundle[i];
        int cB = bStat->clofbundle[j];
        if (bStat->arcV[i][j].size() > 0){
            for (int k1 = 0; k1 < bStat->arcV[i][j].size(); k1++){
                int k = bStat->arcV[i][j][k1];
                cStat->vArcPlus[cA][k].push_back(bStat->bArcVec[a]);
                cStat->vArcMinus[cB][k].push_back(bStat->bArcVec[a]);
            }
        }
    } 

    //cout << "Vector of arcs of cluster traverssed by k: " << endl;

    //for (int i = 0; i < cStat->vArcPlus.size(); i++){
    //    for (int j = 0; j < cStat->vArcPlus[i].size(); j++){
    //        for (int k = 0; k < cStat->vArcPlus[i][j].size(); k++){
    //            cout << "cluster: " << i << " - " << "vehicle: " << j << " - " << "Bundle arc: " << cStat->vArcPlus[i][j][k].first << "-" << cStat->vArcPlus[i][j][k].second << endl;
    //        }
    //    }
    //    cout << endl;
    //} 
}

void printBundleInfoP(instanceStat *inst, bundleStat *bStat, clSt *cStat){

    //cout << "\nBundle Vector: [";

    //for (int i = 0; i < bStat->bundleVec.size(); i++){
    //    cout << "(" << i << "): [";
    //    for (int j = 0; j < bStat->bundleVec[i].size(); j++){
    //        cout << bStat->bundleVec[i][j];
    //        if (j < bStat->bundleVec[i].size() - 1){
    //            cout << ",";
    //        }
    //        else{
    //            cout << "] ";
    //        }
    //    }
    //    cout << endl;
    //}


    //cout << "\nParcel Bundles: " << endl;

    //for (int i = 0 ; i < bStat->parcelBundleVec.size(); i++){
    //cout << i + inst->n << ": ";
    //for(int j = 0; j < bStat->parcelBundleVec[i].size(); j++){
    //    cout << bStat->parcelBundleVec[i][j] << " ";
    //}
    //cout << endl;
    //}
    //cout << endl;

    //std::ofstream file("bundleProfits.csv");

    //if (!file.is_open()) {
    //    std::cerr << "Failed to open file for writing.\n";
    //    return;
    //}
    //file << -1 << ",";
    //for (int i = 0; i < bStat->bundleProfVec.size(); i++) {
    //    file << i;
    //    if (i < bStat->bundleProfVec.size() - 1) file << ",";

    //}
    //file << "\n";    
    //for (int i = 0; i < bStat->bundleProfVec.size(); i++) {
    //    file << i << "," << bStat->bundleProfVec[i] << "\n";
    //}

    //file.close();


    //cout << "\nBundle Profit: " << endl;
    //for (int i = 0; i < bStat->bundleProfVec.size(); i++){
    //    cout << i << ": " << setw(3) << std:: right << bStat->bundleProfVec[i] << endl;
    //}
    //cout << endl;

    // getchar();

    //cout << "\nBundle Service Times: ";
    //for (int i = 0; i < bStat->bundleServVec.size(); i++){
    //cout << i  << ": " <<   bStat->bundleServVec[i] << endl;
    //}
    //cout << endl;

    ////// getchar();

    //cout<< "\nFeasible arcs between bundles:" << endl;
    //for (int i = 0; i < bStat->bundleVec.size(); i++){
    //    if (i == 0){
    //        cout << setw(3) << " ";
    //    }
    //    cout << setw(3) << std::right << i;
    //}
    //cout << endl;

    //for (int i = 0; i < bStat->bundleVec.size(); i++){
    //    cout << setw(3) << std::right << i;
    //    for (int j = 0; j < bStat->bundleVec.size(); j++){
    //        cout << setw(3) << std:: right << bStat->bArcs[i][j];
    //    }
    //    cout << endl;
    //}

    //file with bundle arcs
    //std::ofstream file("bundleArcs.csv");

    //if (!file.is_open()) {
    //    std::cerr << "Failed to open file for writing.\n";
    //    return;
    //}
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i;
    //    if (i < bStat.bundleVec.size() - 1) file << ",";

    //}
    //file << "\n";    
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i << ",";
    //    for (int j = 0; j < bStat.bundleVec.size(); j++){
    //        file << bStat.bArcs[i][j];
    //        if (j < bStat.bundleVec.size() - 1) file << ",";
    //    }
    //    file << "\n";
    //}

    //file.close();

    //cout << "Bundle beginning times: " << endl;
    //for (int i = 0; i < bStat->bundleStart.size(); i++){
    //    cout << setw(3) << std::right << i << ": " << std:: right << bStat->bundleStart[i];
    //    cout << endl;
    //}
    //// getchar();


    //cout << "Bundle ending times: " << endl;
    //for (int i = 0; i < bStat->bundleEnd.size(); i++){
    //    cout << setw(3) << std::right << i << ": " << std:: right << bStat->bundleEnd[i];
    //    cout << endl;
    //}
    //// getchar();

    //for (int i = 0; i < cStat->clusterVec.size(); i++){
    //    cout << "\nCluster " << i << ": [";
    //    for(int k = 0; k < cStat->clusterVec[i].size(); k++){
    //        cout << "(" << cStat->clusterVec[i][k] << ") " << "[";
    //        for (int j = 0; j < bStat->bundleVec[cStat->clusterVec[i][k]].size(); j++){
    //            cout << setw(3) << std:: right << bStat->bundleVec[cStat->clusterVec[i][k]][j];
    //            if (j < bStat->bundleVec[cStat->clusterVec[i][k]].size() - 1){
    //                cout << ",";
    //            }
    //            else{
    //                cout << "] ";
    //            }
    //        }
    
    //    }
    //    cout << "]" << endl;
    //}

    //int bundlelimit = inst->m*0.2 + 1;
    int bundlelimit = inst->n*0.2 + 1;

    cout << "Used bundles: " << endl;
    for (int i = 0; i < cStat->clusterVec.size(); i++){
        cout << "\nCluster " << i << ": [";
        if (i < inst->n){
            for(int k = 0; k < bundlelimit+1; k++){
                int bundle = bStat->fullsorted[i][k];
                cout << "(" << bundle << ") " << "[";
                for (int j = 0; j < bStat->bundleVec[bundle].size(); j++){
                    cout << setw(3) << std:: right << bStat->bundleVec[bundle][j];
                    if (j < bStat->bundleVec[bundle].size() - 1){
                        cout << ",";
                    }
                    else{
                        cout << "] ";
                    }
                }
        
            }
        }
        else{
            for(int k = 0; k < cStat->clusterVec[i].size(); k++){
                cout << "(" << cStat->clusterVec[i][k] << ") " << "[";
                for (int j = 0; j < bStat->bundleVec[cStat->clusterVec[i][k]].size(); j++){
                    cout << setw(3) << std:: right << bStat->bundleVec[cStat->clusterVec[i][k]][j];
                    if (j < bStat->bundleVec[cStat->clusterVec[i][k]].size() - 1){
                        cout << ",";
                    }
                    else{
                        cout << "] ";
                    }
                }
            }
        }
        cout << "]" << endl;
    }

    // getchar();

    //cout<< "\nFeasible arcs between clusters:" << endl;
    //for (int i = 0; i < cStat->clusterVec.size(); i++){
    //    if (i == 0){
    //        cout << setw(3) << " ";
    //    }
    //    cout << setw(3) << std::right << i;
    //}
    //cout << endl;
    //for (int i = 0; i < cStat->clusterVec.size(); i++){
    //    cout << setw(3) << std::right << i;
    //    for (int j = 0; j < cStat->clusterVec.size(); j++){
    //        cout << setw(3) << std:: right << cStat->cArcs[i][j];
    //    }
    //    cout << endl;
    //}

    //// getchar();
}


void bundleMethodPartial(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
    bundleStat bStat;
    fipBundleStats fipStat;
    clSt cStat;

    vector< vector<bParcelStruct> > clsParcel;
    //vector< vector<int> > fullsorted;

    int p = -1; //number of parcel requests to be associated with a passenger request(1A) or the number of best matchings
    int Q = 5;

    set<string> onlyBundleExec;
    onlyBundleExec.insert("bundle");
    onlyBundleExec.insert("bundle2");
    onlyBundleExec.insert("bundle5");
    onlyBundleExec.insert("bundle6");
    onlyBundleExec.insert("bundlep");

    initVecs2(inst, clsParcel, &bStat, problem);

    makeSmallerProblem2(inst, nodeVec, mdist, p, clsParcel, problem, Q);

    //cout << "Before making bundles" << endl;
    makeBundlesP(inst, nodeVec, &bStat, &cStat, clsParcel, problem);
    //cout << "After making bundles" << endl;

    //printBundleInfoP(inst, &bStat, &cStat);

    
    ////Uncomment from this line
    makeParcelBundlesP(inst, nodeVec, &bStat, problem);

    bundleProfitP(inst, mdist, nodeVec, &bStat);

    //printBundleInfoP(inst, &bStat, &cStat);

    makeStartTimesP(inst, mdist, nodeVec, &bStat, problem);

    makeBundleReferenceP(inst, mdist, nodeVec, &bStat);

    selectEligibleBundlesP(inst, mdist, nodeVec, problem, &bStat);
    
    orderBundles(inst, &bStat, &cStat);

    initArcsP(inst, &bStat, &cStat);

    //printBundleInfoP(inst, &bStat, &cStat);

    feasibleBundleArcsP(inst, mdist, nodeVec, &bStat, &cStat, p, problem);
    //printBundleInfoP(inst, &bStat, &cStat);
    //getchar();
    //clearBndStats(sStat);
    //std::ofstream file("bundleArcs.csv");

    //if (!file.is_open()) {
    //    std::cerr << "Failed to open file for writing.\n";
    //    return;
    //}
    //file << -1 << ",";
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i;
    //    if (i < bStat.bundleVec.size() - 1) file << ",";

    //}
    //file << "\n";    
    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    file << i << ",";
    //    for (int j = 0; j < bStat.bundleVec.size(); j++){
    //        file << bStat.bArcs[i][j];
    //        if (j < bStat.bundleVec.size() - 1) file << ",";
    //    }
    //    file << "\n";
    //}

    //file.close();

    ////For passenger only solutions:
    //// feasibleBundleArcsPass2(inst, mdist, nodeVec, &bStat, &cStat, p, problem);

    
    feasibleClusterArcsP(inst, nodeVec, &bStat, &cStat, problem);


    /////////////////////////////////////////

    if (onlyBundleExec.find(problem->model) != onlyBundleExec.end()) {
        mipbundlep(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat);

        if(sStat->feasible){
            // solStatIni(sStat);

            nodeSolution2 (inst, mdist, &bStat, nodeVec, sStat, problem, false);

            stillTimeBundle2(inst, mdist, &bStat, nodeVec, sStat);

            mipSolStatsPlus (inst, mdist, &bStat, nodeVec, sStat);

            // // cout << sStat.tParcel << " " << sStat.tPass << " " << sStat.tBoth << " " << sStat.tNone << endl;

            printStats(inst, sStat);
            
            printBundleFile(inst, sStat, problem, "bundlep");
        }

        fipStructBundle(inst, sStat, &bStat, &fipStat, problem->model, true);
    }

    //string filename; 
    
    //if (problem->model == "bundle3" || problem->model == "bundle4") {
    //    filename = "src/Aux/bundleSol/" + inst->InstName + ".txt";
    //} else if (problem->model == "bundle7") {
    //    filename = "src/Aux/bundleSolRem1/" + inst->InstName + ".txt";
    //} else if (problem->model == "bundle8") {
    //    filename = "src/Aux/bundleSolRem2/" + inst->InstName + ".txt";
    //}

    //ifstream iFile(filename);

    //if (!iFile.is_open()) {
    //    return;
    //}
    
    //if(sStat->servedParcels < inst->m){
    //    setUpFipBundle(inst, mdist, nodeVec, &bStat, problem, &fipStat);

    //    clearArcs(&bStat);
    //    initArcs2(inst, &bStat, &cStat);
    //    if (fipStat.solPass.size() < 1) {
    //        fipStructBundle(inst, sStat, &bStat, &fipStat, problem->model, false);
    //    }
    //    feasibleBundleArcs2next(inst, mdist, nodeVec, &bStat, &cStat, p, problem, &fipStat);

    //    clearBndStats(sStat);
    //    //std::ofstream file("bundleArcs.csv");

    //    //if (!file.is_open()) {
    //    //    std::cerr << "Failed to open file for writing.\n";
    //    //    return;
    //    //}
    //    //file << -1 << ",";
    //    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    //    file << i;
    //    //    if (i < bStat.bundleVec.size() - 1) file << ",";

    //    //}
    //    //file << "\n";    
    //    //for (int i = 0; i < bStat.bundleVec.size(); i++) {
    //    //    file << i << ",";
    //    //    for (int j = 0; j < bStat.bundleVec.size(); j++){
    //    //        file << bStat.bArcs[i][j];
    //    //        if (j < bStat.bundleVec.size() - 1) file << ",";
    //    //    }
    //    //    file << "\n";
    //    //}

    //    //file.close();
       
       
    //    // cout << "\nParcel Bundles: " << endl;

    //    // for (int i = 0 ; i < bStat.parcelBundleVec.size(); i++){
    //    // cout << i + inst->n << ": ";
    //    // for(int j = 0; j < bStat.parcelBundleVec[i].size(); j++){
    //    //     cout << bStat.parcelBundleVec[i][j] << " ";
    //    // }
    //    // cout << endl;
    //    // }
    //    // cout << endl;
        
    //    if (problem->model == "bundle3") {
    //        fipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat, &fipStat);
    //    } else {
    //        mfipbundle(inst, nodeVec, mdist, &bStat, &cStat, problem, sStat, &fipStat);
    //    }

    //    // cout << "after fip" << endl;

    //    nodeSolution2 (inst, mdist, &bStat, nodeVec, sStat, problem, true);
        
    //    stillTimeBundle2(inst, mdist, &bStat, nodeVec, sStat);

    //    mipSolStatsPlus (inst, mdist, &bStat, nodeVec, sStat);

    //    printStats(inst, sStat);

    //    printBundleFile(inst, sStat, problem, problem->model);
    //}

    //iFile.close();

    for ( int i = 0; i < inst->V + inst->dummy; i++) {
        delete[] mdist[i];
    }
    delete[] mdist;
}
