#ifndef RIDETIMECHECKER_H
#define RIDETIMECHECKER_H

#include "Checker.h"

class RideTimeChecker : public Checker {
   protected:
    vector<int> pairPosition;
    vector<int> blockPath;
    vector<vector<double> > sumWT;
    vector<double> rideTimes;
    vector<double> waitingTime;
    vector<double> departureTime;
    vector<double> arrivalTime;
    vector<double> startTime;

    void createStructures();
    void updateStructures(double min_FTS_WT, int pos_from);

    double getArrivalTime(double cost_arc, int pos);
    double getStartTime(double earliestTW, int pos);
    double getDepartureTime(double service_time, int pos);
    double getMinFTS(int pos);
    void getWaitingTimeRoute();

    double getRideTime(int pickup, int delivery);
    double getWT(int pos_to_node);
    double getFTS(int pos_from_node, int pos_to_node);
    double getSumWT(double sum_waiting_time, int j);

    void algorithmFTS();

    bool verifyRideTime();
    bool verification();
    void createSchedules();
    void getRouteRideTime();
    void printStructures(string filePath);
    void printStructures();
    void printDetails();

   public:
    RideTimeChecker(vector<int> *path, Data *data);
    RideTimeChecker() = default;
    bool check();

    ~RideTimeChecker() {}
};

#endif