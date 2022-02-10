#ifndef ARCSCHECKER_H
#define ARCSCHECKER_H

#include "Checker.h"

class ArcsChecker : public Checker {
   public:
    ArcsChecker(vector<int> *path, Data *data, Vehicle *vehicle);
    ArcsChecker() = default;
    bool check();

    ~ArcsChecker() {}
};

#endif