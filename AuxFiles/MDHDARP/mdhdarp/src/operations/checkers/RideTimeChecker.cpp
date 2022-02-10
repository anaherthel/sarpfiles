#include "RideTimeChecker.h"

RideTimeChecker::RideTimeChecker(vector<int> *path, Data *data) {
    this->data          = data;
    this->path          = path;
    this->pairPosition  = vector<int>(data->requests.size(), -1);
    this->arrivalTime   = vector<double>(path->size(), 0);
    this->startTime     = vector<double>(path->size(), 0);
    this->departureTime = vector<double>(path->size(), 0);
    this->waitingTime   = vector<double>(path->size(), 0);
    this->rideTimes     = vector<double>(path->size(), 0);
    createStructures();
}

// 8-Step Algotihm by Rodrigo:
bool RideTimeChecker::check() {
    if (verification())
        algorithmFTS();

    return this->verifyRideTime();
}

void RideTimeChecker::createStructures() {
    pairPosition[path->at(0)] = path->size() - 1;

    for (int i = 1; i < path->size() - 1; ++i) {
        if (path->at(i) < data->n) {
            pairPosition[path->at(i) + data->n] = i;
        } else {
            pairPosition[path->at(i) - data->n] = i;
        }
    }

    createSchedules();
    getRouteRideTime();
    getWaitingTimeRoute();

    this->sumWT = vector<vector<double>>(path->size(), vector<double>(path->size(), 0.0));
    for (int i = 1; i < path->size(); i++)
        this->sumWT[i][0] = this->sumWT[0][i] = getSumWT(sumWT[0][i - 1], i);
}

void RideTimeChecker::createSchedules() {
    arrivalTime[0]   = data->requests[path->at(0)].getEarliestTW();
    startTime[0]     = getStartTime(data->requests[path->at(0)].getEarliestTW(), 0);
    departureTime[0] = getDepartureTime(data->requests[path->at(0)].getServiceTime(), 0);

    Request *node_to;

    for (int i = 1; i < path->size(); i++) {
        node_to = &data->requests[path->at(i)];

        arrivalTime[i]   = getArrivalTime(data->t[path->at(i - 1)][path->at(i)], i - 1);
        startTime[i]     = getStartTime(node_to->getEarliestTW(), i);
        departureTime[i] = getDepartureTime(node_to->getServiceTime(), i);
    }
}

void RideTimeChecker::getRouteRideTime() {
    int n;
    this->rideTimes[0] = getRideTime(0, pairPosition[path->at(0)]);
    for (int i = 1; i < path->size() - 1; i++) {
        n = pairPosition[path->at(i)];

        this->rideTimes[i] = (data->requests[path->at(i)].getIsPickup()) ? getRideTime(i, n) : 0;
    }
    this->rideTimes[path->size() - 1] = 0;
}

void inline RideTimeChecker::updateStructures(double min_FTS_WT,
                                              int from) {
    int n;
    waitingTime[from] += min_FTS_WT;
    startTime[from]     = arrivalTime[from] + waitingTime[from];
    departureTime[from] = startTime[from] + data->requests[path->at(from)].getServiceTime();
    Request *req;
    for (int i = from + 1; i < path->size(); i++) {
        req = &data->requests[path->at(i)];

        arrivalTime[i]   = getArrivalTime(data->t[path->at(i - 1)][path->at(i)], i - 1);
        startTime[i]     = getStartTime(req->getEarliestTW(), i);
        departureTime[i] = getDepartureTime(req->getServiceTime(), i);
        waitingTime[i]   = getWT(i);

        if (req->getIsPickup())
            continue;

        n = pairPosition[path->at(i)];
        if (from >= n)
            rideTimes[n] = getRideTime(n, i);
    }
    rideTimes[0] = getRideTime(0, path->size() - 1);
}

bool RideTimeChecker::verifyRideTime() {
    for (int i = 0; i < path->size() - 1; i++) {
        if (!data->requests[path->at(i)].getIsPickup())
            continue;

        if ((rideTimes[i] - data->requests[path->at(i)].getMaxRideTime()) >
            EPS)
            return false;
    }
    return true;
}

double inline RideTimeChecker::getArrivalTime(double cost_arc, int pos) {
    return departureTime[pos] + cost_arc;
}

double inline RideTimeChecker::getStartTime(double earliestTW, int pos) {
    return max(earliestTW, arrivalTime[pos]);
}

double inline RideTimeChecker::getDepartureTime(double service_time,
                                                int pos) {
    return startTime[pos] + service_time;
}

void inline RideTimeChecker::getWaitingTimeRoute() {
    waitingTime[0] = 0;
    for (int i = 1; i < path->size() - 1; i++) {
        waitingTime[i] = getWT(i);
    }
    waitingTime[path->size() - 1] = 0;
}

double inline RideTimeChecker::getRideTime(int pickup, int delivery) {
    return startTime[delivery] - departureTime[pickup];
}

