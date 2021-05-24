#include "Data.h"

Data::Data(const char *filePath, const char *p_id) {
    path      = filePath;
    problemId = p_id[0];
}

void Data::read() {
    input_file = fopen(path, "r");
    fscanf(input_file, "%d %d", &v, &n);  // Requestes, number of cars

    switch (problemId) {
        case '0':
            readDARP();
            break;
        case '1':
            readHDARP();
            break;
        case '2':
            readMDHDARP();
            break;
        default:
            puts("MISSING PROBLEM ID");
            break;
    }

    fclose(input_file);

    V.resize(2 * n + v);
    t.resize(V.size(), vector<double>(V.size()));
    c.resize(V.size(), vector<double>(V.size()));
    A_.resize(V.size(), vector<bool>(V.size(), false));  // Start arcs as valid
    NCA.resize(V.size(), vector<bool>(V.size(), true));
    NCB.resize(V.size(), vector<bool>(V.size(), true));

    bool isPickup = true;
    // Map vehicle depots to dummy requests
    for (int k = 0; k < v; ++k) {
        Request request(isPickup, 0, vehicles[k].getRouteDur(),
                        vehicles[k].getEarliestTW(), vehicles[k].getLatestTW(),
                        vector<int>(R, 0), Xd[vehicles[k].getDepot()],
                        Yd[vehicles[k].getDepot()]);
        requests.push_back(request);
    }
    // Calc matrix time
    for (int i = 0; i < V.size(); ++i) {
        double Xi = requests[i].getX();
        double Yi = requests[i].getY();

        for (int j = 0; j < V.size(); ++j) {
            double Xj = requests[j].getX();
            double Yj = requests[j].getY();

            c[i][j] = t[i][j] = sqrt(pow(Xi - Xj, 2) + pow(Yi - Yj, 2));
        }
    }
    for (int k = 0; k < v; ++k) {
        int i;
        for (i = 0; i < k; ++i) {
            if (vehicles[k].isSameType(vehicles[i])) {
                break;
            }
        }
        if (i == k) {
            fleet.push_back(vehicles[k]);
        }
    }

    depot_vhc.resize(m, vector<int>(fleet.size(), 0));
    for (int k = 0; k < v; ++k) {
        for (int f = 0; f < fleet.size(); f++) {
            if (vehicles[k].isSameType(fleet[f])) {
                depot_vhc[vehicles[k].getDepot()][f]++;
            }
        }
    }
}

void Data::readDARP() {
    n /= 2;
    m = 1;
    R = 1;
    maxCapacity.resize(R, 0);  // Max value of overall capcity in each resource
    int id;
    vector<int> resource(R);
    double routeDur, ride, service, TW[2], point[2];
    // Vehicle reading
    fscanf(input_file, "%lf", &routeDur);
    for (int r = 0; r < R; ++r) {
        fscanf(input_file, "%d", &maxCapacity[r]);
    }
    fscanf(input_file, "%lf", &ride);

    // Depot reading
    fscanf(input_file, "%d %lf %lf %lf", &id, &Xd[0], &Yd[0], &service);
    for (int r = 0; r < R; ++r) {
        fscanf(input_file, "%d", &resource[r]);
    }
    fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);
    // Vehicle read
    for (int k = 0; k < v; ++k) {
        Vehicle vehicle(false, R, maxCapacity, routeDur, TW[0], TW[1], 0);
        vehicles.push_back(vehicle);
    }
    // Request read
    for (int i = 0; i < (n << 1); ++i) {
        fscanf(input_file, "%d %lf %lf %lf", &id, &point[0], &point[1], &service);
        for (int r = 0; r < R; ++r) {
            fscanf(input_file, "%d", &resource[r]);
        }
        fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);

        bool isPickup = (i < n) ? 1 : 0;

        Request request(isPickup, service,
                        ride, TW[0], TW[1],
                        resource, point[0],
                        point[1]);
        requests.push_back(request);
    }
}

