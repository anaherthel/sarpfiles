#include "modelrfn.h"
#include <cstdlib>
#include <stdio.h>

// ============================================================
// Helpers
// ============================================================

// Travel time from end-of-node i to start-of-node j
static inline double travelTime(int i, int j, double **mdist, double vmed) {
    return mdist[i][j] / vmed;
}

// ============================================================
// Build logical PU/DL location lists
//
// For each node in nodeVec we create one or two RfnLoc entries:
//   - Passengers (i < n): synthesized PU (physIndex=i, role=PU) and
//     DL (physIndex=i, role=DL). Same physIndex; the dist matrix entry
//     mdist[i][j] already uses xf/yf of i → xs/ys of j (end→start),
//     so for the internal passenger leg we use delta[i] as its traversal
//     time and profit[i] as its contribution (already net of internal cost).
//   - Parcel PU (n <= i < n+m): physIndex=i, role=PU
//   - Parcel DL (n+m <= i < n+2m): physIndex=i, role=DL
//   - Depots (n+2m <= i): handled separately in arc enumeration
// ============================================================
void buildRfnLocs(instanceStat *inst, vector<nodeStat> &nodeVec,
                  vector<RfnLoc> &puLocs, vector<RfnLoc> &dlLocs) {
    puLocs.clear();
    dlLocs.clear();

    // Passengers: synthesize PU and DL from the merged node
    for (int i = 0; i < inst->n; i++) {
        RfnLoc pu;
        pu.physIndex   = i;
        pu.role        = RFN_PU;
        pu.customerId  = i;
        pu.isPassenger = true;
        pu.isDepot     = false;
        puLocs.push_back(pu);

        RfnLoc dl;
        dl.physIndex   = i;
        dl.role        = RFN_DL;
        dl.customerId  = i;
        dl.isPassenger = true;
        dl.isDepot     = false;
        dlLocs.push_back(dl);
    }

    // Parcel PUs
    for (int i = inst->n; i < inst->n + inst->m; i++) {
        RfnLoc pu;
        pu.physIndex   = i;
        pu.role        = RFN_PU;
        pu.customerId  = i;
        pu.isPassenger = false;
        pu.isDepot     = false;
        puLocs.push_back(pu);
    }

    // Parcel DLs
    for (int i = inst->n + inst->m; i < inst->n + 2*inst->m; i++) {
        RfnLoc dl;
        dl.physIndex   = i;
        dl.role        = RFN_DL;
        dl.customerId  = i;
        dl.isPassenger = false;
        dl.isDepot     = false;
        dlLocs.push_back(dl);
    }
}

// ============================================================
// Compute E_f, L_f, T_f and late-schedule coefficients c1/c2.
//
// IMPORTANT: For merged passengers, the seq contains ONLY the PU loc
// (role==RFN_PU). The DL is implicit — delta[pax] already accounts for
// both pickup service, in-vehicle ride, and dropoff service.
// Passenger DL locs must NEVER appear in seq.
//
// For parcels: separate PU and DL physIndices; service time = delta[i].
// ============================================================
static bool computeFragmentTiming(instanceStat *inst,
                                   vector<nodeStat> &nodeVec,
                                   double **mdist,
                                   const vector<RfnLoc> &seq,
                                   double /*unused*/,
                                   double &T_f, double &E_f, double &L_f,
                                   vector<double> &c1, vector<double> &c2) {
    int K = seq.size();
    if (K == 0) return false;

    // Service time at each seq position
    auto svcAt = [&](int k) -> double {
        return nodeVec[seq[k].physIndex].delta;
        // For merged passenger PU: delta covers full merged service.
        // For parcel PU/DL:        delta covers loading/unloading service.
    };

    // --- Forward pass ---
    vector<double> fwdTime(K, 0);
    fwdTime[0] = nodeVec[seq[0].physIndex].e;
    for (int k = 1; k < K; k++) {
        int prev = seq[k-1].physIndex;
        int cur  = seq[k].physIndex;
        double travel = travelTime(prev, cur, mdist, inst->vmed);
        fwdTime[k] = max(fwdTime[k-1] + svcAt(k-1) + travel,
                         nodeVec[cur].e);
        if (fwdTime[k] > nodeVec[cur].l + 1e-9) return false;
    }
    E_f = fwdTime[0];

    // --- Backward pass ---
    vector<double> bwdTime(K, 0);
    bwdTime[K-1] = nodeVec[seq[K-1].physIndex].l;
    for (int k = K-2; k >= 0; k--) {
        int cur  = seq[k].physIndex;
        int next = seq[k+1].physIndex;
        double travel = travelTime(cur, next, mdist, inst->vmed);
        bwdTime[k] = min(bwdTime[k+1] - svcAt(k) - travel,
                         nodeVec[cur].l);
        if (bwdTime[k] < nodeVec[cur].e - 1e-9) return false;
    }
    L_f = bwdTime[0];

    if (E_f > L_f + 1e-9) return false;

    // --- T_f ---
    T_f = 0.0;
    for (int k = 0; k < K; k++) {
        T_f += svcAt(k);
        if (k < K-1) {
            T_f += travelTime(seq[k].physIndex, seq[k+1].physIndex,
                              mdist, inst->vmed);
        }
    }

    // --- Maximum route time check ---
    // Route duration (depot departure to last service completion) is
    // non-increasing in the start time, so the minimum standalone duration
    // of the fragment is obtained by starting at L_f and serving each
    // location as early as the windows allow. A fragment whose minimum
    // duration already exceeds maxTime can never be part of a feasible route.
    {
        double t = L_f;
        for (int k = 1; k < K; k++) {
            double travel = travelTime(seq[k-1].physIndex, seq[k].physIndex,
                                       mdist, inst->vmed);
            t = max(t + svcAt(k-1) + travel, nodeVec[seq[k].physIndex].e);
        }
        double minDur = (t + svcAt(K-1)) - L_f;
        if (minDur > inst->maxTime + 1e-9) return false;
    }

    // --- Late schedule coefficients ---
    // τ^late_{f,k}(t) = min{t + c1[k], c2[k]}
    // c1[k] = T_f(1, k) = cumulative travel+service from position 1 to k
    // c2[k] = l[seq[k]] (time window upper bound clips the schedule)
    c1.resize(K);
    c2.resize(K);
    double cumT = 0.0;
    for (int k = 0; k < K; k++) {
        c1[k] = cumT;
        c2[k] = nodeVec[seq[k].physIndex].l;
        if (k < K-1) {
            double svc;
            if (seq[k].isPassenger && seq[k].role == RFN_PU) {
                svc = nodeVec[seq[k].physIndex].delta;
            } else if (seq[k].isPassenger && seq[k].role == RFN_DL) {
                svc = 0.0;
            } else {
                svc = nodeVec[seq[k].physIndex].delta;
            }
            int a = seq[k].physIndex, b_idx = seq[k+1].physIndex;
            double tr = (a == b_idx) ? 0.0 : travelTime(a, b_idx, mdist, inst->vmed);
            cumT += svc + tr;
        }
    }

    return true;
}

