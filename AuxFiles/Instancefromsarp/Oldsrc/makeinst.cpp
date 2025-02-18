#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>	
#include <iterator>
#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <locale.h>
#include <sys/time.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <string>

using namespace std;

struct NodesStruct{
    double dist;
    int index;
};

struct OrStruct{
    double vxs;
    double vys;
    double vload;
    double ve;
    double vl;
    int index;
};

struct CandStruct{
    int index;
    int realInd;
    bool chosen;
    int label1;
    int label2;
    //l1: 1 - customer; 2 - parcel; 3 - depot
    //l2: 1 - pickup; 2 - delivery; 3 - depot
};

struct Info{
    long long seed;
    int n;
    int m;
    double speed = 41;
    int K;
    // vector< pair<int, int> > dimVec;
    vector< vector< pair<int, int> > > dimVec;//vector of scale, with every combination of n and m

    vector< pair<double, double> > tsVec;
    vector< pair <double, double> > coordVec;
    vector<int> loadVec;
    vector<double> delta;


    vector<int> scaleVec;
    string scale;
    vector<int> vecOfn;
    vector<int> vecOfm;

    vector<int> vecOfLambda;
};

// double CalcDistEuc (double X1, double Y1, double X2, double Y2);
// void genPoints (int argc, char** argv, vector<int> &vecOfn, vector<int> &vecOfm, vector<int> &vecOfLambda, Info *info);
// double fRand(double fMin, double fMax);
// bool compareDist(const NodesStruct &a, const NodesStruct &b);
// void classNodes(Info *info, int *lambda, NodesStruct nodeVar, vector<NodesStruct> closeVec, vector< vector<double> > &dist, vector<int> unclassVec, vector<CandStruct> &nodeVec);
// void organizeNodes(Info *info, vector<CandStruct> nodeVec, vector<CandStruct> &orgNodes);
// bool compareIndex(const CandStruct &a, const CandStruct &b);
// void createTimesLoad(Info *info, vector<CandStruct> &orgNodes);
// void createDelta(Info *info, vector<CandStruct> &orgNodes);
void ReadData(int argc, char** argv, Info *info);
void extractData(Info *info, vector<OrStruct> &NdVec);
void output(Info *info, vector<OrStruct> &newvec, int p);
void getInstanceScale(int argc, char** argv, Info *info);
double CalcMan (double Xs, double Ys, double Xf, double Yf);
double CalcManOr (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);
constexpr double degreesToRadians(double degrees);
//double CalcManKm(vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J);

double CalcManKm(double Xs, double Ys, double Xf, double Yf);

double CalcManOr (vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J){
    return abs(Xf[I] - Xs[J]) + abs(Yf[I] - Ys[J]);
}

double CalcMan (double Xs, double Ys, double Xf, double Yf){
    return abs(Xf - Xs) + abs(Yf - Ys);
}

constexpr double degreesToRadians(double degrees) {
    return degrees * M_PI / 180.0;
}

// Function to calculate Manhattan distance in kilometers
//double CalcManKm(vector<double> &Xs, vector<double> &Ys, vector<double> &Xf, vector<double> &Yf, int I, int J) {
double CalcManKm(double Xs, double Ys, double Xf, double Yf){

    // Earth's approximate radius (for conversion purposes)

    //double lat1 = Xs[I];
    //double lon1 = Ys[I];
    //double lat2 = Xf[J];
    //double lon2 = Ys[J];

    double lat1 = Xs;
    double lon1 = Ys;
    double lat2 = Xf;
    double lon2 = Yf;

    constexpr double kmPerDegreeLat = 111.0;

    // Calculate absolute differences in latitude and longitude
    double deltaLat = std::abs(lat2 - lat1) * kmPerDegreeLat;

    // Average latitude in radians for longitude scaling
    double avgLat = degreesToRadians((lat1 + lat2) / 2.0);

    // Calculate kilometers per degree of longitude at the average latitude
    double kmPerDegreeLon = kmPerDegreeLat * std::cos(avgLat);

    double deltaLon = std::abs(lon2 - lon1) * kmPerDegreeLon;

    // Return the Manhattan distance in kilometers
    return deltaLat + deltaLon;
}

