#include "Perturbation.h"

Perturbation::Perturbation(Data *data, Statistics *stats,
                           ADS *ads, int I_p,
                           Route *extraRoute) : data(data),
                                                stats(stats),
                                                ads(ads),
                                                I_p(I_p),
                                                extraRoute(extraRoute) {
}

Perturbation::~Perturbation() {
}

MoveInfo Perturbation::getRandomRmvPos(Route *route, int route_idx) {
    NEIGHBORHOOD neighborhood = INSERTION;
    int pickup, delivery;
    int route_pos_1, route_pos_2;

    route_pos_1 = 1 + (rand() % (route->getPathSize() - 2));
    pickup      = route->getRequestIdByPos(route_pos_1);

    if (pickup >= data->n) {
        route_pos_1 = ads->pairPosition[route_idx][route_pos_1].second;
        pickup      = pickup - data->n;
    }

    delivery    = pickup + data->n;
    route_pos_2 = ads->pairPosition[route_idx][route_pos_1].second;

    return MoveInfo(neighborhood, route_idx, route_pos_1, route_pos_2);
}

int Perturbation::getRandomRmvPosZero(int route_idx) {
    return (rand() % (ads->loadZeroIndexes[route_idx].size() - 1));
}

MoveInfo Perturbation::getRandomInsPos(Route *route) {
    NEIGHBORHOOD neighborhood = INSERTION;

    int route_idx = data->v;
    int route_pos_1, route_pos_2;

    route_pos_1 = 1 + (rand() % (route->getPathSize() - 1));
    route_pos_2 = route_pos_1 + (rand() % (route->getPathSize() - route_pos_1));

    return MoveInfo(neighborhood, route_idx, route_pos_1, route_pos_2);
}

void Perturbation::requestPerturbation(Solution *solution) {
    NEIGHBORHOOD neighborhood = RELOCATE_INTER;
    MoveInfo move_info_rmv, move_info_ins;
    Route *route;
    int route_idx, pickup, delivery;
    int cnt = 0;

    do {
        cnt++;
        route_idx = rand() % data->v;
    } while (solution->getRoute(route_idx)->getPathSize() <= 2 && cnt <= PERTURB_MAX_TRY);

    if (cnt >= PERTURB_MAX_TRY)
        return;

    route         = solution->getRoute(route_idx);
    move_info_rmv = getRandomRmvPos(route, route_idx);
    move_info_ins = getRandomInsPos(extraRoute);

    pickup   = route->removeRequest(move_info_rmv.route_1_pos_1);
    delivery = route->removeRequest(move_info_rmv.route_1_pos_2 - 1);

    extraRoute->insertRequest(pickup, move_info_ins.route_1_pos_1);
    extraRoute->insertRequest(delivery, move_info_ins.route_1_pos_2 + 1);

    route->update();
    ads->update(move_info_rmv.route_1, move_info_rmv.route_1_pos_1, route->getPathSize() - 1);
    extraRoute->update();
    return;
}

void Perturbation::blockPerturbation(Solution *solution) {
    NEIGHBORHOOD neighborhood = RELOCATE_BLOCK;
    MoveInfo move_info_ins;
    Route *route;
    vector<int> *removed_block;
    int route_idx, route_1_pos_1, route_1_pos_2, random_pos;
    int cnt = 0;

    do {
        cnt++;
        route_idx = rand() % data->v;
    } while ((solution->getRoute(route_idx)->getPathSize() <= 2 ||
              ads->loadZeroIndexes[route_idx].size() <= 1) &&
             cnt <= PERTURB_MAX_TRY);

    if (cnt >= PERTURB_MAX_TRY)
        return;

    route = solution->getRoute(route_idx);

    random_pos    = getRandomRmvPosZero(route_idx);
    route_1_pos_1 = ads->loadZeroIndexes[route_idx][random_pos] + 1;
    route_1_pos_2 = ads->loadZeroIndexes[route_idx][random_pos + 1] -
                    ads->loadZeroIndexes[route_idx][random_pos];

    move_info_ins = getRandomInsPos(extraRoute);

    removed_block = route->removeBlockByPos(route_1_pos_1, route_1_pos_2);
    extraRoute->insertBlockByPos(move_info_ins.route_1_pos_1, removed_block);
    delete removed_block;

    route->update();
    ads->update(route_idx, route_1_pos_1, route->getPathSize() - 1);
    extraRoute->update();
    return;
}

Solution *Perturbation::run(Solution *solution) {
    Solution *sol     = new Solution(*solution);
    int total_changes = (rand() % I_p) + 1;
    int coin;

    ads->updateSol(sol);
    ads->update();

    if (sol->getRoutesSize() == sol->data->v) {
        extraRoute->clear();
    } else {
        sol->removeRoute();
    }

    for (int i = 0; i < total_changes; ++i) {
        coin = rand() % 2;

        if (coin) {  // Block Pertb
            blockPerturbation(sol);
        } else {  // Single Pertb
            requestPerturbation(sol);
        }
    }
    sol->addRoute(extraRoute);
    ads->update(data->v, 0, extraRoute->getPathSize() - 1);

    return sol;
}