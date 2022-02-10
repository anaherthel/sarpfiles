#include "../LocalSearch.h"

void LocalSearch::applyRelocateIntra(MoveInfo move_info) {
    Route *route = solution->getRoute(move_info.route_1);
    route->reinsertRequests(
        move_info.route_1_pos_1, move_info.route_1_pos_2,
        move_info.route_2_pos_1, move_info.route_2_pos_2);
}

vector<int> LocalSearch::newPathRelocateIntra(MoveInfo move_info) {
    int route_idx        = move_info.route_1;
    int pickup_pos_src   = move_info.route_1_pos_1;
    int delivery_pos_src = move_info.route_1_pos_2;
    int pickup_pos_dst   = move_info.route_2_pos_1;
    int delivery_pos_dst = move_info.route_2_pos_2;

    Route *route = solution->getRoute(route_idx);
    int len      = route->getPathSize();

    if (pickup_pos_src < pickup_pos_dst) {
        if (delivery_pos_src < delivery_pos_dst) {
            delivery_pos_dst--;
        }
        if (delivery_pos_src < pickup_pos_dst) {
            pickup_pos_dst--;
        }
        pickup_pos_dst--;
    } else {
        if (delivery_pos_src < delivery_pos_dst) {
            delivery_pos_dst--;
        }
        if (pickup_pos_src >= delivery_pos_dst) {
            delivery_pos_dst++;
        }
    }

    vector<int> new_path(route->getPath());

    int pickup   = route->getRequestIdByPos(pickup_pos_src);
    int delivery = route->getRequestIdByPos(delivery_pos_src);

    for (int i = 0, j = 0; j < len; ++i) {
        if ((route->getRequestIdByPos(i) == pickup) ||
            (route->getRequestIdByPos(i) == delivery)) {
            continue;
        } else if (pickup_pos_dst == j) {
            new_path[j] = pickup;
            --i;
            ++j;
        } else if (delivery_pos_dst == j) {
            new_path[j] = delivery;
            --i;
            ++j;
        } else {
            new_path[j] = route->getRequestIdByPos(i);
            ++j;
        }
    }

    return new_path;
}

void LocalSearch::exploreRelocateIntra(bool *moveEvaluation) {
    MoveInfo best_move, move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    double key;

    Route *route;

    vector<int> rt_path;

    int pickup, delivery;
    int pickup_pos_1, delivery_pos_1;
    int pickup_pos_2, delivery_pos_2;
    int pickup_init_pos, pickup_last_pos;
    int delivery_init_pos, delivery_last_pos;
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
            !route->getNeighborhoodStatus(RELOCATE_INTRA))
            continue;

        move_desc = ads->getMMD(RELOCATE_INTRA, key);
        if (move_desc.status == CHANGEABLE) {
            *moveEvaluation = true;
            best_move       = move_desc.moveInfo;
            best_move.setRoute(route_idx);
            updateLocalSearchIntra(best_move);
            continue;

        } else if (move_desc.status == UNCHANGEABLE) {
            solution->getRoute(route_idx)->setNeighborhoodStatus(RELOCATE_INTRA, false);
            continue;
        }

        for (pickup_pos_1 = 1; pickup_pos_1 < route_len - 1; ++pickup_pos_1) {
            pickup   = route->getRequestIdByPos(pickup_pos_1);
            delivery = pickup + data->n;

            if (pickup >= data->n)
                continue;

            delivery_pos_1 = ads->pairPosition[route_idx][pickup_pos_1].second;

            pickup_init_pos = ads->startSearch[route_idx][pickup];
            pickup_last_pos = min(ads->endSearch[route_idx][pickup], route_len);

            for (pickup_pos_2 = pickup_init_pos; pickup_pos_2 < pickup_last_pos; ++pickup_pos_2) {
                if (!ads->checkReinsertionVertice(route_idx, pickup_pos_1, delivery_pos_1, pickup_pos_2))
                    continue;

                delivery_init_pos = max(pickup_pos_2,
                                        ads->startSearch[route_idx][delivery] - 2);
                delivery_last_pos = ads->endSearch[route_idx][delivery];

                for (delivery_pos_2 = delivery_init_pos; delivery_pos_2 < delivery_last_pos; ++delivery_pos_2) {
                    move_info = MoveInfo(RELOCATE_INTRA, route_idx, pickup_pos_1,
                                         delivery_pos_1, pickup_pos_2, delivery_pos_2);
                    new_cost  = ads->evalRelocateIntra(move_info);

                    if (new_cost == INFEASIBLE)
                        continue;

                    if (improve(best_cost, new_cost)) {
                        rt_path   = newPathRelocateIntra(move_info);
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
            solution->getRoute(route_idx)->setNeighborhoodStatus(RELOCATE_INTRA, false);
            move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(RELOCATE_INTRA));
            ads->insertMMD(key, move_desc);
        }
    }
}

bool LocalSearch::assertRelocateIntra(MoveInfo move_info) {
    int route = move_info.route_1;
    int pos1  = move_info.route_1_pos_1;
    int pos2  = move_info.route_1_pos_2;
    int pos3  = move_info.route_2_pos_1;
    int pos4  = move_info.route_2_pos_2;

    int pickup = solution->getRoute(route)->getRequestIdByPos(pos1);

    cout << pos1 << " to " << pos3 << endl;
    cout << pos2 << " to " << pos4 << endl;

    Solution *auxSol = new Solution(*solution);
    double totalCost, cost, new_cost, auxCost;

    if ((auxSol->getCost() == solution->getCost()) &&
        (auxSol->getCost() == solution->getCost())) {
        cout << "1 - OK ==> Copy working\n";
    } else {
        cout << "1 - WRONG ==> Copy not working\n";
        getchar();
    }

    cout << "Original\n";
    auxSol->getRoute(route)->printPath();

    auxSol->getRoute(route)->reinsertRequests(pos1, pos2, pos3, pos4);
    // cout << "Change: " << pos1 << " to " << pos3 << endl;
    // auxSol->getRoute(route)->reinsertRequests(pos1, pos3);
    // auxSol->getRoute(route)->printPath();
    // cout << "Change: " << pos2 << " to " << pos4 << endl;
    // auxSol->getRoute(route)->reinsertRequests(pos2, pos4);

    auxSol->getRoute(route)->printPath();

    auxSol->update();
    auxCost = auxSol->getCost();

    cost     = solution->getRoute(route)->getCost();
    new_cost = ads->evalRelocateIntra(move_info);

    totalCost = solution->getCost() - cost + new_cost;
    cout << totalCost << " must be equal to\n"
         << auxCost << endl;

    if (abs(totalCost - auxCost) <= EPS) {
        cout << "2 - OK ==> Consistent value after reinsertion inside route\n";
    } else {
        cout << "2 - WRONG ==> reinsertion not working\n";
        getchar();
    }

    return (abs(totalCost - auxCost) <= EPS);
}