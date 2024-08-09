#include "modelnode.h"
#include <cstdlib>
#include <stdio.h>
#include <set>

void initArcs (instanceStat *inst, nodeArcsStruct *nas){
    vector<bool> auxVec;
    vector< pair<int,int> > auxPairVec;

    vector<int> aux1d;
    vector< vector<int> > aux2d;
    
    nas->arcs.clear();
    nas->allArcs.clear();
    nas->vArcPlus.clear();
    nas->vArcMinus.clear();
    nas->arcV.clear();
    nas->arcPlus.clear();
    nas->arcMinus.clear();
    nas->arcNN.clear();
    nas->arcNplus.clear();
    nas->arcPN.clear();
    nas->arcPD.clear();
    nas->arcPP.clear();
    nas->arcnf.clear();

    for (int k = 0; k < inst->K; k++){
        nas->arcPlus.push_back(auxPairVec);
    }

    for(int i = 0; i < inst->V + inst->dummy; i++){
        aux2d.push_back(aux1d);

        nas->vArcPlus.push_back(nas->arcPlus);
        nas->vArcMinus.push_back(nas->arcPlus);
    }

    nas->arcPlus.clear();

    for(int i = 0; i < inst->V + inst->dummy; i++){
        for(int j = 0; j < inst->V + inst->dummy; j++){
            auxVec.push_back(false);
        }
        nas->arcs.push_back(auxVec);
        nas->arcPlus.push_back(auxPairVec);
        nas->arcMinus.push_back(auxPairVec);
        


        nas->arcV.push_back(aux2d);
        
        auxVec.clear();
    }
} 

void feasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem, vector<nodeStat> &nodeVec, double **mdist){
    int auxK;

    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;

    int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

    //independently from sarp scenario, these arcs are always true

    for(int i = inst->n + 2*inst->m; i < inst->V; i++){//i is a starting point

        int l = i + inst->K; //its dummy node

        nas->arcs[i][l] = true;
        nas->fArc.first = i;
        nas->fArc.second = l;
        nas->arcMinus[l].push_back(nas->fArc);
        nas->arcPlus[i].push_back(nas->fArc);
        nas->allArcs.push_back(nas->fArc);

        auxK = l - inst->V;
        nas->arcV[i][l].push_back(auxK);
        
        for (int j = 0; j < inst->n; j++){ //j is a passenger node

            double ttij = mdist[i][j]/inst->vmed;
            ttij = std::round(ttij * multiplier) / multiplier;

            if (nodeVec[i].e + ttij < nodeVec[j].l){
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);
                nas->allArcs.push_back(nas->fArc);

                nas->arcnf.push_back(nas->fArc);
                auxK = i - fDepot;
                nas->arcV[i][j].push_back(auxK);
            }         
        }
        for (int j = inst->n; j < inst->n + inst->m; j++){ //j is a parcel pu node
            double ttij = mdist[i][j]/inst->vmed;
            ttij = std::round(ttij * multiplier) / multiplier;
            if (ttij < inst->T){
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);
                nas->allArcs.push_back(nas->fArc);

                nas->arcnf.push_back(nas->fArc);
                auxK = i - fDepot;
                nas->arcV[i][j].push_back(auxK);
            }
        }

    }

    for (int i = 0; i < inst->n; i++){//i is a passenger node
        for(int j = 0; j < inst->n; j++){// j is a passenger req
            if(i != j){
                double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j 
                ttij = std::round(ttij * multiplier) / multiplier;
                //if lowest time for req i + travel time from i to j is lower or equal to
                //the latest point in time to serve request j. If latest time == T, it is always valid                        

                if (nodeVec[i].e + ttij < nodeVec[j].l){
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);

                    nas->arcNN.push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);

                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        nas->arcV[i][j].push_back(k);
                    }                        
                }
           }
        }

        for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
            nas->arcs[i][j] = true;
            nas->fArc.first = i;
            nas->fArc.second = j;
            nas->arcMinus[j].push_back(nas->fArc);
            nas->arcPlus[i].push_back(nas->fArc);

            nas->arcNplus.push_back(nas->fArc);

            nas->allArcs.push_back(nas->fArc);
            auxK = j - inst->V;
            nas->arcV[i][j].push_back(auxK);
        }
    }

    if (problem->p3 > 0){ // arcs for baseline scenarios
        //passenger-passenger
        if (problem->p1 > 0){ //if BL2
            for (int i = 0; i < inst->n; i++){ //i is a passenger request
                for (int j = inst->n; j < inst->n + inst->m; j++){// j is a parcel pu req
                    double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j
                    ttij = std::round(ttij * multiplier) / multiplier;
                    //if lowest time for req i + travel time from i to j is lower or equal to
                    //the latest point in time to serve request j. If latest time == T, it is always valid                        
                    if (nodeVec[i].e + ttij < nodeVec[j].l){                
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);

                        nas->arcNplus.push_back(nas->fArc);

                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                        for (int k = 0; k < inst->K; k++){
                            nas->arcV[i][j].push_back(k);
                        }       
                    }                 
                }                
            }
        }
        //parcelpu-parcel
        for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
            for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
                if (j + inst->m != i && i != j && i + inst->m != j){
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    
                    if (j < inst->n + inst->m){
                        nas->arcPP.push_back(nas->fArc);
                    }

                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        nas->arcV[i][j].push_back(k);
                    } 
                }
            }
        }

        //parceldl-parcel
        for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node
            for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
                if (i != j && j != i - inst->m){//different nodes; no dl to its pickup
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        nas->arcV[i][j].push_back(k);
                    } 
                }
            }
            if (problem->p1 > 0){ //if BL2
                for (int j = 0; j < inst->n; j++){//j is a passenger node
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);

                    nas->arcPN.push_back(nas->fArc);

                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        nas->arcV[i][j].push_back(k);
                    }  
                } 
            }

            for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                auxK = j - inst->V;
                nas->arcV[i][j].push_back(auxK);
            } 
        }

        if (problem->dParcel > 0){//direct parcel delivery

            for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
                int j = i + inst->m; //j is i's delivery location

                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                nas->arcPD.push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                nas->arcnf.push_back(nas->fArc);
                for (int k = 0; k < inst->K; k++){
                    nas->arcV[i][j].push_back(k);
                }                                                
            }
        }
    }

    else{

        for (int i = 0; i < inst->n; i++){//i is a passenger node
            for(int j = inst->n; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
                double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j
                ttij = std::round(ttij * multiplier) / multiplier;
                //if lowest time for req i + travel time from i to j is lower or equal to
                //the latest point in time to serve request j. If latest time == T, it is always valid                        
                if (nodeVec[i].e + ttij < nodeVec[j].l){
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);

                    nas->arcNplus.push_back(nas->fArc);

                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        nas->arcV[i][j].push_back(k);
                    }
                }
            }
        }

        for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node           
            for (int j = 0; j < inst->n; j++){ //j is a passenger node
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);
                
                nas->arcPN.push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                nas->arcnf.push_back(nas->fArc);
                for (int k = 0; k < inst->K; k++){
                    nas->arcV[i][j].push_back(k);
                }
            }      
        }

        for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node           
            for (int j = 0; j < inst->n; j++){ //j is a passenger node
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                // if (j < inst->n){//j is a passenger node
                //     nas->arcPN.push_back(nas->fArc);
                // }

                nas->allArcs.push_back(nas->fArc);
                nas->arcnf.push_back(nas->fArc);
                for (int k = 0; k < inst->K; k++){
                    nas->arcV[i][j].push_back(k);
                }

            }

            for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                auxK = j - inst->V;
                nas->arcV[i][j].push_back(auxK);
            }
        }

        //%%%%%%%%%%%%%%%%%%%%%%%
        //specific cases
        //%%%%%%%%%%%%%%%%%%%%%%%

        if (problem->p2 > 0){ //multi parcel
            for (int i = inst->n; i < inst->n + 2*inst->m; i++){//i is a parcel pu or dl node                   
                for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
                    if (j + inst->m != i && i != j && i + inst->m != j){ // no dl to its pu; no pu to its dl; not a node to itself
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);

                        //if (j < inst->n + inst->m){
                        //    nas->arcPP.push_back(nas->fArc);
                        //}
                        nas->arcPP.push_back(nas->fArc);

                        
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                        for (int k = 0; k < inst->K; k++){
                            nas->arcV[i][j].push_back(k);
                        }
                    }                    
                }
            }
        }
        else { //single parcel 
            for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node                   
                for (int j = inst->n; j < inst->n + inst->m; j++){//j is a parcel pu node
                    if (j + inst->m != i){ // no dl to its pu; no pu to its dl; not a node to itself
                        nas->arcs[i][j] = true;
                        nas->fArc.first = i;
                        nas->fArc.second = j;
                        nas->arcMinus[j].push_back(nas->fArc);
                        nas->arcPlus[i].push_back(nas->fArc);

                        if (j < inst->n + inst->m){
                            nas->arcPP.push_back(nas->fArc);
                        }                        
    
                        nas->allArcs.push_back(nas->fArc);
                        nas->arcnf.push_back(nas->fArc);
                        for (int k = 0; k < inst->K; k++){
                            nas->arcV[i][j].push_back(k);
                        }
                    }                    
                }
            }
        }
        if (problem->dParcel > 0){//direct parcel delivery

            for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
                int j = i + inst->m; //j is i's delivery location

                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                nas->arcPD.push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                nas->arcnf.push_back(nas->fArc);
                for (int k = 0; k < inst->K; k++){
                    nas->arcV[i][j].push_back(k);
                }                                                
            }
        }
    }

    for (int a = 0; a < nas->allArcs.size(); a++){
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;

        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            nas->vArcPlus[i][k].push_back(nas->allArcs[a]);
            nas->vArcMinus[j][k].push_back(nas->allArcs[a]);
        }

    }
}

