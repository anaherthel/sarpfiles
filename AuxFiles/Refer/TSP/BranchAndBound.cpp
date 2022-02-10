#include <cstdlib>
#include "BranchAndBound.h"
#include "Utils.h"
#include <stdio.h>

using namespace std;

void getVectorSolution (int size, bbNode_t *bbNode, hungarian_problem_t* hp)
{
	int i = 0;
	bool route = 1;

	int consult[7]  = { };

	list<int> candList;
	list<int>::iterator it;

	vector<int> subtour;

	subtour.push_back(0);
	consult[0] = 1;
	cout << endl;
	bbNode->solution.clear();

	for (int k = 1; k < size; k++) {
		candList.push_back(k);
	}

	while (!candList.empty()){
		while (route){
			route = 0;
			for (int j = 0; j < size; j++){
				if (hp->assignment[i][j] == 1){
					if (consult[j] == 0){
						subtour.push_back(j);
						i = j;
						candList.remove(j);
/*						cout << "Solution subtour: ";
						printSolution(subtour);
						cout << "\nList: ";
						printList(candList);*/
						consult[j] = 1;
						route = 1;
					}
				}
			}
		}

/*		cout << "\nConsult Array: [ ";

		for (int n = 0; n < size; n++){
			cout << consult[n] << " ";
		}

		cout << "]" << endl;*/

		if (consult[subtour[0]] == 1){
			subtour.push_back(subtour[0]);
			bbNode->solution.push_back(subtour);
		}

		for (it = candList.begin(); it != candList.end(); it++){
			if (consult[*it] == 0){
				subtour.clear();
				subtour.push_back(*it);
				candList.erase(it);
				i = *it;
				consult[i] = 1;
				route = 1;
			}
			break;
		}
	}

	cout << "\nNode solution: \n";
	
	for (int m = 0; m < bbNode->solution.size(); m++){
		printSolution(bbNode->solution[m]);
	}
}

void verifyCicle (bbNode_t *bbNode, vector <int> &sCicle, bool &cicle)
{
	int c = 99999;

	if (bbNode->solution.size() > 1){
		cicle = 1;
		for (int i = 0; i < bbNode->solution.size(); i++){
			if (bbNode->solution[i].size() < c){
				sCicle = bbNode->solution[i];
				c = sCicle.size();
			}
		}
/*		cout << "\nSmallest cicle: ";
		printSolution(sCicle);*/
	}

	else{
		cicle = 0;
	}

}

bbNode_t newBBNode(bbNode_t bbNode)
{
	bbNode.LB = 0;
	bbNode.fArcs.clear();

	return bbNode;
}

void BBProf (vector <int> &sCicle, bool cicle, hungarian_problem_t *hp, double **distM, int size, int m)
{	
	double globalLB = 99999;
	double globalUB = 99999;
	double cost;

	int counter = 0;
	int label;

	pair<int, int> arc;

	bbNode_t bbNode;

	list<bbNode_t> nodeList;
	list<bbNode_t>::iterator it1;
	list<bbNode_t>::iterator it2;

	vector< pair<int, int> > inArcs;

	vector<int> tourSolution;
	vector<int> globalSolution;

	do{

		hungarian_init(hp, distM, size, size, m);
		
		if (counter < 1){
			bbNode = newBBNode(bbNode);
			bbNode.LB = hungarian_solve(hp);
			globalLB = bbNode.LB;
			cout << "Cost of root: " << globalLB;
		}

		else{

			/*label = 0;
			cout << "\nList1:\n";
			for (it2 = nodeList.begin(); it2 != nodeList.end(); it2++){
				label++;
				cout << "Nó " << label << ": " << endl;
				for(int j = 0; j < it2->fArcs.size(); j++){
					cout << it2->fArcs[j].first << " " << it2->fArcs[j].second << endl;
				}
			}*/
			
			it1 = nodeList.end();
			it1--;
			for(int k = 0; k < it1->fArcs.size(); k++){
				hp->cost[it1->fArcs[k].first][it1->fArcs[k].second] = 99999;
			}

			it1->LB = hungarian_solve(hp);
			cost = it1->LB;
			cout << "\nCost of node: " << cost;
			
			if (cost >= globalUB){
				counter++;
				nodeList.erase(it1);
				continue;
			}
			
			if (cost > 0){
				if (cost < globalLB){
					globalLB = cost;
				}
			}

			inArcs.clear();
			
			inArcs = it1->fArcs;
			cout << "\nInherited Arcs: \n";

			for (int k = 0; k < inArcs.size(); k++){
				cout << inArcs[k].first << " " << inArcs[k].second << endl;
			}
			
			nodeList.erase(it1);
		}

		getVectorSolution(size, &bbNode, hp);

		if(bbNode.solution.size() <= 1){
			tourSolution = bbNode.solution[0];
		}

		verifyCicle(&bbNode, sCicle, cicle);

		if(!cicle){
			if (counter < 1){
				globalUB = globalLB;
				globalSolution = tourSolution;
			}
			else if(cost < globalUB){
				globalUB = cost;
				globalSolution = tourSolution;
 			}
		}

/*		cout << "\nGlobal LB: " << globalLB << "\n";
		cout << "\nGlobal UB: " << globalUB << "\n";*/
		cost = 0;

		if(cicle){
			for (int i = sCicle.size() - 2; i >= 0 ; i--){
				bbNode = newBBNode(bbNode);
				arc.first = sCicle[i];
				arc.second = sCicle[i + 1];
				bbNode.fArcs.push_back(arc);
				if (counter > 0){
					for(int j = inArcs.size() - 1; j >= 0; j--){
						arc.first = inArcs[j].first;
						arc.second = inArcs[j].second;
						bbNode.fArcs.insert(bbNode.fArcs.begin(), arc);
					}				
				}
				nodeList.push_back(bbNode);
			}
		}		

		cout << endl;

		counter++;

	} while(!nodeList.empty());

	cout << "\n\n----X----\nBranch and Bound Depth First\nFinal Solution Value: "<< globalUB;
	cout << "\nFinal Solution: ";
	printSolution(globalSolution);	
	cout << endl << endl;
}