// ============================================================
// Try to add a fragment candidate to the network.
// Runs timing checks; if feasible, creates an RfnFragment and appends it.
// ============================================================
static void tryAddFragment(instanceStat *inst,
                            vector<nodeStat> &nodeVec,
                            double **mdist,
                            FragmentNetwork &fn,
                            const vector<RfnLoc> &seq,
                            FragType ftype,
                            const RfnNode &startNode,
                            const RfnNode &endNode,
                            double fragCost,
                            double fragProfit,
                            const set<int> &paxServed,
                            const set<int> &parcelsServed) {
    double T_f, E_f, L_f;
    vector<double> c1, c2;

    if (!computeFragmentTiming(inst, nodeVec, mdist, seq, 0.0,
                               T_f, E_f, L_f, c1, c2))
        return;

    RfnFragment f;
    f.id            = fn.frags.size();
    f.type          = ftype;
    f.startNode     = startNode;
    f.endNode       = endNode;
    f.seq           = seq;
    f.cost          = fragCost;
    f.profitContrib = fragProfit;
    f.T_f           = T_f;
    f.E_f           = E_f;
    f.L_f           = L_f;
    f.c1            = c1;
    f.c2            = c2;
    f.paxServed     = paxServed;
    f.parcelsServed = parcelsServed;
    f.minimal       = false;
    f.dominated     = false;

    for (const auto &loc : seq)
        f.locs.push_back(loc.physIndex);

    fn.frags.push_back(f);
}

// ============================================================
// Compute travel cost along a sequence of RfnLoc entries
// (inter-node travel only; passenger internal leg cost is already
// netted into totalCustomProfit, so we only count arcs between distinct
// physical nodes here).
// ============================================================
static double seqTravelCost(instanceStat *inst, double **mdist,
                             const vector<RfnLoc> &seq) {
    double cost = 0.0;
    for (int k = 0; k + 1 < (int)seq.size(); k++) {
        int a = seq[k].physIndex;
        int b = seq[k+1].physIndex;
        if (a != b)
            cost += inst->costkm * mdist[a][b];
    }
    return cost;
}

// ============================================================
// Enumerate Type-0 (parcel-only) fragments.
//
// A Type-0 fragment contains no passenger.
// It picks up a set of parcels (size 1..Q_par) then delivers a subset,
// with arbitrary ordering of PUs and DLs (all PUs precede all DLs).
//
// We enumerate all valid (PU-permutation, DL-subset-permutation) pairs
// and apply the k² trimming: for each base path of k pickups + k deliveries
// we also generate the k² sub-fragments by varying the initial load
// (r pickups already on board) and final load (s deliveries deferred).
// ============================================================
static void enumerateType0(instanceStat *inst,
                            vector<nodeStat> &nodeVec,
                            double **mdist,
                            probStat *problem,
                            FragmentNetwork &fn) {

    int n = inst->n, m = inst->m;
    int Q = (problem->p2 >= 1) ? 1 : 1; // parcel capacity: 1 for initial version; increase for multi-parcel

    // Parcel PU indices: [n, n+m)
    // Parcel DL indices: [n+m, n+2m)  — parcel j PU is at n+j, DL at n+m+j (0-indexed j in [0,m))

    // For small m and Q, enumerate combinations of parcel PUs of size k=1..Q
    // then all orderings of PUs, then all subsets/orderings of DLs.
    // Pruned aggressively by TW checks.

    vector<int> parcelIds; // 0-indexed parcel ids
    for (int j = 0; j < m; j++) parcelIds.push_back(j);

    // Generate all subsets of parcels of size k (1 <= k <= Q)
    // For each subset, generate all PU orderings and DL orderings.
    // We use iterative enumeration with bitmask for small m.

    for (int k = 1; k <= min(Q, m); k++) {
        // All subsets of size k from parcelIds
        vector<int> indices(k);
        for (int i = 0; i < k; i++) indices[i] = i;

        // Iterate over combinations using next_combination pattern
        bool done = false;
        while (!done) {
            vector<int> subset;
            for (int i : indices) subset.push_back(parcelIds[i]);

            // All permutations of PU ordering
            sort(subset.begin(), subset.end());
            do {
                // All permutations of DL ordering (must be subset of PUs)
                vector<int> dlPerm = subset; // same set, different order
                sort(dlPerm.begin(), dlPerm.end());
                do {
                    // Build base sequence: [PU_j1, PU_j2, ..., DL_j'1, DL_j'2, ...]
                    vector<RfnLoc> baseSeq;
                    for (int j : subset) {
                        RfnLoc loc;
                        loc.physIndex   = n + j;
                        loc.role        = RFN_PU;
                        loc.customerId  = n + j;
                        loc.isPassenger = false;
                        loc.isDepot     = false;
                        baseSeq.push_back(loc);
                    }
                    for (int j : dlPerm) {
                        RfnLoc loc;
                        loc.physIndex   = n + m + j;
                        loc.role        = RFN_DL;
                        loc.customerId  = n + m + j;
                        loc.isPassenger = false;
                        loc.isDepot     = false;
                        baseSeq.push_back(loc);
                    }

                    // Apply k² construction: remove r PUs from start (initial load)
                    // and s DLs from end (final load)
                    for (int r = 0; r < k; r++) {
                        for (int s = 0; s < k; s++) {
                            // Fragment visits PUs [r..k-1] and DLs [0..k-1-s]
                            // Initial load: parcels subset[0..r-1]
                            // Final load:   parcels dlPerm[k-s..k-1]

                            // Must have at least one node
                            int puCount = k - r;
                            int dlCount = k - s;
                            if (puCount <= 0 || dlCount <= 0) continue;

                            // Build the trimmed sequence
                            vector<RfnLoc> seq;
                            for (int i = r; i < k; i++)       seq.push_back(baseSeq[i]);         // PUs
                            for (int i = 0; i < k - s; i++)   seq.push_back(baseSeq[k + i]);     // DLs

                            // Build start / end nodes
                            set<int> initLoad, finalLoad;
                            for (int i = 0; i < r; i++)       initLoad.insert(n + subset[i]);
                            for (int i = k - s; i < k; i++)   finalLoad.insert(n + m + dlPerm[i]);
                            // Convert finalLoad from DL physIndex to PU physIndex for parcelLoad tracking
                            set<int> finalLoadPU;
                            for (int dlIdx : finalLoad)        finalLoadPU.insert(dlIdx - m);

                            RfnNode startN, endN;
                            startN.physIndex       = seq.front().physIndex;
                            startN.parcelLoad      = initLoad;
                            startN.paxOnboard      = false;
                            startN.isDeliveryBound = false; // pickup boundary
                            startN.isOriginDepot   = false;
                            startN.isDestDepot     = false;

                            endN.physIndex         = seq.back().physIndex;
                            endN.parcelLoad        = finalLoadPU;
                            endN.paxOnboard        = false;
                            endN.isDeliveryBound   = true;  // delivery boundary
                            endN.isOriginDepot     = false;
                            endN.isDestDepot       = false;

                            // Parcels fully served = those picked up and delivered inside
                            set<int> served;
                            for (int i = r; i < k - s; i++) served.insert(n + subset[i]);

                            double cost   = seqTravelCost(inst, mdist, seq);
                            double profit = 0.0;
                            for (int pId : served)
                                profit += nodeVec[pId].profit;

                            tryAddFragment(inst, nodeVec, mdist, fn,
                                           seq, FRAG_TYPE0, startN, endN,
                                           cost, profit, {}, served);
                        }
                    }
                } while (next_permutation(dlPerm.begin(), dlPerm.end()));
            } while (next_permutation(subset.begin(), subset.end()));

            // Advance to next combination
            int i = k - 1;
            while (i >= 0 && indices[i] == m - k + i) i--;
            if (i < 0) { done = true; break; }
            indices[i]++;
            for (int j = i + 1; j < k; j++) indices[j] = indices[j-1] + 1;
        }
    }
}

