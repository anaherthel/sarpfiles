#include "data.h"
#include <stdlib.h>

Data::Data(char** argv){

	int N;
	string arquivo;

	instancia = argv[1];


	ifstream in( instancia, ios::in);

	if( !in ) {
		cout << "Bugadao\n";
		exit (1);
	}

	in >> arquivo;
	in >> instance;

	in >> arquivo;
	in >> machines;

	in >> arquivo;
	in >> jobs;

	in >> arquivo;
	in >> slits;

	in >> arquivo;
	in >> colors;

	in >> arquivo;
	in >> mWidth;

	in >> arquivo;

	cost1 = 0;


	jobinfo auxjob;

	// jweight = new int[jobs];
	// jwidth = new int[jobs];
	// jlength = new int[jobs];
	// jcolor = new int[jobs];
	// jthick = new int[jobs];

	for (int i = 0; i < jobs; i++){
		in >> auxjob.jweight;
		in >> auxjob.jwidth;
		in >> auxjob.jlength;
		in >> auxjob.jthick;
		in >> auxjob.jcolor;
		auxjob.jnumber = i;
		jobsList.push_back(auxjob);

		// jweight[i] = auxjob.jweight;
		// jwidth[i] = auxjob.jwidth;
		// jlength[i] = auxjob.jlength;
		// jthick[i] = auxjob.jthick;
		// jcolor[i] = auxjob.jcolor;
	}

	//get setup between colors
	colorSetup = new int*[colors];
	for (int i = 0; i < colors; i++){
		colorSetup[i] = new int[colors];
	}

	in >> arquivo;

	for (int i = 0; i < colors; i++){
		for (int j = 0; j < colors; j++){
			in >> colorSetup[i][j];
			// cout << aux[i][j] << " ";
		}
	}


}

// //preprocess data
// void DataPacking::preprocess(DataPacking &data){

// 	//sort jobs: color > thickness > width > length
// 	data.jobsList.sort(sortJobs);

// 	//create one list of jobs for each type of job

// }

// bool sortJobs(DataPacking::jobinfo &first, DataPacking::jobinfo &second){

// 	return 0;
// }