void fipPassSol(instanceStat *inst, fipStats *fipStat) {
    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;

    string filename = "src/Aux/fippassSol/" + inst->InstName + ".txt";

	ifstream iFile(filename);

    double trash;
    iFile >> trash;
    iFile >> trash;

	int nMandatory;
	iFile >> nMandatory;
    std::set<int> emptyRoutes;
    vector<pair<int, int>> nodeLocal;
    vector<int> sequencePairs;
    vector<int> NodeVehicle;

    sequencePairs.clear();
    NodeVehicle.clear();
    sequencePairs.resize(inst->n + 2*inst->m + 2*inst->K, -1);
    NodeVehicle.resize(inst->n + 2*inst->m + 2*inst->K, -1);

    for (int k = 0; k < inst->K; k++) {
        fipStat->solPass.push_back(vector<int>());
    }

    for (int i = 0; i < nMandatory; i++) {
		int u;
		int v;

		iFile >> u >> v;

		emptyRoutes.insert(v - inst->V);
	}

    int nSolNodes;
	iFile >> nSolNodes;

	for (int i = 0; i < nSolNodes; i++) {
		int u;
		int k;

		iFile >> u >> k;

        fipStat->solPass[k].push_back(u);

		nodeLocal.push_back(make_pair(u, k));
        NodeVehicle[u] = k;
	}

	int nSequencePairs;
	iFile >> nSequencePairs;

	for (int i = 0; i < nSequencePairs; i++) {
		int u;
		int v;

        iFile >> u >> v;

		sequencePairs[u] = v;
	}
	iFile.close();
}

void fipPassSolBundles(instanceStat *inst, fipStats *fipStat) {
    int fDepot = inst->n + 2*inst->m;
    int fDummy = inst->n + 2*inst->m + inst->K;

    string filename = "src/Aux/bundleSol/" + inst->InstName + ".txt";

	ifstream iFile(filename);

    double trash;
    iFile >> fipStat->solprofit;

	int nRoutes;
    iFile >> nRoutes;

    for (int i = 0; i < nRoutes; i++) {
        int nElements;
        iFile >> nElements;

        fipStat->solPass.push_back(vector<int>());

        for (int j = 0; j < nElements; j++) {
            int newElement;
            iFile >> newElement;

            fipStat->solPass[i].push_back(newElement);
        }
    }

	iFile.close();
}

// void mfeasibleArcs (instanceStat *inst, nodeArcsStruct *nas, probStat* problem, vector<nodeStat> &nodeVec, double **mdist, fipStats *fipStat){
//     int auxK;

//     int fDepot = inst->n + 2*inst->m;
//     int fDummy = inst->n + 2*inst->m + inst->K;

//     int decimalPlaces = 4;
//     double multiplier = std::pow(10, decimalPlaces);

//     string filename = "src/fippassSol/" + inst->InstName + ".txt";

// 	ifstream iFile(filename);

// 	int nMandatory;
// 	iFile >> nMandatory;
//     std::set<int> emptyRoutes;
//     vector<pair<int, int>> nodeLocal;
//     vector<int> sequencePairs;
//     vector<int> NodeVehicle;

//     sequencePairs.clear();
//     NodeVehicle.clear();
//     sequencePairs.resize(inst->n + 2*inst->m + 2*inst->K, -1);
//     NodeVehicle.resize(inst->n + 2*inst->m + 2*inst->K, -1);

//     int nSolNodes;
// 	iFile >> nSolNodes;

// 	for (int i = 0; i < nSolNodes; i++) {
// 		int u;
// 		int k;

// 		iFile >> u >> k;

//         if (fipStat->solPass.size() <= k) {
//             fipStat->solPass.push_back(vector<int>());
//         }

//         fipStat->solPass[k].push_back(u);

// 		nodeLocal.push_back(make_pair(u, k));
//         NodeVehicle[u] = k;
// 	}

// 	for (int i = 0; i < nMandatory; i++) {
// 		int u;
// 		int v;

// 		iFile >> u >> v;

//         fipStat->solPass.push_back(vector<int>());
//         fipStat->solPass[u - fDepot].push_back(u);
//         fipStat->solPass[u - fDepot].push_back(v);

// 		emptyRoutes.insert(v - inst->V);
// 	}

// 	int nSequencePairs;
// 	iFile >> nSequencePairs;

// 	for (int i = 0; i < nSequencePairs; i++) {
// 		int u;
// 		int v;

//         iFile >> u >> v;

// 		sequencePairs[u] = v;
// 	}

// 	iFile.close();

//     // Only one arc is allowed between the elements in the first solution

//     for (int i = 0; i < nodeLocal.size(); i++) {
//         int u = nodeLocal[i].first;
//         int v = sequencePairs[u];
//         int k = nodeLocal[i].second;

//         if (v == -1) continue;

//         nas->arcs[u][v] = true;
//         nas->fArc.first = u;
//         nas->fArc.second = v;
//         nas->arcMinus[v].push_back(nas->fArc);
//         nas->arcPlus[u].push_back(nas->fArc);
//         nas->allArcs.push_back(nas->fArc);

//         nas->arcV[u][v].push_back(k);
//     }

//     // In empty routes, we should ensure arc between depot and dummy depot

//     for (auto k : emptyRoutes) {
//         int u = k + fDepot;
//         int v = k + fDummy;

//         nas->arcs[u][v] = true;
//         nas->fArc.first = u;
//         nas->fArc.second = v;
//         nas->arcMinus[v].push_back(nas->fArc);
//         nas->arcPlus[u].push_back(nas->fArc);
//         nas->allArcs.push_back(nas->fArc);

//         nas->arcV[u][v].push_back(k);

//         if (u < inst->n && v < inst->n) {
//             nas->arcNN.push_back(nas->fArc);
//             nas->arcNplus.push_back(nas->fArc);
//             nas->arcnf.push_back(nas->fArc);
//         } else if (u < inst->n) {
//             nas->arcNplus.push_back(nas->fArc);
//         }
//     }

//     // Ensure arcs to parcel in non empty routes

//     for(int i = inst->n + 2*inst->m; i < inst->V; i++){//i is a starting point

//         if (emptyRoutes.find(i - fDepot) != emptyRoutes.end()) continue;

//         // int l = i + inst->K; //its dummy node

//         // nas->arcs[i][l] = true;
//         // nas->fArc.first = i;
//         // nas->fArc.second = l;
//         // nas->arcMinus[l].push_back(nas->fArc);
//         // nas->arcPlus[i].push_back(nas->fArc);
//         // nas->allArcs.push_back(nas->fArc);

//         // auxK = l - inst->V;
//         // nas->arcV[i][l].push_back(auxK);
        
//         // for (int j = 0; j < inst->n; j++){ //j is a passenger node

//         //     double ttij = mdist[i][j]/inst->vmed;
//         //     ttij = std::round(ttij * multiplier) / multiplier;