// ============================================================
// Enumerate Type-1 (passenger-contained) fragments.
//
// A Type-1 fragment contains exactly one passenger (merged node i < n).
// The passenger is treated as an atomic "PU→DL" unit occupying delta[i]
// time. Before the passenger PU, the vehicle may carry parcels from a
// previous fragment (initial load). During / after the passenger atom,
// parcel PUs and DLs may occur — up to Q_par total parcels on board at
// any point.
//
// We enumerate all placements of the passenger atom within a parcel
// sub-sequence:  [parcelPUs...] [pax] [parcelDLs...]  — where the
// passenger atom is a single synthetic stop that contributes delta[i]
// to traversal time and profit[i] to the fragment profit.
// ============================================================
static void enumerateType1(instanceStat *inst,
                            vector<nodeStat> &nodeVec,
                            double **mdist,
                            probStat *problem,
                            FragmentNetwork &fn) {

    int n = inst->n, m = inst->m;
    int Q = (problem->p2 >= 1) ? 1 : 1; // parcel capacity: 1 for initial version

    for (int pax = 0; pax < n; pax++) {
        // For each passenger, enumerate fragments that contain this passenger.
        // The structure is: optParcelPUs | paxAtom | optParcelDLs
        // with k² trimming applied to the parcel part.
        // For simplicity in this initial version: enumerate parcel combos
        // of size 0..Q_par around the passenger.

        for (int kPar = 0; kPar <= min(Q, m); kPar++) {
            if (kPar == 0) {
                // Passenger alone: seq = [paxPU] only.
                // The DL is implicit (delta[pax] covers both stops + ride).
                // The fragment starts AND ends at physIndex=pax (the merged node
                // serves as both the pickup and delivery boundary).
                vector<RfnLoc> seq;
                RfnLoc puLoc;
                puLoc.physIndex = pax; puLoc.role = RFN_PU; puLoc.customerId = pax;
                puLoc.isPassenger = true; puLoc.isDepot = false;
                seq.push_back(puLoc);

                RfnNode startN, endN;
                startN.physIndex       = pax; startN.parcelLoad = {}; startN.paxOnboard = false;
                startN.isDeliveryBound = false; // pickup boundary
                startN.isOriginDepot   = false; startN.isDestDepot = false;
                // endNode at same physIndex; delivery boundary (dropoff at pax's xf/yf)
                endN.physIndex         = pax; endN.parcelLoad = {}; endN.paxOnboard = false;
                endN.isDeliveryBound   = true;  // delivery boundary
                endN.isOriginDepot     = false; endN.isDestDepot = false;

                double cost   = 0.0; // no inter-node travel in seq
                double profit = nodeVec[pax].profit;

                tryAddFragment(inst, nodeVec, mdist, fn,
                               seq, FRAG_TYPE1, startN, endN,
                               cost, profit, {pax}, {});
                continue;
            }

            // Enumerate parcel subsets of size kPar
            vector<int> indices(kPar);
            for (int i = 0; i < kPar; i++) indices[i] = i;
            bool done = false;
            while (!done) {
                vector<int> parcelSubset;
                for (int i : indices) parcelSubset.push_back(i);

                // Enumerate all PU orderings and DL orderings for this subset
                vector<int> puPerm = parcelSubset;
                sort(puPerm.begin(), puPerm.end());
                do {
                    vector<int> dlPerm = parcelSubset;
                    sort(dlPerm.begin(), dlPerm.end());
                    do {
                        // Base sequence: [parcelPUs...] [paxPU] [parcelDLs...]
                        // paxDL is NOT in seq — delta[pax] already handles the full service.
                        // baseSeq layout: [0..kPar-1]=parcelPUs, [kPar]=paxPU, [kPar+1..kPar+kPar]=parcelDLs
                        vector<RfnLoc> baseSeq;
                        for (int j : puPerm) {
                            RfnLoc l; l.physIndex = n+j; l.role = RFN_PU;
                            l.customerId = n+j; l.isPassenger = false; l.isDepot = false;
                            baseSeq.push_back(l);
                        }
                        {
                            RfnLoc puLoc;
                            puLoc.physIndex = pax; puLoc.role = RFN_PU; puLoc.customerId = pax;
                            puLoc.isPassenger = true; puLoc.isDepot = false;
                            baseSeq.push_back(puLoc); // index kPar
                        }
                        for (int j : dlPerm) {
                            RfnLoc l; l.physIndex = n+m+j; l.role = RFN_DL;
                            l.customerId = n+m+j; l.isPassenger = false; l.isDepot = false;
                            baseSeq.push_back(l);
                        }
                        // baseSeq size = kPar + 1 + kPar = 2*kPar + 1

                        // k² trimming on the parcel part
                        for (int r = 0; r < kPar; r++) {
                            for (int s = 0; s < kPar; s++) {
                                int dlCount = kPar - s;
                                vector<RfnLoc> seq;
                                for (int i = r; i < kPar; i++) seq.push_back(baseSeq[i]);      // parcelPUs[r..kPar-1]
                                seq.push_back(baseSeq[kPar]);                                    // paxPU (no paxDL)
                                for (int i = 0; i < dlCount; i++) seq.push_back(baseSeq[kPar + 1 + i]); // parcelDLs[0..dlCount-1]

                                set<int> initLoad;
                                for (int i = 0; i < r; i++) initLoad.insert(n + puPerm[i]);

                                set<int> finalLoadPU; // parcel PU indices still on board
                                for (int i = dlCount; i < kPar; i++) finalLoadPU.insert(n + puPerm[i]);

                                // startNode: first element of seq (pickup boundary)
                                RfnNode startN;
                                startN.physIndex       = seq.front().physIndex;
                                startN.parcelLoad      = initLoad;
                                startN.paxOnboard      = false;
                                startN.isDeliveryBound = false;
                                startN.isOriginDepot   = false; startN.isDestDepot = false;

                                // endNode: delivery boundary
                                RfnNode endN;
                                endN.paxOnboard        = false;
                                endN.isDeliveryBound   = true;
                                endN.isOriginDepot     = false; endN.isDestDepot = false;
                                endN.parcelLoad        = finalLoadPU;
                                if (dlCount > 0) {
                                    endN.physIndex = seq.back().physIndex; // last parcel DL
                                } else {
                                    endN.physIndex = pax; // pax's dropoff (DL boundary at pax)
                                }

                                // Parcel PU indices served inside this fragment:
                                // positions r..r+dlCount-1 of puPerm, but capped at kPar-1
                                set<int> parcelsServed;
                                for (int i = r; i < r + dlCount && i < kPar; i++)
                                    parcelsServed.insert(n + puPerm[i]);

                                double cost   = seqTravelCost(inst, mdist, seq);
                                double profit = nodeVec[pax].profit;
                                for (int pId : parcelsServed) profit += nodeVec[pId].profit;

                                tryAddFragment(inst, nodeVec, mdist, fn,
                                               seq, FRAG_TYPE1, startN, endN,
                                               cost, profit, {pax}, parcelsServed);
                            }
                        }
                    } while (next_permutation(dlPerm.begin(), dlPerm.end()));
                } while (next_permutation(puPerm.begin(), puPerm.end()));

                // Advance combination
                int i = kPar - 1;
                while (i >= 0 && indices[i] == m - kPar + i) i--;
                if (i < 0) { done = true; break; }
                indices[i]++;
                for (int j = i + 1; j < kPar; j++) indices[j] = indices[j-1] + 1;
            }
        }
    }
}