void ReadData(int argc, char** argv, Info *info)
{
    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    string file, ewf;
    int n;
    int m;
    int K;
    double service;
    int originalV;

    std::string line;

    // Step 1: Read and process the first line
    if (std::getline(in, line)) {
        std::istringstream firstLineStream(line);
        firstLineStream >> K >> service >> n >> m;

        // Output the parsed values from the first line
        //std::cout << "First line (metadata): " << K << ", " << service
                //  << ", " << n << ", " << m << std::endl;
    }

    //getchar();
    //in >> K;
    //in >> service;
    //in >> n;
    //in >> m;

    int V = n + m + 2;
    
    originalV = 2*n + 2*m + 2;    

    string strN, strM, strP;
    
    OrStruct nd;
    
    vector<OrStruct> Ndvec;

    while (std::getline(in, line)) {
        std::istringstream lineStream(line);
        int id, temp, load;
        double lat, lon, twe, twl;

        // Parse values from the line
        lineStream >> id >> lat >> lon >> temp >> load >> twe >> twl;

        // Output the parsed values for each data line
        //std::cout << "ID: " << id << ", Lat: " << lat << ", Lon: " << lon
        //        << ", Load: " << load << ", TWE: " << twe << ", TWL: " << twl << std::endl;
        
        nd.index = id;
        nd.vxs = lat;
        nd.vys = lon;
        nd.vload = load;
        nd.ve = twe;
        nd.vl = twl;
        Ndvec.push_back(nd);
        //getchar();
    }

    in.close();
    
    //cout << "size of Ndvec: " << Ndvec.size() << endl;
    //cout << "Checking the vector: " << endl;
    //for (int i = 0; i < Ndvec.size(); i++){
    //    cout << "i: " << Ndvec[i].index << " - " << Ndvec[i].vload << " - " << Ndvec[i].ve << "\t";

    //}
    //Ndvec.pop_back();
    //getchar();

    extractData(info, Ndvec);

}

