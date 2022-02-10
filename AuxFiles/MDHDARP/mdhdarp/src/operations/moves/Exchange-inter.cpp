#include "../LocalSearch.h"

void LocalSearch::applyExchangeInter(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int pickup_route_1 = route_1->getRequestIdByPos(route_1_pos_1);
    int pickup_route_2 = route_2->getRequestIdByPos(route_2_pos_1);

    route_1->changeRequest(route_1_pos_1, pickup_route_2);
    route_1->changeRequest(route_1_pos_2, pickup_route_2 + data->n);

    route_2->changeRequest(route_2_pos_1, pickup_route_1);
    route_2->changeRequest(route_2_pos_2, pickup_route_1 + data->n);
}

vector<int> LocalSearch::newPathExchangeInter(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();

    int pickup   = route_2->getRequestIdByPos(route_2_pos_1);
    int delivery = route_2->getRequestIdByPos(route_2_pos_2);

    vector<int> new_path(route_1->getPath());
    new_path[route_1_pos_1] = pickup;
    new_path[route_1_pos_2] = delivery;

    return new_path;
}

void LocalSearch::exploreExchangeInter(bool *moveEvaluation) {
    MoveInfo best_move, move_info, inverted_move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;

    pair<double, double> key;

    Route *route_1, *route_2;

    vector<int> rt_path;

    int pickup_route_1, pickup_route_2;
    int route_1_pos, route_2_pos;
    int route_1_len, route_2_len;
    int init_pos, last_pos;
    int sol_routes_size = solution->getRoutesSize();

    double old_cost_route_1, old_cost_route_2;
    double eval_cost, old_cost, new_cost;
    double best_sol_cost, sol_cost;
    best_sol_cost = sol_cost = solution->getCost();

    bool improved = false, any_improve;

    for (int route_1_idx = 0; route_1_idx < sol_routes_size; ++route_1_idx) {
        route_1          = solution->getRoute(route_1_idx);
        route_1_len      = route_1->getPathSize();
        old_cost_route_1 = route_1->getCost();

        if (route_1_len <= 2)
            continue;

        for (int route_2_idx = route_1_idx + 1; route_2_idx < sol_routes_size; ++route_2_idx) {
            if (!solution->getInterNStatus(EXCHANGE_INTER, route_1_idx, route_2_idx))
                continue;

            any_improve      = false;
            route_2          = solution->getRoute(route_2_idx);
            route_2_len      = route_2->getPathSize();
            old_cost_route_2 = route_2->getCost();
            old_cost         = old_cost_route_2 + old_cost_route_1;
            key              = make_pair(old_cost_route_1, old_cost_route_2);

            if (ads->useMMD(route_1_idx, route_2_idx)) {
                move_desc = ads->getMMD(EXCHANGE_INTER, key);

                if (move_desc.status != UNVISITED &&
                    route_1->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_1)->getVehicleCopy() &&
                    route_2->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_2)->getVehicleCopy()) {
                    if (move_desc.status == CHANGEABLE) {
                        move_info = move_desc.moveInfo;
                        move_info.setRoutes(route_1_idx, route_2_idx);

                        new_cost  = ads->evalExchangeInter(move_info);
                        eval_cost = sol_cost - old_cost + new_cost;

                        if (improve(best_sol_cost, eval_cost)) {
                            improved      = true;
                            best_sol_cost = eval_cost;
                            best_move     = move_info;
                        }
                        continue;
                    } else if (move_desc.status == UNCHANGEABLE) {
                        continue;
                    }
                }
            }

            for (route_1_pos = 1; route_1_pos < route_1_len - 1; ++route_1_pos) {
                pickup_route_1 = route_1->getRequestIdByPos(route_1_pos);

                if (pickup_route_1 >= data->n)
                    continue;

                if (route_2_idx < data->v) {
                    init_pos = max(ads->startSearch[route_2_idx][pickup_route_1] - 1, 1);
                    if (route_2->getRequestIdByPos(init_pos + 1) >= data->n) {
                        init_pos = max(init_pos - 1, 1);
                    }
                    last_pos = ads->endSearch[route_2_idx][pickup_route_1];
                } else {
                    init_pos = 1;
                    last_pos = route_2_len - 1;
                }

                for (route_2_pos = init_pos; route_2_pos < last_pos; ++route_2_pos) {
                    pickup_route_2 = route_2->getRequestIdByPos(route_2_pos);
                    if (pickup_route_2 >= data->n)
                        continue;

                    move_info = MoveInfo(EXCHANGE_INTER, route_1_idx, route_2_idx,
                                         route_1_pos, ads->pairPosition[route_1_idx][route_1_pos].second,
                                         route_2_pos, ads->pairPosition[route_2_idx][route_2_pos].second);

                    new_cost = ads->evalExchangeInter(move_info);

                    if (new_cost == INFEASIBLE)
                        continue;

                    eval_cost = sol_cost - old_cost + new_cost;

                    if (improve(best_sol_cost, eval_cost)) {
                        if (!route_1->isExtra()) {
                            rt_path   = newPathExchangeInter(move_info);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }
                        }

                        inverted_move_info = move_info.invertedMoveInfo();

                        if (!route_2->isExtra()) {
                            rt_path   = newPathExchangeInter(inverted_move_info);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }
                        }

                        improved      = true;
                        best_sol_cost = eval_cost;
                        best_move     = move_info;

                        if (ads->useMMD(route_1_idx, route_2_idx)) {
                            key = make_pair(old_cost_route_1, old_cost_route_2);
                            ads->insertMMD(key, MoveDesc(CHANGEABLE, best_move));

                            key = make_pair(old_cost_route_2, old_cost_route_1);
                            ads->insertMMD(key, MoveDesc(CHANGEABLE, inverted_move_info));
                        }
                    }

                    if (improve(old_cost, new_cost)) {
                        any_improve = true;
                    }
                }
            }

            if (!any_improve) {
                solution->setPairNeighbStatus(EXCHANGE_INTER, route_1_idx, route_2_idx, false);
                solution->setPairNeighbStatus(EXCHANGE_INTER, route_2_idx, route_1_idx, false);

                if (ads->useMMD(route_1_idx, route_2_idx)) {
                    key       = make_pair(old_cost_route_1, old_cost_route_2);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_INTER, route_1_idx, route_2_idx));
                    ads->insertMMD(key, move_desc);

                    key       = make_pair(old_cost_route_2, old_cost_route_1);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_INTER, route_2_idx, route_1_idx));
                    ads->insertMMD(key, move_desc);
                }
            }
        }
    }
    if (improved) {
        *moveEvaluation = true;
        updateLocalSearchInter(best_move);
        solution->setPairNeighbStatusAllTrue(best_move.route_1, best_move.route_2);
    }
}

