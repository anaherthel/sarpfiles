
#include "Functions.h"
#include <stdlib.h>


void preprocess(Data &data){

	//sort jobs: color > thickness > width > length
	data.jobsList.sort(sortJobs);

	data.jweight = new int[data.jobs];
	data.jwidth = new int[data.jobs];
	data.jlength = new int[data.jobs];
	data.jcolor = new int[data.jobs];
	data.jthick = new int[data.jobs];


	// cout << "sorted jobs: " << endl;
	int counter = 0;
	for (list<Data::jobinfo>::iterator it = data.jobsList.begin(); it != data.jobsList.end(); it++){
		// cout << it->jnumber << endl;
		data.jweight[counter] = it->jweight;
		data.jwidth[counter] = it->jwidth;
		data.jlength[counter] = it->jlength;
		data.jthick[counter] = it->jthick;
		data.jcolor[counter] = it->jcolor;
		counter++;
	}
	cout << endl;

	Data::typeinfo auxtype;

	//Count number of types of jobs
	int currColor = data.jobsList.begin()->jcolor;
	int currThick = data.jobsList.begin()->jthick;
	auxtype.typeColor = currColor;
	auxtype.typeThick = currThick;
	auxtype.nJobsPerType = 0;
	data.typeInfo.push_back(auxtype);
	data.nTypes = 1;
	for (list<Data::jobinfo>::iterator it = data.jobsList.begin(); it != data.jobsList.end(); it++){
		if ( (it->jcolor != currColor) || (it->jthick != currThick) ){
			currThick = it->jthick;
			currColor = it->jcolor;
			data.nTypes++;
			auxtype.typeColor = currColor;
			auxtype.typeThick = currThick;
			auxtype.nJobsPerType = 1;
			data.typeInfo.push_back(auxtype);
		} else {
			data.typeInfo[data.nTypes-1].nJobsPerType++;
		}
	}


	//transform setup between colors into setup between jobs
	data.setup = new int*[data.jobs];
	for (int i = 0; i < data.jobs; i++){
		data.setup[i] = new int[data.jobs];
	}

	for (int i = 0; i < data.jobs; i++)
	{
		for (int j = 0; j < data.jobs; j++)
		{
			data.setup[i][j] = data.colorSetup[data.jcolor[i]][data.jcolor[j]];
		}
	}


	// for (list<Data::jobinfo>::iterator it1 = data.jobsList.begin(); it1 != data.jobsList.end(); it1++){
	// 	for (list<Data::jobinfo>::iterator it2 = data.jobsList.begin(); it2 != data.jobsList.end(); it2++){
	// 		data.setup[it1->jnumber][it2->jnumber] = data.colorSetup[it1->jcolor][it2->jcolor];
	// 	}
	// }




}

//sort jobs: color > thickness > width > length
bool sortJobs(Data::jobinfo &first, Data::jobinfo &second){

	if (first.jcolor == second.jcolor){

		if (first.jthick == second.jthick){

			if (first.jwidth == second.jwidth){

				return (first.jlength > second.jlength);

			} else {
				return (first.jwidth > second.jwidth);
			}

		} else{
			return (first.jthick < second.jthick);
		}

	} else{
		return (first.jcolor < second.jcolor);
	}

	// return 0;
}