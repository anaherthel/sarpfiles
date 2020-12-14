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

struct instInfoN
{
    //General Info
    string name;//instance name
    string scen;//scenario
    int pperc;//percentage of parcels
    string stats;//solution status
    double soltime;
    double objval;//obj function value (will be desmembered)
    double pCust;
    double pParc;
    double costs;
	int K;//number of vehicles
    int n;//number of passengers
    int m;//number of parcels
	int servP;//served parcels
    double lb; //lower bound
    double ub; //uppper bound
    double gap; //gap btw ub lb
    long int tree; //size of tree
};

struct distInfo
{
	//Distance Info
	double cDist; //client
	double pDist; //parcel
	double bDist; //both together
	double eDist; //empty vehicle
};

struct timeInfo
{
	//Time Info
	double cTime;
	double pTime;
	double bTime;
	double eTime;
	double cWait;
	double pWait;	
};

void readData (int argc, char** argv, vector<instInfoN>& vecInst, vector<distInfo>& vecDist, vector<timeInfo>& vecTime);
bool substrPosition(string str, string sub_str);
void createOutputInst(vector<instInfoN>& vecInst, string instName);
void createOutputDist(vector<instInfoN>& vecInst, vector<distInfo>& vecDist, string instName);
void createOutputTime(vector<instInfoN>& vecInst, vector<timeInfo>& vecTime, string instName);
string getInstName (char **argv);

//************************end of headers

void createOutputInst(vector<instInfoN>& vecInst, string instName)
{

    string outputname;

    outputname = "stats" + instName + ".csv";
    
    ofstream ofile;
    
    ofile.open(outputname);

    string firstInst = vecInst[0].name;


    for(int i = 0; i < vecInst.size(); i++){
        if(vecInst[i].name == firstInst){
            ofile << vecInst[i].scen << ", " << vecInst[i].pperc << endl;
            ofile << "Instance,n,m,served,prof Customer,prof Parcel,costs,K,Sol Time,Sol Val,sol Stat,LB,UB,GAP" << endl;
        }
        ofile << vecInst[i].name << "," << vecInst[i].n << "," << vecInst[i].m << "," << vecInst[i].servP << "," << vecInst[i].pCust << "," << vecInst[i].pParc << "," << vecInst[i].costs << "," << vecInst[i].K << "," << vecInst[i].soltime << "," << vecInst[i].objval << "," << vecInst[i].stats << "," << vecInst[i].lb << "," << vecInst[i].ub << "," << vecInst[i].gap << endl;
    } 
}

void createOutputDist(vector<instInfoN>& vecInst, vector<distInfo>& vecDist, string instName)
{

    string outputname;

    outputname = "dist" + instName + ".csv";
    
    ofstream ofile;
    
    ofile.open(outputname);

    string firstInst = vecInst[0].name;

    double totalDist;

    for(int i = 0; i < vecInst.size(); i++){
        if(vecInst[i].name == firstInst){
            ofile << vecInst[i].pperc << endl;
            ofile << "Instance,c(km),p(km),b(km),e(km),Total,c(%),p(%),b(%),e(%)" << endl;
        }
        totalDist = vecDist[i].cDist + vecDist[i].pDist + vecDist[i].bDist + vecDist[i].eDist;

        ofile << vecInst[i].name << "," << vecDist[i].cDist << "," << vecDist[i].pDist << "," << vecDist[i].bDist << "," << vecDist[i].eDist << "," << totalDist << "," << (vecDist[i].cDist/totalDist)*100 << "," << (vecDist[i].pDist/totalDist)*100 << "," << (vecDist[i].bDist/totalDist)*100 << "," << (vecDist[i].eDist/totalDist)*100 << endl;

    } 
}

void createOutputTime(vector<instInfoN>& vecInst, vector<timeInfo>& vecTime, string instName)
{

    string outputname;

    outputname = "time" + instName + ".csv";
    
    ofstream ofile;
    
    ofile.open(outputname);

    string firstInst = vecInst[0].name;

    double totalTime;

    for(int i = 0; i < vecInst.size(); i++){
        if(vecInst[i].name == firstInst){
            ofile << vecInst[i].pperc << endl;
            ofile << "Instance,c(h),p(h),b(h),e(h),waitc(h),waitp(h),Total,c(%),p(%),b(%),e(%),waitc(%),waitp(%)" << endl;
        }
        totalTime = vecTime[i].cTime + vecTime[i].pTime + vecTime[i].bTime + vecTime[i].eTime + vecTime[i].cWait + vecTime[i].pWait;

        ofile << vecInst[i].name << "," << vecTime[i].cTime << "," << vecTime[i].pTime << "," << vecTime[i].bTime << "," << vecTime[i].eTime << "," << vecTime[i].cWait << "," << vecTime[i].pWait << "," << totalTime << "," << (vecTime[i].cTime/totalTime)*100 << "," << (vecTime[i].pTime/totalTime)*100 << "," << (vecTime[i].bTime/totalTime)*100 << "," << (vecTime[i].eTime/totalTime)*100 << "," << (vecTime[i].cWait/totalTime)*100 << "," << (vecTime[i].pWait/totalTime)*100 << endl;
    } 
}

