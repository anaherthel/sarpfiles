#include "ILS.h"

ILS::ILS(Data *data, Statistics *stats,
         int I_r, int I_ils, int I_p, int sp_tl) : data(data),
                                                   stats(stats),
                                                   I_r(I_r),
                                                   I_ils(I_ils),
                                                   I_p(I_p),
                                                   sp_tl(sp_tl),
                                                   incumbent(false),
                                                   solution(nullptr),
                                                   bestIterSolution(nullptr),
                                                   bestSolution(new Solution(this->data)),
                                                   bestStartsPool(new RoutePool()) {}

ILS::ILS(Data *data, int I_ils, int I_p, Solution *sol) : data(data),
                                                          stats(new Statistics()),
                                                          I_r(1),
                                                          I_ils(I_ils),
                                                          I_p(I_p),
                                                          incumbent(true),
                                                          solution(nullptr),
                                                          bestIterSolution(nullptr),
                                                          bestSolution(new Solution(*sol)) {}

ILS::~ILS() {}

Solution *ILS::run(RoutePool *pool, RoutePool *pool_temp) {
    Vehicle extraVhc(true, data->R);
    extraRoute = new Route(data, extraVhc);

    ADS *ads                 = new ADS(data, stats, solution);
    LocalSearch *localSearch = new LocalSearch(data, stats, ads);
    ConstructiveProcedure constructive(this->data, this->stats, ads);
    Perturbation perturbation(this->data, this->stats, ads, I_p, extraRoute);
    SetPartitioning sp_iter(data, I_ils, I_p, sp_tl, true, pool, pool_temp);
    SetPartitioning sp(data, I_ils, I_p, sp_tl, false, pool, pool_temp);

    int i_r      = 0;
    int i_ils    = 0;
    int n_routes = 0;
    double delta;
    double solCost = 0.0;

    for (i_r = 0; i_r < I_r; ++i_r) {
        if (!incumbent) {
            pool_temp->clear();
            solution = new Solution(this->data);
            ads->updateSol(solution);
            constructive.initSolution(solution);
        } else {
            solution = new Solution(*bestSolution);
            ads->updateSol(solution);
            constructive.initSPSolution(solution);
        }

        bestIterSolution = new Solution(*solution);

        i_ils = 0;

        while (i_ils <= I_ils) {
            stats->localSearch.setStart();
            localSearch->run(solution, i_ils, I_ils);
            stats->localSearch.setEnd();

            delete solution;
            solution = localSearch->getBestSolution();

            if (ads->useMSD((solution->getRoutesSize()))) {
                if (ads->isInMSD(solution->getCost())) {
                    if (ads->getMSD(solution->getCost()) < i_r) {
                        break;
                    }
                } else {
                    ads->insertMSD(solution->getCost(), i_r);
                    pool->updateSet(solution);
                    pool_temp->updateSet(solution);
                }
            }

            if (improve(bestIterSolution->getCost(), solution->getCost())) {
                delete bestIterSolution;
                bestIterSolution = new Solution(*solution);
                // printf("Best Iter Cost: %.10lf\n",
                // bestIterSolution->getCost());
                i_ils = 0;
            }

            delete solution;

            // stats->perturbation.setStart();
            solution = perturbation.run(bestIterSolution);
            // stats->perturbation.setEnd();
            ++i_ils;
        }

        if (!incumbent) {
            // stats->SPIn.setStart();
            pool_temp->mergePool(bestStartsPool);
            pool_temp->setToList();

            if (improve(bestSolution->getCost(), bestIterSolution->getCost())) {
                pool_temp->addInitialSol(bestIterSolution);
            } else {
                pool_temp->addInitialSol(bestSolution);
            }

            double bestIterCost = bestIterSolution->getCost();
            delete bestIterSolution;

            bestIterSolution = sp_iter.run();
            bestIterSolution->repairSol();
            bestStartsPool->updateSet(bestIterSolution);
            // stats->SPIn.setEnd();
        }

        if (improve(bestSolution->getCost(), bestIterSolution->getCost())) {
            delete bestSolution;
            bestSolution = new Solution(*bestIterSolution);
            // printf("Best Cost: %.10lf\n", bestSolution->getCost());
        }

        // printf("Pool size: %lu\n", pool->pool.size());

        delete solution;
        delete bestIterSolution;
    }

    // ============== FINISH ILS FOR THE FIRST TIME ==============

    if (!incumbent && data->n <= 100) {
        // stats->SP.setStart();

        pool->setToList();
        pool->addInitialSol(bestSolution);

        delete bestSolution;
        bestSolution = sp.run();

        // stats->SP.setEnd();
    }

    bestSolution->repairSol();

    delete localSearch;
    delete ads;

    return bestSolution;
}