//         //     if (nodeVec[i].e + ttij < nodeVec[j].l){
//         //         nas->arcs[i][j] = true;
//         //         nas->fArc.first = i;
//         //         nas->fArc.second = j;
//         //         nas->arcMinus[j].push_back(nas->fArc);
//         //         nas->arcPlus[i].push_back(nas->fArc);
//         //         nas->allArcs.push_back(nas->fArc);

//         //         nas->arcnf.push_back(nas->fArc);
//         //         auxK = i - fDepot;
//         //         nas->arcV[i][j].push_back(auxK);
//         //     }         
//         // }
//         for (int j = inst->n; j < inst->n + inst->m; j++){ //j is a parcel pu node
//             double ttij = mdist[i][j]/inst->vmed;
//             ttij = std::round(ttij * multiplier) / multiplier;
//             if (ttij < inst->T){
//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);
//                 nas->allArcs.push_back(nas->fArc);

//                 nas->arcnf.push_back(nas->fArc);
//                 auxK = i - fDepot;
//                 nas->arcV[i][j].push_back(auxK);
//             }
//         }
//     }

//     // for (int i = 0; i < inst->n; i++){//i is a passenger node
//     //     for(int j = 0; j < inst->n; j++){// j is a passenger req
//     //         if(i != j){
//     //             double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j 
//     //             ttij = std::round(ttij * multiplier) / multiplier;
//     //             //if lowest time for req i + travel time from i to j is lower or equal to
//     //             //the latest point in time to serve request j. If latest time == T, it is always valid                        

//     //             if (nodeVec[i].e + ttij < nodeVec[j].l){
//     //                 nas->arcs[i][j] = true;
//     //                 nas->fArc.first = i;
//     //                 nas->fArc.second = j;
//     //                 nas->arcMinus[j].push_back(nas->fArc);
//     //                 nas->arcPlus[i].push_back(nas->fArc);

//     //                 nas->arcNN.push_back(nas->fArc);
//     //                 nas->arcNplus.push_back(nas->fArc);

//     //                 nas->allArcs.push_back(nas->fArc);
//     //                 nas->arcnf.push_back(nas->fArc);
//     //                 for (int k = 0; k < inst->K; k++){
//     //                     nas->arcV[i][j].push_back(k);
//     //                 }                        
//     //             }
//     //        }
//     //     }

//     //     for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
//     //         nas->arcs[i][j] = true;
//     //         nas->fArc.first = i;
//     //         nas->fArc.second = j;
//     //         nas->arcMinus[j].push_back(nas->fArc);
//     //         nas->arcPlus[i].push_back(nas->fArc);

//     //         nas->arcNplus.push_back(nas->fArc);

//     //         nas->allArcs.push_back(nas->fArc);
//     //         auxK = j - inst->V;
//     //         nas->arcV[i][j].push_back(auxK);
//     //     }
//     // }

//     if (problem->p3 > 0){ // arcs for baseline scenarios
//         //passenger-passenger
//         if (problem->p1 > 0){ //if BL2
//             for (int i = 0; i < inst->n; i++){ //i is a passenger request
//                 for (int j = inst->n; j < inst->n + inst->m; j++){// j is a parcel pu req
//                     double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j
//                     ttij = std::round(ttij * multiplier) / multiplier;
//                     //if lowest time for req i + travel time from i to j is lower or equal to
//                     //the latest point in time to serve request j. If latest time == T, it is always valid                        
//                     if (nodeVec[i].e + ttij < nodeVec[j].l){                
//                         nas->arcs[i][j] = true;
//                         nas->fArc.first = i;
//                         nas->fArc.second = j;
//                         nas->arcMinus[j].push_back(nas->fArc);
//                         nas->arcPlus[i].push_back(nas->fArc);

//                         nas->arcNplus.push_back(nas->fArc);

//                         nas->allArcs.push_back(nas->fArc);
//                         nas->arcnf.push_back(nas->fArc);

//                         nas->arcV[i][j].push_back(NodeVehicle[i]);
//                     }                 
//                 }                
//             }
//         }
//         // parcelpu-parcel
//         for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
//             for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
//                 if (j + inst->m != i && i != j && i + inst->m != j){
//                     nas->arcs[i][j] = true;
//                     nas->fArc.first = i;
//                     nas->fArc.second = j;
//                     nas->arcMinus[j].push_back(nas->fArc);
//                     nas->arcPlus[i].push_back(nas->fArc);
                    
//                     if (j < inst->n + inst->m){
//                         nas->arcPP.push_back(nas->fArc);
//                     }

//                     nas->allArcs.push_back(nas->fArc);
//                     nas->arcnf.push_back(nas->fArc);
//                     for (int k = 0; k < inst->K; k++){
//                         if (emptyRoutes.find(k) != emptyRoutes.end()) continue;
//                         nas->arcV[i][j].push_back(k);
//                     } 
//                 }
//             }
//         }

//         //parceldl-parcel
//         for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node
//             for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
//                 if (i != j && j != i - inst->m){//different nodes; no dl to its pickup
//                     nas->arcs[i][j] = true;
//                     nas->fArc.first = i;
//                     nas->fArc.second = j;
//                     nas->arcMinus[j].push_back(nas->fArc);
//                     nas->arcPlus[i].push_back(nas->fArc);
//                     nas->allArcs.push_back(nas->fArc);
//                     nas->arcnf.push_back(nas->fArc);
//                     for (int k = 0; k < inst->K; k++){
//                         if (emptyRoutes.find(i - fDepot) != emptyRoutes.end()) continue;
//                         nas->arcV[i][j].push_back(k);
//                     } 
//                 }
//             }
//             if (problem->p1 > 0){ //if BL2
//                 for (int j = 0; j < inst->n; j++){//j is a passenger node
//                     nas->arcs[i][j] = true;
//                     nas->fArc.first = i;
//                     nas->fArc.second = j;
//                     nas->arcMinus[j].push_back(nas->fArc);
//                     nas->arcPlus[i].push_back(nas->fArc);

//                     nas->arcPN.push_back(nas->fArc);

//                     nas->allArcs.push_back(nas->fArc);
//                     nas->arcnf.push_back(nas->fArc);

//                     nas->arcV[i][j].push_back(NodeVehicle[j]);
//                 } 
//             }

//             for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
//                 if (emptyRoutes.find(j - inst->V) != emptyRoutes.end()) continue;

//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);

//                 nas->allArcs.push_back(nas->fArc);
//                 auxK = j - inst->V;
//                 nas->arcV[i][j].push_back(auxK);
//             } 
//         }

//         if (problem->dParcel > 0){//direct parcel delivery

//             for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
//                 int j = i + inst->m; //j is i's delivery location

//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);

//                 nas->arcPD.push_back(nas->fArc);

//                 nas->allArcs.push_back(nas->fArc);
//                 nas->arcnf.push_back(nas->fArc);
//                 for (int k = 0; k < inst->K; k++){
//                     if (emptyRoutes.find(k) != emptyRoutes.end()) continue;
//                     nas->arcV[i][j].push_back(k);
//                 }                                                
//             }
//         }
//     }

//     else{

//         for (int i = 0; i < inst->n; i++){//i is a passenger node
//             for(int j = inst->n; j < inst->n + 2*inst->m; j++){// j is a parcel req (pu or del)
//                 double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j
//                 ttij = std::round(ttij * multiplier) / multiplier;
//                 //if lowest time for req i + travel time from i to j is lower or equal to
//                 //the latest point in time to serve request j. If latest time == T, it is always valid                        
//                 if (nodeVec[i].e + ttij < nodeVec[j].l){
//                     nas->arcs[i][j] = true;
//                     nas->fArc.first = i;
//                     nas->fArc.second = j;
//                     nas->arcMinus[j].push_back(nas->fArc);
//                     nas->arcPlus[i].push_back(nas->fArc);

//                     nas->arcNplus.push_back(nas->fArc);

//                     nas->allArcs.push_back(nas->fArc);
//                     nas->arcnf.push_back(nas->fArc);

//                     nas->arcV[i][j].push_back(NodeVehicle[i]);
//                 }
//             }
//         }

//         for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node           
//             for (int j = 0; j < inst->n; j++){ //j is a passenger node
//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);
                
//                 nas->arcPN.push_back(nas->fArc);

//                 nas->allArcs.push_back(nas->fArc);
//                 nas->arcnf.push_back(nas->fArc);

//                 nas->arcV[i][j].push_back(NodeVehicle[j]);
//             }      
//         }

//         for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node           
//             for (int j = 0; j < inst->n; j++){ //j is a passenger node
//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);