string getInstName (char **argv){

    string filename(argv[1]);

    string::size_type loc = filename.find_last_of("/");
    string::size_type loc2 = filename.find_first_of(".");
    string InstanceName;

    InstanceName.append(filename, loc+1, loc2-loc-1 );

    return InstanceName;
}


bool substrPosition(string str, string sub_str) {
    bool flag = false;
    for (int i = 0; i < str.length(); i++) {
      if (str.substr(i, sub_str.length()) == sub_str) {
         // cout << i << " ";
         flag = true;
      }
    }

    return flag;
}

// void extractData(bool flag, string tag, instInfoN &inst, char varType, string varStruct)
// {
//     // flag = substrPosition(line, "size of m");

//     // if (flag) {
//     //     loc = line.find_first_of(":");
//     //     inst.m = stoi(line.substr(loc + 1, line.size() - loc - 1));
//     //     cout << "num of m:" << inst.m << endl;
//     //     getchar();
//     // }
//     // flag = false;

//     flag = substrPosition(line, tag);

//     if (flag) { 
//         loc = line.find_first_of(":");
//         inst. = stoi(line.substr(loc + 1, line.size() - loc - 1));
//         cout << "num of m:" << inst.m << endl;
//         getchar();
//     }
//     flag = false;
// }

void readData (int argc, char** argv, vector<instInfoN>& vecInst, vector<distInfo>& vecDist, vector<timeInfo>& vecTime)
{
   if (argc < 3) {
        cout << "\nMissing parameters\n";
        cout << " ./exeTreatRes [TestRaw] [Model]"<< endl;
        exit(1);
    }
    
    if (argc > 3) {
        cout << "\nToo many parameters\n";
        cout << " ./exeTreatRes [TestRaw] [Model]" << endl;
        exit(1);
    }

	string file;
    string line;

    instInfoN inst;
    distInfo dist;
    timeInfo time;

    string::size_type loc, loc2;

    char *instance; 
    instance = argv[1];

    ifstream in(instance, ios::in);

    string testStr;
    bool optflag = false;

    bool okall = false;

    if( !in ) {
        cout << "the file could not be opened\n";
        exit (1);
    }

    bool flag = false;

	if(in.is_open()) {

	    while(getline(in, line)) {

            flag = substrPosition(line, "Solving");

	        if (flag) {
                loc = line.find_first_of(":");
                loc2 = line.find_last_of(":", line.size());
                inst.name = line.substr(loc + 3, loc2 - loc - 3);

                // cout << "Name of instance:" << inst.name << endl;
                // getchar();
            }
            flag = false;

            flag = substrPosition(line, "Scenario");

            if (flag) {
                loc = line.find_first_of(":");
                inst.scen = line.substr(loc + 3, line.size() - loc - 3);

                // cout << "Name of scenario:" << inst.scen << endl;
                // getchar();
            }
            flag = false;

            flag = substrPosition(line, "ParcelP");

            if (flag) {
                loc = line.find_first_of(":");
                inst.pperc = stoi(line.substr(loc + 1, line.size() - loc - 1)); //string to int

                // cout << "Number of parcels:" << inst.pperc << endl;
                // getchar();
            }
            flag = false;

            flag = substrPosition(line, "Sol status");

            if (flag) {
                loc = line.find_first_of(":");
                testStr = line.substr(loc + 2, line.size() - loc - 2);
                if (testStr == "Optimal" || testStr == "Feasible" ){
                    optflag = true;
                    inst.stats = line.substr(loc + 2, line.size() - loc - 2);
                    // cout << "Status:" << inst.stats << endl;
                    // getchar();
                }
                else{
                    optflag = false;
                }
            }
            flag = false;

            if (optflag){

                flag = substrPosition(line, "Tree_Size");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.tree = stoi(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "tree size:" << inst.tree << endl;
                    // getchar();
                }
                flag = false;
                
                flag = substrPosition(line, "LB");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.lb = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "LB:" << inst.lb << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "UB");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.ub = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "UB:" << inst.ub << endl;
                    // getchar();
                    inst.gap = (double)((inst.ub - inst.lb)/inst.ub) * 100;
                    // cout << "GAP:" << inst.gap << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Obj Val");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.objval = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "objval:" << inst.objval << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Solve Time");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.soltime = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "objval:" << inst.soltime << endl;
                    // getchar();
                }
                flag = false;

               flag = substrPosition(line, "Customer profit");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.pCust = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "numVeh:" << inst.K << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Parcel profit");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.pParc = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "numVeh:" << inst.K << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Costs");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.costs = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "numVeh:" << inst.K << endl;
                    // getchar();
                }
                flag = false;


                flag = substrPosition(line, "Number of Vehicles");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.K = stoi(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "numVeh:" << inst.K << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "size of n");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.n = stoi(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "num of n:" << inst.n << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "size of m");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.m = stoi(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "num of m:" << inst.m << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Served parcels");

                if (flag) {
                    loc = line.find_first_of(":");
                    inst.servP = stoi(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Served parcels:" << inst.servP << endl;
                    // getchar();
                }
                flag = false;

                //*******************************

                flag = substrPosition(line, "Total passenger time");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.cTime = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Customer Time:" << time.cTime << endl;
                    // getchar();
                }
                flag = false;
               
                flag = substrPosition(line, "Total parcel time");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.pTime = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Parcel Time:" << time.pTime << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Total combined transportation time");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.bTime = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Both Time:" << time.bTime << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Total idle time");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.eTime = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Empty Time:" << time.eTime << endl;
                    // getchar();
                }
                flag = false;
                
                flag = substrPosition(line, "Waiting time passenger");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.cWait = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Wait Time Customer:" << time.cWait << endl;
                    // getchar();
                }
                flag = false;
                
                flag = substrPosition(line, "Waiting time goods");

                if (flag) {
                    loc = line.find_first_of(":");
                    time.pWait = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Wait Time Parcel:" << time.pWait << endl;
                    // getchar();
                }
                flag = false;
               
                //******************************************

                flag = substrPosition(line, "Total passenger distance");

                if (flag) {
                    loc = line.find_first_of(":");
                    dist.cDist = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Client dist:" << dist.cDist << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Total parcel distance");

                if (flag) {
                    loc = line.find_first_of(":");
                    dist.pDist = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Parcel dist:" << dist.pDist << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Total combined transportation distance");

                if (flag) {
                    loc = line.find_first_of(":");
                    dist.bDist = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Both dist:" << dist.bDist << endl;
                    // getchar();
                }
                flag = false;

                flag = substrPosition(line, "Total idle distance");

                if (flag) {
                    loc = line.find_first_of(":");
                    dist.eDist = stod(line.substr(loc + 1, line.size() - loc - 1));
                    // cout << "Empty dist:" << dist.eDist << endl;
                    // getchar();
                    okall = true;
                }
                flag = false;

            }



            if(okall){
                vecInst.push_back(inst); //inst done
                vecTime.push_back(time); //time done
                vecDist.push_back(dist); //dist done
                okall = false;
            }

        }


        in.close();
	}
    
    // cout << "Size of vector inst: " << vecInst.size() << endl;
    // cout << "Size of vector dist: " << vecDist.size() << endl;
    // cout << "Size of vector time: " << vecTime.size() << endl;
    
    // getchar();


    // cout << "Vec of instance: " << endl;

    // for (int i = 0; i < vecInst.size(); i++){
    //     cout << vecInst[i]->name << " " << vecInst[i]->scen << " " << vecInst[i]->pperc << endl;
    //     getchar();

    // }
    // cout << endl;



    // while ( file.compare("Solving:") != 0){
    //     in >> file;
    // }
}

int main (int argc, char *argv[])
{
    vector<instInfoN> vecInst;
    vector<distInfo> vecDist;
    vector<timeInfo> vecTime;
        
    string instName;

    readData (argc, argv, vecInst, vecDist, vecTime);

    instName = getInstName(argv);

    createOutputInst(vecInst, instName);

    createOutputDist(vecInst, vecDist, instName);

    createOutputTime(vecInst, vecTime, instName);


    return 0;

}