#include "Request.h"

#include <cstring>
#include <string>

int Request::objCounter = 0;

Request::Request(bool isPickup, double serviceTime, double maxRideTime,
                 double earliestTW, double latestTW,
                 vector<int> resourceRequest, double X, double Y) : isPickup{isPickup},
                                                                    serviceTime{serviceTime},
                                                                    maxRideTime{maxRideTime},
                                                                    earliestTW{earliestTW},
                                                                    latestTW{latestTW},
                                                                    resourceRequest{resourceRequest},
                                                                    X{X},
                                                                    Y{Y} {
    this->id = objCounter++;
}
void Request::print() {
    string s = isPickup ? "P" : "D";
    printf("Req %d is %s | Pos: (%.2lf, %.2lf) demands (%d", id, s.c_str(), X,
           Y, resourceRequest[0]);
    for (int r = 1; r < resourceRequest.size(); ++r) {
        printf(", %d", resourceRequest[r]);
    }
    printf("), MaxRT %.2lf, service %.2lf, and TW of [%.2lf, %.2lf]\n",
           maxRideTime, serviceTime, earliestTW, latestTW);
}
