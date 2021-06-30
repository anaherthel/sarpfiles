#ifndef _ROUTE_H_
#define _ROUTE_H_

#include <vector>
#include <algorithm>

#include "CVRPInstance.h"

class Route {
protected:
    // the route
    vector<int> nodes_;
    // the cost of this route
    int cost_;
    // route length
    int length_;
    // vehicle load
    int load_;

    int first2improvement(const CVRPInstance& inst);

public:
    // default constructor: route with only starting and ending depot visits
    Route();

    // access route elements
    int cost() const { return cost_; };
    int length() const { return length_; };
    int load() const { return load_; };
    // access nodes
    vector<int>::iterator begin() { return nodes_.begin(); }
    vector<int>::const_iterator begin() const { return nodes_.begin(); }
    vector<int>::iterator end() { return nodes_.end(); }
    vector<int>::const_iterator end() const { return nodes_.end(); }
    int operator[] (int i) { return nodes_[i]; } /*using the .begin() iterator method might be better!*/

    // evaluate the cost of cheapest insertion of node in this route
    // return a <position, cost> pair
    // If no insertion is feasible, the cost is INT_MAX
    pair<int, int> cheapestInsertion(const CVRPInstance& inst, int node) const;

    // pre-condition: the insertion is feasible
    void insert(const CVRPInstance& inst, int node, int position);

    void erase(const CVRPInstance& inst, int position);

    // improve route with 2-opt
    // returns the number of improvements performed
    int two_opt(const CVRPInstance& inst);

    int local_search(const CVRPInstance& inst);

    int bestTwoOpt(const CVRPInstance& inst);

    // relocate K customers
    int relocateK(const CVRPInstance& inst, int k);

    //swap any 2 customers
    int swap(const CVRPInstance& inst);

    int VND(const CVRPInstance& inst);
};

ostream & operator<<(ostream &os, const Route &route);

#endif
