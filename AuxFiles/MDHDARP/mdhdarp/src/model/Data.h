#ifndef _DATA_H_
#define _DATA_H_

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <fstream>  // std::ifstream
#include <iostream>
#include <limits>
#include <vector>

#include "Data.h"
#include "Request.h"
#include "Vehicle.h"

using namespace std;
typedef pair<int, int> arc;

class Data {
   public:
    const char *path;  // Instance path
    FILE *input_file;  // File instance
    char problemId;    // 0 - DARP, 1 - HDARP, 2 - MDHDARP

    int n;         // Number of requests
    int v;         // Number of vehicles
    int m;         // Number of depots
    int R;         // Number of resources
    double Xd[4];  // X coord of each depot
    double Yd[4];  // Y coord of each depot

    vector<int> V;              // All vertices
    vector<Vehicle> vehicles;   // Set of Vehicles
    vector<Request> requests;   // Set of Requests
    vector<arc> A;              // Set of arcs
    vector<vector<arc>> Graph;  // Graph
    vector<vector<double>> t;   // Time Cost Matrix
    vector<vector<double>> c;   // Cost Matrix
    vector<vector<bool>> A_;    // Invalid arcs
    vector<vector<bool>> NCA;   // If a vertex can come after other vertex
    vector<vector<bool>> NCB;   // If a vertex can come before other vertex
    vector<vector<int>> depot_vhc;
    vector<Vehicle> fleet;
    vector<int> maxCapacity;

    Data(const char *filePath, const char *p_id);

    void read();
    void preprocessing();
    void print();

   private:
    // DARP dataset by Cordeau and Laporte (2003)
    void readDARP();
    // HDARP dataset by Parragh (2011) and Braekers et al (2014)
    void readHDARP();
    // MDHDARP dataset by Braekers et al. (2014) and Malheiros et al. (2020)
    void readMDHDARP();
    bool check_RT(int n0, int n1, int n2, int n3);
    void adaptResources();
};

#endif