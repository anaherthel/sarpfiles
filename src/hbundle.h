#ifndef HBUNDLE_H_INCLUDED
#define HBUNDLE_H_INCLUDED

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
#include "readdata.h"
#include "functions.h"

class hbundle {
protected:

    vector<nodeStat> twSort;
    vector< vector<int> > proxSort;

    int pairings;

public:
    void orderRequests(instanceStat *inst, vector<nodeStat> &nodeVec, double ***Mdist);
};

// ostream & operator<<(ostream &os, const FLPInstance &instance);


#endif