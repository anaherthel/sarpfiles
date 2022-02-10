#ifndef _Functions_H_
#define _Functions_H_

#include <vector>
#include <list>
#include <set>
#include <stdio.h>
#include <fstream>
#include <iostream>
#include "data.h"


using namespace std;


void preprocess(Data &data);

bool sortJobs(Data::jobinfo &first,  Data::jobinfo &second);

#endif