void BBLarg (vector <int> &sCicle, bool cicle, hungarian_problem_t *hp, double **distM, int size, int m)
{

	double globalLB = 99999;
	double globalUB = 99999;
	double cost;

	int counter = 0;
	int label;

	pair<int, int> arc;

	bbNode_t bbNode;

	list<bbNode_t> nodeList;
	list<bbNode_t>::iterator it1;
	list<bbNode_t>::iterator it2;

	vector< pair<int, int> > inArcs;

	vector<int> tourSolution;
	vector<int> globalSolution;

	do{

		hungarian_init(hp, distM, size, size, m);
		
		if (counter < 1){
			bbNode = newBBNode(bbNode);
			bbNode.LB = hungarian_solve(hp);
			globalLB = bbNode.LB;
			cout << "\nCost of root: " << bbNode.LB;
		}

		else{

			label = 0;
			cout << "\nList1:\n";
			for (it2 = nodeList.begin(); it2 != nodeList.end(); it2++){
				label++;
				cout << "Nó " << label << ": " << endl;
				for(int j = 0; j < it2->fArcs.size(); j++){
					cout << it2->fArcs[j].first << " " << it2->fArcs[j].second << endl;
				}
			}
			
			it1 = nodeList.begin();
			for(int k = 0; k < it1->fArcs.size(); k++){
				hp->cost[it1->fArcs[k].first][it1->fArcs[k].second] = 99999;
			}

			it1->LB = hungarian_solve(hp);
			cost = it1->LB;
			cout << "\nCost of node: " << cost;
			
			if (cost >= globalUB){
				counter++;
				nodeList.erase(it1);
				continue;
			}
			
			if (cost > 0){
				if (cost < globalLB){
					globalLB = cost;
				}
			}

			inArcs.clear();
			
			inArcs = it1->fArcs;
			cout << "\nInherited Arcs: \n";

			for (int k = 0; k < inArcs.size(); k++){
				cout << inArcs[k].first << " " << inArcs[k].second << endl;
			}
			
			nodeList.erase(it1);
		}

		getVectorSolution(size, &bbNode, hp);

		if(bbNode.solution.size() <= 1){
			tourSolution = bbNode.solution[0];
		}

		verifyCicle(&bbNode, sCicle, cicle);

		if(!cicle){
			if (counter < 1){
				globalUB = globalLB;
				globalSolution = tourSolution;
			}
			else if(cost < globalUB){
				globalUB = cost;
				globalSolution = tourSolution;
 			}
		}

/*		cout << "\nGlobal LB: " << globalLB << "\n";
		cout << "\nGlobal UB: " << globalUB << "\n";*/
		cost = 0;

		if(cicle){
			for (int i = 0; i < sCicle.size() - 1; i++){
				bbNode = newBBNode(bbNode);
				arc.first = sCicle[i];
				arc.second = sCicle[i + 1];
				bbNode.fArcs.push_back(arc);
				if (counter > 0){
					for(int j = inArcs.size() - 1; j >= 0; j--){
						arc.first = inArcs[j].first;
						arc.second = inArcs[j].second;
						bbNode.fArcs.insert(bbNode.fArcs.begin(), arc);
					}				
				}
				nodeList.push_back(bbNode);
			}
		}		

		cout << endl;

		counter++;

	} while(!nodeList.empty());

	cout << "\n\n----X----\nBranch and Bound Breadth First\nFinal Solution Value: "<< globalUB;
	cout << "\n\nFinal Solution: ";
	printSolution(globalSolution);	
	cout << endl << endl;
}