//                 // if (j < inst->n){//j is a passenger node
//                 //     nas->arcPN.push_back(nas->fArc);
//                 // }

//                 nas->allArcs.push_back(nas->fArc);
//                 nas->arcnf.push_back(nas->fArc);

//                 nas->arcV[i][j].push_back(NodeVehicle[j]);
//             }

//             for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node

//                 if (emptyRoutes.find(j - inst->V) != emptyRoutes.end()) continue;

//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);

//                 nas->allArcs.push_back(nas->fArc);
//                 auxK = j - inst->V;
//                 nas->arcV[i][j].push_back(auxK);
//             }
//         }

//         //%%%%%%%%%%%%%%%%%%%%%%%
//         //specific cases
//         //%%%%%%%%%%%%%%%%%%%%%%%

//         if (problem->p2 > 0){ //multi parcel
//             for (int i = inst->n; i < inst->n + 2*inst->m; i++){//i is a parcel pu or dl node                   
//                 for (int j = inst->n; j < inst->n + 2*inst->m; j++){//j is a parcel pu or dl node
//                     if (j + inst->m != i && i != j && i + inst->m != j){ // no dl to its pu; no pu to its dl; not a node to itself
//                         nas->arcs[i][j] = true;
//                         nas->fArc.first = i;
//                         nas->fArc.second = j;
//                         nas->arcMinus[j].push_back(nas->fArc);
//                         nas->arcPlus[i].push_back(nas->fArc);

//                         //if (j < inst->n + inst->m){
//                         //    nas->arcPP.push_back(nas->fArc);
//                         //}
//                         nas->arcPP.push_back(nas->fArc);

                        
//                         nas->allArcs.push_back(nas->fArc);
//                         nas->arcnf.push_back(nas->fArc);
//                         for (int k = 0; k < inst->K; k++){
//                             if (emptyRoutes.find(k) != emptyRoutes.end()) continue;
//                             nas->arcV[i][j].push_back(k);
//                         }
//                     }                    
//                 }
//             }
//         }
//         else { //single parcel 
//             for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++){//i is a parcel dl node                   
//                 for (int j = inst->n; j < inst->n + inst->m; j++){//j is a parcel pu node
//                     if (j + inst->m != i){ // no dl to its pu; no pu to its dl; not a node to itself
//                         nas->arcs[i][j] = true;
//                         nas->fArc.first = i;
//                         nas->fArc.second = j;
//                         nas->arcMinus[j].push_back(nas->fArc);
//                         nas->arcPlus[i].push_back(nas->fArc);

//                         if (j < inst->n + inst->m){
//                             nas->arcPP.push_back(nas->fArc);
//                         }                        
    
//                         nas->allArcs.push_back(nas->fArc);
//                         nas->arcnf.push_back(nas->fArc);
//                         for (int k = 0; k < inst->K; k++){
//                             if (emptyRoutes.find(k) != emptyRoutes.end()) continue;
//                             nas->arcV[i][j].push_back(k);
//                         }
//                     }                    
//                 }
//             }
//         }
//         if (problem->dParcel > 0){//direct parcel delivery

//             for (int i = inst->n; i < inst->n + inst->m; i++){//i is a parcel pu node
//                 int j = i + inst->m; //j is i's delivery location

//                 nas->arcs[i][j] = true;
//                 nas->fArc.first = i;
//                 nas->fArc.second = j;
//                 nas->arcMinus[j].push_back(nas->fArc);
//                 nas->arcPlus[i].push_back(nas->fArc);

//                 nas->arcPD.push_back(nas->fArc);

//                 nas->allArcs.push_back(nas->fArc);
//                 nas->arcnf.push_back(nas->fArc);
//                 for (int k = 0; k < inst->K; k++){
//                     if (emptyRoutes.find(k) != emptyRoutes.end()) continue;
//                     nas->arcV[i][j].push_back(k);
//                 }                                                
//             }
//         }
//     }

//     for (int a = 0; a < nas->allArcs.size(); a++){
//         int i = nas->allArcs[a].first;
//         int j = nas->allArcs[a].second;

//         for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
//             int k = nas->arcV[i][j][k1];
//             nas->vArcPlus[i][k].push_back(nas->allArcs[a]);
//             nas->vArcMinus[j][k].push_back(nas->allArcs[a]);
//         }

//     }
// }

void viewSol (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat){
    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    vector<int> auxSolOrder;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int currSP;
    vector<int> orderVec;

	// solStatIni(sStat);

    for (int k = 0; k < inst->K; k++){
        sStat->solOrder.push_back(auxSolOrder);
    }

    // if (sStat->fipstage > 0){
    //     for (int k = 0; k < inst->K; k++){
    //         currSP = 2*inst->n + 2*inst->m + k;

    //         cout  << "currentsp: " << currSP << " - first pass: " << sStat->solPass[k][0] << endl;

    //         sStat->solOrder[k].push_back(currSP);
    //         sStat->solOrder[k].push_back(sStat->solPass[k][0]);


    //         // auxVec.push_back(auxPair);

    //         for (int i = 0; i < sStat->solvec[k].size(); i++){
    //             auxPair.first = sStat->solvec[k][i].first;
    //             auxPair.second = sStat->solvec[k][i].second;            
    //             auxVec.push_back(auxPair);
    //         }
    //         cout << "here1";
    //         getchar();
    //         cout  << "auxVec: " << endl;
    //         for (int i = 0; i < auxVec.size(); i++){
    //             cout  << auxVec[i].first << " " << auxVec[i].second << endl;
    //         }

    //         while(!auxVec.empty()){
    //             if (sStat->solOrder[k].empty()){

    //                 for (int i = 0; i < auxVec.size(); i++){
    //                     if (auxVec[i].first == sStat->solPass[k][0]){
    //                         // sStat->solOrder[k].push_back(auxVec[i].first);
    //                         sStat->solOrder[k].push_back(auxVec[i].second);

    //                         auxVec.erase(auxVec.begin()+i);
    //                     }
    //                 }
    //             }
    //             else{
    //                 for (int j = 0; j < auxVec.size(); j++){
    //                     if(auxVec[j].first == sStat->solOrder[k].back()){
    //                         sStat->solOrder[k].push_back(auxVec[j].second);

    //                         auxVec.erase(auxVec.begin()+j);
    //                     }
    //                 }
    //             }       
    //         }
    //         sStat->solOrder[k].push_back(currSP + inst->K + k);
    //     }

    //     cout  << "\nNumber of Vehicles: " << inst->K << endl;

    //     cout  << "\nSolution: " << endl;
    //     for (int k = 0; k < inst->K; k++){
    //         cout  << "Vehicle " << k << ": ";
    //         for (int i = 0; i < sStat->solOrder[k].size(); i++){
    //             if (i < sStat->solOrder[k].size() - 1){
    //                 cout  << sStat->solOrder[k][i] << " - ";
    //             }
    //             else{
    //                 cout  << sStat->solOrder[k][i];
    //             }
    //         }
    //         cout  << endl;
    //     }
    //     cout  << endl;

    // }
    // else{
        for (int k = 0; k < inst->K; k++){
            currSP = inst->V - inst->K + k;

            for (int i = 0; i < sStat->solvec[k].size(); i++){
                auxPair.first = sStat->solvec[k][i].first;
                auxPair.second = sStat->solvec[k][i].second;            
                auxVec.push_back(auxPair);
            }
            cout << "here1";
            // getchar();
            cout  << "auxVec: " << endl;
            // for (int i = 0; i < auxVec.size(); i++){
            //     cout  << auxVec[i].first << " " << auxVec[i].second << endl;
            // }

            while(!auxVec.empty()){
                if (sStat->solOrder[k].empty()){

                    for (int i = 0; i < auxVec.size(); i++){
                        if (auxVec[i].first == currSP){
                            sStat->solOrder[k].push_back(auxVec[i].first);
                            sStat->solOrder[k].push_back(auxVec[i].second);

                            auxVec.erase(auxVec.begin()+i);

                        }
                    }
                }
                else{

                    for (int j = 0; j < auxVec.size(); j++){
                        if(auxVec[j].first == sStat->solOrder[k].back()){
                            sStat->solOrder[k].push_back(auxVec[j].second);

                            auxVec.erase(auxVec.begin()+j);
                        }
                    }
                }       
            cout << "auxvec size: " << auxVec.size();
            // getchar();
            }
            cout << "here3";
            // getchar();
        }

        cout  << "\nNumber of Vehicles: " << inst->K << endl;

        cout  << "\nSolution: " << endl;
        for (int k = 0; k < inst->K; k++){
            cout  << "Vehicle " << k << ": ";
            for (int i = 0; i < sStat->solOrder[k].size(); i++){
                if (i < sStat->solOrder[k].size() - 1){
                    cout  << sStat->solOrder[k][i] << " - ";
                }
                else{
                    cout  << sStat->solOrder[k][i];
                }
            }
            cout  << " - Total time: " << sStat->solBegin[sStat->solOrder[k][sStat->solOrder[k].size()-2]] - sStat->solBegin[sStat->solOrder[k][0]] << endl;
        }
        cout  << endl;

        // disregard if fip
        cout  << "\nSolution structure: " << endl;
        for (int k = 0; k < inst->K; k++){
            cout  << "Vehicle " << k << ": ";
            for (int i = 0; i < sStat->solOrder[k].size(); i++){
                if (i < sStat->solOrder[k].size() - 1){
                    if (sStat->solOrder[k][i] < inst->n){
                        cout  << "d" << " - ";
                    }
                    else if (sStat->solOrder[k][i] < inst->n + inst->m){
                        cout  << "P" << " - ";
                        sStat->servedParcels++;
                    }
                    else if (sStat->solOrder[k][i] < inst->n + 2*inst->m){
                        cout  << "D" << " - ";
                    }
                    else if (sStat->solOrder[k][i] < inst->n + 2*inst->m + inst->K){
                        cout  << "S" << " - ";
                    }                                      
                }
                else{

                    cout  << "f";
                }
            }
            cout  << endl;
        }
        cout  << endl;   
        // getchar(); 
    // }
}

