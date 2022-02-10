#ifndef ROUTE_H
#define ROUTE_H

#include <algorithm>
#include <climits>
#include <fstream>
#include <iostream>
#include <queue>
#include <utility>
#include <vector>

#include "Data.h"
#include "Sequence.h"
#include "Utils.h"
#include "Vehicle.h"

using namespace std;

typedef pair<double, int> pdi;

class Route {
   protected:
    int id;
    static int objCounter;
    Data *data;
    Vehicle vehicle;
    int penalCoef;
    double cost;
    bool extra;
    vector<bool> neighborhoodStatus;

   public:
    vector<int> path;

    Route(Data *data, Vehicle vhc);
    Route(const Route &rt);
    ~Route();

    inline int getId() { return id; }
    inline Vehicle *getVehicle() { return &vehicle; }
    inline Vehicle getVehicleCopy() { return vehicle; }
    inline void setVehicle(Vehicle vhc) { vehicle = vhc; }
    void updateDepot();

    vector<int> getPath() { return path; }
    vector<int>::iterator getPathBegin() { return path.begin(); }
    vector<int>::iterator getPathEnd() { return path.end(); }
    inline int getPathSize() { return path.size(); }
    inline double getCost() { return cost; }

    inline Request *getRequestByPos(int position) { return &data->requests[path[position]]; }
    inline int getRequestIdByPos(int pos) { return path[pos]; }

    void insertRequest(int id, int position);
    void insertBlockByPos(int position, vector<int> *block);
    int removeRequest(int pos);
    vector<int> *removeBlock(int start, int end);
    vector<int> *removeBlockByPos(int pos, int blockSize);
    vector<int> *removeBlockById(int id, int blockSize);

    void swapRequests(int idx1, int idx2);
    void changeRequest(int idx, int idVertex);
    void changeVhc(Vehicle vhc);
    void reinsertRequests(int idx1, int idx2, int idx3, int idx4);
    void rmvInsertRequests(int idx1, int idx2, int idx3, int idPickup);

    bool getNeighborhoodStatus(NEIGHBORHOOD neighborhood) { return neighborhoodStatus[neighborhood]; }
    void setNeighborhoodStatus(NEIGHBORHOOD neighborhood, bool value) { neighborhoodStatus[neighborhood] = value; }

    int getPenalCoef() { return penalCoef; }
    bool isExtra() { return extra; }

    void update();
    void calcCost();

    void print();
    void printPath();
    void clear();

    bool operator==(const Route &r) const;
    bool operator<(const Route &r) const;
};

#endif