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
    vector<double> delta;
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

    //cout << "Instance Index: " << InstanceIndex << endl;
    // getchar();

    return InstanceIndex;
}

int getRandomValue(int min, int max) {
    return rand() % (max - min + 1) + min;
}

double CalcDistEuc2 (double X1, double Y1, double X2, double Y2){

    double a = pow ( X1 - X2, 2 );
    double b = pow ( Y1 - Y2, 2 );
    //cout << "returning: " << floor(sqrt(a + b)*10)/10 << endl;
    return floor(sqrt(a + b)*10)/10;
}


void createDelta(Info *info)
{
    pair <double,double> p1, p2;

    double dist;
    double delta;

    double scalingfactor = 50; //to scale grubhub (ghsarp) distance values

    for (int i = 0; i < info->n; i++){
        int j = info->n + i;


        p1.first = info->coordVec[i].first;
        p1.second = info->coordVec[i].second;
        p2.first = info->coordVec[j].first;
        p2.second = info->coordVec[j].second;

        //cout << "pairs of nodes: " << i << " - " << j << endl;
        //cout << "p1: " << p1.first << " - " << p1.second << endl;
        //cout << "p2: " << p2.first << " - " << p2.second << endl;
        //getchar();

        //dist = CalcDistEuc(p1.first, p1.second, p2.first, p2.second);
        dist = CalcDistEuc2(p1.first, p1.second, p2.first, p2.second);
        dist = dist/scalingfactor;
 
        delta = (double)dist/info->speed;

        info->delta.push_back(delta);
    }
    for (int i = info->n; i < 2*info->n; i++){ //dummy deltas for delivery of passengers
        delta = 0;

        info->delta.push_back(delta);
    }

    //________________________________________
    //if tight time windows, deltas for parcels also need to be stored
    for (int i = 2*info->n; i < (2*info->n + info->m); i++){
        int j = info->m + i;
        p1.first = info->coordVec[i].first;
        p1.second = info->coordVec[i].second;
        p2.first = info->coordVec[j].first;
        p2.second = info->coordVec[j].second;
        //cout << "Node: " << i << " - " << j << endl;

        //cout << "Coordinates: " << endl;
        //cout << "p1: " << p1.first << " - " << p1.second << endl;
        //cout << "p2: " << p2.first << " - " << p2.second << endl;

        //dist = CalcDistEuc(p1.first, p1.second, p2.first, p2.second);
        dist = CalcDistEuc2(p1.first, p1.second, p2.first, p2.second);
        //cout << "dist: " << dist << endl;

        dist = dist/scalingfactor;

        //cout << "After scaling: " << dist << endl;

        delta = (double)dist/info->speed;

        //cout << "Value of delta: " << delta << endl;

        info->delta.push_back(delta);
    }
    //____________________________________
    
    //cout <<"Delta vector: " << endl;

    //for (int i = 0; i < info->delta.size(); i++){
    //    cout << info->delta[i] << endl;
    //}
    //getchar();

}

