#ifndef VEHICLE_H
#define VEHICLE_H

#include <cstdio>
#include <vector>

#include "Utils.h"

using namespace std;

class Vehicle {
   private:
    int id;
    int depot;
    int resources;
    bool extra;
    static int objCounter;
    vector<int> capacity;
    double routeDur;
    double earliestTW;
    double latestTW;
    double violCoef;

   public:
    Vehicle(bool extra, int resources, vector<int> capacity, double routeDur,
            double earliestTW, double latestTW, int depot);

    Vehicle(bool extra, int resources);

    int getId() { return id; }
    int getDepot() { return depot; }
    int getResources() { return resources; }
    bool isExtra() { return extra; }
    vector<int> getCapacity() { return capacity; }
    double getRouteDur() { return routeDur; }
    double getEarliestTW() { return earliestTW; }
    double getLatestTW() { return latestTW; }
    double getViolCoef() { return violCoef; }

    void setDepot(int d) { this->depot = d; }
    void setE(double e) { this->earliestTW = e; }
    void setL(double l) { this->latestTW = l; }
    void setCapacity(int idx, int value) { this->capacity[idx] = value; }

    bool isSameType(Vehicle vhc);

    void print();

    bool operator==(const Vehicle &vhc) const;
};

#endif
