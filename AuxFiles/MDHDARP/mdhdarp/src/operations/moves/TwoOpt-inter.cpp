#include "../LocalSearch.h"

void LocalSearch::applyTwoOptInter(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_2_pos_1 = move_info.route_2_pos_1;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();
    int route_2_len = route_2->getPathSize();

    vector<int> *to_insert_route_1 = route_1->removeBlockByPos(route_1_pos_1 + 1, route_1_len - route_1_pos_1 - 2);
    vector<int> *to_insert_route_2 = route_2->removeBlockByPos(route_2_pos_1 + 1, route_2_len - route_2_pos_1 - 2);

    route_1->insertBlockByPos(route_1_pos_1 + 1, to_insert_route_2);
    route_2->insertBlockByPos(route_2_pos_1 + 1, to_insert_route_1);

    delete to_insert_route_1;
    delete to_insert_route_2;
}

vector<int> LocalSearch::newPathTwoOptInter(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1 + 1;
    int route_2_pos_1 = move_info.route_2_pos_1 + 1;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();
    int route_2_len = route_2->getPathSize() - 1;

    vector<int>::const_iterator first = route_1->getPathBegin();
    vector<int>::const_iterator last  = route_1->getPathBegin() + route_1_pos_1;

    vector<int> new_path(first, last);

    for (int i = route_2_pos_1; i < route_2_len; ++i) {
        new_path.push_back(route_2->getRequestIdByPos(i));
    }
    new_path.push_back(new_path[0]);

    return new_path;
}