void printSolFile (instanceStat *inst, solStats *sStat, probStat* problem, bool hasParcel){

    if (problem->model == "fip" && !hasParcel) {
        string filename = "src/Aux/fippassSol/" + inst->InstName + ".txt";

        // TODO UNCOMMENT //  << filename << endl;

        ofstream oFile(filename);

        vector<pair<int, int>> emptyRoutes;
        vector<pair<int, int>> nodeInRoute;
        vector<pair<int, int>> sequencePairs;

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            if (sStat->solOrder[k].size() < 3) {
                int u = sStat->solOrder[k][0] - inst->n;
                int v = sStat->solOrder[k][1] - inst->n;

                emptyRoutes.push_back(make_pair(u, v));
            }
        }

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            for (int i = 0; i < sStat->solOrder[k].size(); i++) {
                int u = sStat->solOrder[k][i];

                if (u < inst->n) { 
                    nodeInRoute.push_back(make_pair(u, k));
                }

                if (u >= 2*inst->n + 2*inst->m) {
                    nodeInRoute.push_back(make_pair(u - inst->n, k));
                }
            }
        }

        for (int k = 0; k < sStat->solOrder.size(); k++) {
            for (int i = 0; i < sStat->solOrder[k].size() - 1; i++) {
                int u = sStat->solOrder[k][i];
                int v = sStat->solOrder[k][i + 1];

                if (u < inst->n) continue;
                if (u >= inst->n) {
                    u -= inst->n;
                }

                if (v >= inst->n) {
                    v -= inst->n;
                }

                sequencePairs.push_back(make_pair(u, v));
            }
        }

        oFile << sStat->time << endl;
        oFile << sStat->solprofit << endl;

        oFile << emptyRoutes.size() << endl;
        for (auto arcPair : emptyRoutes) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile << nodeInRoute.size() << endl;
        for (auto arcPair : nodeInRoute) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile << sequencePairs.size() << endl;
        for (auto arcPair : sequencePairs) {
            oFile << arcPair.first << " " << arcPair.second << endl;
        }

        oFile.close();
    }


    // // TODO UNCOMMENT //  << "\nNumber of Vehicles: " << inst->K << endl;

    // // TODO UNCOMMENT //  << "\nSolution: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < sStat->solOrder[k].size(); i++){
    //         if (i < sStat->solOrder[k].size() - 1){
    //             // TODO UNCOMMENT //  << sStat->solOrder[k][i] << " - ";
    //         }
    //         else{
    //             // TODO UNCOMMENT //  << sStat->solOrder[k][i];
    //         }
    //     }
    //     // TODO UNCOMMENT //  << " - Total time: " << sStat->solBegin[sStat->solOrder[k][sStat->solOrder[k].size()-2]] - sStat->solBegin[sStat->solOrder[k][0]] << endl;
    // }
    // // TODO UNCOMMENT //  << endl;

    // // disregard if fip
    // // TODO UNCOMMENT //  << "\nSolution structure: " << endl;
    // for (int k = 0; k < inst->K; k++){
    //     // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
    //     for (int i = 0; i < sStat->solOrder[k].size(); i++){
    //         if (i < sStat->solOrder[k].size() - 1){
    //             if (sStat->solOrder[k][i] < inst->n){
    //                 // TODO UNCOMMENT //  << "d" << " - ";
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + inst->m){
    //                 // TODO UNCOMMENT //  << "P" << " - ";
    //                 sStat->servedParcels++;
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + 2*inst->m){
    //                 // TODO UNCOMMENT //  << "D" << " - ";
    //             }
    //             else if (sStat->solOrder[k][i] < inst->n + 2*inst->m + inst->K){
    //                 // TODO UNCOMMENT //  << "S" << " - ";
    //             }                                      
    //         }
    //         else{

    //             // TODO UNCOMMENT //  << "f";
    //         }
    //     }
    //     // TODO UNCOMMENT //  << endl;
    // }
    // // TODO UNCOMMENT //  << endl; 
    
    string filename2;
    
    if (problem->model == "fip" && !hasParcel)
    {
        filename2 = "src/Results/fippassResults/" + inst->InstName + ".csv";
    }
    else if (problem->model == "fip" && hasParcel)
    {
        filename2 = "src/Results/fipResults/" + inst->InstName + ".csv";
    }
    else if (problem->model == "nodefip")
    {
        filename2 = "src/Results/mnodefipresults/" + inst->InstName + ".csv"; 
    } else if (problem->model == "bundle4") {
        filename2 = "src/Results/newBundlefip1/" + inst->InstName + ".csv"; 
    }
    
    ofstream oFile2(filename2);

    oFile2 << inst->InstName + ",";
    oFile2 << sStat->time;
    oFile2 << ",";
    oFile2 << sStat->solDual;
    oFile2 << ",";
    oFile2 << sStat->solprofit;
    oFile2 << ",";
    if (problem->model == "fip") {
        oFile2 << 0;
    } else if (problem->model == "nodefip" || problem->model == "bundle4"){
        oFile2 << sStat->servedParcels;
    }
    oFile2 << ",";
    oFile2 << sStat->tNone;
    oFile2 << ",";
    oFile2 << sStat->dNone;
    oFile2 << ",";
    oFile2 << sStat->status;

    oFile2.close();

    // oFile2 << inst->InstName + ", ";
    // oFile2 << sStat->
}

void printSolFileFip1 (instanceStat *inst, solStats *sStat, probStat* problem, bool hasParcel, fipStats *fipStat){
    
    string filename2;
    
    filename2 = "src/Results/fipResults/" + inst->InstName + ".csv";
    
    ofstream oFile2(filename2);

    oFile2 << inst->InstName + ",";
    oFile2 << fipStat->time;
    oFile2 << ",";
    oFile2 << sStat->solDual;
    oFile2 << ",";
    oFile2 << sStat->solprofit;
    oFile2 << ",";
    oFile2 << fipStat->servedParcels;
    oFile2 << ",";
    oFile2 << fipStat->tNone;
    oFile2 << ",";
    oFile2 << fipStat->dNone;
    oFile2 << ",";
    oFile2 << sStat->status;

    oFile2.close();
}

