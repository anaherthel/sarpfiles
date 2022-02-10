#ifndef BRANCHANDBOUND_H_INCLUDED
#define BRANCHANDBOUND_H_INCLUDED

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
#include "hungarian.h"

using namespace std;

typedef struct bbNode{
	double LB;
	vector< vector<int> > solution;
	vector< pair<int, int> > fArcs;
} bbNode_t;

void getVectorSolution (int size, bbNode_t *bbNode, hungarian_problem_t *hp);
void verifyCicle (bbNode_t *bbNode, vector <int> &sCicle, bool &cicle);
bbNode_t newBBNode(bbNode_t *bbNode);
void BBDFS(vector <int> &sCicle, bool cicle, double **distM, int size);
void BBBFS(vector <int> &sCicle, bool cicle, double **distM, int size);

#endif