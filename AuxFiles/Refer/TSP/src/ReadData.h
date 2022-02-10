#ifndef READDATA_H_INCLUDED
#define READDATA_H_INCLUDED

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

void ReadData( int argc, char** argv, int* Dimension, double ***Mdist );
double CalcDistEuc ( double *X, double *Y, int I, int J );
double CalcDistAtt ( double *X, double *Y, int I, int J );
double CalcLatLong ( double *X, double *Y, int n, double *latit, double* longit );
double CalcDistGeo ( double *latit, double *longit, int I, int J );

#endif