void generateInst(int argc, char** argv, Info *info){

    cout << "n: " << info->n << " - m: " << info->m << endl;
    //info->seed = pow(info->n, 8) + pow(info->m, 3) + pow(stod(info->index), 6); //ghsarp 10-5-5 could not be solved
    info->seed = pow(info->n, 5) + pow(info->m, 2) + pow(stod(info->index), 3); 
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
    std::string line;

    char *instance;
    instance = argv[1];
    cout << "Instance: " << instance << endl;
    //ifstream in(instance, ios::in);

    std::ifstream in(instance);

    bool inNodeCoordSection = false;

    if (!in.is_open()) {
        std::cerr << "Error: Could not open the file!" << std::endl;
        exit(1);
    }

    while ( file.compare("DIMENSION:") != 0 && file.compare("DIMENSION") != 0 ){
        in >> file;
    }
    
    in >> instV;


    while (file.compare("NODE_COORD_SECTION") != 0){
        in >> file;
    }

    //auxtempdata.first = 0;
    //auxtempdata.second = 0;

    for (int i = 0; i < instV; i++){
        in >> file >> auxtempdata.first >> auxtempdata.second;
        tempData.push_back(auxtempdata);
    }

    //cout << "tempData size: " << tempData.size() << endl;
    //for (int i = 0; i < tempData.size(); i++){
    //    cout << "(A) Node: " << i << " - " << tempData[i].first << " - " << tempData[i].second << endl;
    //}

    //for (int i = 0; i < instV; i++) {
    //    in >> file >> tempData[i].first >> tempData[i].second;
    //}

    //rearranging passengers and parcels pickup and delivery coordinates
    //order: pass PU - pass DL - parc PU - parcDL

    for (int i = 2; i < 2*info->n + 2*info->m + 2; i++){
        auxtempdata.first = tempData[i].first;
        auxtempdata.second = tempData[i].second;
        //cout << "Node: " << i << " - " << auxtempdata.first << " - " << auxtempdata.second << endl;
        if (i < 2 + 2*info->n){

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
    auxtempdata = tempData[0];
    //cout << "tempData first: " << auxtempdata.first << "-" << auxtempdata.second << endl;
    info->coordVec.push_back(auxtempdata);
    info->loadVec.push_back(0);

    cout << "Coordinates rearranged: " << endl;
    for (int i = 0; i < info->coordVec.size(); i++){
        cout << i << "\t" << info->coordVec[i].first << "\t" << info->coordVec[i].second << endl;
    }
    //getchar();

    for (int i = 0; i < info->coordVec.size(); i++){
        auxtempdata.first = 0;
        auxtempdata.second = 0;
        info->twVec.push_back(auxtempdata);
    }

    //calculate deltas
    createDelta(info);

    //generate time windows
    double timePoint;
    double service = 5;

    for (int i = 0; i < info->n; i++){ // passenger pickup node
        double delta = ceil(info->delta[i]*60);
        cout << "\n\nPASSENGERS" << endl;
        cout << "Nodes: " << i << " - " << i + info->n << endl;
        cout << "Delta in hours: " << info->delta[i] << endl;
            //timePoint = 30 + (rand() % 1400);
            //timePoint = getRandomValue(560, 1110);
            timePoint = 560 + rand() % 480;


        cout << "Calculated time point: " << timePoint << endl;
        cout << "Threshold: " << timePoint + delta + 2*service + 60 << endl;
        cout << "Calulated delta: " << delta << endl;
        while (timePoint + delta + 2*service > 1140){  
            cout << "regen time point passenger" << endl;

            timePoint = 560 + rand() % 480; 

            //info->tsVec[i].first = 30 + rand() % 1300;
            //info->tsVec[i].first = 30 + rand() % 1000;
            // cout << "Time point for node " << i << ": " << info->tsVec[i].first << endl; 
            // //getchar();
        }

        auxtempdata.first = timePoint;
        auxtempdata.second = timePoint;

        info->twVec[i].first = auxtempdata.first;
        info->twVec[i].second = auxtempdata.second;
        
        int dl = i + info->n;

        if (delta < 1){
            delta = 1;
        }

        info->twVec[dl].first = auxtempdata.first + delta;
        info->twVec[dl].second = auxtempdata.second + delta;
    }

    for (int i = 2*info->n; i < 2*info->n + info->m; i++){ //adapted for tighter parcel time windows
        cout << "\n\nPARCELS" << endl;

        cout << "Nodes: " << i << " - " << i + info->m << endl;
        cout << "Delta in hours: " << info->delta[i] << endl;
        double delta = ceil(info->delta[i]*60);

        //timePoint = 30 + (rand() % 1400);
        timePoint = 540 + rand() % 470;

        cout << "Calculated time point: " << timePoint << endl;
        cout << "Threshold: " << timePoint + delta + 2*service + 60 << endl;
        cout << "Calulated delta: " << delta << endl;



        while (timePoint + delta + 2*service + 60 > 1140){  //remove possibility of parcel being delivered after 1140
            //cout << "regen time point parcel" << endl;
            timePoint = 540 + rand() % 470; 

            //info->tsVec[i].first = 30 + rand() % 1300;
            //info->tsVec[i].first = 30 + rand() % 1000;
            // cout << "Time point for node " << i << ": " << info->tsVec[i].first << endl; 
            // //getchar();
        }

        //if (timePoint < 60){
        //    timePoint += (rand() % 60)*2;
        //}

        //else if (timePoint > 1380){
        //    timePoint -= (rand() % 60)*2;
        //}

        auxtempdata.first = timePoint;
        //auxtempdata.second = timePoint + 30;
        auxtempdata.second = timePoint + 60;

        info->twVec[i].first = auxtempdata.first;
        info->twVec[i].second = auxtempdata.second;

        int dl = i + info->m;

        if (delta < 1){
            delta = 1;
        }

        info->twVec[dl].first = auxtempdata.second + delta + service;
        //info->twVec[dl].second = info->twVec[dl].first + 60; //older assumption
        info->twVec[dl].second = 1140;  

        //cout << "Time windows vector: " << endl;
        //for (int a = 0; a < info->twVec.size(); a++){
        //    cout << "node: " <<  a << "\t" << info->twVec[a].first << "\t" << info->twVec[a].second << endl;
        //}
        //getchar();
    }



    for (int i = 2*info->n + 2*info->m; i < info->coordVec.size(); i++){
        

        //auxtempdata.first = 0;
        //auxtempdata.second = 1440;
        //auxtempdata.first = 540;
        //auxtempdata.second = 1140;

        info->twVec[i].first = 540;
        info->twVec[i].second = 1140;
    }

    //making file

    //cout << "Time windows vector: " << endl;
    //for (int a = 0; a < info->twVec.size(); a++){
    //    cout << "node: " <<  a << "\t" << info->twVec[a].first << "\t" << info->twVec[a].second << endl;
    //}
    //getchar();    

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

    cout << "\n\nInstance size: " << info.V << endl;

    for (int i = 5; i < 11; i++){
        info.parcelamounts.push_back(i);
    }
    //for (int i = 0; i < info.parcelamounts.size(); i++){
    //    cout << info.parcelamounts[i] << " - ";
    //}  
    //cout << endl;
    
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