void testRoute (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec){
    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    vector<int> auxSolOrder;

    int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int currSP;
    vector<int> orderVec;

    vector<int> infroute {0 ,4, 3, 0 };
	// solStatIni(sStat);
    vector<int> infroutenew;
    
    int newindex, dummydepot;

    double travel_time = 0, duration = 0, startPoint = 0, endPoint = 0, lpasse = 0, fpasse = 0, accTime;

    for(int i = 0; i < infroute.size() - 1; i++){
        if (infroute[i] < inst->K){
            newindex = inst->n + 2*inst->m + infroute[i];
            dummydepot = newindex + infroute[i];
            infroutenew.push_back(newindex);
        }
        else{
            newindex = infroute[i] - inst->K;
            infroutenew.push_back(newindex);
        }
    }

    infroutenew.push_back(dummydepot);

    int a = infroutenew[0];
    int b = infroutenew[1];

    accTime = mdist[a][b]/inst->vmed;
    accTime = std::round(accTime * multiplier) / multiplier;
    travel_time += accTime;
    bool firstPass = false;

    for (int i = 1; i < infroutenew.size() - 2; i++){
        int a = infroutenew[i];
        int b = infroutenew[i+1];
        double travTime = mdist[a][b]/inst->vmed;
        travTime = std::round(travTime * multiplier) / multiplier;
        travel_time += travTime + nodeVec[a].delta;

        if (!firstPass){
            if (a < inst->n){
                fpasse = nodeVec[a].e;
                firstPass = true;
            }
            else{                 
                
                accTime += travTime + inst->service;
            }
        }
    }
    b = infroutenew[infroutenew.size() - 2];
    travel_time += nodeVec[b].delta;
    startPoint = fpasse - accTime;
    accTime = 0;

    for (int i = infroutenew.size() - 2; i > 0; i--){
        int a = infroutenew[i];
        int b = infroutenew[i-1];

        //if (a < inst->n){
        //    fpasse = nodeVec[a].e;
        //    break;
        //}
        //else{
        //    accTime += mdist[b][a]/inst->vmed + inst->service;
        //}
        
        if (a < inst->n){
            lpasse = nodeVec[a].e + nodeVec[a].delta;
            break;
        }
        else{
            double reverseTT = mdist[b][a]/inst->vmed;
            reverseTT = std::round(reverseTT * multiplier) / multiplier;
            accTime += reverseTT + inst->service;
        }
    }

    endPoint = lpasse + accTime;
    accTime = 0;
    
    for (int i = 0; i < infroutenew.size(); i++){
        if (i < infroutenew.size() - 1){

            // TODO UNCOMMENT //  << infroutenew[i] << " - ";
        }
        else{

            // TODO UNCOMMENT //  << infroutenew[i];
        }
    }
    // TODO UNCOMMENT //  << " duration:" << endPoint - startPoint << " sum_travel_times:" << travel_time << endl;

    for (int i = 0; i < infroute.size() - 1; i++){
        int a = infroute[i];
        int b = infroute[i+1];
        if (i < infroute.size() - 2){
            // TODO UNCOMMENT //  << "Xi" << a << "_" << b << "_" << infroute[0] << "_OspV +";

        }
        else{
            // TODO UNCOMMENT //  << "Xi" << a << "_" << b << "_" << infroute[0] << "_OspV <=" << infroute.size() - 2 << endl;

        }
    }

    for (int i = 0; i < infroutenew.size(); i++){
        if (i < infroutenew.size() - 1){
            if (infroutenew[i] < inst->n){
                // TODO UNCOMMENT //  << "d" << " - ";
            }
            else if (infroutenew[i] < inst->n + inst->m){
                // TODO UNCOMMENT //  << "P" << " - ";
                //sStat->servedParcels++;
            }
            else if (infroutenew[i] < inst->n + 2*inst->m){
                // TODO UNCOMMENT //  << "D" << " - ";
            }
            else if (infroutenew[i] < inst->n + 2*inst->m + inst->K){
                // TODO UNCOMMENT //  << "S" << " - ";
            }                                      
        }
        else{

            // TODO UNCOMMENT //  << "f";
        }
    }
    // TODO UNCOMMENT //  << endl;
}

void viewSolVRPS (instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, solStats *sStat){
    bool inserted;

    vector< pair <int, int> > auxVec;
    pair<int, int> auxPair;
    vector<int> auxSolOrder;
    // int setN = bStat->bundleVec.size() - inst->K - 1;
    int currSP;
    vector<int> orderVec;
    int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);

	// solStatIni(sStat);

    for (int k = 0; k < inst->K; k++){
        sStat->solOrder.push_back(auxSolOrder);
    }

    for (int k = 0; k < inst->K; k++){
        currSP = inst->V - inst->K + k;

        for (int i = 0; i < sStat->solvec[k].size(); i++){
            auxPair.first = sStat->solvec[k][i].first;
            auxPair.second = sStat->solvec[k][i].second;            
            auxVec.push_back(auxPair);
        }
        // // TODO UNCOMMENT // << "here1";
        // getchar();
        // // TODO UNCOMMENT //  << "auxVec: " << endl;
        // for (int i = 0; i < auxVec.size(); i++){
        //     // TODO UNCOMMENT //  << auxVec[i].first << " " << auxVec[i].second << endl;
        // }

        while(!auxVec.empty()){
            if (sStat->solOrder[k].empty()){

                for (int i = 0; i < auxVec.size(); i++){
                    if (auxVec[i].first == currSP){
                        sStat->solOrder[k].push_back(auxVec[i].first);
                        sStat->solOrder[k].push_back(auxVec[i].second);

                        auxVec.erase(auxVec.begin()+i);

                    }
                }
            }
            else{

                for (int j = 0; j < auxVec.size(); j++){
                    if(auxVec[j].first == sStat->solOrder[k].back()){
                        sStat->solOrder[k].push_back(auxVec[j].second);

                        auxVec.erase(auxVec.begin()+j);
                    }
                }
            }       
        // // TODO UNCOMMENT // << "auxvec size: " << auxVec.size();
        // getchar();
        }
        // // TODO UNCOMMENT // << "here3";
        // getchar();
    }

    //// TODO UNCOMMENT //  << "\nNumber of Vehicles: " << inst->K << endl;

    for (int k = 0; k < inst->K; k++){
        // TODO UNCOMMENT //  << "-------------------------" << endl;
        
        // TODO UNCOMMENT //  << "nbVar = " << sStat->solOrder[k].size() - 1 << endl;
        // TODO UNCOMMENT //  << "Ordered solution : 0(0,0,0) -> ";
        double btime = 0;
        double partialSol = 0;
        double triplen = 0;
        int load = 0;

        for (int i = 1; i < sStat->solOrder[k].size() - 1; i++){
            
            triplen = mdist[sStat->solOrder[k][i - 1]][sStat->solOrder[k][i]]/inst->vmed;
            triplen = std::round(triplen * multiplier) / multiplier;
            btime = sStat->solBegin[sStat->solOrder[k][i]];
            double ttime = nodeVec[sStat->solOrder[k][i]].delta + triplen;

            if (sStat->solOrder[k][i] < inst->n){             
                if (sStat->solOrder[k][i - 1] >= inst->n && sStat->solOrder[k][i - 1] < inst->n + inst->m){
                    load = 5;
                }
                else{
                    load = 1;
                }
            }
            else if (sStat->solOrder[k][i] >= inst->n + inst->m && sStat->solOrder[k][i] < inst->n + 2*inst->m){
                load = 0;
            }
            if (i < sStat->solOrder[k].size() - 1){
                // TODO UNCOMMENT //  << sStat->solOrder[k][i] + 1 << "(" << btime << ","<< load << "," << ttime << ") -> ";
            }
            else{
                // TODO UNCOMMENT //  << sStat->solOrder[k][i] + 1 << "(" << btime << ","<< load << "," << ttime << ")";
            }
        }
        // TODO UNCOMMENT //  << endl;
        for (int i = 0; i < sStat->solOrder[k].size() - 2; i++){
            double arcCost = 0;
            if (sStat->solOrder[k][i] < inst->n){
                arcCost = - inst->costkm*(mdist[sStat->solOrder[k][i]][sStat->solOrder[k][i+1]]);

            }
            else{
                arcCost = nodeVec[sStat->solOrder[k][i]].profit - inst->costkm*(mdist[sStat->solOrder[k][i]][sStat->solOrder[k][i+1]]);
            }
            partialSol += arcCost;
            if (i < 1){
                // TODO UNCOMMENT //  << "Solution includes var[Xi" << k << "_" << sStat->solOrder[k][i+1] + inst->K << "_" << k << "_OspV(" << arcCost << ")] = 1" << endl;
            }
            else if (i + 1 < sStat->solOrder[k].size() - 1){
                // TODO UNCOMMENT //  << "Solution includes var[Xi" << sStat->solOrder[k][i] + inst->K << "_" << sStat->solOrder[k][i+1] + inst->K << "_" << k << "_OspV(" << arcCost << ")] = 1" << endl;
            }
            else{

            }
        }

        // TODO UNCOMMENT //  << "Solution: solCost = " << partialSol << endl;

        //// TODO UNCOMMENT //  << " - Total time: " << sStat->solBegin[sStat->solOrder[k][sStat->solOrder[k].size()-2]] - sStat->solBegin[sStat->solOrder[k][0]] << endl;
    }
    // TODO UNCOMMENT //  << endl;

    // disregard if fip
    // TODO UNCOMMENT //  << "\nSolution structure: " << endl;
    for (int k = 0; k < inst->K; k++){
        // TODO UNCOMMENT //  << "Vehicle " << k << ": ";
        for (int i = 0; i < sStat->solOrder[k].size(); i++){
            if (i < sStat->solOrder[k].size() - 1){
                if (sStat->solOrder[k][i] < inst->n){
                    // TODO UNCOMMENT //  << "d" << " - ";
                }
                else if (sStat->solOrder[k][i] < inst->n + inst->m){
                    // TODO UNCOMMENT //  << "P" << " - ";
                    sStat->servedParcels++;
                }
                else if (sStat->solOrder[k][i] < inst->n + 2*inst->m){
                    // TODO UNCOMMENT //  << "D" << " - ";
                }
                else if (sStat->solOrder[k][i] < inst->n + 2*inst->m + inst->K){
                    // TODO UNCOMMENT //  << "S" << " - ";
                }                                      
            }
            else{

                // TODO UNCOMMENT //  << "f";
            }
        }
        // TODO UNCOMMENT //  << endl;
    }
    // TODO UNCOMMENT //  << endl;   

}