// ============================================================
// Main enumeration entry point
// ============================================================
void enumerateFragments(instanceStat *inst, vector<nodeStat> &nodeVec,
                        double **mdist, probStat *problem,
                        FragmentNetwork &fn) {
    fn.frags.clear();
    enumerateType0(inst, nodeVec, mdist, problem, fn);
    enumerateType1(inst, nodeVec, mdist, problem, fn);

    cout << "RFN: enumerated " << fn.frags.size() << " fragments ("
         << inst->n << " passengers, " << inst->m << " parcels)" << endl;
    // Debug: show paxServed for first 10 fragments
    for (int fi = 0; fi < min((int)fn.frags.size(), 10); fi++) {
        cout << "  frag " << fi << " type=" << fn.frags[fi].type
             << " paxServed={";
        for (int p : fn.frags[fi].paxServed) cout << p << ",";
        cout << "} parcelsServed={";
        for (int p : fn.frags[fi].parcelsServed) cout << p << ",";
        cout << "} seq=[";
        for (auto &l : fn.frags[fi].seq) cout << l.physIndex << "(" << (l.role==RFN_PU?"PU":"DL") << "),";
        cout << "]" << endl;
    }
}

// ============================================================
// Arc enumeration
// Arcs connect End(f) delivery-boundary nodes to Start(g) pickup-boundary
// nodes where the loads match exactly (same parcelLoad, same paxOnboard).
// Also adds depot-out arcs (origin depot → any empty-load pickup node)
// and depot-in arcs (any empty-load delivery node → dest depot).
// ============================================================
void enumerateArcs(instanceStat *inst, vector<nodeStat> &nodeVec,
                   double **mdist, FragmentNetwork &fn) {
    fn.arcs.clear();

    int fDepot = inst->n + 2*inst->m;       // first origin depot index
    int fDummy = inst->n + 2*inst->m + inst->K; // first dummy dest depot

    // Collect all distinct start/end boundary nodes
    // Then for each pair where loads match, compute arc cost and add.

    auto addArc = [&](const RfnNode &from, const RfnNode &to,
                      double cost, int vehicle,
                      bool depotOut, bool depotIn) {
        RfnArc a;
        a.id          = fn.arcs.size();
        a.from        = from;
        a.to          = to;
        a.cost        = cost;
        a.vehicle     = vehicle;
        a.isDepotOut  = depotOut;
        a.isDepotIn   = depotIn;
        fn.arcs.push_back(a);
    };

    // 1. Fragment-to-fragment arcs: End(f) → Start(g) when loads match
    for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
        const RfnNode &endN = fn.frags[fi].endNode;
        for (int gi = 0; gi < (int)fn.frags.size(); gi++) {
            if (fi == gi) continue;
            const RfnNode &startN = fn.frags[gi].startNode;
            if (!(endN.parcelLoad == startN.parcelLoad)) continue;
            if (endN.paxOnboard != startN.paxOnboard) continue;
            // Passengers are self-contained in Types 0/1: paxOnboard is always false at boundaries
            int from_phys = endN.physIndex;
            int to_phys   = startN.physIndex;
            double cost = inst->costkm * mdist[from_phys][to_phys];
            addArc(endN, startN, cost, -1, false, false);
        }
    }

    // 2. Depot-out arcs: each origin depot k → each fragment startNode with empty load
    for (int k = 0; k < inst->K; k++) {
        int depotPhys = fDepot + k;
        RfnNode depotNode;
        depotNode.physIndex       = depotPhys;
        depotNode.parcelLoad      = {};
        depotNode.paxOnboard      = false;
        depotNode.isDeliveryBound = false; // depot-out arc goes TO a pickup node
        depotNode.isOriginDepot   = true;
        depotNode.isDestDepot     = false;

        for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
            const RfnNode &startN = fn.frags[fi].startNode;
            if (!startN.parcelLoad.empty() || startN.paxOnboard) continue;
            double cost = inst->costkm * mdist[depotPhys][startN.physIndex];
            addArc(depotNode, startN, cost, k, true, false);
            fn.depotOutArcIds.push_back(fn.arcs.size() - 1);
        }
    }

    // 3. Depot-in arcs: each fragment endNode with empty load → its vehicle's dummy depot
    for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
        const RfnNode &endN = fn.frags[fi].endNode;
        if (!endN.parcelLoad.empty() || endN.paxOnboard) continue;
        // Can go to any dest depot (open route — vehicle doesn't need to return to origin)
        for (int k = 0; k < inst->K; k++) {
            int dummyPhys = fDummy + k;
            RfnNode dummyNode;
            dummyNode.physIndex       = dummyPhys;
            dummyNode.parcelLoad      = {};
            dummyNode.paxOnboard      = false;
            dummyNode.isDeliveryBound = true; // depot-in arc comes FROM a delivery node
            dummyNode.isOriginDepot   = false;
            dummyNode.isDestDepot     = true;
            double cost = 0.0; // dummy depots incur no travel cost
            addArc(endN, dummyNode, cost, k, false, true);
        }
    }

    cout << "RFN: enumerated " << fn.arcs.size() << " arcs" << endl;
}

// ============================================================
// Build incidence maps
// ============================================================
void buildIncidenceMaps(instanceStat *inst, FragmentNetwork &fn) {
    fn.fragStartAtNode.clear();
    fn.fragEndAtNode.clear();
    fn.arcFromNode.clear();
    fn.arcToNode.clear();
    fn.fragsCoveringPax.assign(inst->n, {});
    fn.fragsCoveringParcel.assign(inst->m, {});
    fn.arcsIntoFrag.assign(fn.frags.size(), {});
    fn.arcsOutOfFrag.assign(fn.frags.size(), {});

    for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
        const RfnFragment &f = fn.frags[fi];
        fn.fragStartAtNode[f.startNode.key()].push_back(fi);
        fn.fragEndAtNode[f.endNode.key()].push_back(fi);

        for (int pax : f.paxServed)
            fn.fragsCoveringPax[pax].push_back(fi);
        for (int parPU : f.parcelsServed)
            fn.fragsCoveringParcel[parPU - inst->n].push_back(fi);
    }

    for (int ai = 0; ai < (int)fn.arcs.size(); ai++) {
        const RfnArc &a = fn.arcs[ai];
        fn.arcFromNode[a.from.key()].push_back(ai);
        fn.arcToNode[a.to.key()].push_back(ai);

        // For FA/AF VIs: which fragments end at arc's from-node / start at arc's to-node
        for (int fi : fn.fragEndAtNode[a.from.key()])
            fn.arcsOutOfFrag[fi].push_back(ai);
        for (int fi : fn.fragStartAtNode[a.to.key()])
            fn.arcsIntoFrag[fi].push_back(ai);
    }
}

