cmake_minimum_required (VERSION 3.12)

project(ShareARide)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
                      ${CMAKE_CURRENT_SOURCE_DIR}/../../Bapcod/CMake ${CMAKE_CURRENT_SOURCE_DIR}/../../Bapcod/CMake/Packages
		      ${CMAKE_CURRENT_SOURCE_DIR}/CMake ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Packages)

find_package(BcpRcsp)
find_package(CGL)
find_package(CVRPSEP)

#make CGL optional
if ("${CGL_INCLUDE_DIR}" STREQUAL "CGL_INCLUDE_DIR-NOTFOUND" OR "${CGL_LIBRARY}" STREQUAL "CGL_LIBRARY-NOTFOUND")
   set(CGL_INCLUDE_DIR "")
   set(CGL_LIBRARY "")
   set(CGL_DEFINITIONS "")
endif()

include(BapcodApplicationProject)
include(BapcodApplicationPackage)
include(Documentation)
include(Profile)

# Optional: Set other libraries
#to use CONCERT
find_library(CONCERT_LIBRARY
             NAMES concert libconcert.a
             PATHS "$ENV{CPLEX_ROOT}/concert/lib/*/static_pic/")
find_library(ILO_LIBRARY
             NAMES libilocplex.a
             PATHS "$ENV{CPLEX_ROOT}/cplex/lib/*/static_pic/")
set(USER_LIBRARIES ${CONCERT_LIBRARY} ${ILO_LIBRARY} ${BCP_RCSP_LIBRARY} ${CGL_LIBRARY})
find_path(SDVRP_CPLEX_INCLUDE_DIR
          "ilcplex/ilocplex.h"
          PATHS "$ENV{CPLEX_ROOT}/cplex/include")
find_path(SDVRP_CONCERT_INCLUDE_DIR
          "ilconcert/iloenv.h"
          PATHS "$ENV{CPLEX_ROOT}/concert/include")
add_definitions(-DIL_STD ${CGL_DEFINITIONS})
include_directories (${SDVRP_CPLEX_INCLUDE_DIR} ${SDVRP_CONCERT_INCLUDE_DIR} ${BCP_RCSP_INCLUDE_DIR} ${CGL_INCLUDE_DIR} ${CVRPSEP_INCLUDE_DIR})


# Execute the build
set_project_version(1 0 0)

# Build Application
try_build_bapcod_application()

# Make Package
try_build_bapcod_application_package()
