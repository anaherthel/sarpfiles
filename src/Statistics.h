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

#endif  