// ============================================================
// Domination (Phase 4 — enabled after base MIP validated)
//
// Group fragments by (startNode, endNode). Within each group:
//  - Type0 loose parcels: keep Pareto-optimal on (cost, T_f)
//  - Type0 tight parcels / Type1: also check late-schedule conditions
// ============================================================
void dominateFragments(instanceStat *inst, FragmentNetwork &fn) {
    // Group by (start key, end key)
    map<pair<string,string>, vector<int>> groups;
    for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
        auto key = make_pair(fn.frags[fi].startNode.key(),
                             fn.frags[fi].endNode.key());
        groups[key].push_back(fi);
    }

    int dominated = 0;
    for (auto &kv : groups) {
        vector<int> &ids = kv.second;
        for (int i = 0; i < (int)ids.size(); i++) {
            if (fn.frags[ids[i]].dominated) continue;
            RfnFragment &f = fn.frags[ids[i]];
            for (int j = 0; j < (int)ids.size(); j++) {
                if (i == j) continue;
                if (fn.frags[ids[j]].dominated) continue;
                RfnFragment &g = fn.frags[ids[j]];

                // f dominates g if:
                // (1) same type, locs, start/end
                if (f.locs != g.locs) continue;
                // (2) cost and traversal no worse
                if (f.cost > g.cost + 1e-9) continue;
                if (f.T_f  > g.T_f  + 1e-9) continue;
                // (3) timing windows no worse
                if (f.E_f > g.E_f + 1e-9) continue;
                if (f.L_f < g.L_f - 1e-9) continue;
                // (4) initial load subset condition (Theorem 1, cond 15):
                //     Load(Start(f)) ⊆ Load(End(g))
                bool loadOk = true;
                for (int pl : f.startNode.parcelLoad) {
                    if (g.endNode.parcelLoad.find(pl) == g.endNode.parcelLoad.end()) {
                        loadOk = false; break;
                    }
                }
                if (!loadOk) continue;
                // f dominates g — mark g
                fn.frags[ids[j]].dominated = true;
                dominated++;
            }
        }
    }
    cout << "RFN: domination removed " << dominated << " fragments ("
         << fn.frags.size() - dominated << " remain)" << endl;
}

// ============================================================
// Valid inequality: Fragment-Arc (FA)
// x_f <= sum_{a arriving at Start(f)} y_a
// x_f <= sum_{a leaving from End(f)}  y_a
// ============================================================
void addRfnFAInequalities(IloModel &model, IloEnv &env,
                          IloBoolVarArray &xf, IloBoolVarArray &ya,
                          FragmentNetwork &fn) {
    char var[128];
    int added = 0;
    for (int fi = 0; fi < (int)fn.frags.size(); fi++) {
        if (fn.frags[fi].dominated) continue;

        // x_f <= sum of arcs INTO the fragment's start boundary
        if (!fn.arcsIntoFrag[fi].empty()) {
            IloExpr expr(env);
            expr += xf[fi];
            for (int ai : fn.arcsIntoFrag[fi]) expr -= ya[ai];
            sprintf(var, "FA_in_%d", fi);
            IloRange c(env, -IloInfinity, expr, 0); // x_f - sum y_a <= 0
            c.setName(var);
            model.add(c);
            added++;
        }

        // x_f <= sum of arcs OUT OF the fragment's end boundary
        if (!fn.arcsOutOfFrag[fi].empty()) {
            IloExpr expr(env);
            expr += xf[fi];
            for (int ai : fn.arcsOutOfFrag[fi]) expr -= ya[ai];
            sprintf(var, "FA_out_%d", fi);
            IloRange c(env, -IloInfinity, expr, 0); // x_f - sum y_a <= 0
            c.setName(var);
            model.add(c);
            added++;
        }
    }
    cout << "RFN: added " << added << " FA inequalities" << endl;
}

// ============================================================
// Valid inequality: Arc-Fragment (AF)
// y_a <= sum_{f whose Start matches arc's to-node} x_f   (non-depot arcs)
// ============================================================
void addRfnAFInequalities(IloModel &model, IloEnv &env,
                          IloBoolVarArray &xf, IloBoolVarArray &ya,
                          FragmentNetwork &fn) {
    char var[128];
    int added = 0;
    for (int ai = 0; ai < (int)fn.arcs.size(); ai++) {
        const RfnArc &a = fn.arcs[ai];
        if (a.isDepotOut || a.isDepotIn) continue;

        // y_a <= sum_{f: Start(f) matches a.to}  →  y_a - sum xf <= 0
        auto it = fn.fragStartAtNode.find(a.to.key());
        if (it == fn.fragStartAtNode.end() || it->second.empty()) continue;
        IloExpr expr(env);
        expr += ya[ai];
        for (int fi : it->second) {
            if (!fn.frags[fi].dominated) expr -= xf[fi];
        }
        sprintf(var, "AF_%d", ai);
        IloRange c(env, -IloInfinity, expr, 0); // y_a - sum x_f <= 0
        c.setName(var);
        model.add(c);
        added++;
    }
    cout << "RFN: added " << added << " AF inequalities" << endl;
}

