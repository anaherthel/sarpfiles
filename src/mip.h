#ifndef MIP_H_INCLUDED
#define MIP_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <cmath>
#include <limits>
#include <float.h>
#include <iomanip>
#include <ctime>
#include <ilcplex/ilocplex.h>
#include <stdlib.h>
#include <iostream>
#include <locale.h>
#include <sys/time.h>
#include <ctime>
#include <unistd.h>
#include "functions.h"

using namespace std;

void mip(instanceStat *inst, vector<nodeStat> &nodeVec, double **mdist, vector< vector<int> > &bundleVec, vector<double> &bundleTimes, vector< vector< vector<int> > > &clusterVec);

#endif