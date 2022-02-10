#include "../LocalSearch.h"

void LocalSearch::applyExchangeIntra(MoveInfo move_info) {
    Route *route = solution->getRoute(move_info.route_1);
    route->swapRequests(move_info.route_1_pos_1, move_info.route_1_pos_2);
}

vector<int> LocalSearch::newPathExchangeIntra(MoveInfo move_info) {
    int route_idx = move_info.route_1;
    int pos_1     = move_info.route_1_pos_1;
    int pos_2     = move_info.route_1_pos_2;

    Route *route = solution->getRoute(route_idx);

    int route_len = route->getPathSize();

    vector<int> new_path(route->getPath());
    swap(new_path[pos_1], new_path[pos_2]);

    return new_path;
}

void LocalSearch::exploreExchangeIntra(bool *moveEvaluation) {
    MoveInfo best_move, move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    double key;

    Route *route;

    vector<int> rt_path;

    int pickup, idPickup2;
    int pos_1, pos_2;
    int init_pos, last_pos;
    int route_len;
    int sol_routes_size = solution->data->v;

    double new_cost, best_cost;

    bool improved = false;

    for (int route_idx = 0; route_idx < sol_routes_size; ++route_idx) {
        improved  = false;
        route     = solution->getRoute(route_idx);
        route_len = route->getPathSize();
        key = best_cost = route->getCost();

        if (route->getPathSize() <= 2 ||
            !route->getNeighborhoodStatus(EXCHANGE_INTRA))
            continue;

        move_desc = ads->getMMD(EXCHANGE_INTRA, key);
        if (move_desc.status == CHANGEABLE) {
            *moveEvaluation = true;
            best_move       = move_desc.moveInfo;
            best_move.setRoute(route_idx);
            updateLocalSearchIntra(best_move);
            continue;

        } else if (move_desc.status == UNCHANGEABLE) {
            solution->getRoute(route_idx)->setNeighborhoodStatus(EXCHANGE_INTRA, false);
            continue;
        }

        for (pos_1 = 1; pos_1 < route->getPathSize() - 2; ++pos_1) {
            pickup = route->getRequestIdByPos(pos_1);

            // If it is pickup, only change the deliveries
            if (pickup < data->n) {
                init_pos = max(ads->startSearch[route_idx][pickup] - 1, pos_1 + 1);
                last_pos = min(route->getPathSize() - 1, ads->endSearch[route_idx][pickup] + 1);

                for (pos_2 = init_pos; pos_2 < last_pos; ++pos_2) {
                    idPickup2 = route->getRequestIdByPos(pos_2);

                    if (idPickup2 < data->n ||
                        // SE FOR O DELIVERY DO REQUEST CONTINUE
                        (pos_2 == ads->pairPosition[route_idx][pos_1].second) ||
                        // SE O NO ESCOLHIO FOR DELIVERY
                        (ads->pairPosition[route_idx][pos_2].first == 1 &&
                         ads->pairPosition[route_idx][pos_1].second < ads->pairPosition[route_idx][pos_2].second))
                        continue;  // E SE A POSICAO DO DELIVERY DO REQUEST
                                   // FOR MENOR Q A POSICAO DO PICKUP DO SEGUNDO
                                   // REQUEST

                    move_info = MoveInfo(EXCHANGE_INTRA, route_idx, ads->pairPosition[route_idx][pos_1].second, pos_2);
                    new_cost  = ads->evalExchangeIntra(move_info);

                    if (new_cost == INFEASIBLE)
                        continue;

                    if (improve(best_cost, new_cost)) {
                        rt_path   = newPathExchangeIntra(move_info);
                        ride_time = RideTimeChecker(&rt_path, data);
                        if (!ride_time.check()) {
                            continue;
                        }

                        improved  = true;
                        best_cost = new_cost;
                        best_move = move_info;
                    }
                }
                // If it is delivery, only change the pickups
            } else {
                pickup = pickup - data->n;

                init_pos = ads->startSearch[route_idx][pickup];
                last_pos = min(pos_1, ads->endSearch[route_idx][pickup]);

                for (pos_2 = init_pos; pos_2 < last_pos; ++pos_2) {
                    idPickup2 = route->getRequestIdByPos(pos_2);

                    if (idPickup2 >= data->n ||
                        (pos_2 == ads->pairPosition[route_idx][pos_1].second) ||
                        (ads->pairPosition[route_idx][pos_2].first == 0  // pickup
                         && ads->pairPosition[route_idx][pos_1].second >
                                ads->pairPosition[route_idx][pos_2].second))
                        continue;

                    move_info = MoveInfo(EXCHANGE_INTRA, route_idx, ads->pairPosition[route_idx][pos_1].second, pos_2);
                    new_cost  = ads->evalExchangeIntra(move_info);

                    if (new_cost == INFEASIBLE)
                        continue;

                    if (improve(best_cost, new_cost)) {
                        rt_path   = newPathExchangeIntra(move_info);
                        ride_time = RideTimeChecker(&rt_path, data);
                        if (!ride_time.check()) {
                            continue;
                        }

                        improved  = true;
                        best_cost = new_cost;
                        best_move = move_info;
                    }
                }
            }
        }
        if (improved) {
            *moveEvaluation = true;
            updateLocalSearchIntra(best_move);
            ads->insertMMD(key, MoveDesc(CHANGEABLE, best_move));
        } else {
            solution->getRoute(route_idx)->setNeighborhoodStatus(EXCHANGE_INTRA, false);
            move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_INTRA));
            ads->insertMMD(key, move_desc);
        }
    }
}

