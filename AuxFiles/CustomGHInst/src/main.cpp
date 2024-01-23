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

using namespace std;

//Take the original instance
//Extract the instance size (10-15)
//From the instance size, consult list of possible parcel percentages (hard coded database)
//for each parcel percentage, generate new instance in csarp format.

struct Info{
    int n;
    int m;
    int V;
    int K;
    int dummy;
    string index;

    double service = 5/60;
    double speed = 41;
    long double seed; 

    vector<int> parcelamounts;
    vector<int> loadVec;
    vector< pair<double, double> > twVec;
    vector< pair <double, double> > coordVec;
};

string getInstName (char **argv);
string getInstIndex (char **argv);
void generateInst(int argc, char** argv, Info *info);
void tofile(Info *info);
void clearInfo(Info *info);

string getInstName (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_last_of("/");
    string::size_type loc2 = filename.find_first_of(".");
    string InstanceName;

    InstanceName.append(filename, loc+1, loc2-loc-1 );

    string::size_type loc3 = InstanceName.find_last_of("-");
    string::size_type loc4 = InstanceName.find_first_of("-");
    string instsizestr;

    instsizestr.append(InstanceName, loc4+1, loc3-loc4-1 );

    return instsizestr;
}

string getInstIndex (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_last_of("-");
    string::size_type loc2 = filename.find_last_of(".");
    string InstanceIndex;

    InstanceIndex.append(filename, loc+1, loc2-loc-1 );

    cout << "Instance Index: " << InstanceIndex << endl;
    // getchar();

    return InstanceIndex;
}

int getRandomValue(int min, int max) {
    return rand() % (max - min + 1) + min;
}

void generateInst(int argc, char** argv, Info *info){

    cout << "n: " << info->n << " - m: " << info->m << endl;
    info->seed = pow(info->n, 8) + pow(info->m, 3) + pow(stod(info->index), 6);
    cout << "seed: " << info->seed << endl;
    srand(info->seed);

    int K = 1;
    
    int scale;
    int scCounter = 0;

    int refpoint = K + 1;
    int instV;
    int dummy = 1;
    info->dummy = info->K;

    vector < pair <double, double> > tempData;
    // vector<double> auxtempdata;
    pair <double, double> auxtempdata;

    vector < pair <double, double> > pasdeliveries;   
    vector < pair <double, double> > paspickups;
    vector < pair <double, double> > pardeliveries;   
    vector < pair <double, double> > parpickups;

    string file;
    char *instance;
    instance = argv[1];
    fstream in(instance, ios::in);

    while ( file.compare("DIMENSION:") != 0 && file.compare("DIMENSION") != 0 ){
        in >> file;
    }
    
    in >> instV;

    cout << "\nn: " << info->n << " m: " << info->m << endl; 

    while (file.compare("NODE_COORD_SECTION") != 0){
        in >> file;
    }

    auxtempdata.first = 0;
    auxtempdata.second = 0;

    for (int i = 0; i < instV; i++){
        tempData.push_back(auxtempdata);
    }

    for (int i = 0; i < instV; i++) {
        in >> file >> tempData[i].first >> tempData[i].second;
    }
    //putting starting point in the end
    auxtempdata.first = tempData[0].first;
    auxtempdata.second = tempData[0].second;

    tempData.push_back(auxtempdata);
    tempData.erase(tempData.begin(), tempData.begin()+2);

    //rearranging passengers and parcels pickup and delivery coordinates
    //order: pass PU - pass DL - parc PU - parcDL

    for (int i = 0; i < instV-2*K; i++){
        auxtempdata.first = tempData[i].first;
        auxtempdata.second = tempData[i].second;
        if (i < 2*info->n){
            if (i % 2 == 0){
                paspickups.push_back(auxtempdata);
            }
            else{
                pasdeliveries.push_back(auxtempdata);
            }
        }
        else{
            if (i % 2 == 0){
                parpickups.push_back(auxtempdata);
            }
            else{
                pardeliveries.push_back(auxtempdata);
            }            
        }
    }

    //coordinates of the depot
    for (int i = 0; i < paspickups.size(); i++){
        auxtempdata.first = paspickups[i].first;
        auxtempdata.second = paspickups[i].second;  
        info->coordVec.push_back(auxtempdata);
        info->loadVec.push_back(3);
    }

    for (int i = 0; i < pasdeliveries.size(); i++){
        auxtempdata.first = pasdeliveries[i].first;
        auxtempdata.second = pasdeliveries[i].second;  
        info->coordVec.push_back(auxtempdata);
        info->loadVec.push_back(-3);
    }

    for (int i = 0; i < parpickups.size(); i++){
        auxtempdata.first = parpickups[i].first;
        auxtempdata.second = parpickups[i].second;  
        info->coordVec.push_back(auxtempdata);
        info->loadVec.push_back(1);
    }

    for (int i = 0; i < pardeliveries.size(); i++){
        auxtempdata.first = pardeliveries[i].first;
        auxtempdata.second = pardeliveries[i].second;  
        info->coordVec.push_back(auxtempdata);
        info->loadVec.push_back(-1);
    }

    //adding depot coordinates last (csarp template - euclidean coordinates)
    auxtempdata = tempData[tempData.size()-1];
    info->coordVec.push_back(auxtempdata);
    info->loadVec.push_back(0);

    // cout << "Coordinates rearranged: " << endl;
    // for (int i = 0; i < info->coordVec.size(); i++){
    //     cout << i << "\t" << info->coordVec[i].first << "\t" << info->coordVec[i].second << endl;
    // }
    // getchar();

    //generate time windows
    double timePoint;

    for (int i = 0; i < info->n; i++){
            //timePoint = 30 + (rand() % 1400);
            timePoint = getRandomValue(560, 1110);

            //if (timePoint < 60){
            //    timePoint += (rand() % 60)*2;
            //}

            //else if (timePoint > 1380){
            //    timePoint -= (rand() % 60)*2;
            //}

        auxtempdata.first = timePoint;
        auxtempdata.second = timePoint;

        info->twVec.push_back(auxtempdata);
    }
    for (int i = info->n; i < info->coordVec.size(); i++){

        //auxtempdata.first = 0;
        //auxtempdata.second = 1440;
        auxtempdata.first = 540;
        auxtempdata.second = 1140;

        info->twVec.push_back(auxtempdata);
    }

    //making file

    tofile(info);

}

