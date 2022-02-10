#ifndef STATISTICS_H
#define STATISTICS_H

#include <cstdio>
#include <vector>
#include <utility>
#include <time.h>

using namespace std;

class Runtime {
public:
    clock_t start, end;
    double total = 0;
    double count = 0;

    double calcTimeStep() {
        return (double)(end - start)/CLOCKS_PER_SEC;
    }

    inline void setStart() {
        start = clock();
    }

    inline void setEnd() {
        end = clock();
        total += calcTimeStep();
        count++;
    }
    
    double printTime() {
        return total;
    }
};


class Statistics {
public:
    vector<int> improves;
    vector<int> improvesPert;
    vector<double> improvesPercent;

    Runtime loadData;
    Runtime ILS, constructive, perturbation;
    Runtime localSearch;

    Runtime shift, swap, cross, swapvhc, swapblock, shiftblock;
    Runtime reinsertion, exchange, exchange2;
    Runtime rt_shift, rt_swap, rt_cross, rt_swapvhc, rt_swapblock, rt_shiftblock;
    Runtime rt_reins, rt_exchange, rt_construc;

    Runtime checkSingleShift, checkPairShift, checkSingleReinsrt, checkPairReinsrt;
    Runtime concatSeq, setSeq;
    Runtime hashUpdate;
    Runtime ads_update;

    Runtime SP, SPIn;    
    Runtime allProgram;

    Statistics() = default;
    ~Statistics() = default;

    void printStatistics();

    void clearImprovesPert(){
        improvesPert.assign(2, 0);
    }

    void clearImproves(int n){
        improves.assign(n, 0);
        improvesPercent.assign(n, 0.0);
    }

    void printImproves();
};

#endif  