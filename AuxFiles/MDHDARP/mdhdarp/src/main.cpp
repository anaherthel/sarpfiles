#include <time.h>

#include <chrono>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <thread>

#include "ILS.h"
#include "model/Data.h"
#include "model/Solution.h"
#include "model/Statistics.h"
#include "model/setpartitioning/RoutePool.h"
#include "model/tests/RouteTest.h"

using namespace std;

int main(int argc, char const *argv[]) {
    if (argc < 3) {
        cout << "Missing Parameters! \n./exec [instance]\n";
        return 0;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    Statistics stats;
    stats.allProgram.setStart();

    clock_t seed = (argc == 3) ? time(NULL) : strtol(argv[3], NULL, 10);
    printf("%d, ", seed);
    srand(seed);

    stats.loadData.setStart();
    Data data(argv[1], argv[2]);
    data.read();
    data.preprocessing();
    // data.print();
    stats.loadData.setEnd();

    // ILS PARAMS:
    int I_r   = 20;                       // # of starts
    int I_ils = max((data.n << 1), 200);  // # perturubations without improves
    int I_p   = 4;                        // # max of operations per perturbation
    int sp_tl = 60;                       // Time limit of SP

    RoutePool *pool      = new RoutePool();
    RoutePool *pool_temp = new RoutePool();

    ILS ils(&data, &stats, I_r, I_ils, I_p, sp_tl);
    Solution *bestSol = ils.run(pool, pool_temp);

    // printf("%.3lf, %d\n", bestSol->getCost(), pool->pool.size());
    printf("%.3lf, ", bestSol->getCost());
    // bestSol->check();
    // bestSol->print();

    stats.allProgram.setEnd();
    stats.printStatistics();

    pool->clear();
    pool_temp->clear();

    delete bestSol;
    delete pool;
    delete pool_temp;

    return 0;
}