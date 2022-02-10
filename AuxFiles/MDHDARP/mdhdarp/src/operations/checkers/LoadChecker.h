#ifndef LOADCHECKER_H
#define LOADCHECKER_H

#include "Checker.h"

class LoadChecker : public Checker {
   private:
    Sequence sequence;

   public:
    LoadChecker(vector<int> *path, Data *data, Vehicle *vehicle);
    LoadChecker() = default;
    bool check();

    ~LoadChecker() {}
};

#endif