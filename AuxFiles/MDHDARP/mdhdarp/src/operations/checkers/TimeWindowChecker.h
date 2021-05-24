#ifndef TIMEWINDOWCHECKER_H
#define TIMEWINDOWCHECKER_H

#include "Checker.h"

class TimeWindowChecker : Checker {
   private:
    Sequence sequence;

   public:
    TimeWindowChecker(vector<int> *path, Data *data, Vehicle *vehicle);
    TimeWindowChecker() = default;
    bool check();

    ~TimeWindowChecker() {}
};

#endif