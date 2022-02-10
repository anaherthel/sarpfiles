#ifndef SARPCONSTRUCTION_H_INCLUDED
#define SARPCONSTRUCTION_H_INCLUDED

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
#include "hbundle.h"
#include "sarpRoute.h"
#include "sarpSolution.h"

class sarpConstruction {
protected:

    vector<int> CLpass;
    vector<int> CLparc;//only pickup requests
    
    vector<nodeStat> passNodes;

public:

    sarpConstruction(instanceStat *inst, vector<nodeStat> &nodeVec);
    ~sarpConstruction(){};
    bool compare(int a, int b);
    int getRandRequestFromCL();
    
    void ConstrProc(instanceStat *inst, vector<nodeStat> &nodeVec, double **Mdist, probStat* problem, sarpSolution *solution);

    void removeFromCL(int id, bool pass);
   
};


#endif