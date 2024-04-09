#ifndef SarCutSeparationRoutine_H_
#define SarCutSeparationRoutine_H_

#include "sarDataC.h"
#include "bcModelCutConstrC.hpp"

struct TWSeqInfo
{
public:
    double earliest;
    double latest;
    double time_warp;
    double duration;

    TWSeqInfo(double earliest, double latest, double time_warp, double duration) :
        earliest(earliest), latest(latest), time_warp(time_warp), duration(duration)
    {}
};

class LazyCutSeparationRoutine : public BcCutSeparationFunctor
{
  SarData & _dataStruct;
  int _cutCount;
  int _cutRound;

  std::vector<std::vector<double>> _xvals;

public:
  LazyCutSeparationRoutine(SarData &dataStruct);
  virtual ~LazyCutSeparationRoutine();
  virtual int operator()(BcFormulation formPtr,
                         BcSolution &primalSol,
                         double &maxViolation,
                         std::list<BcConstr> &cutList);

  void updateDynamicValues(BcSolution &primalSol);
  bool solIsInteger();
  bool feasibleWrtDuration(std::vector<int>& route);
  vector<vector<int>> extractRoutes();


};

#endif /* SarCutSeparationRoutine_H_ */
