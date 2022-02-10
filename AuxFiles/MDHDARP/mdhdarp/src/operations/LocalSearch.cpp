#include "LocalSearch.h"

LocalSearch::LocalSearch(Data *data, Statistics *stats, ADS *ads) : data(data),
                                                                    stats(stats),
                                                                    ads(ads) {
}

LocalSearch::~LocalSearch() {}

void LocalSearch::runIntra(Solution *sol) {
    NEIGHBORHOOD intraNeighborhood;

    repopulateIntra();

    while (!intraNeighborhoodList.empty()) {
        intraNeighborhood = getRandomMove(&intraNeighborhoodList);

        if (rvndIntra(intraNeighborhood)) {
            repopulateIntra();
        }
    }
}
// Block Extern
void LocalSearch::runInter(Solution *sol) {
    int interNNeighborhood, interBNeighborhood;
    NEIGHBORHOOD interNeighborhood, blockNeighborhood;

    //BLOCk EXTERN
    repopulateBlock();

    while (!blockNeighborhoodList.empty()) {
        blockNeighborhood = getRandomMove(&blockNeighborhoodList);

        if (rvndInter(blockNeighborhood)) {
            repopulateBlock();
            repopulateInter();
            while (!interNeighborhoodList.empty()) {
                interNeighborhood = getRandomMove(&interNeighborhoodList);
                if (rvndInter(interNeighborhood)) {
                    repopulateInter();
                }
            }
        }
    }
}

void LocalSearch::run(Solution *sol, int iter, int maxIter) {
    this->solution = sol;
    bool improved  = true;
    double sol_cost;

    while (improved) {
        improved = false;
        this->runInter(sol);

        if (iter == maxIter) {
            sol_cost = sol->getCost();
            this->runIntra(sol);
            if (improve(sol->getCost(), sol_cost)) {
                improved = true;
            }
        }
    }

    bestSolution = new Solution(*sol);
}

bool LocalSearch::rvndInter(NEIGHBORHOOD move) {
    bool moveEvaluation = false;

    //MSD
    if (solution->getRoutesSize() == data->v &&
        ads->MSD.find(solution->getCost()) != ads->MSD.end()) {
        return moveEvaluation;
    }

    switch (move) {
        case EXCHANGE_INTER:
            exploreExchangeInter(&moveEvaluation);
            break;
        case RELOCATE_INTER:
            exploreRelocateInter(&moveEvaluation);
            break;
        case TWO_OPT:
            exploreTwoOptInter(&moveEvaluation);
            break;
        case EXCHANGE_VHC:
            exploreExchangeVhcInter(&moveEvaluation);
            break;
        case EXCHANGE_BLOCK:
            exploreExchangeBlock(&moveEvaluation);
            break;
        case RELOCATE_BLOCK:
            exploreRelocateBlock(&moveEvaluation);
            break;
        default:
            break;
    }
    return moveEvaluation;
}

void LocalSearch::updateLocalSearchInter(MoveInfo move_info) {
    NEIGHBORHOOD neighborhood = move_info.neighborhood;
    switch (neighborhood) {
        case EXCHANGE_INTER:
            applyExchangeInter(move_info);
            break;
        case RELOCATE_INTER:
            applyRelocateInter(move_info);
            break;
        case TWO_OPT:
            applyTwoOptInter(move_info);
            break;
        case EXCHANGE_VHC:
            applyExchangeVhcInter(move_info);
            break;
        case EXCHANGE_BLOCK:
            applyExchangeBlock(move_info);
            break;
        case RELOCATE_BLOCK:
            applyRelocateBlock(move_info);
            break;
        default:
            break;
    }

    int route_1       = move_info.route_1;
    int route_2       = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    solution->updateRoute(route_1);
    solution->updateRoute(route_2);

    // stats->ads_update.setStart();
    if (neighborhood == EXCHANGE_INTER) {
        ads->update(route_1, route_1_pos_1, route_1_pos_2 + 1);
        ads->update(route_2, route_2_pos_1, route_2_pos_2 + 1);
    } else {
        ads->update(route_1, route_1_pos_1, solution->getRoute(route_1)->getPathSize() - 1);
        ads->update(route_2, route_2_pos_1, solution->getRoute(route_2)->getPathSize() - 1);
    }

    //Remove penalized route
    if ((solution->getRoute(route_1)->getPathSize() == 2 && route_1 >= data->v) ||
        (solution->getRoute(route_2)->getPathSize() == 2 && route_2 >= data->v)) {
        ads->update(data->v, 0, solution->getRoute(data->v)->getPathSize() - 1);
        solution->removeRoute();
    }
    // stats->ads_update.setEnd();
}

bool LocalSearch::rvndIntra(NEIGHBORHOOD move) {
    bool moveEvaluation = false;
    //MSD
    if (solution->getRoutesSize() == data->v &&
        ads->MSD.find(solution->getCost()) != ads->MSD.end())
        return moveEvaluation;

    switch (move) {
        case RELOCATE_INTRA:
            exploreRelocateIntra(&moveEvaluation);
            break;
        case EXCHANGE_INTRA:
            exploreExchangeIntra2(&moveEvaluation);
            break;
        default:
            break;
    }
    return moveEvaluation;
}

void LocalSearch::updateLocalSearchIntra(MoveInfo move_info) {
    NEIGHBORHOOD neighborhood = move_info.neighborhood;
    int route                 = move_info.route_1;
    double routeKey           = solution->getRoute(route)->getCost();

    switch (neighborhood) {
        case RELOCATE_INTRA:
            applyRelocateIntra(move_info);
            break;
        case EXCHANGE_INTRA:
            applyExchangeIntra(move_info);
        default:
            break;
    }

    solution->setPairNeighbStatusAllTrue(route);
    solution->updateRoute(route);

    // stats->ads_update.setStart();
    ads->update(route, 0, solution->getRoute(route)->getPathSize() - 1);
    // stats->ads_update.setEnd();
}

NEIGHBORHOOD LocalSearch::getRandomMove(vector<NEIGHBORHOOD> *list) {
    int randomPosition = rand() % list->size();
    NEIGHBORHOOD move  = list->at(randomPosition);

    list->erase(list->begin() + randomPosition);
    return move;
}

void LocalSearch::repopulateInter() {
    interNeighborhoodList.assign(interList.begin(), interList.end());
}

void LocalSearch::repopulateIntra() {
    this->intraNeighborhoodList.assign(intraList.begin(), intraList.end());
}

void LocalSearch::repopulateBlock() {
    blockNeighborhoodList.assign(blockList.begin(), blockList.end());
}