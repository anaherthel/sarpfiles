# ScSp SARP on the published instances — CPLEX

Single customer, single parcel: at most one parcel aboard, passengers chain only with an empty box.
Instances: `sarpinstances/G*-V*/{csarp,ghsarp,sfsarp}`, 229 files (V1 108 = G1 64 + G2 44, V2 55,
V3 30, V4 36). Each instance is solved by two models: `node` (disaggregated, mipnode.cpp) and
`bundle` (modelbundle.cpp + mipbundle.cpp).

Setup: CPLEX 22.1.1, 1 thread, 3600 s (`SARP_TIMELIMIT`). `logs.tgz` holds one CPLEX log per run.

Columns: `type,instance,model,scen,status,obj,bound,time,nodes,rc`, and in `1AD/` also `tries`
(1 unless the memory watcher stopped the run and it was started again; it never did). `obj =
-1e+30` means no feasible solution was found.

## 1AD/ — the scenario in use

Each passenger C is served alone or together with one parcel pickup P and delivery D, in one of
the blocks P-C-D, C-P-D, P-D-C. Every direct P-D needs its own adjacent passenger, and there is
no standalone P-D.

- Code: `scsp-published` at `a8f2e6b6`: node model rewritten for this scenario (`f5a2f4bd`) and
  both models run with MIP gaps 0 (`MIPGap`, `AbsMIPGap`), so Optimal means proved exactly.
- Run 2026-10-07: node on q-primal, bundle on q-dual, 30 runs in parallel on each (runner
  `runcpx.sh`, memory watcher `memwatch2.sh`, both in `~/scsp`).
- bundle: 227 Optimal, 2 Unknown (sarp-60-30-C-1-V4 and C-2-V4: the root LP does not finish
  within the hour).
- node: 161 Optimal, 65 Feasible at the time limit, 3 Unknown (sarp-50-25-C-2-V3,
  sarp-60-30-B-1-V4, sarp-60-30-C-1-V4).
- node == bundle on all 161 instances that both solve to optimality. On the 64 where node stops
  at Feasible and bundle is Optimal, no node incumbent is above, and no node bound below, the
  bundle optimum; the node incumbents are on average 10.3% below it.
- 1AD ≥ 1A on every run optimal in both scenarios (385 runs over both models, 373 strictly
  higher).
- Cross-check with BaPCod (sharearidec branch `scsp-1ad`): equal to the disaggregated BaPCod
  model on all 224 instances both solve to optimality, and to the bundle BaPCod model on all 210.
- Known quirk: sarp-12-6-B-2-V1 (node) reports Optimal at 39.716 with a bound of 40.366; every
  other run, CPLEX and BaPCod, agrees on 39.716.

## outdated/1AD_gap1e-4/ — superseded

The first 1AD run (2026-10-05, code `f5a2f4bd`, 40 runs in parallel on q-primal), with CPLEX's
default relative MIP gap of 1e-4. Its Optimal means optimal to within 0.01%: 45 Optimal rows have
a bound above the value, and on sfsarp-40-20-2-V1 the bundle model stopped at 215.51317 against
a true optimum of 215.52927.

## 1A_parked/ — kept aside, may not be used

Passenger in the middle only (P-C-D). Code `c041b1d6`, run 2026-09-29/30, with the default
1e-4 relative gap (Optimal within 0.01%). bundle 229 Optimal; node 158 Optimal, 68 Feasible,
3 Unknown. node == bundle where both are optimal.