void extractData(Info *info, vector<OrStruct> &Ndvec){

    double ninst = 5;

    vector<OrStruct> auxnp, auxnd, auxmp, auxmd, dp;

    vector<OrStruct> newvec;

 

    for (int i = 0; i < Ndvec.size() - 1; i++){
        if (Ndvec[i].vload == 3){
            auxnp.push_back(Ndvec[i]);
        }
        else if (Ndvec[i].vload == 1){
            auxmp.push_back(Ndvec[i]);
        }
        else if (Ndvec[i].vload == -3){
            auxnd.push_back(Ndvec[i]);
        }
        else if (Ndvec[i].vload == -1){
            auxmd.push_back(Ndvec[i]);
        }
        else{
            dp.push_back(Ndvec[i]);
        }
    }

    //cout << "\nAux MD: " << endl;

    //for (int i = 0; i < auxmd.size(); i++){
    //    cout << "i: " << auxmd[i].index << " - " << auxmd[i].vload  << endl;
    //}

    //cout << "depot nd: " << endl;
    //for (int i = 0; i < dp.size(); i++){
    //    cout << "i: " << dp[i].index << " - " << dp[i].vxs << " - " << dp[i].vys << "::" << dp[i].vload << " - " << dp[i].ve << " - " << dp[i].vl << endl;
    //}

    //getchar();
    for (int i = 0; i < info->dimVec.size(); i++){
        for (int j = 0; j < info->dimVec[i].size(); j++){
            for (int p = 1; p < ninst+1; p++){
                info->n = info->dimVec[i][j].first;
                info->m = info->dimVec[i][j].second;

                //info->seed = pow(info->n, 8) + pow(info->m, 7) + pow(p, 6);//changed (same as gh)
                info->seed = pow(info->n, 6) + pow(info->m, 6) + pow(p, 4);

                srand(info->seed);

                int newV = info->n + info->m + 1;
                int c;
                int counter = 0;

                for (int j = 0; j < info->n; j++){
                    c = rand() % auxnp.size();
                    // cout << "c: " << c;
                    newvec.insert(newvec.begin() + counter, auxnp[c]);
                    newvec.push_back(auxnd[c]);
                    counter++;
                    //cout << "vector populating n: " << endl;
                    for (int v = 0; v < newvec.size(); v++){
                        cout << newvec[v].index << " " << newvec[v].vload << endl;
                    }
                    //getchar();
                }
                counter = 0;
                for (int j = 0; j < info->m; j++){
                    c = rand() % auxmp.size();
                    //cout << "Value of c: " << c << endl;
                    //newvec.insert(newvec.begin() + info->n + counter, auxmp[c]);
                    newvec.insert(newvec.begin() + 2*info->n + counter, auxmp[c]);
                    //newvec.push_back(auxmp[c]);
                    newvec.push_back(auxmd[c]);
                    //cout << "index of mp and load: " << auxmp[c].index << "; " << auxmp[c].vload << " - index of md: " << auxmd[c].index << "; " << auxmd[c].vload << endl;
                    counter++;
                    //cout << "vector populating m: " << endl;
                    for (int v = 0; v < newvec.size(); v++){
                        cout << newvec[v].index << " " << newvec[v].vload << endl;
                    }
                    //getchar();
                }

                //newvec.insert(newvec.begin(), dp[0]);
                newvec.push_back(dp[0]);
                // for (int i = 0; i < newvec.size(); i++){
                //     cout << "i: " << newvec[i].index << " - " << newvec[i].vload << " - " << newvec[i].ve << endl;
                // }
                // //getchar();


                double service = 5;
                double timePoint = 0;

                for (int i = 0; i < newvec.size(); i++){

                    if (newvec[i].vload == 3){
                        newvec[i].vl = newvec[i].ve;
                    }

                    else if (newvec[i].vload == -3){
                        //newvec[i].ve = newvec[i - info->n - info->m].ve;
                        newvec[i].ve = newvec[i - info->n].ve;
                        newvec[i].vl = newvec[i].ve;
                    }
                    //cout << "i: " << newvec[i].index << " - " << newvec[i].vload << " - " << newvec[i].ve << endl;

                    else if (newvec[i].vload == 1){
                        double vxs = newvec[i + 1].vxs;
                        double vys = newvec[i + 1].vys;
                        //double vxf = newvec[i + info->n + info->m + 1].vxs;
                        //double vyf = newvec[i + info->n + info->m + 1].vys;
                        double vxf = newvec[i + info->m + 1].vxs;
                        double vyf = newvec[i + info->m + 1].vys;                        
                        //cout << "node: " << i << endl;
                        //cout << std::fixed << std::setprecision(5) << "vxs: " << vxs << " - vys: " << vys << " - vxf: " << vxf << " - vyf: " << vyf << endl;

                        //double calcDelta = CalcMan(vxs, vys, vxf, vyf);
                        double calcDist = CalcManKm(vxs, vys, vxf, vyf);

                        double calcDelta = calcDist/info->speed;

                        //cout << "Calculated delta in km: " << calcDist << endl;
                        
                        double deltaMin = ceil(calcDelta*60);
                        //cout << "Calculated delta in hours: " << calcDelta << endl;
                        //cout << "Calculated delta in minutes: " << deltaMin << endl;

                        timePoint = 540 + rand() % 470;

                        while (timePoint + 60 + deltaMin + 2*service > 1140){  //remove possibility of parcel being delivered after 1140

                            timePoint = 540 + rand() % 470; 
                        }
                        newvec[i].ve = timePoint;
                        //newvec[i].vl = timePoint + 30;
                        newvec[i].vl = timePoint + 60; //1hour of pickup TW

                        //int dl = i + info->m + info->n;
                        int dl = i + info->m;
                        //cout << "Pickup index: " << i << " - Delivery index: " << dl << endl;

                        if (deltaMin < 1){
                            deltaMin = 1;
                        }

                        //version with tight delivery TW
                        newvec[dl].ve = newvec[i].vl + deltaMin + service;
                        //newvec[dl].vl = newvec[dl].ve + 60;
                        newvec[dl].vl = 1140; //latest delivery time is the end of tw with time for service
                        

                        //cout << "Calculated delta in minutes: " << deltaMin << endl;
                        //cout << "Time point PU: " << newvec[i].ve << " - " << newvec[i].vl << endl;
                        //cout << "Time point DL: " << newvec[dl].ve << " - " << newvec[dl].vl << endl;
                        //getchar();

                    }
                }

                //fix depot

                //newvec[0].ve = 540;
                //newvec[0].vl = 1020;
                //newvec[0].vxs = ;
                //newvec[0].vys = ;    

                cout << "Vector: " << endl;
                for (int i = 0; i < newvec.size(); i++){
                    cout << "i: " << newvec[i].index << " - " << newvec[i].vxs << "    " << newvec[i].vys << " :: " << newvec[i].vload << " - " << newvec[i].ve << " - " << newvec[i].vl << endl;
                }
                //getchar();
                output(info, newvec, p);
            }
        }
    }

}

