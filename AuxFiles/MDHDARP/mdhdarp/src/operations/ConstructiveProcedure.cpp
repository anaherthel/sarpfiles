#include "ConstructiveProcedure.h"

ConstructiveProcedure::ConstructiveProcedure(
    Data *data, Statistics *stats, ADS *ads) : data(data),
                                               stats(stats),
                                               ads(ads) {}

bool ConstructiveProcedure::compare(int a, int b) {
    return data->requests[a].getEarliestTW() <
           data->requests[b].getEarliestTW();
}

void ConstructiveProcedure::initSolution(Solution *sol) {
    do {
        sol->clear();
        initCL();
        addRequestInEachRoute(sol);
        parallelInsertion(sol);
    } while (!CL.empty());
    sol->update();
    ads->update();
}

void ConstructiveProcedure::initSPSolution(Solution *sol) {
    sol->repairSol();
    ads->update();
}

void ConstructiveProcedure::initCL() {
    CL.clear();
    CL.reserve(data->n);
    for (int i = 0; i < data->n; ++i) {
        CL.push_back(i);
    }
}

void ConstructiveProcedure::addRequestInEachRoute(Solution *sol) {
    ads->update();
    int random_request;
    bool has_insertion;
    MoveInfo move_info;

    for (int i = 0; i < sol->getRoutesSize(); ++i) {
        has_insertion = false;
        while (!has_insertion) {
            random_request = getRandRequestFromCL();

            move_info = MoveInfo(INSERTION, -1, i, -1, -1, 1, 1);

            if (ads->evalInsertionRequest(move_info, random_request) !=
                INFEASIBLE) {
                vector<int> *request =
                    new vector<int>{random_request, random_request + data->n};
                sol->getRoute(i)->insertBlockByPos(1, request);
                delete request;
                removeFromCL(random_request);
                has_insertion = true;
            }
        }
    }

    sol->update();
    ads->update();
}

int ConstructiveProcedure::getRandRequestFromCL() {
    int random_position = rand() % this->CL.size();
    int random_request  = this->CL[random_position];
    return random_request;
}

void ConstructiveProcedure::verifyCL() {
    if (!CL.empty()) {
        printf("CL not empty, size: %lu\n", CL.size());
        for (int i = 0; i < CL.size(); ++i)
            printf("%d ", CL[i]);
        puts("");
    } else {
        printf("CL is empty\n");
    }
}

void ConstructiveProcedure::removeFromCL(int id) {
    for (int i = 0; i < CL.size(); ++i) {
        if (this->CL[i] == id) {
            this->CL.erase(this->CL.begin() + i);
            break;
        }
    }
}

vector<int> ConstructiveProcedure::newPathConstructive(MoveInfo move_info,
                                                       int candidate) {
    int route_idx        = move_info.route_2;
    int pickup_pos_dst   = move_info.route_2_pos_1;
    int delivery_pos_dst = move_info.route_2_pos_2 + 1;
    int pickup           = candidate;
    int delivery         = candidate + data->n;

    Route *route = ads->solution->getRoute(route_idx);

    vector<int> new_path(route->getPath());

    new_path.insert(new_path.begin() + pickup_pos_dst, pickup);
    new_path.insert(new_path.begin() + delivery_pos_dst, delivery);

    return new_path;
}

void ConstructiveProcedure::parallelInsertion(Solution *solution) {
    MoveInfo move_info;
    RideTimeChecker ride_time;

    Route *route;

    vector<int> rt_path;

    int pickup, delivery;
    int pickup_pos, delivery_pos;
    int selected_pickup, selected_pickup_pos;
    int selected_delivery, selected_delivery_pos;
    int selected_route;
    int pickup_init_pos, pickup_last_pos;
    int delivery_init_pos, delivery_last_pos;
    int sol_routes_size = solution->getRoutesSize();

    double best_cost, eval_cost;

    bool insertion = true, any_improve;

    sort(CL.begin(), CL.end(), [this](int a, int b) {
        return compare(a, b);
    });

    while (insertion && !CL.empty()) {
        for (int request = 0; request < CL.size(); ++request) {
            pickup    = CL[request];
            delivery  = pickup + data->n;
            best_cost = INF_COST;
            insertion = false;

            for (int route_idx = 0; route_idx < sol_routes_size; ++route_idx) {
                route           = solution->getRoute(route_idx);
                pickup_init_pos = ads->startSearch[route_idx][pickup];
                pickup_last_pos = ads->endSearch[route_idx][pickup];

                for (pickup_pos = pickup_init_pos; pickup_pos < pickup_last_pos; ++pickup_pos) {
                    if (!ads->checkInsertionVertice(route_idx, pickup_pos, pickup))
                        continue;

                    delivery_init_pos = max(pickup_pos,
                                            ads->startSearch[route_idx][delivery] - 1);
                    delivery_last_pos = ads->endSearch[route_idx][delivery];

                    for (delivery_pos = delivery_init_pos; delivery_pos < delivery_last_pos; ++delivery_pos) {
                        move_info = MoveInfo(INSERTION, -1, route_idx, -1, -1, pickup_pos, delivery_pos);

                        eval_cost = ads->evalInsertionRequest(move_info, pickup);

                        if (eval_cost == INFEASIBLE)
                            continue;

                        if (improve(best_cost, eval_cost)) {
                            rt_path   = newPathConstructive(move_info, pickup);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }

                            best_cost             = eval_cost;
                            selected_pickup_pos   = pickup_pos;
                            selected_delivery_pos = delivery_pos + 1;
                            selected_pickup       = pickup;
                            selected_delivery     = delivery;
                            selected_route        = route_idx;
                            insertion             = true;
                        }
                    }
                }
            }

            if (insertion) {
                solution->getRoute(selected_route)->insertRequest(selected_pickup, selected_pickup_pos);
                solution->getRoute(selected_route)->insertRequest(selected_delivery, selected_delivery_pos);
                this->removeFromCL(selected_pickup);
                solution->getRoute(selected_route)->update();

                ads->update(selected_route, selected_pickup_pos,
                            solution->getRoute(selected_route)->getPathSize() - 1);
                request--;
            }
        }
    }
}