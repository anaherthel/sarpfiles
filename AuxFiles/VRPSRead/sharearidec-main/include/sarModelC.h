/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#ifndef SarModel_h
#define SarModel_h

#include "bcModelingLanguageC.hpp"
#include "sarParameters.h"
#include "sarDataC.h"

#include <vector>

enum SelectedModel{EXTREME_ROUTE_XY, DISCRETIZED, EXTREME_ROUTE_XD, CVRP, EXTREME_ROUTE_XYS,
                   STRONG_K_PATH_CUTS_BASED, FLOW, STRONG_K_PATH_CUTS_BASED_MULTI_NODE,
                   STRONG_K_PATH_CUTS_BASED_MULTI_ARC, MULTI_ARC_WITH_FLOW_VARS};


void buildModel(SarData & dataStruct, BcModel & sarModel,
                                    SarApplicationSpecificParam & appParam);

#endif