void output(instanceStat *inst, vector<nodeStat> &nodeVec,  solStats *sStat, probStat* problem){

    //output
    string btoutputname;

    btoutputname = "bt-" + inst->InstName + "-" + problem->scen + ".txt";
    // // TODO UNCOMMENT //  << "output bt: " << btoutputname << endl;

    ofstream ofile;

    ofile.open(btoutputname);
    
    // ofile << K << "\t" << 5 << "\t" << dimVec[i].first << "\t" << dimVec[i].second << endl;

    for (int i = 0; i < inst->n; i++){
        ofile << i << "\t" << setw(9) << fixed << setprecision(4) << sStat->solBegin[i] << endl;
    }
    // for (int i = 0; i < inst->n; i++){
    //     ofile << i << "\t" << setw(9) << fixed << setprecision(4) << sStat->solBegin[i] << endl;

    // }
}

void nodeMethod (nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
	
	nodeArcsStruct nas;
	
	// for (int i = 0; i < inst->V; i++){
	// 	// TODO UNCOMMENT //  << "load " << i << ": " << nodeVec[i].load << endl;
	// }
	
	// for (int i = 0; i < inst->n; i++){
	// 	// TODO UNCOMMENT //  << "delta " << i << ": " << nodeVec[i].delta << endl;
	// }

    // // TODO UNCOMMENT //  << "\nDistance Matrix: " << endl;

    // for (int i = 0; i < inst->V + inst->dummy; i++){
    // 	for (int j = 0; j < inst->V + inst->dummy; j++){
    // 		// TODO UNCOMMENT //  << setw(8) << setprecision(5) << mdist[i][j] << " ";
    // 	}
    // 	// TODO UNCOMMENT //  << endl;
    // }
    // getchar();

    // // TODO UNCOMMENT //  << "\nDelta vector: " << endl;

    // for (int i = 0; i < nodeVec.size(); i++){
    //     // TODO UNCOMMENT //  << i << ": " << nodeVec[i].delta << endl;
    // }
    // // TODO UNCOMMENT //  << endl;
    // getchar();

    //// TODO UNCOMMENT //  << "\nProfit vector: " << endl;

    //for (int i = 0; i < nodeVec.size(); i++){
    //    // TODO UNCOMMENT //  << i << ": " << nodeVec[i].profit << endl;
    //}
    //// TODO UNCOMMENT //  << endl;
    // getchar();

	initArcs(inst, &nas);
	feasibleArcs (inst, &nas, problem, nodeVec, mdist);
    
    //printStructures(&nas);

    // getchar();

 //    // TODO UNCOMMENT //  << "arcs NN: " << endl;
	// for (int i = 0; i < nas.arcNN.size(); i++){
	// 	// TODO UNCOMMENT //  << nas.arcNN[i].first << " - " << nas.arcNN[i].second << " | | ";
	// }
	// getchar();
	// // TODO UNCOMMENT //  << "Arcs that leave a pickup: " << endl;

	// for (int i = 0; i < nas.arcPN.size(); i++){
	// 	// TODO UNCOMMENT // << nas.arcPN[i].first << "-" << nas.arcPN[i].second << "  |  ";
	// }
	// // TODO UNCOMMENT //  << endl;

    mipnode(inst, nodeVec, mdist, problem, &nas, sStat);
    
    // mtznode(inst, nodeVec, mdist, problem, &nas, sStat);

	if(sStat->feasible){
        //viewSolVRPS (inst, mdist, nodeVec, sStat);

		viewSol (inst, mdist, nodeVec, sStat);
		mipSolStats (inst, mdist, nodeVec, sStat);

		printStats(inst, sStat);

        if (inst->preInst == 1) {
            output(inst, nodeVec,  sStat, problem);



        }
	}

    //// TODO UNCOMMENT //  << "before test route" << endl;
    //testRoute(inst, mdist, nodeVec);

    
	for ( int i = 0; i < inst->V + inst->dummy; i++) {
		delete[] mdist[i];
	}
	delete[] mdist;
}

