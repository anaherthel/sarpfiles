# ScSp SARP on the published instances — CPLEX

Single customer, single parcel: at most one parcel aboard, passengers chain only with an empty box.
Instances: `sarpinstances/G*-V*/{csarp,ghsarp,sfsarp}`, 229 files (V1 108 = G1 64 + G2 44, V2 55,
V3 30, V4 36). Each instance is solved by two models: `node` (disaggregated, mipnode.cpp) and
`bundle` (modelbundle.cpp + mipbundle.cpp).

Setup: CPLEX 22.1.1, 1 thread, 3600 s (`SARP_TIMELIMIT`), 40 runs in parallel on q-primal
(`~/scsp`, runner `runjob.sh` / `runjob1AD.sh`). `logs.tgz` holds one CPLEX log per run.

Columns: `type,instance,model,scen,status,obj,bound,time,nodes,rc`. `obj = -1e+30` means no
feasible solution was found.

## 1AD/ — the scenario in use

Each passenger C is served alone or together with one parcel pickup P and delivery D, in one of
the blocks P-C-D, C-P-D, P-D-C. Every direct P-D needs its own adjacent passenger, and there is
no standalone P-D.

- Code: `scsp-published` at `f5a2f4bd` (node model rewritten for this scenario). Run 2026-10-05.
- bundle: 227 Optimal, 2 Unknown (sarp-60-30-C-1-V4 and C-2-V4: the root LP took about
  3150 s and did not finish).
- node: 161 Optimal, 63 Feasible at the time limit, 5 Unknown.
- node == bundle on all 161 instances that both solve to optimality. On the 62 where node
  stops at Feasible and bundle is Optimal, no node incumbent is above, and no node bound below,
  the bundle optimum.
- 1AD ≥ 1A on every run optimal in both scenarios (385 runs over both models, 373 strictly
  higher).

## 1A_parked/ — kept aside, may not be used

Passenger in the middle only (P-C-D). Code `c041b1d6`, run 2026-09-29/30.
bundle 229 Optimal; node 158 Optimal, 68 Feasible, 3 Unknown. node == bundle where both are
optimal.
