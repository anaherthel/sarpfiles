#include "RouteTest.h"

RouteTest::RouteTest(Data* data, Vehicle vhc):
data(data),
route(new Route(data, vhc)){}

RouteTest::RouteTest(Data* data, const Route& other):
data(data),
route(new Route(other)){}

bool RouteTest::generalTest(){

    //Heuristic check
    // 32 3 10 26 19 2 18 7 1 23 17 6 15 31 22 32 

    this->route->insertRequest(9, 1);
    this->route->insertRequest(17, 2);
    this->route->insertRequest(33, 3);
    this->route->insertRequest(8, 4);
    this->route->insertRequest(20, 5);
    this->route->insertRequest(1, 6);
    this->route->insertRequest(41, 7);
    this->route->insertRequest(7, 8);
    this->route->insertRequest(31, 9);
    this->route->insertRequest(44, 10);
    this->route->insertRequest(32, 11);
    this->route->insertRequest(2, 12);
    this->route->insertRequest(25, 13);
    this->route->insertRequest(5, 14);
    this->route->insertRequest(13, 15);
    this->route->insertRequest(29, 16);
    this->route->insertRequest(26, 17);
    this->route->insertRequest(16, 18);
    this->route->insertRequest(4, 19);
    this->route->insertRequest(28, 20);
    this->route->insertRequest(37, 21);
    this->route->insertRequest(19, 22);
    this->route->insertRequest(23, 23);
    this->route->insertRequest(40, 24);
    this->route->insertRequest(47, 25);
    this->route->insertRequest(43, 26);
    

    this->route->update();

    cout << "INIT\n";
    this->route->printPath();

    cout << endl;


    return true;
}