#include "functions.h"
#include <cstdlib>
#include <stdio.h>

void distScale(instanceStat *inst, int *instV, vector <vector <double> > &tempData, double *curAvg, int *scale){
    double distSum, avgDist, totalAvg;

    vector<double> avgVec;

    distSum = 0;
    // // TODO UNCOMMENT //  << "\nNumber of nodes: " << tempData.size() << " and " << tempData[0].size() << endl;

    // // TODO UNCOMMENT //  << "\ncurrent scale: " << *scale << endl;
    // // TODO UNCOMMENT //  << "\ncurrent average: " << *curAvg << endl;
    // getchar();

    // // TODO UNCOMMENT //  << "\nDistance Matrix (Pre-adapting): " << endl;

    // for (int i = 0; i < tempData.size() - 1; i++){
    //     for (int j = 0; j < tempData[i].size() - 1; j++){
    //         // TODO UNCOMMENT //  << setw(5) << setprecision(5) << tempData[i][j] << " ";
    //     }
    //     // TODO UNCOMMENT //  << endl;
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

    // // TODO UNCOMMENT //  << "\nVector of average distances: " << endl; 
    // for(int i = 0; i < avgVec.size(); i++){
    //     // TODO UNCOMMENT //  << i << ": " << avgVec[i] << endl;
    // }
    // getchar();

    distSum = 0;

    for(int i = 0; i < avgVec.size(); i++){
        distSum += avgVec[i];
    }

    // // TODO UNCOMMENT //  << "\nDistSum: " << distSum << endl;
    totalAvg = distSum/avgVec.size();


    // // TODO UNCOMMENT //  << "\nTotal average: " << totalAvg << endl;
    // getchar();
    *curAvg = totalAvg;
}

double calcEucDist (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    return sqrt(pow(Xf[I] - Xs[J], 2) + pow(Yf[I] - Ys[J], 2));
}

double calcEucDist2 (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    double a = pow(Xf[I] - Xs[J], 2);
    double b = pow(Yf[I] - Ys[J], 2);
    return floor(sqrt(a + b)*10)/10;
}

double CalcMan (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    return abs(Xf[I] - Xs[J]) + abs(Yf[I] - Ys[J]);
}

double valRound(double value){
    //int decimalPlaces = 4;
    //double multiplier = std::pow(10, decimalPlaces);
    double multiplier = 10;

    return std::floor(value * multiplier) / multiplier;
}

double timeRound(double value){
    int decimalPlaces = 3;
    double multiplier = std::pow(10, decimalPlaces);
    //double multiplier = 1000;

    return std::floor(value * multiplier) / multiplier;
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

    // // TODO UNCOMMENT //  << "\nParam2: " << param2 << endl;
    // getchar();
 
    instParam.push_back(stoi(param1));
    instParam.push_back(stoi(param2));
}

// OBS: DESCRIPTION
    // fip: fip with single insertion
    // nodefip: fip with multiple insertions
    // bundle2: execute only bundles without selection
    // bundle3: execute mono insertion bundlefip without selection
    // bundle4: execute multi insertion bundlefip without selection
    // bundle5: execute only bundle with profit based selection
    // bundle6: execute only bundle with priority based selection
    // bundle7: execute multi insertion bundlefip with profit based selection
    // bundle8: execute multi insertion bundlefip with priority based selection
