#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void distScale(instanceStat *inst, int *instV, vector <vector <double> > &tempData, double *curAvg, int *scale){
    double distSum, avgDist, totalAvg;

    vector<double> avgVec;

    distSum = 0;
    // cout << "\nNumber of nodes: " << tempData.size() << " and " << tempData[0].size() << endl;

    // cout << "\ncurrent scale: " << *scale << endl;
    // cout << "\ncurrent average: " << *curAvg << endl;
    // getchar();

    // cout << "\nDistance Matrix (Pre-adapting): " << endl;

    // for (int i = 0; i < tempData.size() - 1; i++){
    //     for (int j = 0; j < tempData[i].size() - 1; j++){
    //         cout << setw(5) << setprecision(5) << tempData[i][j] << " ";
    //     }
    //     cout << endl;
    // }
    // getchar();

    for (int i = 0; i < tempData.size() - 1; i++){
        distSum = 0;
        for (int j = 0; j < tempData.size() - 1; j++){
            distSum += (double)(tempData[i][j]/ *scale);
        }
        avgDist = distSum/(tempData.size() - 1);
        avgVec.push_back(avgDist);
    }

    // cout << "\nVector of average distances: " << endl; 
    // for(int i = 0; i < avgVec.size(); i++){
    //     cout << i << ": " << avgVec[i] << endl;
    // }
    // getchar();

    distSum = 0;

    for(int i = 0; i < avgVec.size(); i++){
        distSum += avgVec[i];
    }

    // cout << "\nDistSum: " << distSum << endl;
    totalAvg = distSum/avgVec.size();


    // cout << "\nTotal average: " << totalAvg << endl;
    // getchar();
    *curAvg = totalAvg;
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

string getInstName (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_last_of("/");
    string::size_type loc2 = filename.find_first_of(".");
    string InstanceName;

    InstanceName.append(filename, loc+1, loc2-loc-1 );

    return InstanceName;
}

void getInstParam (instanceStat *inst, vector<int> &instParam){

    string::size_type loc = inst->InstName.find_first_of("-");
    string::size_type loc2 = inst->InstName.find_last_of("-");
    string param1;
    string param2;

    param1.append(inst->InstName, loc+1, loc2-loc-1);
    // param2.append(inst->InstName.c_str(), loc2+1);
    param2.append(inst->InstName, loc2+1, inst->InstName.back());

    // cout << "\nParam2: " << param2 << endl;
    // getchar();
 
    instParam.push_back(stoi(param1));
    instParam.push_back(stoi(param2));
}

void solveselect(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
    
    if (problem->model == "node"){
        nodeMethod(node, inst, mdist, nodeVec, problem, sStat);

    }
    else if (problem->model == "bundle"){
        bundleMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "fip"){
        fipMethod(node, inst, mdist, nodeVec, problem, sStat);

    }
    // else if (){

    // }

    else if (problem->model == "math"){
        // h.orderRequests(&inst, nodeVec, distMatrix, &problem);
        // h.buildDistVec(&inst, nodeVec, distMatrix, &problem);
        // h.buildBundles(&inst, nodeVec, distMatrix, &problem);
        // h.orgBundles(&inst, nodeVec, distMatrix, bStat, &problem);
        // h.hbundleMethod(&inst, nodeVec, distMatrix, &problem, &sStat);
    }

    else if (problem->model == "mio"){
        
    }

    else if (problem->model == "ils"){
        // h.orderRequests(&inst, nodeVec, distMatrix, &problem);
        // h.buildDistVec(&inst, nodeVec, distMatrix, &problem);
        // h.buildBundles(&inst, nodeVec, distMatrix, &problem);
        // h.orgBundles(&inst, nodeVec, distMatrix, bStat, &problem);
        // sCon.ConstrProc(&inst, nodeVec, distMatrix, &problem);

        // clock_t seed = (argc == 5) ? time(NULL) : strtol(argv[4], NULL, 10);
        sarpILS sILS;

        clock_t seed = 1642521807;
        srand(seed);


        cout << "\n\nSeed: " << std::setprecision(5) << seed << endl << endl;
        // getchar();
        // t1 = sILS.get_wall_time();
        sILS.stats.setStart();

        sILS.ILS(inst, nodeVec, mdist, problem);

        // t2 = sILS.get_wall_time();
        // deltaT = t2 - t1;

        sILS.stats.setEnd();

        cout << "\nTotal Run Time: " << std::setprecision(8) <<  sILS.stats.printTime() << endl;

        cout << "END OF METHOD" << endl;
        // sILS.function();
    }
    // else if (problem.model == "twostage"){
    // 	twoStageMethod(&node, &inst, distMatrix, nodeVec, &problem, &sStat);			
    // }

    // if (problem.scen == "PC"){
    // 	if (trialMulti > 1){
    // 		// if (trialK < inst.n + inst.m){
    // 		if (trialK < inst.n){
    // 			trialK++;	
    // 		}

    // 		else{
    // 			break;
    // 		}
    // 	}
    // 	else {
    // 		trialMulti = 1.5;
    // 	}
    // }

    // else{
    // 	if (trialMulti > 1){
    // 		if (trialK < inst.n){
    // 			trialK++;	
    // 		}

    // 		else{
    // 			break;
    // 		}
    // 	}
    // 	else {
    // 		trialMulti = 1.5;
    // 	}	
    // }
    
// }



}