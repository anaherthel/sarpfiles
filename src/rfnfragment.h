#ifndef RFNFRAGMENT_H_INCLUDED
#define RFNFRAGMENT_H_INCLUDED

#include <vector>
#include <set>
#include <map>
#include <tuple>
#include <string>
#include <algorithm>

using namespace std;

// -------------------------------------------------------
// Node roles in the logical (synthesized) PU/DL layer
// -------------------------------------------------------
enum RfnRole { RFN_PU, RFN_DL };

// A logical location in the RFN enumeration layer.
// Passengers are synthesized into two RfnLoc entries (PU + DL)
// even though they map to a single merged physIndex in nodeVec.
struct RfnLoc {
    int      physIndex;   // index into nodeVec / distMatrix
    RfnRole  role;        // PU or DL
    int      customerId;  // passenger index [0,n) or parcel index [0,m); -1 = depot
    bool     isPassenger; // true = passenger, false = parcel
    bool     isDepot;

    bool isPickup()   const { return role == RFN_PU; }
    bool isDelivery() const { return role == RFN_DL; }
};

// -------------------------------------------------------
// Abstract network node = (physIndex, parcelLoad, paxOnboard)
// At Type-0 and Type-1 fragment boundaries paxOnboard == false.
// -------------------------------------------------------
struct RfnNode {
    int          physIndex;        // physical location in nodeVec
    set<int>     parcelLoad;       // parcel ids on board (parcel PU physIndex, n..n+m-1)
    bool         paxOnboard;       // true only for Type-2 boundaries (disabled in v1)
    bool         isDeliveryBound;  // true = delivery boundary; false = pickup boundary
    bool         isOriginDepot;
    bool         isDestDepot;

    bool operator==(const RfnNode &o) const {
        return physIndex == o.physIndex &&
               parcelLoad == o.parcelLoad &&
               paxOnboard == o.paxOnboard &&
               isDeliveryBound == o.isDeliveryBound;
    }
    bool operator<(const RfnNode &o) const {
        if (physIndex != o.physIndex)           return physIndex < o.physIndex;
        if (parcelLoad != o.parcelLoad)         return parcelLoad < o.parcelLoad;
        if (paxOnboard != o.paxOnboard)         return paxOnboard < o.paxOnboard;
        return isDeliveryBound < o.isDeliveryBound;
    }

    // Canonical string key — must include role to distinguish PU vs DL boundary
    // at the same physical location (e.g., merged passenger node).
    string key() const {
        string k = to_string(physIndex)
                 + "|" + to_string((int)paxOnboard)
                 + "|" + to_string((int)isDeliveryBound)
                 + "|";
        for (int p : parcelLoad) k += to_string(p) + ",";
        return k;
    }
};

// -------------------------------------------------------
// Fragment types
// -------------------------------------------------------
enum FragType { FRAG_TYPE0, FRAG_TYPE1 };

// -------------------------------------------------------
// Restricted Fragment
// -------------------------------------------------------
struct RfnFragment {
    int                  id;
    FragType             type;

    RfnNode              startNode;   // Start(f): pickup boundary node
    RfnNode              endNode;     // End(f):   delivery boundary node

    vector<RfnLoc>       seq;         // full ordered sequence of locations visited
    vector<int>          locs;        // just the physIndex values in seq (for SR ineqs)

    double               cost;        // total travel cost inside fragment (costkm * dist)
    double               profitContrib; // parcel profits served + pax profit if Type1
    double               T_f;         // min traversal time (no waiting)
    double               E_f;         // earliest feasible start time at startNode
    double               L_f;         // latest feasible start time at startNode

    set<int>             paxServed;     // passenger ids fully served inside (physIndex)
    set<int>             parcelsServed; // parcel ids (PU physIndex) fully served inside

    // Late-schedule coefficients τ^late_{f,k}(t) = min{t + c1[k], c2[k]}
    vector<double>       c1;  // per location in seq
    vector<double>       c2;

    bool                 minimal;
    bool                 dominated;
};

// -------------------------------------------------------
// Arc: delivery boundary → pickup boundary (or depot)
// Load is preserved exactly across an arc.
// -------------------------------------------------------
struct RfnArc {
    int      id;
    RfnNode  from;      // delivery boundary node (or origin depot)
    RfnNode  to;        // pickup boundary node (or dest depot)
    double   cost;      // costkm * dist[from.phys][to.phys]
    int      vehicle;   // depot arcs are vehicle-specific; -1 = any vehicle
    bool     isDepotOut; // leaves an origin depot
    bool     isDepotIn;  // enters a dest depot
};

// -------------------------------------------------------
// Fragment Network
// -------------------------------------------------------
struct FragmentNetwork {
    vector<RfnFragment>   frags;
    vector<RfnArc>        arcs;

    // Incidence indices (all indexed by RfnNode::key())
    map<string, vector<int>> fragStartAtNode;  // F^-_n : frags whose startNode matches
    map<string, vector<int>> fragEndAtNode;    // F^+_n : frags whose endNode matches
    map<string, vector<int>> arcFromNode;      // A^-_n : arcs leaving from a node
    map<string, vector<int>> arcToNode;        // A^+_n : arcs arriving at a node

    // Cover indices
    vector<vector<int>>   fragsCoveringPax;    // [pax physIndex] -> list of frag ids
    vector<vector<int>>   fragsCoveringParcel; // [parcel PU physIndex - n] -> list of frag ids

    // Depot arc indices
    vector<int>           depotOutArcIds;  // arcs leaving origin depots (for vehicle limit)

    // Per-fragment arc adjacency (for FA/AF valid inequalities)
    vector<vector<int>>   arcsIntoFrag;   // [frag id] -> arc ids arriving (from's arc)
    vector<vector<int>>   arcsOutOfFrag;  // [frag id] -> arc ids departing (to's arc)
};

#endif
