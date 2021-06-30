#include "Route.h"

#include <iterator>
#include <climits>

Route::Route(){
    nodes_.push_back(0);
    nodes_.push_back(0);
    cost_ = 0;
    length_ = 0;
    load_ = 0;
}

// ostream & operator<<(ostream &os, const Route &route){
//     os << " Route visiting nodes ";
//     copy(route.begin(), route.end(), ostream_iterator<int>(cout, " "));
//     os << "(cost=" << route.cost() << ", length=" << route.length()
//        << ", load=" << route.load() << ")";
//     return os;
// }

// // evaluate the cost of cheapest insertion of node in this route
// // returns a <position, cost> pair
// pair<int, int> Route::cheapestInsertion(const CVRPInstance &inst, int node) const {
//     int cheapest = INT_MAX;
//     int bestpos = -1;
//     // only check insertions if customer demand fits in the truck
//     if (load_ + inst.q(node) <= inst.Q()) {
//         // try inserting at every position
//         for (unsigned int p=1; p < nodes_.size(); p++) {
//             // cost of inserting node before node currently at position p
//             int delta = inst.d(nodes_[p-1], node) + inst.d(node, nodes_[p])
//                 - inst.d(nodes_[p-1], nodes_[p]);
//             if (delta + length_ + inst.s() > inst.L()) {
//                 // case where max. length is exceeded: skip this insertion
//                 continue;
//             } else if (delta < cheapest) {
//                 cheapest = delta;
//                 bestpos = p;
//             }
//         }
//     }
//     return make_pair(bestpos, cheapest);
// }

// void Route::insert(const CVRPInstance &inst, int node, int position) {
//     int delta = inst.d(nodes_[position-1], node)
//         + inst.d(node, nodes_[position])
//         - inst.d(nodes_[position-1], nodes_[position]); 
//     cost_ += delta;
//     length_ += delta + inst.s();
//     nodes_.insert(nodes_.begin() + position, node);
//     load_ += inst.q(node);
// }

// void Route::erase(const CVRPInstance &inst, int position) {
//     int node = nodes_[position];
//     int delta = inst.d(nodes_[position - 1], nodes_[position + 1])
//         - inst.d(nodes_[position - 1], nodes_[position])
//         - inst.d(nodes_[position], nodes_[position + 1]);
//     cost_ -= delta;
//     length_ -= delta + inst.s();
//     nodes_.erase(nodes_.begin() + position);
//     load_ -= inst.q(node);
// }