void Data::readHDARP() {
    m = 1;
    R = 4;
    maxCapacity.resize(R, 0);  // Max value of overall capcity in each resource

    int id;
    vector<int> resource(R);
    double service, ride, TW[2], point[2], routeDur;

    // Vehicle read
    for (int k = 0; k < v; ++k) {
        fscanf(input_file, "%lf", &routeDur);

        for (int r = 0; r < R; ++r) {
            fscanf(input_file, "%d", &resource[r]);
        }

        Vehicle vehicle(false, R, resource, routeDur, 0.0, 0.0, 0);
        vehicles.push_back(vehicle);
    }

    // Read and ignore first request line
    fscanf(input_file, "%d %lf %lf %lf %lf", &id, &Xd[0], &Yd[0], &service, &ride);
    for (int r = 0; r < R; ++r) {
        fscanf(input_file, "%d", &resource[r]);
    }
    fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);

    for (int k = 0; k < v; ++k) {
        vehicles[k].setE(TW[0]);
        vehicles[k].setL(TW[1]);
    }

    // Request read
    for (int i = 0; i < (n << 1); ++i) {
        fscanf(input_file, "%d %lf %lf %lf %lf", &id, &point[0], &point[1], &service, &ride);
        for (int r = 0; r < R; ++r) {
            fscanf(input_file, "%d", &resource[r]);
        }
        fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);

        bool isPickup = (i < n) ? 1 : 0;

        Request request(isPickup, service,
                        ride, TW[0], TW[1],
                        resource, point[0],
                        point[1]);
        requests.push_back(request);
    }

    adaptResources();
}
void Data::readMDHDARP() {
    m = min(4, v);  // Number of depots
    R = 4;
    maxCapacity.resize(R, 0);  // Max value of overall capcity in each resource

    int id;
    vector<int> resource(R);
    double service, ride, TW[2], point[2], routeDur;

    // Vehicle read
    for (int k = 0; k < v; ++k) {
        fscanf(input_file, "%lf", &routeDur);
        for (int r = 0; r < R; ++r) {
            fscanf(input_file, "%d", &resource[r]);
        }
        // CHANGE TO k % m
        Vehicle vehicle(false, R, resource, routeDur, 0.0, 0.0, k % 4);
        vehicles.push_back(vehicle);
    }

    // Read and ignore first request line
    fscanf(input_file, "%d %lf %lf %lf %lf", &id, &Xd[0], &Yd[0], &service, &ride);
    for (int r = 0; r < R; ++r) {
        fscanf(input_file, "%d", &resource[r]);
    }
    fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);

    Xd[0] = Xd[2] = -5;
    Xd[1] = Xd[3] = 5;
    Yd[0] = Yd[3] = -5;
    Yd[1] = Yd[2] = 5;

    for (int k = 0; k < v; ++k) {
        vehicles[k].setE(TW[0]);
        vehicles[k].setL(TW[1]);
    }

    // Request read
    for (int i = 0; i < (n << 1); ++i) {
        fscanf(input_file, "%d %lf %lf %lf %lf", &id, &point[0], &point[1], &service, &ride);
        for (int r = 0; r < R; ++r) {
            fscanf(input_file, "%d", &resource[r]);
        }
        fscanf(input_file, "%lf %lf", &TW[0], &TW[1]);

        bool isPickup = (i < n) ? 1 : 0;

        Request request(isPickup, service, ride, TW[0], TW[1], resource,
                        point[0], point[1]);
        requests.push_back(request);
    }
    adaptResources();
}
void Data::adaptResources() {
    // Update resources for each vehicle and get max Vehicle Capacity
    for (int k = 0; k < v; ++k) {
        vehicles[k].setCapacity(1, vehicles[k].getCapacity()[1] +
                                       vehicles[k].getCapacity()[2]);

        vehicles[k].setCapacity(0, vehicles[k].getCapacity()[0] +
                                       vehicles[k].getCapacity()[1]);

        for (int r = 0; r < R; ++r)
            maxCapacity[r] = max(maxCapacity[r], vehicles[k].getCapacity()[r]);
    }

    // Update resources for each pickup and delivery node
    for (int i = 0; i < n; i++) {
        requests[i].setResource(1, requests[i].getResourceRequest()[1] +
                                       requests[i].getResourceRequest()[2]);

        requests[i].setResource(0, requests[i].getResourceRequest()[0] +
                                       requests[i].getResourceRequest()[1]);

        requests[i + n].setResource(
            1, requests[i + n].getResourceRequest()[1] +
                   requests[i + n].getResourceRequest()[2]);

        requests[i + n].setResource(
            0, requests[i + n].getResourceRequest()[0] +
                   requests[i + n].getResourceRequest()[1]);
    }
}
// *********** PREPROCESSING **************
void Data::preprocessing() {
    // Tightening TW
    for (int i = 0; i < n; i++) {
        if (requests[i + n].getEarliestTW() <= requests[i].getEarliestTW()) {
            requests[i + n].setEarliestTW(min(requests[i + n].getLatestTW(),
                                              requests[i].getEarliestTW() +
                                                  requests[i].getServiceTime() +
                                                  t[i][i + n]));
            requests[i + n].setLatestTW(min(requests[i + n].getLatestTW(),
                                            requests[i].getLatestTW() +
                                                requests[i].getServiceTime() +
                                                requests[i].getMaxRideTime()));
        } else if (requests[i].getLatestTW() >= requests[i + n].getLatestTW()) {
            requests[i].setLatestTW(
                max(requests[i].getEarliestTW(),
                    requests[i + n].getLatestTW() -
                        (t[i][i + n] + requests[i].getServiceTime())));
            requests[i].setEarliestTW(max(requests[i].getEarliestTW(),
                                          requests[i + n].getEarliestTW() -
                                              (requests[i].getMaxRideTime() +
                                               requests[i].getServiceTime())));
        }
    }

    // Preprocessing 1 - Depots
    for (int d1 = 0; d1 < v; ++d1) {
        for (int d2 = 0; d2 < v; ++d2) {
            if (d1 != d2)
                A_[d1 + (n << 1)][d2 + (n << 1)] = true;
        }
    }

    // Preprocessing 2 - i + n to i, i to depot, depot to i + n
    for (int i = 0; i < n; ++i) {
        A_[n + i][i] = A_[i][i] = A_[n + i][n + i] = true;

        for (int d = 0; d < v; ++d) {
            A_[(n << 1) + d][i + n] = A_[i][(n << 1) + d] = true;
            c[(n << 1) + d][i + n] = c[i][(n << 1) + d] = INF_COST;
        }
    }

    // Preprocessing 3 - TW
    for (int i = 0; i < V.size(); ++i) {
        for (int j = 0; j < V.size(); ++j) {
            if ((requests[i].getLatestTW() +
                 requests[i].getServiceTime() +
                 t[i][j]) <
                requests[j].getEarliestTW()) {
                t[i][j] = requests[j].getEarliestTW() -
                          requests[i].getLatestTW() -
                          requests[i].getServiceTime();
            }
        }
    }

    // //Preprocessing 4 - RT for i, j, i + n
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < (n << 1); ++j) {
            if ((t[i][j] + requests[j].getServiceTime() + t[j][i + n]) >
                (requests[i].getMaxRideTime())) {
                A_[i][j] = A_[j][i + n] = true;
            }
        }
    }

    // Preprocessing 5 - TW i cannot come before j
    for (int i = 0; i < V.size(); ++i) {
        for (int j = 0; j < V.size(); ++j) {
            if ((requests[i].getEarliestTW() + requests[i].getServiceTime() +
                 t[i][j]) > (requests[j].getLatestTW())) {
                A_[i][j]  = true;
                NCB[j][i] = NCA[i][j] = false;
            }
        }
    }

    // Preprocessing 6 - Load
    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            for (int r = 0; r < R; r++) {
                if ((requests[i].getResourceRequest()[r] +
                     requests[j].getResourceRequest()[r]) >
                    maxCapacity[r]) {
                    c[i][j] = c[i][j + n] = c[i + n][j + n] = c[j][i] =
                        c[j][i + n] = c[j + n][i + n] = INF_COST;
                    A_[i][j] = A_[i][j + n] = A_[i + n][j + n] = A_[j][i] =
                        A_[j][i + n] = A_[j + n][i + n] = true;
                }
            }
        }
    }
    // Preprocessing 7 - TW by pair requests
    double d1, d2, d3, d4;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                // j-i-n+j-n+i
                d1 = max(requests[i].getEarliestTW(),
                         (requests[j].getEarliestTW() +
                          t[j][i] +
                          requests[j].getServiceTime()));

                d2 = max(requests[j + n].getEarliestTW(),
                         (d1 +
                          t[i][j + n] +
                          requests[i].getServiceTime()));

                if ((d2 +
                     t[j + n][i + n] +
                     requests[j + n].getServiceTime()) >
                    requests[i + n].getLatestTW()) {
                    A_[i][j + n] = true;
                }

                // i-n+i-j-n+j
                d1 = max(requests[i + n].getEarliestTW(),
                         (requests[i].getEarliestTW() +
                          t[i][i + n] +
                          requests[i].getServiceTime()));

                d2 = max(requests[j].getEarliestTW(),
                         (d1 +
                          t[i + n][j] +
                          requests[i + n].getServiceTime()));

                if ((d2 +
                     t[j][j + n] +
                     requests[j].getServiceTime()) >
                    requests[j + n].getLatestTW()) {
                    A_[i + n][j] = true;
                }

                // i-j-n+i-n+j and i-j-n+j-n+i
                d1 = max(requests[j].getEarliestTW(),
                         (requests[i].getEarliestTW() +
                          t[i][j] +
                          requests[i].getServiceTime()));

                d2 = max(requests[i + n].getEarliestTW(),
                         (d1 +
                          t[j][i + n] +
                          requests[j].getServiceTime()));

                if ((d2 +
                     t[i + n][j + n] +
                     requests[i + n].getServiceTime()) >
                    requests[j + n].getLatestTW()) {
                    d2 = max(requests[j + n].getEarliestTW(),
                             (d1 +
                              t[j][j + n] +
                              requests[j].getServiceTime()));

                    if ((d2 +
                         t[j + n][i + n] +
                         requests[j + n].getServiceTime()) >
                        requests[i + n].getLatestTW()) {
                        A_[i][j] = true;
                    }
                }

                // i-j-n+i-n+j and j-i-n+i-n+j
                d1 = max(requests[j].getEarliestTW(),
                         (requests[i].getEarliestTW() +
                          t[i][j] +
                          requests[i].getServiceTime()));

                d2 = max(requests[i + n].getEarliestTW(),
                         (d1 +
                          t[j][i + n] +
                          requests[j].getServiceTime()));

                if (((d2 +
                      t[i + n][j + n] +
                      requests[i + n].getServiceTime()) >
                     requests[j + n].getLatestTW())) {
                    d1 = max(requests[i].getEarliestTW(),
                             (requests[j].getEarliestTW() +
                              t[j][i] +
                              requests[j].getServiceTime()));

                    d2 = max(requests[i + n].getEarliestTW(),
                             (d1 +
                              t[i][i + n] +
                              requests[i].getServiceTime()));

                    if ((d2 +
                         t[i + n][j + n] +
                         requests[i + n].getServiceTime()) >
                        requests[j + n].getLatestTW()) {
                        A_[i + n][j + n] = true;
                    }
                }
            }
        }
    }
    // Infeasible arcs by ridetime pair requests
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                int c1 = !check_RT(j, i, j + n, i + n);
                int c2 = !check_RT(i, i + n, j, j + n);
                int c3 = !check_RT(i, j, i + n, j + n);
                int c4 = !check_RT(i, j, j + n, i + n);
                int c5 = !check_RT(j, i, i + n, j + n);
                int c6 = !check_RT(j, j + n, i, i + n);

                if (c1 && c2 && c3 && c4 && c5 && c6 && i < j) {
                    A_[i][j] = A_[i][j + n] = A_[i + n][j] = A_[i + n][j + n] =
                        true;
                    A_[j][i] = A_[j + n][i] = A_[j][i + n] = A_[j + n][i + n] =
                        true;
                } else {
                    if (c1)
                        A_[i][j + n] = true;
                    if (c2)
                        A_[i + n][j] = true;
                    if (c3 && c4)
                        A_[i][j] = true;
                    if (c3 && c5)
                        A_[i + n][j + n] = true;
                }
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int d = 0; d < v; ++d) {
            if (!A_[(n << 1) + d][i]) {
                A.push_back(make_pair((2 * n) + d, i));
            }
        }
    }

    for (int i = 0; i < (n << 1); ++i) {
        for (int j = 0; j < (n << 1); ++j) {
            if (!A_[i][j]) {
                A.push_back(make_pair(i, j));
            }
        }
    }

    for (int i = 0; i < n; ++i) {
        for (int d = 0; d < v; ++d) {
            if (!A_[i + n][(n << 1) + d]) {
                A.push_back(make_pair(i + n, (n << 1) + d));
            }
        }
    }
    // Penalize ilegal arcs
    for (int i = 0; i < V.size(); ++i) {
        for (int j = 0; j < V.size(); ++j) {
            if (A_[i][j]) {
                c[i][j] = INF_COST;
            }
        }
    }
}

