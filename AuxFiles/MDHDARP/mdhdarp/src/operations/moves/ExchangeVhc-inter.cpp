#include "../LocalSearch.h"

void LocalSearch::applyExchangeVhcInter(MoveInfo move_info) {
    int route_1_idx = move_info.route_1;
    int route_2_idx = move_info.route_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();
    int route_2_len = route_2->getPathSize();

    vector<int> *to_insert_route_1 = route_1->removeBlockByPos(1, route_1_len - 2);
    vector<int> *to_insert_route_2 = route_2->removeBlockByPos(1, route_2_len - 2);

    route_1->insertBlockByPos(1, to_insert_route_2);
    route_2->insertBlockByPos(1, to_insert_route_1);

    delete to_insert_route_1;
    delete to_insert_route_2;
}

vector<int> LocalSearch::newPathExchangeVhcInter(MoveInfo move_info) {
    int route_1_idx = move_info.route_1;
    int route_2_idx = move_info.route_2;

    Route *route_1 = solution->getRoute(route_1_idx);
    Route *route_2 = solution->getRoute(route_2_idx);

    int route_1_len = route_1->getPathSize();

    vector<int> new_path(route_1->getPath());
    new_path[0] = new_path[route_1_len - 1] = route_2->getRequestIdByPos(0);

    return new_path;
}

void LocalSearch::exploreExchangeVhcInter(bool *moveEvaluation) {
    MoveInfo best_move, move_info, inverted_move_info;
    RideTimeChecker ride_time;

    Route *route_1, *route_2;

    vector<int> rt_path;

    int route_1_len, route_2_len;
    int sol_routes_size = data->v;

    double old_cost_route_1, old_cost_route_2;
    double eval_cost, old_cost, new_cost;
    double best_sol_cost, sol_cost;

    sol_cost = best_sol_cost = solution->getCost();

    bool improved = false, any_improve;

    for (int route_1_idx = 0; route_1_idx < sol_routes_size; ++route_1_idx) {
        route_1          = solution->getRoute(route_1_idx);
        old_cost_route_1 = route_1->getCost();
        route_1_len      = route_1->getPathSize();

        if (route_1_len == 2)
            continue;

        for (int route_2_idx = route_1_idx + 1; route_2_idx < sol_routes_size; ++route_2_idx) {
            any_improve      = false;
            route_2          = solution->getRoute(route_2_idx);
            route_2_len      = route_2->getPathSize();
            old_cost_route_2 = route_2->getCost();
            old_cost         = old_cost_route_2 + old_cost_route_1;

            if (route_2_len == 2)
                continue;

            move_info = MoveInfo(EXCHANGE_VHC, route_1_idx, route_2_idx);
            new_cost  = ads->evalExchangeVehicle(move_info);

            if (new_cost == INFEASIBLE)
                continue;

            eval_cost = sol_cost - old_cost + new_cost;

            if (improve(best_sol_cost, eval_cost)) {
                rt_path   = newPathExchangeVhcInter(move_info);
                ride_time = RideTimeChecker(&rt_path, data);
                if (!ride_time.check()) {
                    continue;
                }

                inverted_move_info = move_info.invertedMoveInfo();
                rt_path            = newPathExchangeVhcInter(inverted_move_info);
                ride_time          = RideTimeChecker(&rt_path, data);
                if (!ride_time.check()) {
                    continue;
                }

                improved      = true;
                best_sol_cost = eval_cost;
                best_move     = move_info;
            }
        }
    }

    if (improved) {
        *moveEvaluation = true;
        updateLocalSearchInter(best_move);
        solution->setPairNeighbStatusAllTrue(best_move.route_1, best_move.route_2);
    }
}

bool LocalSearch::assertExchangeVhcInter(MoveInfo move_info) {
}