void LocalSearch::exploreExchangeIntra2(bool *moveEvaluation) {
    MoveInfo best_move, move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    double key;

    Route *route;

    vector<int> rt_path;

    int vertex_1, vertex_2;
    int pos_1, pos_2;
    int init_pos, last_pos;
    int route_len;
    int sol_routes_size = solution->data->v;

    double new_cost, best_cost;

    bool improved = false;

    for (int route_idx = 0; route_idx < sol_routes_size; ++route_idx) {
        improved  = false;
        route     = solution->getRoute(route_idx);
        route_len = route->getPathSize();
        key = best_cost = route->getCost();

        if (route->getPathSize() <= 2 ||
            !route->getNeighborhoodStatus(EXCHANGE_INTRA))
            continue;

        move_desc = ads->getMMD(EXCHANGE_INTRA, key);
        if (move_desc.status == CHANGEABLE) {
            *moveEvaluation = true;
            best_move       = move_desc.moveInfo;
            best_move.setRoute(route_idx);
            updateLocalSearchIntra(best_move);
            continue;

        } else if (move_desc.status == UNCHANGEABLE) {
            solution->getRoute(route_idx)->setNeighborhoodStatus(EXCHANGE_INTRA, false);
            continue;
        }

        for (pos_1 = 1; pos_1 < route->getPathSize() - 2; ++pos_1) {
            vertex_1 = route->getRequestIdByPos(pos_1);

            init_pos = pos_1 + 1;
            if (vertex_1 < data->n) {
                last_pos = ads->pairPosition[route_idx][pos_1].second;
            } else {
                last_pos = route->getPathSize() - 1;
            }
            last_pos = min(last_pos, ads->endSearch[route_idx][vertex_1]);

            for (pos_2 = init_pos; pos_2 < last_pos; ++pos_2) {
                vertex_2 = route->getRequestIdByPos(pos_2);

                if (vertex_2 >= data->n &&
                    pos_1 <= ads->pairPosition[route_idx][pos_2].second)
                    continue;

                move_info = MoveInfo(EXCHANGE_INTRA, route_idx, pos_1, pos_2);
                new_cost  = ads->evalExchangeIntra(move_info);

                if (new_cost == INFEASIBLE)
                    continue;

                if (improve(best_cost, new_cost)) {
                    rt_path   = newPathExchangeIntra(move_info);
                    ride_time = RideTimeChecker(&rt_path, data);
                    if (!ride_time.check()) {
                        continue;
                    }

                    improved  = true;
                    best_cost = new_cost;
                    best_move = move_info;
                }
            }
        }
        if (improved) {
            *moveEvaluation = true;
            updateLocalSearchIntra(best_move);
            ads->insertMMD(key, MoveDesc(CHANGEABLE, best_move));
        } else {
            solution->getRoute(route_idx)->setNeighborhoodStatus(EXCHANGE_INTRA, false);
            move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_INTRA));
            ads->insertMMD(key, move_desc);
        }
    }
}

bool LocalSearch::assertExchangeIntra(MoveInfo move_info, int type) {
    NEIGHBORHOOD exchange_intra = move_info.neighborhood;
    MoveInfo move_info_to_eval;

    int route = move_info.route_1;
    int pos1  = move_info.route_1_pos_1;
    int pos2  = move_info.route_1_pos_2;
    int pos3  = move_info.route_2_pos_1;
    int pos4  = move_info.route_2_pos_1;

    int idPickup1 = solution->getRoute(route)->getRequestIdByPos(pos1);
    int idPickup2 = solution->getRoute(route)->getRequestIdByPos(pos3);

    Solution *auxSol = new Solution(*solution);
    double cost, totalCost, newCost, auxCost;

    if (auxSol->getCost() == solution->getCost()) {
        cout << "1 - OK ==> Copy working\n";
    } else {
        cout << "1 - WRONG ==> Copy not working\n";
        getchar();
    }

    auxSol->getRoute(route)->swapRequests(pos1, pos3);
    auxSol->getRoute(route)->swapRequests(pos2, pos4);
    auxSol->update();
    auxCost = auxSol->getCost();

    cost = solution->getRoute(route)->getCost();

    if (type == 1) {
        move_info_to_eval = MoveInfo(exchange_intra, route, pos2, pos4);
        newCost           = ads->evalExchangeIntra(move_info_to_eval);
    } else {
        move_info_to_eval = MoveInfo(exchange_intra, route, pos1, pos3);
        newCost           = ads->evalExchangeIntra(move_info_to_eval);
    }

    totalCost = solution->getCost() - cost + newCost;
    cout << totalCost << " must be equal to\n"
         << auxCost << endl;

    if (abs(totalCost - auxCost) <= EPS) {
        cout << "2 - OK ==> Consistent value after swap inside route\n";
    } else {
        cout << "2 - WRONG ==> Swap not working\n";
    }

    return (abs(totalCost - auxCost) <= EPS);
}