#include "../LocalSearch.h"

void LocalSearch::applyRelocateInter(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int to_insert_pickup   = route_1->removeRequest(route_1_pos_1);
    int to_insert_delivery = route_1->removeRequest(route_1_pos_2 - 1);

    route_2->insertRequest(to_insert_pickup, route_2_pos_1);
    route_2->insertRequest(to_insert_delivery, route_2_pos_2 + 1);
}

vector<int> LocalSearch::newPathRelocateInter(MoveInfo move_info) {
    int route_1_idx      = move_info.route_1;
    int route_2_idx      = move_info.route_2;
    int pickup_pos_src   = move_info.route_1_pos_1;
    int delivery_pos_src = move_info.route_1_pos_2;
    int pickup_pos_dst   = move_info.route_2_pos_1;
    int delivery_pos_dst = move_info.route_2_pos_2 + 1;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int pickup   = route_1->getRequestIdByPos(pickup_pos_src);
    int delivery = route_1->getRequestIdByPos(delivery_pos_src);

    vector<int> new_path(route_2->getPath());

    new_path.insert(new_path.begin() + pickup_pos_dst, pickup);
    new_path.insert(new_path.begin() + delivery_pos_dst, delivery);

    return new_path;
}

void LocalSearch::exploreRelocateInter(bool *moveEvaluation) {
    MoveInfo move_info, best_move;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    pair<double, double> key;

    Route *route_1, *route_2;

    vector<int> rt_path;

    int pickup, delivery;
    int pickup_pos_1, delivery_pos_1;
    int pickup_pos_2, delivery_pos_2;
    int pickup_init_pos, pickup_last_pos;
    int delivery_init_pos, delivery_last_pos;
    int route_1_len, route_2_len;
    int sol_routes_size = solution->getRoutesSize();

    double old_cost_route_1, old_cost_route_2;
    double new_cost_route_1, new_cost_route_2;
    double eval_cost, old_cost, new_cost;
    double best_sol_cost, sol_cost;

    best_sol_cost = sol_cost = solution->getCost();

    bool improved = false, any_improve;

    for (int route_1_idx = 0; route_1_idx < sol_routes_size; ++route_1_idx) {
        route_1          = solution->getRoute(route_1_idx);
        old_cost_route_1 = route_1->getCost();
        route_1_len      = route_1->getPathSize();

        if (route_1->getPathSize() <= 2)
            continue;

        for (int route_2_idx = 0; route_2_idx < data->v; ++route_2_idx) {
            if (route_1_idx == route_2_idx ||
                !solution->getInterNStatus(RELOCATE_INTER, route_1_idx, route_2_idx))
                continue;

            any_improve      = false;
            route_2          = solution->getRoute(route_2_idx);
            route_2_len      = route_2->getPathSize();
            old_cost_route_2 = route_2->getCost();
            old_cost         = old_cost_route_1 + old_cost_route_2;

            if (ads->useMMD(route_1_idx, route_2_idx)) {
                key       = make_pair(old_cost_route_1, old_cost_route_2);
                move_desc = ads->getMMD(RELOCATE_INTER, key);

                if (move_desc.status != UNVISITED &&
                    route_1->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_1)->getVehicleCopy() &&
                    route_2->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_2)->getVehicleCopy()) {
                    if (move_desc.status == CHANGEABLE) {
                        pickup = route_1->getRequestIdByPos(move_desc.moveInfo.route_1_pos_1);

                        move_info = move_desc.moveInfo;
                        move_info.setRoutes(route_1_idx, route_2_idx);

                        new_cost = ads->evalRemoveRequest(route_1_idx, move_info.route_1_pos_1) +
                                   ads->evalInsertionRequest(move_info, pickup);

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

            for (pickup_pos_1 = 1; pickup_pos_1 < route_1_len - 1; ++pickup_pos_1) {
                pickup   = route_1->getRequestIdByPos(pickup_pos_1);
                delivery = pickup + data->n;

                if (pickup >= data->n)
                    continue;

                delivery_pos_1   = ads->pairPosition[route_1_idx][pickup_pos_1].second;
                new_cost_route_1 = ads->evalRemoveRequest(route_1_idx, pickup_pos_1);

                pickup_init_pos = ads->startSearch[route_2_idx][pickup];
                pickup_last_pos = ads->endSearch[route_2_idx][pickup];

                for (pickup_pos_2 = pickup_init_pos; pickup_pos_2 < pickup_last_pos; ++pickup_pos_2) {
                    if (!ads->checkInsertionVertice(route_2_idx, pickup_pos_2, pickup))
                        continue;

                    route_2_len = ads->endSearch[route_2_idx][delivery];

                    delivery_init_pos = max(pickup_pos_2, ads->startSearch[route_2_idx][delivery]);
                    delivery_last_pos = route_2_len;

                    for (delivery_pos_2 = delivery_init_pos; delivery_pos_2 < delivery_last_pos; ++delivery_pos_2) {
                        move_info = MoveInfo(RELOCATE_INTER,
                                             route_1_idx, route_2_idx,
                                             pickup_pos_1, delivery_pos_1,
                                             pickup_pos_2, delivery_pos_2);

                        new_cost_route_2 = ads->evalInsertionRequest(move_info, pickup);

                        if (new_cost_route_2 == INFEASIBLE)
                            continue;

                        new_cost  = new_cost_route_1 + new_cost_route_2;
                        eval_cost = sol_cost - old_cost + new_cost;

                        if (improve(best_sol_cost, eval_cost)) {
                            rt_path   = newPathRelocateInter(move_info);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }

                            improved      = true;
                            best_sol_cost = eval_cost;
                            best_move     = move_info;

                            // if (!assertRelocateInter(best_move)) {
                            //     cout << "ERROR\n";
                            //     getchar();
                            // }

                            if (ads->useMMD(route_1_idx, route_2_idx)) {
                                ads->insertMMD(key, MoveDesc(CHANGEABLE, best_move));
                            }
                        }

                        if (improve(old_cost, new_cost)) {
                            any_improve = true;
                        }
                    }
                }
            }

            if (!any_improve) {
                solution->setPairNeighbStatus(RELOCATE_INTER, route_1_idx, route_2_idx, false);

                if (ads->useMMD(route_1_idx, route_2_idx)) {
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(RELOCATE_INTER, route_1_idx, route_2_idx));
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

bool LocalSearch::assertRelocateInter(MoveInfo move_info) {
    int route_1       = move_info.route_1;
    int route_2       = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Solution *aux_sol = new Solution(*solution);
    double old_cost_route_1, old_cost_route_2;
    double new_cost_route_1, new_cost_route_2;
    double old_cost, new_cost, aux_cost;

    if (aux_sol->getCost() == solution->getCost()) {
        cout << "1 - OK ==> Copy working\n";
    } else {
        cout << "1 - WRONG ==> Copy not working\n";
        delete aux_sol;
        return false;
    }

    // Remove part
    old_cost_route_1 = solution->getRoute(route_1)->getCost();
    new_cost_route_1 = ads->evalRemoveRequest(route_1, route_1_pos_1);

    new_cost = solution->getCost() - old_cost_route_1 + new_cost_route_1;

    int to_insert_pickup   = aux_sol->getRoute(route_1)->removeRequest(route_1_pos_1);
    int to_insert_delivery = aux_sol->getRoute(route_1)->removeRequest(route_1_pos_2 - 1);
    aux_sol->update();

    aux_cost = aux_sol->getCost();

    if (to_insert_pickup ==
            solution->getRoute(route_1)->getRequestIdByPos(route_1_pos_1) &&
        to_insert_delivery ==
            solution->getRoute(route_1)->getRequestIdByPos(route_1_pos_2)) {
        cout << "2 - OK ==> Removing same pickup and delivery node\n";
    } else {
        cout << "2 - WRONG ==> Removing different pickup and delivery node\n";
        cout << "Aux removing part: " << to_insert_pickup << " ";
        cout << to_insert_delivery << endl;
        cout << "Real removing part: " << solution->getRoute(route_1)->getRequestIdByPos(route_1_pos_1);
        cout << " " << solution->getRoute(route_1)->getRequestIdByPos(route_1_pos_2) << endl;
        delete aux_sol;
        return false;
    }

    if (assertEqual(new_cost_route_1, aux_sol->getRoute(route_1)->getCost())) {
        cout << "3 - OK ==> Consistent value after remove between sequence and "
                "real route\n";
    } else {
        cout << "3 - WRONG ==> Inconsistent value after remove between ";
        cout << "new_cost_route_1 != aux_sol->getRoute(route1)->getPenalizedCost()\n";
        cout << new_cost_route_1 << " must be equal to ";
        cout << aux_sol->getRoute(route_1)->getCost() << endl;
        delete aux_sol;
        return false;
    }

    if (assertEqual(new_cost, aux_cost)) {
        cout << "4 - OK ==> Consistent value after remove between solutions\n";
    } else {
        cout << "4 - WRONG ==> Inconsistent value after remove between solutions\n";
        cout << "cost != aux_cost\n";
        printf("%.10lf must be equal to %.10lf\n", new_cost, aux_cost);
        delete aux_sol;
        return false;
    }

    // Insert part
    int request_id   = solution->getRoute(route_1)->getRequestIdByPos(route_1_pos_1);
    old_cost_route_2 = solution->getRoute(route_2)->getCost();
    new_cost_route_2 = ads->evalInsertionRequest(move_info, request_id);

    aux_sol->getRoute(route_2)->insertRequest(to_insert_pickup, route_2_pos_1);
    aux_sol->getRoute(route_2)->insertRequest(to_insert_delivery, route_2_pos_2 + 1);
    aux_sol->update();

    if (assertEqual(new_cost_route_2, aux_sol->getRoute(route_2)->getCost())) {
        cout << "5 - OK ==> Consistent value after insert between sequence and ";
        cout << "real route\n";
    } else {
        cout << "5 - WRONG ==> Inconsistent value after insert between ";
        cout << "new_cost_route_2 != aux_sol->getRoute(route2)->getPenalizedCost()\n";
        cout << new_cost_route_2 << " must be equal to ";
        cout << aux_sol->getRoute(route_2)->getCost() << endl;
        delete aux_sol;
        return false;
    }

    new_cost += (new_cost_route_2 - old_cost_route_2);
    aux_cost = aux_sol->getCost();

    if (assertEqual(new_cost, aux_cost)) {
        cout << "6 - OK ==> RELOCATE-INTER consistent solutions\n";
    } else {
        cout << "6 - WRONG ==> RELOCATE-INTER inconsistent solutions\n";
        cout << "new_cost != aux_cost\n";
        cout << new_cost << " must be equal to " << aux_cost << endl;
        delete aux_sol;
        return false;
    }

    delete aux_sol;
    return assertEqual(new_cost, aux_cost);
}