#ifndef ROUTETEST_H
#define ROUTETEST_H

#include "../Route.h"
#include "../Sequence.h"
#include <iostream>

using namespace std;

class RouteTest{

protected:
    Data *data;
    Route* route;

public:

    RouteTest(Data *data, Vehicle vhc);
    RouteTest(Data* data, const Route& other);
    
    virtual ~RouteTest() = default;

    bool generalTest();

};

#endif