void Data::print() {
    printf(
        "Number of Requests: %d\nNumber of Vehicles: %d\nNumber of Depots: "
        "%d\n\n",
        n, v, m);

    for (int i = 0; i < v; ++i) {
        vehicles[i].print();
    }

    printf("\n");

    for (int i = 0; i < requests.size(); ++i) {
        requests[i].print();
    }
}

/**
* RideTime checker by Tang et. al. (2010)
* in paths of size 4
**/

bool Data::check_RT(int n0, int n1, int n2, int n3) {
    int len = 4;
    int path[len];

    path[0] = n0;
    path[1] = n1;
    path[2] = n2;
    path[3] = n3;

    int pickup;
    double waiting_time = 0.0, delta;

    vector<double> arrival_time(len, 0.0);
    vector<double> departure_time(len, 0.0);
    vector<double> start_time(len, 0.0);
    vector<int> pair_position(len);

    for (int i = 0; i < len; ++i) {
        for (int j = 0; j < len; ++j) {
            if (path[i] == (path[j] + n) || path[i] == (path[j] - n)) {
                pair_position[i] = j;
                pair_position[j] = i;
            }
        }
    }

    /******************************************
     First Step
     ******************************************/

    start_time[0]     = requests[path[0]].getEarliestTW();
    departure_time[0] = start_time[0] + requests[path[0]].getServiceTime();

    for (int i = 1; i < len; i++) {
        arrival_time[i] = departure_time[i - 1] + t[path[i - 1]][path[i]];
        start_time[i]   = max(arrival_time[i], requests[path[i]].getEarliestTW());
        if ((start_time[i] - requests[path[i]].getLatestTW()) > EPS) {
            return false;
        }
        departure_time[i] = start_time[i] + requests[path[i]].getServiceTime();
    }

    /*******************************************
     *  Second Step
     *******************************************/
    waiting_time = start_time[len - 1] - arrival_time[len - 1];

    for (int i = len - 2; i >= 0; i--) {
        if (requests[path[i]].getIsPickup()) {
            delta = (start_time[pair_position[i]] - departure_time[i]) -
                    requests[path[i]].getMaxRideTime();
            if (delta > EPS) {
                if ((delta - waiting_time) > EPS) {
                    return false;
                }
                start_time[i]     = start_time[i] + delta;
                departure_time[i] = start_time[i] + requests[path[i]].getServiceTime();
                waiting_time -= delta;

                for (int j = i + 1; j < len; j++) {
                    arrival_time[j]   = departure_time[j - 1] + t[path[j - 1]][path[j]];
                    start_time[j]     = max(start_time[j], arrival_time[j]);
                    departure_time[j] = start_time[j] + requests[path[j]].getServiceTime();
                }
            }

            if ((start_time[i] - requests[path[i]].getLatestTW()) > EPS) {
                return false;
            }
        }

        waiting_time += (start_time[i] - arrival_time[i]);
    }

    /************************************************
     *  Third Step
     ************************************************/

    for (int i = 1; i < len; i++) {
        arrival_time[i] = departure_time[i - 1] + t[path[i - 1]][path[i]];
        start_time[i]   = max(start_time[i], arrival_time[i]);
        if ((start_time[i] - requests[path[i]].getLatestTW()) > EPS) {
            return false;
        }
        departure_time[i] = start_time[i] + requests[path[i]].getServiceTime();

        if (!requests[path[i]].getIsPickup() || i == (len - 1)) {
            pickup = pair_position[i];
            delta  = (start_time[i] - departure_time[pickup]) -
                    requests[path[pickup]].getMaxRideTime();
            if (delta > EPS) {
                return false;
            }
        }
    }
    return true;
}