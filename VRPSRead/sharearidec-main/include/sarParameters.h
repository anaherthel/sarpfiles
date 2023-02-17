/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#ifndef Sar_Parameters_h
#define Sar_Parameters_h

#include "bcModelParameterC.hpp"
#include "bcParameterParserC.hpp"
#include "bcModelingLanguageC.hpp"
#include <iostream>
#include <string>

// #define ScSp 0
// #define ScMp 1
// #define McSp 2
// #define McMp 3

/* PROBLEM SPECIFIC PARAMETERS */
class SarApplicationSpecificParam : public ParameterParser
{
private:
    ApplicationParameter<double> _cutOffValue;
    ApplicationParameter<int> _setting;

public:
    SarApplicationSpecificParam(std::string parameterFileName, int argc, char *argv[], std::ostream &os = std::cout);

    virtual ~SarApplicationSpecificParam()
    {
    }
    std::ostream &print(std::ostream &os = std::cout) const;

    double cutOffValue() const;
    int setting() const;
};

inline std::ostream &operator<<(std::ostream &os, const SarApplicationSpecificParam &that)
{
  return that.print(os);
}

#endif // CVRPparameters_h
