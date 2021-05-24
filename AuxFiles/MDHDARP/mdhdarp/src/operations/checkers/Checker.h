#ifndef CHECKER_H
#define CHECKER_H

#include "../../model/Data.h"
#include "../../model/Request.h"
#include "../../model/Sequence.h"
#include "../../model/Vehicle.h"

class Checker {
   protected:
    vector<int> *path;
    Data *data;
    Vehicle *vehicle;

   public:
    Checker(vector<int> *path, Data *data) : path(path),
                                             data(data) {}
    Checker(vector<int> *path, Data *data, Vehicle *vehicle) : path(path),
                                                               data(data),
                                                               vehicle(vehicle) {}
    Checker() {}

    virtual bool check() = 0;
};

#endif