#ifndef LOCALSEARCH_H_INCLUDED
#define LOCALSEARCH_H_INCLUDED

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

void swap (double **distM, vector<int> &sol);
void relocate1 (double **distM, vector<int> &sol);
//void relocate2 (double **distM, vector<int> &sol);
void relocateK (double **distM, vector<int> &sol, int k);
void twoOpt(double **distM, vector<int> &sol);
void RVND(double **distM, vector<int> &sol, double &solCost);
void perturbation(double **distM, vector<int> &bestSol, vector<int> &sol, double &solCost);

#endif