void LocalSearch::exploreTwoOptInter(bool *moveEvaluation) {
    MoveInfo best_move, move_info, inverted_move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    pair<double, double> key;

    Route *route_1, *route_2;

    vector<int> rt_path;

    int route_1_pos, route_2_pos;
    int route_1_len, route_2_len;
    int sol_routes_size = solution->getRoutesSize();

    double route_1_cost, route_2_cost;
    double eval_cost, old_cost, new_cost;
    double best_sol_cost, sol_cost;

    bool improved = false, any_improve;

    sol_cost = best_sol_cost = solution->getCost();

    for (int route_1_idx = 0; route_1_idx < sol_routes_size; ++route_1_idx) {
        route_1      = solution->getRoute(route_1_idx);
        route_1_cost = route_1->getCost();
        route_1_len  = ads->loadZeroIndexes[route_1_idx].size();

        for (int route_2_idx = route_1_idx + 1; route_2_idx < sol_routes_size; ++route_2_idx) {
            if (route_1_idx == route_2_idx ||
                !solution->getInterNStatus(TWO_OPT, route_1_idx, route_2_idx))
                continue;

            any_improve  = false;
            route_2      = solution->getRoute(route_2_idx);
            route_2_len  = ads->loadZeroIndexes[route_2_idx].size();
            route_2_cost = route_2->getCost();
            old_cost     = route_1_cost + route_2_cost;

            if (ads->useMMD(route_1_idx, route_2_idx)) {
                key       = make_pair(route_1_cost, route_2_cost);
                move_desc = ads->getMMD(TWO_OPT, key);

                if (move_desc.status != UNVISITED &&
                    route_1->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_1)->getVehicleCopy() &&
                    route_2->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_2)->getVehicleCopy()) {
                    if (move_desc.status == CHANGEABLE) {
                        move_info = move_desc.moveInfo;
                        move_info.setRoutes(route_1_idx, route_2_idx);

                        new_cost  = ads->evalTwoOpt(move_info);
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

            for (int load_zero_idx_1 = 0; load_zero_idx_1 < route_1_len; ++load_zero_idx_1) {
                route_1_pos = ads->loadZeroIndexes[route_1_idx][load_zero_idx_1];

                for (int load_zero_idx_2 = 0; load_zero_idx_2 < route_2_len; ++load_zero_idx_2) {
                    route_2_pos = ads->loadZeroIndexes[route_2_idx][load_zero_idx_2];

                    move_info = MoveInfo(TWO_OPT, route_1_idx, route_2_idx, route_1_pos, route_2_pos);
                    new_cost  = ads->evalTwoOpt(move_info);

                    if (new_cost == INFEASIBLE)
                        continue;

                    eval_cost = sol_cost - old_cost + new_cost;

                    if (improve(best_sol_cost, eval_cost)) {
                        if (!route_1->isExtra()) {
                            rt_path   = newPathTwoOptInter(move_info);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }
                        }

                        inverted_move_info = move_info.invertedMoveInfo();

                        if (!route_2->isExtra()) {
                            rt_path   = newPathTwoOptInter(inverted_move_info);
                            ride_time = RideTimeChecker(&rt_path, data);
                            if (!ride_time.check()) {
                                continue;
                            }
                        }

                        improved      = true;
                        best_sol_cost = eval_cost;
                        best_move     = move_info;

                        if (ads->useMMD(route_1_idx, route_2_idx)) {
                            key = make_pair(route_1_cost, route_2_cost);
                            ads->insertMMD(key, MoveDesc(CHANGEABLE, best_move));

                            key = make_pair(route_2_cost, route_1_cost);
                            ads->insertMMD(key, MoveDesc(CHANGEABLE, inverted_move_info));
                        }
                    }

                    if (improve(old_cost, new_cost)) {
                        any_improve = true;
                    }
                }
            }

            if (!any_improve) {
                solution->setPairNeighbStatus(TWO_OPT, route_1_idx, route_2_idx, false);
                solution->setPairNeighbStatus(TWO_OPT, route_2_idx, route_1_idx, false);

                if (ads->useMMD(route_1_idx, route_2_idx)) {
                    key       = make_pair(route_1_cost, route_2_cost);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(TWO_OPT, route_1_idx, route_2_idx));
                    ads->insertMMD(key, move_desc);

                    key       = make_pair(route_2_cost, route_1_cost);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(TWO_OPT, route_2_idx, route_1_idx));
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

bool LocalSearch::assertTwoOptInter(MoveInfo move_info) {
    puts("ASSERTING CROSS");
    int r1   = move_info.route_1;
    int r2   = move_info.route_2;
    int pos1 = move_info.route_1_pos_1;
    int pos2 = move_info.route_2_pos_1;

    // Pegando referencia
    Solution *auxSol = new Solution(*solution);
    double cost, costR, newCost, auxCost;

    // printf("\nR1: %d - %d\nR2: %d - %d\n", id_pickup_r1, id_delivery_r1,
    // id_pickup_r2, id_delivery_r2);

    if (auxSol->getCost() == solution->getCost()) {
        cout << "1 - OK ==> Copy working\n";
    } else {
        cout << "1 - WRONG ==> Copy not working\n";
    }

    Route *route1 = auxSol->getRoute(r1);
    Route *route2 = auxSol->getRoute(r2);

    route1->printPath();
    route2->printPath();

    cout << pos1 << "  " << pos2 << endl;

    costR = route1->getCost() + route2->getCost();
    cout << "OLD COST OK: " << costR << endl;
    newCost = ads->evalTwoOpt(move_info);
    cout << "EVAL COST OK: " << newCost << endl;

    cost = solution->getCost() - costR + newCost;

    vector<int> *toInsertR1 =
        route1->removeBlockByPos(pos1 + 1, route1->getPathSize() - pos1 - 2);
    cout << "REMOVE R1 OK\n";
    vector<int> *toInsertR2 =
        route2->removeBlockByPos(pos2 + 1, route2->getPathSize() - pos2 - 2);
    cout << "REMOVE R2 OK\n";

    route1->insertBlockByPos(pos1 + 1, toInsertR2);
    cout << "INSERT R1 OK\n";
    route2->insertBlockByPos(pos2 + 1, toInsertR1);
    cout << "INSERT R2 OK\n";

    auxSol->update();

    auxSol->getRoute(r1)->printPath();
    auxSol->getRoute(r2)->printPath();

    auxCost = auxSol->getCost();

    if (abs(auxCost - cost) <= EPS) {
        cout << "4 - OK ==> Cross consistent solutions\n";
    } else {
        cout << "4 - WRONG ==> Cross inconsistent solutions\n";
        cout << "cost == auxCost\n";
        cout << cost << " must be equal to " << auxCost << endl;
        getchar();
    }

    delete toInsertR1;
    delete toInsertR2;

    printf("SOLUTION REAL COST: %.4lf | SOLUTION EVALUATED COST: %.4lf\n",
           auxCost, cost);

    return (abs(cost - auxCost) <= EPS);
}