#ifndef GA_H_INCLUDED
#define GA_H_INCLUDED

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

typedef struct individual{
	double cost;
	vector<int> solution;
} individual_t;

/*individual_t nome;

nome.cost = 10;

&nome

nome->cost

pop[0].cost */

void CrossoverOX(double **distM, vector<individual_t> &pop, int size, int parent1);
void Mutation(double **distM, vector<individual_t> &pop, int size);
bool compareCosts(const individual_t &lhs, const individual_t &rhs);

#endif