void solveselect(nodeStat *node, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, probStat* problem, solStats *sStat){
    if (problem->model == "node"){
        nodeMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "nodefip"){
        fipnodeMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle"){
        bundleMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle2"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
        //bundleMethodPartial(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundlep"){
        bundleMethodPartial(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundlep2"){
        // bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
        fipnodeMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle3"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle4"){
        // bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
        fipnodeMethod(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle5"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle6"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle7"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "bundle8"){
        bundleMethod2(node, inst, mdist, nodeVec, problem, sStat);
    }

    else if (problem->model == "fip"){
        fipMethod(node, inst, mdist, nodeVec, problem, sStat);

    }
    // else if (){

    // }

//Bundle 7 e 8 por enquanto não fazem nada.

//Bundle 5 é o método de bundle padrão, só que selecionando bundles com base no lucro deles (aqueles dois métodos iniciais)

//Bundle 6 - método de bundle padrão, só que selecionando bundles com base em prioridade ( o terceiro método que a gente tava tentando desenvolver)

//Bundle 3 - bundlefip com uma única inserção

//Bundle 4 - bundlefip com múltiplas inserções

//Bundle2 - método bundle padrão

//Bundle 1 - esse eu não sei, ele já existia quando peguei no código, mas deve ser o método de bundle padrão antigo

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


        // TODO UNCOMMENT //  << "\n\nSeed: " << std::setprecision(5) << seed << endl << endl;
        // getchar();
        // t1 = sILS.get_wall_time();
        sILS.stats.setStart();

        sILS.ILS(inst, nodeVec, mdist, problem);

        // t2 = sILS.get_wall_time();
        // deltaT = t2 - t1;

        sILS.stats.setEnd();

        // TODO UNCOMMENT //  << "\nTotal Run Time: " << std::setprecision(8) <<  sILS.stats.printTime() << endl;

        // TODO UNCOMMENT //  << "END OF METHOD" << endl;
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

int permutationCount = 0;
int setCount = 0;
int infSets, feasSets;

int testDurations(int a, int b, int c, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec) {
    // Process the current permutation here.
    //std::// TODO UNCOMMENT //  << a << " " << b << " " << c << std::endl;
    permutationCount++;
    int counter = 0;
    double avgOK, avgInfeasible, avgExceeded;

    int result; //1- feasible; 2- infeasible; 3- exceed duration

    double duration = 0;
    double start, end;
    double cumulativeTT = 0;
    bool passenger = false;

    if (a < inst->n){
        passenger = true;
    }
    else{
        if(a > inst->n+inst->m && (a == b + inst->m || a == c + inst->m)){
            return 2;
        }
    }
    start = nodeVec[a].e;
    end = nodeVec[a].e + nodeVec[a].delta;

    if (b < inst->n){
        if(!passenger){
            passenger = true;
            start = nodeVec[b].e - mdist[a][b]/inst->vmed - nodeVec[a].delta;
        }
        else{
            if ((nodeVec[a].e > nodeVec[b].e) ||
                (end + mdist[a][b]/inst->vmed + nodeVec[a].delta > nodeVec[b].e)){
                return 2;
            }
            end = nodeVec[b].e + nodeVec[b].delta;
        }
    }
    else{
        if(b > inst->n+inst->m && b == c + inst->m){
            return 2;
        }
        end = end + mdist[a][b]/inst->vmed + nodeVec[b].delta;
    }
    if (c < inst->n){
        if (!passenger){
            passenger = true;
            start = nodeVec[c].e - cumulativeTT;
            end = nodeVec[c].e + nodeVec[c].delta;
        }
        else{
            if ((nodeVec[b].e > nodeVec[c].e) ||
                (end + mdist[b][c]/inst->vmed + nodeVec[a].delta > nodeVec[c].e)){
                return 2;
            }            
            end = nodeVec[c].e + nodeVec[c].delta;
        }

    }
    else{
        end = end + mdist[b][c]/inst->vmed + nodeVec[c].delta;
        
    }

    duration = end - start;
    if (duration > inst->maxTime){
        result = 3;
    }
    else{
        result = 1;
    }

    return result;

}

void generatePermutations(int a, int b, int c, instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec, vector<int>& stats) {
    int permutation[3] = {a, b, c};
    int result = 0;
    bool infeasible = false;
    bool feasible = false;
    do {

        result = testDurations(permutation[0], permutation[1], permutation[2], inst, mdist, nodeVec);
        stats[result-1] += 1;
        if (result > 2 && !infeasible){
            infeasible  = true;
        }
        else if (result <= 2 && !feasible){
            if(infeasible){
                feasible = false;
                continue;
            }
            else{
                feasible = true;
            }
        }
        //// TODO UNCOMMENT //  << "Result: " << result << endl;
        
    } while (std::next_permutation(permutation, permutation + 3));

    if (feasible) {
        feasSets++;
    } else if (infeasible) {
        infSets++;
    }
}

void startPermutation(instanceStat *inst, double **mdist, vector<nodeStat> &nodeVec){

    int size = inst->n + 2*inst->m;
    int count = 3;
    vector<int> stats = {0, 0, 0}; //feasible, infeasible, exceed duration
    infSets = 0;
    feasSets = 0;

    for (int i = 0; i < size - (count - 1); ++i) {
        for (int j = i + 1; j < size - (count - 2); ++j) {
            for (int k = j + 1; k < size - (count - 1); ++k) {
                setCount++;
                generatePermutations(i, j, k, inst, mdist, nodeVec, stats);
            }
        }
    }

    // TODO UNCOMMENT //  << "Stats on permutations of 3" << endl;

    // TODO UNCOMMENT //  << "Feasible permutations: " <<  (static_cast<double>(stats[0]) / permutationCount)*100 << "%" << endl;
    // TODO UNCOMMENT //  << "Infeasible permutations: " <<  (static_cast<double>(stats[1]) / permutationCount)*100 << "%" << endl;
    // TODO UNCOMMENT //  << "Exceed duration: " <<  (static_cast<double>(stats[2]) / permutationCount)*100 << "%" << endl;

    // TODO UNCOMMENT //  << "\nInfeasible sets of 3: " << (static_cast<double>(infSets) / setCount)*100 << "%" << endl;
}

