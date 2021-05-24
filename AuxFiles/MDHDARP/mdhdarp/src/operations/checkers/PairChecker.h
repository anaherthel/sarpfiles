#ifndef PAIRCHECKER_H
#define PAIRCHECKER_H

#include "Checker.h"

class PairChecker : public Checker {
   private:
    vector<int> pairPosition;

   public:
    PairChecker(vector<int> *path, Data *data, Vehicle *vehicle);
    PairChecker() = default;
    bool check();

    ~PairChecker() {}
};

#endif