double inline RideTimeChecker::getSumWT(double sum_waitingTime, int j) {
    return sum_waitingTime + waitingTime[j];
}

double inline RideTimeChecker::getWT(int to_node) {
    return startTime[to_node] - arrivalTime[to_node];
}

double inline RideTimeChecker::getFTS(int from_node, int to_node) {
    int n         = (to_node != path->size() - 1) ? pairPosition[path->at(to_node)] : 0;
    Request *from = &data->requests[path->at(to_node)];
    Request *to   = &data->requests[path->at(n)];

    if ((!from->getIsPickup() || to_node == path->size() - 1) && from_node > n) {
        double diff_latestTW_start_time = from->getLatestTW() - startTime[to_node];
        double diff_max_rt_actual_rt    = to->getMaxRideTime() - rideTimes[n];
        double value                    = min(diff_latestTW_start_time, diff_max_rt_actual_rt);
        return sumWT[from_node][to_node] + fabs(value);
    }

    return sumWT[from_node][to_node] + fabs(from->getLatestTW() - startTime[to_node]);
}

bool RideTimeChecker::verification() {
    double delta;
    for (int i = 0; i < path->size() - 1; i++) {
        if (!data->requests[path->at(i)].getIsPickup())
            continue;

        delta = rideTimes[i] - data->requests[path->at(i)].getMaxRideTime();
        if (delta <= EPS)
            continue;

        if ((delta - fabs(sumWT[0][pairPosition[path->at(i)]] - sumWT[0][i])) > EPS)
            return false;
    }
    return true;
}

void RideTimeChecker::algorithmFTS() {
    size_t path_size = path->size();

    double min_FTS_WT = getMinFTS(0);
    updateStructures(min_FTS_WT, 0);

    if (!verifyRideTime()) {
        for (int i = 1; i < path_size - 1; i++) {
            if (!data->requests[path->at(i)].getIsPickup())
                continue;
            for (int j = i + 1; j < path_size; j++)
                sumWT[j][i] = sumWT[i][j] = getSumWT(sumWT[i][j - 1], j);

            min_FTS_WT = getMinFTS(i);
            updateStructures(min_FTS_WT, i);
            if (verifyRideTime())
                break;
        }
    }
}

double RideTimeChecker::getMinFTS(int pos) {
    double min_FTS = INT32_MAX;
    for (int i = pos; i < path->size(); i++) {
        min_FTS = min(min_FTS, getFTS(pos, i));
    }
    return min(min_FTS, sumWT[pos][path->size() - 2]);
}

void RideTimeChecker::printStructures(string filePath) {
    ofstream file;
    file.open(filePath, ios::app);
    if (file.is_open()) {
        if (verifyRideTime()) {
            for (int i = 0; i < path->size(); i++) {
                if (!data->requests[path->at(i)].getIsPickup())
                    file << path->at(i) << " A: " << arrivalTime[i]
                         << " B: " << startTime[i] << " RT: 0" << endl;
                else
                    file << path->at(i) << " A: " << arrivalTime[i]
                         << " B: " << startTime[i] << " RT: " << rideTimes[i]
                         << endl;
                ;
            }
            file << endl
                 << endl;
        } else
            file << "Ride Time infeasible" << endl;

        file.close();
    } else
        cout << " NOT OPEN !!" << endl;
}

void RideTimeChecker::printStructures() {
    if (!verifyRideTime()) {
        puts("Infeasible in Ride Time");
    }
    double arc_cost = 0;
    for (int i = 0; i < path->size(); i++) {
        printf(
            "\n%3d A: %10.4lf  B: %10.4lf  D: %10.4lf  - [%10.4lf, %10.4lf] "
            " RT: %24.17lf",
            path->at(i), arrivalTime[i], startTime[i], departureTime[i],
            data->requests[path->at(i)].getEarliestTW(),
            data->requests[path->at(i)].getLatestTW(), rideTimes[i]);
        printf("  WT: %9.4lf  ArcCost: %9.4lf", startTime[i] - arrivalTime[i],
               arc_cost);
        if (i != path->size() - 1)
            arc_cost = data->t[path->at(i)][path->at(i + 1)];
    }

    puts("");
}

void RideTimeChecker::printDetails() {
    vector<int> load = {0, 0, 0, 0};
    for (int i = 0; i < path->size() - 1; i++) {
        for (int j = 0; j < 4; j++) {
            load[j] += data->requests[path->at(i)].getResourceRequest()[j];
        }

        data->requests[path->at(i)].print();
        printf(
            "ST: %.4lf DT: %.4lf RT: %.4lf TT: %.4lf LOAD: (%d, %d, %d, %d)\n",
            startTime[i], departureTime[i], rideTimes[i],
            data->t[path->at(i)][path->at(i + 1)], load[0], load[1], load[2], load[3]);
    }
}