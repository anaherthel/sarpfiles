#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

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

void printSolution (vector<int> &sol);
void printList (list<int> &lst);
double costCalc (double **distM, vector<int> &sol);
void generateRandomSolution (vector<int> &sol, int maxSize, list<int> &candList, list<int>::iterator &itr, double **distM);
void matrixInfinity (double **distM, int size);
void printMatrix (double **distM, int size);
void trMatrix (double **distM, int size);

#endif
