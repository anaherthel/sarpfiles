#ifndef infoSeq_H
#define infoSeq_H

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <vector>

#include "Data.h"

using namespace std;

class Sequence {
   public:
    double duration;  // minimum duration
    double timewarp;  // minimum timewarp use

    double earliest;  // earliest visit
    double latest;    // latest visit

    double distance;   // cumulated distance
    int first_client;  // first client of the subsequence
    int last_client;   // last client of the subsequence
    int load_sum[10];  // Sum of the loads deliverd/collected (max of 10 resources)
    int load_max[10];  // Maximum cumulative load (max of 10 resources)

    double delta, deltaWT, deltaTW;

    Sequence() = default;

    inline Sequence(Data *data, Request *request) {
        this->duration = request->getServiceTime();
        this->timewarp = 0;
        this->earliest = request->getEarliestTW();
        this->latest   = request->getLatestTW();
        this->distance = 0;

        this->first_client = request->getId();
        this->last_client  = request->getId();

        this->delta = this->deltaTW = this->deltaWT = 0.0;

        for (int i = 0; i < data->R; ++i) {
            this->load_max[i] = max(0, request->getResourceRequest()[i]);
            this->load_sum[i] = request->getResourceRequest()[i];
        }
    }

    double getDuration() { return duration; }
    double getTimewarp() { return timewarp; }
    double getEarliest() { return earliest; }
    double getLatest() { return latest; }
    double getDistance() { return distance; }

    int getFirstClient() { return first_client; }
    int getLastClient() { return last_client; }

    inline double getCost(Vehicle *vehicle) {
        return (distance * vehicle->getViolCoef());
    }

    inline bool isFeasible(Vehicle *vehicle) {
        if (timewarp != 0)
            return false;

        for (int r = 0; r < vehicle->getResources(); ++r) {
            if (max(0, load_max[r] - vehicle->getCapacity()[r])) {
                return false;
            }
        }
        return distance < vehicle->getLatestTW();
    }

    // Concatenation
    inline static Sequence concatenate(Data *data, Sequence &info1, Sequence &info2) {
        Sequence sub;

        sub.delta = info1.duration - info1.timewarp +
                    data->t[info1.last_client][info2.first_client];

        sub.deltaWT  = max(info2.earliest - sub.delta - info1.latest, 0.0);
        sub.deltaTW  = max(info1.earliest + sub.delta - info2.latest, 0.0);
        sub.duration = info1.duration + info2.duration +
                       data->t[info1.last_client][info2.first_client] +
                       sub.deltaWT;
        sub.earliest = max(info2.earliest - sub.delta, info1.earliest) - sub.deltaWT;
        sub.latest   = min(info2.latest - sub.delta, info1.latest) + sub.deltaTW;
        sub.timewarp = info1.timewarp + info2.timewarp + sub.deltaTW;
        sub.distance = info1.distance + info2.distance +
                       data->c[info1.last_client][info2.first_client];

        sub.first_client = info1.first_client;
        sub.last_client  = info2.last_client;

        for (int i = 0; i < data->R; ++i) {
            sub.load_sum[i] = info1.load_sum[i] + info2.load_sum[i];
            sub.load_max[i] = max(info1.load_max[i], info1.load_sum[i] + info2.load_max[i]);
        }

        return sub;
    }
};

#endif