void getInstanceScale(int argc, char** argv, Info *info){
    
    if (argc < 3) {
        cout << "\nMissing parameters\n";
        // cout << " ./exeSARP [Instance] [Optimization strategy] [Scenario]"<< endl;
        cout << " ./exe [Instance] [Instance Scale]"<< endl;
        exit(1);
    }
    
    if (argc > 3) {
        cout << "\nToo many parameters\n";
        cout << " ./exe [Instance] [Instance Scale]" << endl;
        exit(1);
    }

    pair<int, int> dimensions;
    vector< pair<int, int> > auxvec;

    info->scale = argv[2];

    if(info->scale == "S"){
        for (int i = 5; i < 11; i++){
            info->vecOfm.push_back(i);
            info->vecOfn.push_back(i);
        }
        for (int i = 10; i < 16; i++){
            info->scaleVec.push_back(i);
            info->dimVec.push_back(auxvec);
        }
        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }

    }
    else if(info->scale == "M"){
       for (int i = 5; i < 31; i++){
            if (i % 5 == 0 && i != 25){
                info->vecOfn.push_back(i);
            }
        }
        for (int i = 10; i < 31; i++){
            if (i % 5 == 0 && i != 25){
                info->vecOfm.push_back(i);
            }
        }

        for (int i = 20; i < 61; i++){
             if (i % 5 == 0 && i != 55){
                info->scaleVec.push_back(i);
                info->dimVec.push_back(auxvec);
            }           
        }

        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }


    }
    else if(info->scale == "L"){
       for (int i = 20; i < 51; i++){
            if (i % 10 == 0){
                info->vecOfn.push_back(i);
            }
        }
        for (int i = 40; i < 71; i++){
            if (i % 10 == 0){
                info->vecOfm.push_back(i);
            }
        }

        for (int i = 70; i < 121; i++){
             if (i % 10 == 0){
                info->scaleVec.push_back(i);
                info->dimVec.push_back(auxvec);
            }           
        }

        for (int k = 0; k < info->scaleVec.size(); k++){
            for (int i = 0; i < info->vecOfn.size(); i++){
                for (int j = 0; j < info->vecOfm.size(); j++){
                    dimensions.first = info->vecOfn[i];
                    dimensions.second = info->vecOfm[j];
                    if (dimensions.first + dimensions.second < info->scaleVec[k]){
                        continue;
                    }
                    else if (dimensions.first + dimensions.second > info->scaleVec[k]){
                        break;
                    }
                    else{
                        info->dimVec[k].push_back(dimensions);
                    }
                }
            }
        }

        cout << "Vectors of dimensions: " << endl;
        for (int i = 0; i < info->dimVec.size(); i++){
            cout << "dimension: " << info->scaleVec[i] << ": " << endl;
            for (int j = 0; j < info->dimVec[i].size(); j++){
                cout << info->dimVec[i][j].first << " - " << info->dimVec[i][j].second << endl;
            }
        }
    }
    else{
        cout << "\nInstance scale should be:\n";
        cout << "1.Small (S)\n2.Medium (M)\n3.Large (L)\n" << endl;
        exit(1);
    }
}

int main (int argc, char *argv[]) {

    Info info;
 
    getInstanceScale(argc, argv, &info);

    ReadData(argc, argv, &info);


    return 0;

}

void output(Info *info, vector<OrStruct> &newvec, int p)
{
    string outputname;
    string instclass;
    string strN, strM, strP;

    strN = to_string(info->n);
    strM = to_string(info->m);
    strP = to_string(p);
    //info->K = floor(info->n/2);
    info->K = info->n-1;

    outputname = "sfsarp-" + strN + "-" + strM + "-" + strP + ".txt";
    cout << "output: " << outputname << endl;
    //getchar();

    ofstream ofile;

    ofile.open(outputname);
    
    ofile << info->K << "\t" << 5 << "\t" << info->n << "\t" << info->m << endl;

    for (int i = 0; i < newvec.size(); i++){
        ofile << i << "\t" << setw(9) << fixed << setprecision(4) << newvec[i].vxs << "\t" << setw(9) << newvec[i].vys << "\t" << fixed << setprecision(0) << newvec[i].vload << "\t" << fixed << setprecision(0) << newvec[i].ve << "\t" << newvec[i].vl << endl;
    }

    newvec.clear();
}