// ============================================================
// MIP model
// ============================================================
void mipRfn(instanceStat *inst, vector<nodeStat> &nodeVec,
            double **mdist, probStat *problem,
            FragmentNetwork &fn, solStats *sStat) {

    char var[128];
    int threads = 1;

    int nFrags = fn.frags.size();
    int nArcs  = fn.arcs.size();

    IloEnv   env;
    IloModel model(env, "rfnSARP");

    // --- Variables ---
    IloBoolVarArray xf(env, nFrags);
    for (int fi = 0; fi < nFrags; fi++) {
        sprintf(var, "x(%d)", fi);
        xf[fi].setName(var);
        model.add(xf[fi]);
        if (fn.frags[fi].dominated)
            model.add(xf[fi] == 0); // fix dominated fragments to 0
    }

    IloBoolVarArray ya(env, nArcs);
    for (int ai = 0; ai < nArcs; ai++) {
        sprintf(var, "y(%d)", ai);
        ya[ai].setName(var);
        model.add(ya[ai]);
    }

    // --- Objective: maximize profit ---
    // Passenger profits are fixed (all passengers must be served) and are
    // captured in totalCustomProfit. Parcel profits and all travel costs
    // are variable.
    IloExpr obj(env);
    obj += inst->totalCustomProfit;

    for (int fi = 0; fi < nFrags; fi++) {
        if (fn.frags[fi].dominated) continue;
        obj += fn.frags[fi].profitContrib * xf[fi];   // parcel (and no-pax profit double-count guard below)
        obj -= fn.frags[fi].cost          * xf[fi];   // fragment-internal travel cost
    }
    for (int ai = 0; ai < nArcs; ai++) {
        obj -= fn.arcs[ai].cost * ya[ai];             // arc travel cost
    }
    // Guard: subtract passenger profits that were added via profitContrib in Type1 frags,
    // because they are already in totalCustomProfit.
    // We add them in profitContrib for correctness when passengers are OPTIONAL (future),
    // but for now passengers are mandatory, so remove the double-count.
    for (int fi = 0; fi < nFrags; fi++) {
        if (fn.frags[fi].dominated) continue;
        if (fn.frags[fi].type == FRAG_TYPE1) {
            for (int pax : fn.frags[fi].paxServed)
                obj -= nodeVec[pax].profit * xf[fi]; // remove pax profit (already in totalCustomProfit)
        }
    }
    model.add(IloMaximize(env, obj));

    // --- Constraint 1: Flow conservation at pickup boundary nodes ---
    // For each distinct pickup boundary node:
    //   sum_{a arriving at node} y_a  ==  sum_{f starting at node} x_f
    {
        set<string> visited;
        for (int fi = 0; fi < nFrags; fi++) {
            string nk = fn.frags[fi].startNode.key();
            if (visited.count(nk)) continue;
            visited.insert(nk);
            IloExpr arcIn(env), fragOut(env);
            for (int ai : fn.arcToNode[nk])   arcIn  += ya[ai];
            for (int fi2: fn.fragStartAtNode[nk])
                if (!fn.frags[fi2].dominated)  fragOut += xf[fi2];
            sprintf(var, "FlowPU_%s", nk.substr(0,20).c_str());
            IloRange c(env, 0, arcIn - fragOut, 0);
            c.setName(var);
            model.add(c);
        }
    }

    // --- Constraint 2: Flow conservation at delivery boundary nodes ---
    // For each distinct delivery boundary node:
    //   sum_{f ending at node} x_f  ==  sum_{a leaving from node} y_a
    {
        set<string> visited;
        for (int fi = 0; fi < nFrags; fi++) {
            string nk = fn.frags[fi].endNode.key();
            if (visited.count(nk)) continue;
            visited.insert(nk);
            IloExpr fragIn(env), arcOut(env);
            for (int fi2: fn.fragEndAtNode[nk])
                if (!fn.frags[fi2].dominated)  fragIn  += xf[fi2];
            for (int ai : fn.arcFromNode[nk])  arcOut += ya[ai];
            sprintf(var, "FlowDL_%s", nk.substr(0,20).c_str());
            IloRange c(env, 0, fragIn - arcOut, 0);
            c.setName(var);
            model.add(c);
        }
    }

    // --- Constraint 3: Passenger cover (mandatory: == 1) ---
    cout << "DEBUG: fragsCoveringPax sizes: ";
    for (int p = 0; p < inst->n; p++) cout << fn.fragsCoveringPax[p].size() << " ";
    cout << endl;
    for (int p = 0; p < inst->n; p++) {
        IloExpr cover(env);
        for (int fi : fn.fragsCoveringPax[p])
            if (!fn.frags[fi].dominated) cover += xf[fi];
        sprintf(var, "CoverPax_%d", p);
        IloRange c(env, 1, cover, 1);
        c.setName(var);
        model.add(c);
    }

    // --- Constraint 4: Parcel cover (optional: <= 1) ---
    for (int j = 0; j < inst->m; j++) {
        IloExpr cover(env);
        for (int fi : fn.fragsCoveringParcel[j])
            if (!fn.frags[fi].dominated) cover += xf[fi];
        sprintf(var, "CoverParcel_%d", j);
        IloRange c(env, -IloInfinity, cover, 1);
        c.setName(var);
        model.add(c);
    }

    // --- Constraint 5: Vehicle limit (<= 1 route per depot) ---
    for (int k = 0; k < inst->K; k++) {
        IloExpr depOut(env);
        for (int ai : fn.depotOutArcIds) {
            if (fn.arcs[ai].vehicle == k) depOut += ya[ai];
        }
        sprintf(var, "VehLimit_%d", k);
        IloRange c(env, -IloInfinity, depOut, 1);
        c.setName(var);
        model.add(c);
    }

    // --- Static valid inequalities ---
    addRfnFAInequalities(model, env, xf, ya, fn);
    addRfnAFInequalities(model, env, xf, ya, fn);

    // --- Export LP for inspection ---
    model.add(IloConversion(env, xf, ILOBOOL));
    IloCplex rfn(model);
    rfn.exportModel("rfnSARP.lp");
    rfn.setParam(IloCplex::Threads, threads);
    rfn.setParam(IloCplex::TiLim, 7200);
    rfn.setParam(IloCplex::MIPDisplay, 2);

    // --- Register IPEC lazy callback ---
    // Checks each integer incumbent for time-window / horizon violations
    // across fragment boundaries and for routes whose minimum duration
    // (depot departure to last service completion) exceeds inst->maxTime,
    // and separates infeasible-chain cuts.

    class RfnLazyCallback : public IloCplex::LazyConstraintCallbackI {
    public:
        IloEnv             cbEnv;
        IloBoolVarArray    cbXf, cbYa;
        instanceStat      *cbInst;
        vector<nodeStat>  *cbNodeVec;
        double           **cbMdist;
        FragmentNetwork   *cbFn;

        RfnLazyCallback(IloEnv e,
                        IloBoolVarArray xf_in, IloBoolVarArray ya_in,
                        instanceStat *inst_in, vector<nodeStat> *nv,
                        double **md, FragmentNetwork *fn_in)
            : IloCplex::LazyConstraintCallbackI(e),
              cbEnv(e), cbXf(xf_in), cbYa(ya_in),
              cbInst(inst_in), cbNodeVec(nv), cbMdist(md), cbFn(fn_in) {}

        IloCplex::CallbackI* duplicateCallback() const {
            return new (getEnv()) RfnLazyCallback(
                cbEnv, cbXf, cbYa, cbInst, cbNodeVec, cbMdist, cbFn);
        }

        // Timing check for the consecutive sub-chain chainFrags[s..e].
        // Concatenates the fragments' location sequences and propagates
        // service times (same conventions as computeFragmentTiming):
        //   1. earliest forward pass  -> time-window / horizon feasibility
        //   2. backward pass          -> latest feasible start
        //   3. forward pass from the latest start -> minimum route duration
        //      (duration is non-increasing in the start time)
        // depotPhys >= 0 adds the depot-out travel leg to the duration.
        // Returns false if no schedule satisfies all time windows, the
        // horizon T, and the maxTime route duration limit.
        bool subchainFeasible(const vector<int> &chainFrags,
                              int s, int e, int depotPhys) {
            vector<nodeStat> &nv = *cbNodeVec;
            vector<int> locs;
            for (int ci = s; ci <= e; ci++)
                for (const RfnLoc &l : cbFn->frags[chainFrags[ci]].seq)
                    locs.push_back(l.physIndex);
            int L = locs.size();
            if (L == 0) return true;

            auto travel = [&](int a, int b) {
                return cbMdist[a][b] / cbInst->vmed;
            };

            // 1. Earliest schedule: time windows and horizon
            vector<double> fwd(L);
            fwd[0] = nv[locs[0]].e;
            for (int k = 1; k < L; k++) {
                fwd[k] = max(fwd[k-1] + nv[locs[k-1]].delta
                                      + travel(locs[k-1], locs[k]),
                             nv[locs[k]].e);
                if (fwd[k] > nv[locs[k]].l + 1e-9) return false;
            }
            if (fwd[L-1] + nv[locs[L-1]].delta > cbInst->T + 1e-9)
                return false;

            // 2. Latest feasible start
            vector<double> bwd(L);
            bwd[L-1] = nv[locs[L-1]].l;
            for (int k = L-2; k >= 0; k--) {
                bwd[k] = min(bwd[k+1] - nv[locs[k]].delta
                                      - travel(locs[k], locs[k+1]),
                             nv[locs[k]].l);
            }

            // 3. Minimum duration: start at the latest feasible time,
            //    then serve every location as early as the windows allow.
            double start = bwd[0];
            double t = start;
            for (int k = 1; k < L; k++) {
                t = max(t + nv[locs[k-1]].delta + travel(locs[k-1], locs[k]),
                        nv[locs[k]].e);
            }
            double dur = (t + nv[locs[L-1]].delta) - start;
            if (depotPhys >= 0)
                dur += travel(depotPhys, locs[0]);
            return dur <= cbInst->maxTime + 1e-9;
        }

        void main() {
            // Step 1: Reconstruct chains from the integer solution
            // For each vehicle k, follow depot-out arc → fragments → depot-in arc
            // to build an ordered chain of fragment indices.

            set<int> reached; // fragments reached from some depot-out arc

            for (int k = 0; k < cbInst->K; k++) {
                // Find the depot-out arc used by vehicle k
                int startArc = -1;
                for (int ai : cbFn->depotOutArcIds) {
                    if (cbFn->arcs[ai].vehicle == k &&
                        getValue(cbYa[ai]) > 0.5) {
                        startArc = ai;
                        break;
                    }
                }
                if (startArc < 0) continue; // vehicle not used

                // Follow the chain
                vector<int> chainFrags, chainArcs;
                RfnNode curNode = cbFn->arcs[startArc].to;

                while (true) {
                    // Find used fragment starting at curNode
                    int usedFrag = -1;
                    auto it = cbFn->fragStartAtNode.find(curNode.key());
                    if (it != cbFn->fragStartAtNode.end()) {
                        for (int fi : it->second) {
                            if (getValue(cbXf[fi]) > 0.5) { usedFrag = fi; break; }
                        }
                    }
                    if (usedFrag < 0) break;
                    chainFrags.push_back(usedFrag);

                    // Find used arc leaving End(usedFrag)
                    int usedArc = -1;
                    auto ita = cbFn->arcFromNode.find(cbFn->frags[usedFrag].endNode.key());
                    if (ita != cbFn->arcFromNode.end()) {
                        for (int ai : ita->second) {
                            if (getValue(cbYa[ai]) > 0.5) { usedArc = ai; break; }
                        }
                    }
                    if (usedArc < 0) break;
                    if (cbFn->arcs[usedArc].isDepotIn) break; // end of route
                    chainArcs.push_back(usedArc);
                    curNode = cbFn->arcs[usedArc].to;
                }

                if (chainFrags.empty()) continue;
                for (int fi : chainFrags) reached.insert(fi);

                int nF = (int)chainFrags.size();
                int depotPhys = cbFn->arcs[startArc].from.physIndex;

                // Step 2: Find the shortest illegal prefix [0..violFrag],
                // first without the depot-out leg (a certificate that is
                // independent of which depot the chain is attached to).
                int  violFrag    = -1;
                bool useDepotArc = false;
                for (int e = 0; e < nF; e++) {
                    if (!subchainFeasible(chainFrags, 0, e, -1)) {
                        violFrag = e;
                        break;
                    }
                }
                if (violFrag < 0) {
                    // Chain feasible on its own — recheck the full route
                    // duration with the depot-out travel leg included.
                    if (subchainFeasible(chainFrags, 0, nF-1, depotPhys))
                        continue; // route is feasible
                    violFrag    = nF - 1;
                    useDepotArc = true;
                }

                // Step 3: Minimal illegal sub-chain — trim fragments from
                // the front while the remainder is still illegal. When the
                // violation needs the depot leg, the chain must stay anchored
                // at fragment 0, so no trimming is possible.
                int sIdx = 0;
                if (!useDepotArc) {
                    while (sIdx < violFrag &&
                           !subchainFeasible(chainFrags, sIdx+1, violFrag, -1))
                        sIdx++;
                }

                // Step 4: Add IPEC cut — forbid simultaneous use of all
                // fragments and connecting arcs of the illegal sub-chain
                // (plus the depot-out arc when the violation depends on it):
                //   sum of vars <= (#vars - 1)
                IloExpr cut(cbEnv);
                int nVars = 0;
                if (useDepotArc) { cut += cbYa[startArc]; nVars++; }
                for (int ci = sIdx; ci <= violFrag; ci++) {
                    cut += cbXf[chainFrags[ci]];
                    nVars++;
                }
                for (int ci = sIdx; ci < violFrag && ci < (int)chainArcs.size(); ci++) {
                    cut += cbYa[chainArcs[ci]];
                    nVars++;
                }
                add(cut <= nVars - 1);
                cut.end();
            }

            // Step 5: Cycle elimination — flow conservation admits closed
            // fragment/arc loops that never touch a depot. Such loops are
            // structurally illegal (every route must start at a depot) and,
            // since they are never reached by the chain reconstruction above,
            // their timing is never checked either. Cut each cycle found.
            for (int fi0 = 0; fi0 < (int)cbFn->frags.size(); fi0++) {
                if (reached.count(fi0)) continue;
                if (cbFn->frags[fi0].dominated) continue;
                if (getValue(cbXf[fi0]) < 0.5) continue;

                // Follow used arcs/fragments from fi0 until the loop closes.
                vector<int> cycFrags, cycArcs;
                int cur = fi0;
                bool closed = false;
                while (true) {
                    cycFrags.push_back(cur);
                    reached.insert(cur);

                    int usedArc = -1;
                    auto ita = cbFn->arcFromNode.find(cbFn->frags[cur].endNode.key());
                    if (ita != cbFn->arcFromNode.end()) {
                        for (int ai : ita->second) {
                            if (getValue(cbYa[ai]) > 0.5) { usedArc = ai; break; }
                        }
                    }
                    if (usedArc < 0 || cbFn->arcs[usedArc].isDepotIn) break;
                    cycArcs.push_back(usedArc);

                    int nextFrag = -1;
                    auto it = cbFn->fragStartAtNode.find(cbFn->arcs[usedArc].to.key());
                    if (it != cbFn->fragStartAtNode.end()) {
                        for (int fi : it->second) {
                            if (getValue(cbXf[fi]) > 0.5) { nextFrag = fi; break; }
                        }
                    }
                    if (nextFrag < 0) break;
                    if (nextFrag == fi0) { closed = true; break; }
                    if (reached.count(nextFrag)) break; // merged into another structure
                    cur = nextFrag;
                }
                if (!closed) continue;

                IloExpr cut(cbEnv);
                int nVars = 0;
                for (int fi : cycFrags) { cut += cbXf[fi]; nVars++; }
                for (int ai : cycArcs)  { cut += cbYa[ai]; nVars++; }
                add(cut <= nVars - 1);
                cut.end();
            }
        }
    };

    rfn.use(new (env) RfnLazyCallback(env, xf, ya, inst, &nodeVec, mdist, &fn));

    // --- Solve ---
    IloNum start_t = rfn.getTime();
    rfn.solve();
    IloNum solveTime = (rfn.getTime() - start_t) / threads;

    cout << "Sol status: " << rfn.getStatus() << endl;
    cout << " Total Time: " << solveTime << endl;

    sStat->feasible = rfn.isPrimalFeasible();

    if (sStat->feasible) {
        sStat->solprofit = rfn.getObjValue();
        sStat->solDual   = rfn.getBestObjValue();
        sStat->time      = solveTime;
        sStat->status    = "Optimal";

        cout << " LB: " << rfn.getObjValue() << endl;
        cout << " UB: " << rfn.getBestObjValue() << endl;

        // Reconstruct solution into sStat->solvec (per-vehicle arc pairs in original node indices)
        vector<pair<int,int>> emptyVec;
        for (int k = 0; k < inst->K; k++)
            sStat->solvec.push_back(emptyVec);

        for (int k = 0; k < inst->K; k++) {
            // Find depot-out arc for vehicle k
            int startArc = -1;
            for (int ai : fn.depotOutArcIds) {
                if (fn.arcs[ai].vehicle == k && rfn.getValue(ya[ai]) > 0.5) {
                    startArc = ai; break;
                }
            }
            if (startArc < 0) continue;

            // Follow chain; record (from.physIndex, to.physIndex) for each hop
            int depotPhys = fn.arcs[startArc].from.physIndex;
            RfnNode curNode = fn.arcs[startArc].to;
            sStat->solvec[k].push_back({depotPhys, curNode.physIndex});

            while (true) {
                int usedFrag = -1;
                auto it = fn.fragStartAtNode.find(curNode.key());
                if (it != fn.fragStartAtNode.end()) {
                    for (int fi : it->second)
                        if (rfn.getValue(xf[fi]) > 0.5) { usedFrag = fi; break; }
                }
                if (usedFrag < 0) break;

                // Add each consecutive pair in the fragment's physical sequence
                const RfnFragment &f = fn.frags[usedFrag];
                for (int si = 0; si + 1 < (int)f.seq.size(); si++) {
                    int a = f.seq[si].physIndex, b = f.seq[si+1].physIndex;
                    if (a != b) sStat->solvec[k].push_back({a, b});
                }

                int usedArc = -1;
                auto ita = fn.arcFromNode.find(f.endNode.key());
                if (ita != fn.arcFromNode.end()) {
                    for (int ai : ita->second)
                        if (rfn.getValue(ya[ai]) > 0.5) { usedArc = ai; break; }
                }
                if (usedArc < 0) break;
                if (fn.arcs[usedArc].isDepotIn) break;
                curNode = fn.arcs[usedArc].to;
                sStat->solvec[k].push_back({fn.arcs[usedArc].from.physIndex,
                                             curNode.physIndex});
            }
        }

        printRfnSolution(inst, mdist, nodeVec, sStat);
    }
    else {
        cout << "Sol status: Infeasible/No solution found" << endl;
    }

    rfn.end();
    env.end();
}