bool LocalSearch::assertExchangeInter(MoveInfo move_info) {
    int route1     = move_info.route_1;
    int route2     = move_info.route_2;
    int pos1Route1 = move_info.route_1_pos_1;
    int pos2Route1 = move_info.route_1_pos_2;
    int pos1Route2 = move_info.route_2_pos_1;
    int pos2Route2 = move_info.route_2_pos_2;

    // Pegando referencia
    Solution *auxSol = new Solution(*solution);
    double cost, costR1, costR2, newCost, auxCost;

    // puts("\n");
    cout << "\nASSERT\n";

    auxSol->print();

    int id_pickup_r1 =
        solution->getRoute(route1)->getRequestIdByPos(pos1Route1);
    int id_delivery_r1 = id_pickup_r1 + data->n;
    int id_pickup_r2 =
        solution->getRoute(route2)->getRequestIdByPos(pos1Route2);
    int id_delivery_r2 = id_pickup_r2 + data->n;

    printf("\nR1: %d - %d\nR2: %d - %d\n", id_pickup_r1, id_delivery_r1,
           id_pickup_r2, id_delivery_r2);

    if ((auxSol->getCost() == solution->getCost()) &&
        (auxSol->getCost() == solution->getCost())) {
        cout << "1 - OK ==> Copy working\n";
    } else {
        cout << "1 - WRONG ==> Copy not working\n";
        cout << "==== Will Remove from: =====\n";
        auxSol->getRoute(route1)->print();
        cout << "==== Will Isert in: =====\n";
        auxSol->getRoute(route2)->print();
    }

    // Remove R1
    vector<int> *toInsertPickup_r1 =
        auxSol->getRoute(route1)->removeBlockByPos(pos1Route1, 1);
    vector<int> *toInsertDelivery_r1 =
        auxSol->getRoute(route1)->removeBlockByPos(pos2Route1 - 1, 1);

    if (toInsertPickup_r1->at(0) == id_pickup_r1 &&
        toInsertDelivery_r1->at(0) == id_delivery_r1) {
        cout << "2 - OK ==> Removing same pickup and delivery node in R1\n";
    } else {
        cout << "2 - WRONG ==> Removing different pickup and delivery node in "
                "R1\n";
        cout << "Real removing part: " << toInsertPickup_r1->at(0) << " "
             << toInsertDelivery_r1->at(0) << endl;
        cout << "Sequence removing : " << pos1Route1 << " " << pos2Route1
             << endl;
    }

    // Remove R2
    vector<int> *toInsertPickup_r2 =
        auxSol->getRoute(route2)->removeBlockByPos(pos1Route2, 1);
    vector<int> *toInsertDelivery_r2 =
        auxSol->getRoute(route2)->removeBlockByPos(pos2Route2 - 1, 1);

    if (toInsertPickup_r2->at(0) == id_pickup_r2 &&
        toInsertDelivery_r2->at(0) == id_delivery_r2) {
        cout << "3 - OK ==> Removing same pickup and delivery node in R2\n";
    } else {
        cout << "3 - WRONG ==> Removing different pickup and delivery node in "
                "R1\n";
        cout << "Real removing part: " << toInsertPickup_r2->at(0) << " "
             << toInsertDelivery_r2->at(0) << endl;
        cout << "Sequence removing : " << pos1Route2 << " " << pos2Route2
             << endl;
    }

    // Change Insertions
    costR1  = solution->getRoute(route1)->getCost();
    newCost = ads->evalExchangeInter(move_info);

    costR2 = solution->getRoute(route2)->getCost();

    cout << "COSTS:\n"
         << solution->getCost() << " costR1 " << costR1 << " costR2 " << costR2
         << endl;
    cout << " newCost " << newCost << endl;

    cost = solution->getCost() - (costR1 + costR2) + (newCost);

    auxSol->getRoute(route1)->insertBlockByPos(pos1Route1, toInsertPickup_r2);
    auxSol->getRoute(route1)->insertBlockByPos(pos2Route1, toInsertDelivery_r2);

    auxSol->getRoute(route2)->insertBlockByPos(pos1Route2, toInsertPickup_r1);
    auxSol->getRoute(route2)->insertBlockByPos(pos2Route2, toInsertDelivery_r1);

    auxSol->update();

    // auxSol->print();

    auxCost = auxSol->getCost();

    if (abs(auxCost - cost) <= EPS) {
        cout << "4 - OK ==> Swap consistent solutions\n";
    } else {
        cout << "4 - WRONG ==> Swap inconsistent solutions\n";
        cout << "cost == auxCost\n";
        cout << cost << " must be equal to " << auxCost << endl;
        getchar();
    }

    printf("SOLUTION REAL COST: %.4lf | SOLUTION EVALUATED COST: %.4lf\n",
           auxCost, cost);

    return (abs(cost - auxCost) <= EPS);
}