/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#include "sarParameters.h"

SarApplicationSpecificParam::SarApplicationSpecificParam(std::string parameterFileName, int argc,
                                                             char* argv[], std::ostream& os) :
        ParameterParser(parameterFileName),
        _cutOffValue("cutOffValue", -1),
        _setting("setting", 0)
{
  addApplicationParameter(_cutOffValue);
  addApplicationParameter(_setting);

  parse(argc, argv);

  print(os);
}

std::ostream & SarApplicationSpecificParam::print(std::ostream& os) const
{
  os << "Split-delivery Vehicle Routing Parameters  " << std::endl;
  os << "Subproblem solver: resource constrained shortest path solver" << std::endl;

  os << _cutOffValue << std::endl;
  os << _setting << std::endl;

  return (os);
}

double SarApplicationSpecificParam::cutOffValue() const
{
  return _cutOffValue();
}

int SarApplicationSpecificParam::setting() const
{
  return _setting();
}
