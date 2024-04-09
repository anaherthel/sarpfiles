
/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application is developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */
#include "bcUsefulHeadFil.hpp"
#include "bcModelingLanguageC.hpp"

#include "sarParameters.h"
#include "sarDataC.h"
#include "sarModelC.h"

#define APPLICATION_VERSION "6.1"   /// this should be increased every commit.
                                    /// also, we should put this number in the commit message
#define APPLICATION_DATE "17/11/2021"

using namespace std;

class SarSolutionCheckAndPrintCallback : public BcSolutionFoundCallback
{
    const SarData & _data;
    const int _model;
public:
    SarSolutionCheckAndPrintCallback(SarData & data, int model) :
            BcSolutionFoundCallback(), _data(data), _model(model) {}
    virtual bool operator()(BcSolution newSolution) const
    {
        switch (_model)
        {
            case STRONG_K_PATH_CUTS_BASED:
                return false;// _data.printSolutionAndCheckIfFeasibleKPathBased(newSolution, false);
            default:
                break;
        }
        std::cout << "SDVRP application WARNING : soluton verification for this model has not been implemented yet"
                  << std::endl;
        return true;
    }
};


void runInstance(BcInitialisation & bapcodInit, SarApplicationSpecificParam & applSpecParam,
                 const std::string & instanceName)
{
    SarData data(applSpecParam, instanceName);
    BcModel sarModel(bapcodInit, instanceName);
    buildModel(data, sarModel, applSpecParam);
    //   sarModel.attach(new SarSolutionCheckAndPrintCallback(data, applSpecParam.model()));

    BcSolution solution = sarModel.solve();
    std::cout << solution << std::endl;

//     switch (applSpecParam.model())
//     {
//         case STRONG_K_PATH_CUTS_BASED:
//             data.printSolutionAndCheckIfFeasibleKPathBased(solution, false);
//             break;
//         default:
//             std::cout << "SDVRP application WARNING : soluton verification for this model has not been implemented yet"
//                       << std::endl;
//             break;
//     }

    bapcodInit.outputBaPCodStatistics(bapcodInit.instanceFile());
}

int main(int argc, char *argv[])
{
  BcInitialisation bapcodInitPtr(argc, argv, "");

  std::cout << "Application version is " << APPLICATION_VERSION << ", " << APPLICATION_DATE << std::endl;
  std::cout << "command line:";
  for (int i = 0; i < argc; i++)
    printf("%s ", argv[i]);
  printf("\n");

  SarApplicationSpecificParam applicationParam(bapcodInitPtr.configFile(), argc, argv);

  runInstance(bapcodInitPtr, applicationParam, bapcodInitPtr.instanceFile());

  return EXIT_SUCCESS;
}

