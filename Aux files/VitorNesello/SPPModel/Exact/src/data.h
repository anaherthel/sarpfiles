#ifndef _DATA_H_
#define _DATA_H_

#include <vector>
#include <list>
#include <stdio.h>
#include <fstream>
#include <iostream>

using namespace std;

class Data{

	private:

	public:

		Data(char** argv);

		//instance info
		char instance[100];
		char* instancia;
		int jobs;
		int machines;
		int slits;
		int colors;
		int mWidth;
		int nTypes;
		int** setup;
		int** colorSetup;

		//type info
		struct typeinfo{
			int nJobsPerType;
			int typeColor;
			int typeThick;
		};
		vector<typeinfo> typeInfo;


		//jobs info
		int* jweight;
		int* jwidth;
		int* jlength;
		int* jcolor;
		int* jthick;

		struct jobinfo{
			int jweight;
			int jwidth;
			int jlength;
			int jcolor;
			int jthick;
			int jnumber;
		};
		list<jobinfo> jobsList;

		//solution info
		//solution > block > points
		struct solPoint{
			int B;
			int S;
			int X;
			int Y;
			int J;
		};
		list<list<solPoint> > solution;
		vector<int> blockCost;
		int cost1;


		struct availablePoint{
			int B;
			int S;
			int X;
			int Y;
			int J;
			int maxLength;
			int maxHeight;
			bool flag;//1 -> can create upleft point
		};

		int **Mdist;

		//Output variables
		int UB;
		double LB;
		int status;
		int nNodes;
		double rootLB;
		

};

#endif