void tofile(Info *info){
    string outputname;
    string strN, strM, strP;
    
    strN = to_string(info->n);
    strM = to_string(info->m);
    // strP = to_string(info->index);


    outputname = "ghsarp-" + strN + "-" + strM + "-" + info->index + ".txt";
    cout << "output: " << outputname << endl;
    // getchar();

    ofstream ofile;

    ofile.open(outputname);
    
    ofile << info->K << "\t" << 5 << "\t" << info->n << "\t" << info->m << endl;

    for (int i = 0; i < info->coordVec.size(); i++){
        ofile << i << "\t" << setw(9) << fixed << setprecision(4) << info->coordVec[i].first << "\t" << setw(9) << info->coordVec[i].second << "\t" << info->loadVec[i] << "\t" << fixed << setprecision(0) << info->twVec[i].first << "\t" << info->twVec[i].second << endl;
    }


    clearInfo(info);
}
void clearInfo(Info *info){
    info->loadVec.clear();
    info->twVec.clear();
    info->coordVec.clear();
}

string getInstanceType (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_first_of("/");
    string::size_type loc2 = filename.find_last_of("/", filename.size());
    string InstanceType;

    InstanceType.append(filename, loc+1, loc2-loc-1 );

    return InstanceType;
}

int main (int argc, char *argv[]) {

    Info info;
    string instname;

    if (argc < 2) {
        cout << "\nMissing parameters\n";
        cout << " ./exeCustInst [Original Instance]"<< endl;
        exit(1);
    }
    
    if (argc > 2) {
        cout << "\nToo many parameters\n";
        cout << " ./exeCustInst [Original Instance]" << endl;
        exit(1);
    }  

    instname = getInstName(argv);
    info.index = getInstIndex(argv);

    info.V = stoi(instname);

    cout << "Instance size: " << info.V << endl;

    for (int i = 5; i < 11; i++){
        info.parcelamounts.push_back(i);
    }
    for (int i = 0; i < info.parcelamounts.size(); i++){
        cout << info.parcelamounts[i] << " - ";
    }  
    cout << endl;
    
    for (int i = 0; i < info.parcelamounts.size(); i++){
        info.m = info.parcelamounts[i];
        info.n = info.V - info.m; 
        if (info.n < 5){
            break;
        }
        info.K = info.n - 1;
        generateInst(argc, argv, &info);
        clearInfo(&info);
    }

    return 0;
}