void fipArcs(instanceStat *inst, nodeArcsStruct *nas, probStat* problem, vector<nodeStat> &nodeVec, double **mdist, int stage){
    //stage 1: solution with only passengers
    //stage 2: solution with parcels as well
    int auxK;

    int fDepot = 2*inst->n + 2*inst->m;
    int fDummy = 2*inst->n + 2*inst->m + inst->K;

    int decimalPlaces = 4;
    double multiplier = std::pow(10, decimalPlaces);
    //independently from sarp scenario, these arcs are always true
    if (stage > 1){
        for (int i = 0; i < nodeVec.size(); i++){
            for (int j = 0; j < nodeVec.size(); j++){
                if (i != j){
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);
                    nas->allArcs.push_back(nas->fArc);
                }
            }
        }
    }


    for(int i = fDepot; i < fDummy; i++){//i is a starting point, j is the corresponding ending depot.

        int j = i + inst->K;

        nas->arcs[i][j] = true;
        nas->fArc.first = i;
        nas->fArc.second = j;
        nas->arcMinus[j].push_back(nas->fArc);
        nas->arcPlus[i].push_back(nas->fArc);
        nas->allArcs.push_back(nas->fArc);

        auxK = j - inst->V;
        nas->arcV[i][j].push_back(auxK);

        for (int l = 0; l < inst->n; l++){//l is a passenger pu node
            nas->arcs[i][l] = true;
            nas->fArc.first = i;
            nas->fArc.second = l;
            nas->arcMinus[l].push_back(nas->fArc);
            nas->arcPlus[i].push_back(nas->fArc);
            nas->allArcs.push_back(nas->fArc);
            nas->arcnf.push_back(nas->fArc);
            auxK = i - fDepot;
            nas->arcV[i][l].push_back(auxK);
        }

        // if (stage > 1){
        //     //Check if we can add parcel between depot and passenger (we cant but there is no prunning of arcs)
        //     for (int l = 2*inst->n; l < 2*inst->n + inst->m; l++){//l is a parcel pu node
        //         nas->arcs[i][l] = true;
        //         nas->fArc.first = i;
        //         nas->fArc.second = l;
        //         nas->arcMinus[l].push_back(nas->fArc);
        //         nas->arcPlus[i].push_back(nas->fArc);
        //         nas->allArcs.push_back(nas->fArc);
        //         nas->arcnf.push_back(nas->fArc);
        //         auxK = i - fDepot;
        //         nas->arcV[i][l].push_back(auxK);
        //     }
        // }
    }

    for (int i = 0; i < 2*inst->n; i++){//i is a passenger node (PU or DL)
        for(int j = 0; j < 2*inst->n; j++){// j is a passenger req (PU or DL)
            if(i != j && j != i - inst->n){//not the same node; not a DL to its PU; not PU to PU or DL to DL
                double ttij = mdist[i][j]/inst->vmed;//travel time between requests i and j 
                ttij = std::round(ttij * multiplier) / multiplier;
                //if lowest time for req i + travel time from i to j is lower or equal to
                //the latest point in time to serve request j. If latest time == T, it is always valid                        
                //// TODO UNCOMMENT //  <<"i: " << i << " - j: " << j << " ttij: " << ttij << " - test: " << (nodeVec[i].e + ttij < nodeVec[j].l) << endl;
                if (nodeVec[i].e + ttij < nodeVec[j].l){
                    nas->arcs[i][j] = true;
                    nas->fArc.first = i;
                    nas->fArc.second = j;
                    nas->arcMinus[j].push_back(nas->fArc);
                    nas->arcPlus[i].push_back(nas->fArc);

                    nas->arcNN.push_back(nas->fArc);
                    nas->arcNplus.push_back(nas->fArc);

                    nas->allArcs.push_back(nas->fArc);
                    nas->arcnf.push_back(nas->fArc);
                    for (int k = 0; k < inst->K; k++){
                        //// TODO UNCOMMENT //  << "here" << endl;
                        nas->arcV[i][j].push_back(k);
                    }                        
                }
           }
        }

        if (i >= inst->n){ //only passenger requests of delivery get to go to the dummy node
            for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
                nas->arcs[i][j] = true;
                nas->fArc.first = i;
                nas->fArc.second = j;
                nas->arcMinus[j].push_back(nas->fArc);
                nas->arcPlus[i].push_back(nas->fArc);

                nas->arcNplus.push_back(nas->fArc);

                nas->allArcs.push_back(nas->fArc);
                auxK = j - inst->V;
                nas->arcV[i][j].push_back(auxK);
            }
        }

        for (int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){// j is a parcel node (PU or DL)           
            nas->arcs[i][j] = true;
            nas->fArc.first = i;
            nas->fArc.second = j;
            nas->arcMinus[j].push_back(nas->fArc);
            nas->arcPlus[i].push_back(nas->fArc);

            nas->arcNplus.push_back(nas->fArc);
            nas->allArcs.push_back(nas->fArc);
            nas->arcnf.push_back(nas->fArc);
            for (int k = 0; k < inst->K; k++){
                nas->arcV[i][j].push_back(k);
            }
        }

    }
    //getchar();

    for (int i = 2*inst->n; i < 2*inst->n + 2*inst->m; i++){//i is a parcel request (PU or DL)
        for (int j = 0; j < 2*inst->n; j++){ //j is a passenger request (PU or DL)
            nas->arcs[i][j] = true;
            nas->fArc.first = i;
            nas->fArc.second = j;
            nas->arcMinus[j].push_back(nas->fArc);
            nas->arcPlus[i].push_back(nas->fArc);
            if (i < 2*inst->n + inst->m){
                nas->arcPN.push_back(nas->fArc);
            }
            nas->allArcs.push_back(nas->fArc);
            nas->arcnf.push_back(nas->fArc);
            for (int k = 0; k < inst->K; k++){
                nas->arcV[i][j].push_back(k);
            }
        }
        // if (stage > 1){
        //     for (int j = 2*inst->n; j < 2*inst->n + 2*inst->m; j++){//j is a parcel request (PU or DL)
        //         if (i != j && j != i - inst->m){
        //             nas->arcs[i][j] = true;
        //             nas->fArc.first = i;
        //             nas->fArc.second = j;
        //             nas->arcMinus[j].push_back(nas->fArc);
        //             nas->arcPlus[i].push_back(nas->fArc);
        //             nas->allArcs.push_back(nas->fArc);
        //             nas->arcnf.push_back(nas->fArc);
        //             for (int k = 0; k < inst->K; k++){
        //                 nas->arcV[i][j].push_back(k);
        //             }    
        //         }
        //     }
        //     if (i >= 2*inst->n + inst->m){ //only passenger requests of delivery get to go to the dummy node
        //         for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
        //             nas->arcs[i][j] = true;
        //             nas->fArc.first = i;
        //             nas->fArc.second = j;
        //             nas->arcMinus[j].push_back(nas->fArc);
        //             nas->arcPlus[i].push_back(nas->fArc);

        //             nas->allArcs.push_back(nas->fArc);
        //             auxK = j - inst->V;
        //             nas->arcV[i][j].push_back(auxK);
        //         }
        //     }
        // }

        
    }



    //check if they can be last before dummy
    // for (int i = 2*inst->n + inst->m; i < 2*inst->n + 2*inst->m; i++){//i is a parcel dl node           
    //     for (int j = inst->V; j < inst->V + inst->dummy; j++){//j is the dummy node
    //         nas->arcs[i][j] = true;
    //         nas->fArc.first = i;
    //         nas->fArc.second = j;
    //         nas->arcMinus[j].push_back(nas->fArc);
    //         nas->arcPlus[i].push_back(nas->fArc);
    //         nas->allArcs.push_back(nas->fArc);

    //         auxK = j - inst->V;
    //         nas->arcV[i][j].push_back(auxK);
    //     }
    // }

    for (int a = 0; a < nas->allArcs.size(); a++){
        int i = nas->allArcs[a].first;
        int j = nas->allArcs[a].second;

        for(int k1 = 0; k1 < nas->arcV[i][j].size(); k1++){
            int k = nas->arcV[i][j][k1];
            nas->vArcPlus[i][k].push_back(nas->allArcs[a]);
            nas->vArcMinus[j][k].push_back(nas->allArcs[a]);
        }

    }
}

void fipMethod(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat*problem, solStats *sStat){

    nodeArcsStruct nas;
    fipStats fipStat;

    initArcs(inst, &nas);
	fipArcs (inst, &nas, problem, nodeVec, mdist, 1);
    //printStructures(&nas);
    
    fipStat.fipstage = 0;
    fippass(inst, nodeVec, mdist, problem, &nas, sStat);
    // TODO UNCOMMENT //  << "After fip pass" << endl;
	if(sStat->feasible){
		viewSol (inst, mdist, nodeVec, sStat);

		mipSolStats (inst, mdist, nodeVec, sStat);

		printStats(inst, sStat);

        printSolFile (inst, sStat, problem, false);

        if (inst->preInst == 1) {
            output(inst, nodeVec,  sStat, problem);
        }
	}

    fipStat.fipstage = 1;
    fipStruct(inst, sStat, &fipStat); 

    initArcs(inst, &nas);
    fipArcs (inst, &nas, problem, nodeVec, mdist, 2);

    // printStructures(&nas);
    // TODO UNCOMMENT //  << endl << endl << "______PART II_____" << endl << endl;

    fipmip(inst, nodeVec, mdist, problem, &nas, sStat, &fipStat);

    bool feasFlag = false;

    if(sStat->feasible){
        feasFlag = true;
    }

    // if(sStat->feasible){
    mergeFipSol(inst, mdist, nodeVec, sStat, &fipStat, feasFlag);
    printSolFileFip1 (inst, sStat, problem, true, &fipStat);

        //calcPassDetour(inst, nodeVec, &fipStat);
    // }


	for ( int i = 0; i < inst->V + inst->dummy; i++) {
		delete[] mdist[i];
	}
	delete[] mdist;

    clearStats(sStat, &fipStat);
}

void fipnodeMethod (nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){

	nodeArcsStruct nas;

    fipStats fipStat;

	initArcs(inst, &nas);
	feasibleArcs(inst, &nas, problem, nodeVec, mdist);

    // TODO UNCOMMENT //  << "aqui 1" << endl;
    if (problem->model == "bundle4") {
        fipPassSolBundles(inst, &fipStat);
    } else {
        fipPassSol(inst, &fipStat);
    }
    // TODO UNCOMMENT //  << "aqui 2" << endl;

    mipnodefip(inst, nodeVec, mdist, problem, &nas, sStat, &fipStat);
    
    // mtznode(inst, nodeVec, mdist, problem, &nas, sStat);

	if(sStat->feasible){
        //viewSolVRPS (inst, mdist, nodeVec, sStat);

		viewSol (inst, mdist, nodeVec, sStat);
		mipSolStats (inst, mdist, nodeVec, sStat);

		printStats (inst, sStat);

        printSolFile (inst, sStat, problem, true);

        if (inst->preInst == 1) {
            output(inst, nodeVec,  sStat, problem);
        }
	}

    //// TODO UNCOMMENT //  << "before test route" << endl;
    //testRoute(inst, mdist, nodeVec);

	for ( int i = 0; i < inst->V + inst->dummy; i++) {
		delete[] mdist[i];
	}
	delete[] mdist;
}
