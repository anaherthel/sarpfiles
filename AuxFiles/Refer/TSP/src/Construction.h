#ifndef CONSTRUCTION_H_INCLUDED
#define CONSTRUCTION_H_INCLUDED

#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <math.h>
#include <limits>
#include <float.h>

using namespace std;

void initialSolutionNN(int n, double **distMatrix, vector <int> &sol, double &solCost);
void initialSolutionCI(int n, double **distMatrix, vector <int> &sol, double &solCost);

#endif
