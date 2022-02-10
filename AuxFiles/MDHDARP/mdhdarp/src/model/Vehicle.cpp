#include "Vehicle.h"

int Vehicle::objCounter = 0;

Vehicle::Vehicle(bool extra, int resources, vector<int> capacity,
                 double routeDur, double earliestTW, double latestTW, int depot) : extra{extra},
                                                                                   resources{resources},
                                                                                   capacity{capacity},
                                                                                   routeDur{routeDur},
                                                                                   earliestTW{earliestTW},
                                                                                   latestTW{latestTW},
                                                                                   depot{depot},
                                                                                   violCoef{1} {
    id = objCounter++;
}

Vehicle::Vehicle(bool extra, int resources) : extra{extra},
                                              resources{resources},
                                              capacity{vector<int>(resources, UNLIMITED_CAPACITY)},
                                              routeDur{UNLIMITED_DURATION},
                                              earliestTW{UNLIMITED_TW[0]},
                                              latestTW{UNLIMITED_TW[1]},
                                              depot{0},
                                              violCoef{10000} {
    id = objCounter++;
}

void Vehicle::print() {
    printf("The vehicle %d has capacity of (%d", id, capacity[0]);
    for (int i = 1; i < resources; ++i) {
        printf(", %d", capacity[i]);
    }
    printf(
        ") is in depot %d has TW of [%.2lf, %.2lf] and "
        "duration of %.2lf\n",
        depot, earliestTW, latestTW, routeDur);
}

bool Vehicle::isSameType(Vehicle vhc) {
    for (int i = 0; i < resources; ++i) {
        if (this->capacity[i] != vhc.capacity[i]) {
            return false;
        }
    }
    return true;
}

bool Vehicle::operator==(const Vehicle &vhc) const {
    for (int i = 0; i < resources; ++i) {
        if (this->capacity[i] != vhc.capacity[i]) {
            return false;
        }
    }
    return (depot == vhc.depot);
}