// ============================================================
// Output
// ============================================================
void printRfnSolution(instanceStat *inst, double **mdist,
                      vector<nodeStat> &nodeVec, solStats *sStat) {
    cout << "\nObj Val: " << setprecision(10) << sStat->solprofit << endl;
    cout << "Solve Time: " << sStat->time << endl;

    int fDepot = inst->n + 2*inst->m;
    int fDummy = fDepot + inst->K;

    for (int k = 0; k < inst->K; k++) {
        if (sStat->solvec[k].empty()) continue;
        cout << "Vehicle " << k << ": ";
        set<int> visited;
        for (auto &arc : sStat->solvec[k]) {
            if (!visited.count(arc.first))  { cout << arc.first  << " - "; visited.insert(arc.first); }
            if (!visited.count(arc.second)) { cout << arc.second << " - "; visited.insert(arc.second); }
        }

        // Minimum route duration: depot departure to last service completion,
        // starting as late as the windows allow (same check as the callback).
        vector<int> locs;
        for (auto &arc : sStat->solvec[k]) {
            if (locs.empty()) locs.push_back(arc.first);
            locs.push_back(arc.second);
        }
        int L = locs.size();
        if (L > 1) {
            vector<double> bwd(L);
            bwd[L-1] = nodeVec[locs[L-1]].l;
            for (int p = L-2; p >= 1; p--) {
                bwd[p] = min(bwd[p+1] - nodeVec[locs[p]].delta
                                      - mdist[locs[p]][locs[p+1]] / inst->vmed,
                             nodeVec[locs[p]].l);
            }
            double start = bwd[1];
            double t = start;
            for (int p = 2; p < L; p++) {
                t = max(t + nodeVec[locs[p-1]].delta
                          + mdist[locs[p-1]][locs[p]] / inst->vmed,
                        nodeVec[locs[p]].e);
            }
            double dur = (t + nodeVec[locs[L-1]].delta) - start
                       + mdist[locs[0]][locs[1]] / inst->vmed;
            cout << "Total time: " << dur;
        }
        cout << endl;
    }
}

// ============================================================
// Top-level entry point
// ============================================================
void rfnMethod(nodeStat *node, instanceStat *inst, double **mdist,
               vector<nodeStat> &nodeVec, probStat *problem, solStats *sStat) {

    cout << "\nRFN Method: n=" << inst->n << " m=" << inst->m
         << " K=" << inst->K << " V=" << inst->V << endl;

    FragmentNetwork fn;
    fn.fragsCoveringPax.resize(inst->n);
    fn.fragsCoveringParcel.resize(inst->m);

    // Phase 1: Enumerate fragments
    enumerateFragments(inst, nodeVec, mdist, problem, fn);

    // Phase 2: Enumerate arcs + build incidence maps
    enumerateArcs(inst, nodeVec, mdist, fn);
    buildIncidenceMaps(inst, fn);

    // Phase 4: Domination (comment out to disable during testing)
    // dominateFragments(inst, fn);

    // Phase 3+5+6: Build and solve MIP
    mipRfn(inst, nodeVec, mdist, problem, fn, sStat);

    for (int i = 0; i < inst->V + inst->dummy; i++)
        delete[] mdist[i];
    delete[] mdist;
}
