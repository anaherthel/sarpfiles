#ifndef REQUEST_H
#define REQUEST_H

#include <cstdio>
#include <utility>
#include <vector>

using namespace std;

class Request {
   private:
    int id;
    static int objCounter;
    bool isPickup;  // True -> pickup, False -> delivery
    double serviceTime;
    double maxRideTime;
    double earliestTW;
    double latestTW;
    double X, Y;
    vector<int> resourceRequest;

   public:
    Request(bool isPickup, double serviceTime, double maxRideTime,
            double earliestTW, double latestTW, vector<int> resourceRequest,
            double X, double Y);

    int getId() { return id; }
    int getCounter() { return objCounter; }
    bool getIsPickup() { return isPickup; }
    double getServiceTime() { return serviceTime; }
    double getMaxRideTime() { return maxRideTime; }
    double getEarliestTW() { return earliestTW; }
    double getLatestTW() { return latestTW; }
    double getX() { return X; }
    double getY() { return Y; }
    void setEarliestTW(double earliestTW) { this->earliestTW = earliestTW; }
    void setLatestTW(double latestTW) { this->latestTW = latestTW; }
    vector<int> getResourceRequest() { return resourceRequest; }
    void setResource(int idx, int value) {
        this->resourceRequest[idx] = value;
    }
    void print();
};

#endif