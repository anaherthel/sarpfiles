#include "../LocalSearch.h"

void LocalSearch::applyExchangeBlock(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    vector<int> *to_insert_route_2 = route_1->removeBlockByPos(route_1_pos_1, route_1_pos_2 - route_1_pos_1 + 1);
    vector<int> *to_insert_route_1 = route_2->removeBlockByPos(route_2_pos_1, route_2_pos_2 - route_2_pos_1 + 1);

    route_1->insertBlockByPos(route_1_pos_1, to_insert_route_1);
    route_2->insertBlockByPos(route_2_pos_1, to_insert_route_2);

    delete to_insert_route_2;
    delete to_insert_route_1;
}

vector<int> LocalSearch::newPathExchangeBlock(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2 + 1;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();

    vector<int>::const_iterator first = route_1->getPathBegin();
    vector<int>::const_iterator last  = route_1->getPathBegin() + route_1_pos_1;

    vector<int> new_path(first, last);

    for (int i = route_2_pos_1; i < route_2_pos_2; ++i) {
        new_path.push_back(route_2->getRequestIdByPos(i));
    }

    for (int i = route_1_pos_2 + 1; i < route_1_len; ++i) {
        new_path.push_back(route_1->getRequestIdByPos(i));
    }

    return new_path;
}

void LocalSearch::exploreExchangeBlock(bool *moveEvaluation) {
    MoveInfo best_move, move_info, inverted_move_info;
    MoveDesc move_desc;
    RideTimeChecker ride_time;
    pair<double, double> key;

    Route *route_1, *route_2;

    vector<int> rt_path;

    int route_1_block_l, route_1_block_r;
    int route_2_block_r, route_2_block_l;
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
        old_cost_route_1 = route_1->getCost();
        route_1_len      = ads->loadZeroIndexes[route_1_idx].size();

        if (route_1_len <= 1)
            continue;

        for (int route_2_idx = route_1_idx + 1; route_2_idx < sol_routes_size; ++route_2_idx) {
            any_improve      = false;
            route_2          = solution->getRoute(route_2_idx);
            route_2_len      = ads->loadZeroIndexes[route_2_idx].size();
            old_cost_route_2 = route_2->getCost();
            old_cost         = old_cost_route_2 + old_cost_route_1;
            key              = make_pair(old_cost_route_1, old_cost_route_2);

            if (route_2_len <= 1 ||
                !solution->getInterNStatus(EXCHANGE_BLOCK, route_1_idx, route_2_idx))
                continue;

            if (ads->useMMD(route_1_idx, route_2_idx)) {
                move_desc = ads->getMMD(EXCHANGE_BLOCK, key);

                if (move_desc.status != UNVISITED &&
                    route_1->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_1)->getVehicleCopy() &&
                    route_2->getVehicleCopy() == solution->getRoute(move_desc.moveInfo.route_2)->getVehicleCopy()) {
                    if (move_desc.status == CHANGEABLE) {
                        move_info = move_desc.moveInfo;
                        move_info.setRoutes(route_1_idx, route_2_idx);

                        new_cost  = ads->evalExchangeBlock(move_info);
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

            for (int i = 1; i < route_1_len; ++i) {
                for (int k1 = i + 1; k1 < route_1_len; ++k1) {
                    route_1_block_l = ads->loadZeroIndexes[route_1_idx][i - 1] + 1;
                    route_1_block_r = ads->loadZeroIndexes[route_1_idx][k1];

                    if ((route_1_block_r - route_1_block_l) <= 1)
                        continue;  // Redundant Move

                    for (int j = 1; j < route_2_len; ++j) {
                        for (int k2 = j + 1; k2 < route_2_len; ++k2) {
                            route_2_block_l = ads->loadZeroIndexes[route_2_idx][j - 1] + 1;
                            route_2_block_r = ads->loadZeroIndexes[route_2_idx][k2];

                            if ((route_2_block_r - route_2_block_l) <= 1)
                                continue;  // Redundant Move

                            move_info = MoveInfo(EXCHANGE_BLOCK, route_1_idx, route_2_idx,
                                                 route_1_block_l, route_1_block_r,
                                                 route_2_block_l, route_2_block_r);
                            new_cost  = ads->evalExchangeBlock(move_info);

                            if (new_cost == INFEASIBLE)
                                continue;

                            eval_cost = sol_cost - old_cost + new_cost;

                            if (improve(best_sol_cost, eval_cost)) {
                                if (!route_1->isExtra()) {
                                    rt_path   = newPathExchangeBlock(move_info);
                                    ride_time = RideTimeChecker(&rt_path, data);
                                    if (!ride_time.check()) {
                                        continue;
                                    }
                                }
                                inverted_move_info = move_info.invertedMoveInfo();
                                if (!route_2->isExtra()) {
                                    rt_path   = newPathExchangeBlock(inverted_move_info);
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
                }
            }

            if (!any_improve) {
                solution->setPairNeighbStatus(EXCHANGE_BLOCK, route_1_idx, route_2_idx, false);
                solution->setPairNeighbStatus(EXCHANGE_BLOCK, route_2_idx, route_1_idx, false);

                if (ads->useMMD(route_1_idx, route_2_idx)) {
                    key       = make_pair(old_cost_route_1, old_cost_route_2);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_BLOCK, route_1_idx, route_2_idx));
                    ads->insertMMD(key, move_desc);

                    key       = make_pair(old_cost_route_2, old_cost_route_1);
                    move_desc = MoveDesc(UNCHANGEABLE, MoveInfo(EXCHANGE_BLOCK, route_2_idx, route_1_idx));
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

bool LocalSearch::assertExchangeBlock(MoveInfo move_info) {
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

    std::vector<int> *toInsertInR2 = auxSol->getRoute(route1)->removeBlockByPos(
        pos1Route1, pos2Route1 - pos1Route1 + 1);
    std::vector<int> *toInsertInR1 = auxSol->getRoute(route2)->removeBlockByPos(
        pos1Route2, pos2Route2 - pos1Route2 + 1);

    auxSol->getRoute(route1)->insertBlockByPos(pos1Route1, toInsertInR1);
    auxSol->getRoute(route2)->insertBlockByPos(pos1Route2, toInsertInR2);
    auxSol->getRoute(route1)->update();
    auxSol->getRoute(route2)->update();

    delete toInsertInR1;
    delete toInsertInR2;

    auxSol->update();

    // Change Insertions
    costR1 = solution->getRoute(route1)->getCost();
    costR2 = solution->getRoute(route2)->getCost();

    newCost = ads->evalExchangeBlock(move_info);
    cost    = solution->getCost() - (costR1 + costR2) + newCost;

    cout << "COSTS:\n"
         << solution->getCost() << " costR1 " << costR1 << " costR2 " << costR2
         << endl;
    cout << " newCost " << newCost << endl;

    auxCost = auxSol->getCost();

    if (abs(auxCost - cost) <= EPS) {
        cout << "2 - OK ==> Swap consistent solutions\n";
    } else {
        cout << "2 - WRONG ==> Swap inconsistent solutions\n";
        cout << "cost == auxCost\n";
        cout << cost << " must be equal to " << auxCost << endl;
        cout << "SOLUTION" << endl;
        solution->print();
        cout << "AUX SOLUTION" << endl;
        auxSol->print();
        getchar();
    }

    printf("SOLUTION REAL COST: %.4lf | SOLUTION EVALUATED COST: %.4lf\n",
           auxCost, cost);

    return (abs(cost - auxCost) <= EPS);
}