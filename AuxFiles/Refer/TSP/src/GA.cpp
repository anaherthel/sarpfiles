#include <cstdlib>
#include "Construction.h"
#include "GA.h"
#include "Utils.h"
#include <stdio.h>

using namespace std;

void CrossoverOX(double **distM, vector<individual_t> &pop, int size, int parent1)
{
	int partition1;
	int partition2;
	int limit;
	int position = 1;
	int k = 1;
	int parent2 = parent1 + 1;

	list<int> l1;
	list<int> l2;
	list<int>::iterator itr;

	individual_t offspring1;
	individual_t offspring2;

	offspring1.solution = pop[parent1].solution;
	offspring2.solution = pop[parent2].solution;

/*	cout << "\nFilho 1: ";
	printSolution(offspring1.solution);

	cout << "Filho 2: ";
	printSolution(offspring2.solution);*/

	limit = floor((size + 1)/ 2) - 1;
	
	partition1 = 1 + rand() % limit;
	partition2 = limit + 1 + rand() % (size - limit - 2);

/*	cout << "\npartition1: " << offspring1.solution[partition1] << endl;
	cout << "partition2: " << offspring1.solution[partition2] << endl;*/
	
	for(int i = partition2 + 1; i < offspring1.solution.size() - 1; i++) {
		l1.push_back(offspring1.solution[i]);
		l2.push_back(offspring2.solution[i]);
	}

	for(int i = 1; i <= partition2; i++) {
		l1.push_back(offspring1.solution[i]);
		l2.push_back(offspring2.solution[i]);
	}

/*	cout << "Lista 1: ";
	printList(l1);

	cout << "Lista 2: ";
	printList(l2);*/

	for (int i = offspring1.solution.size() - 2; i > partition2; i--) {
		offspring1.solution.erase(offspring1.solution.begin() + i);
		offspring2.solution.erase(offspring2.solution.begin() + i);
	}

	for (int i = partition1; i > 0; i--) {
		offspring1.solution.erase(offspring1.solution.begin() + i);
		offspring2.solution.erase(offspring2.solution.begin() + i);
	}

/*	cout << "\nFilho 1 apagando: ";
	printSolution(offspring1.solution);

	cout << "Filho 2 apagando: ";
	printSolution(offspring2.solution);*/

	for (int i = l1.size() + 1; i >= 0; i--) {
		for (int j = offspring2.solution.size() + 1; j >= 0; j--){
			if (offspring2.solution[j] == i){
				l1.remove(offspring2.solution[j]);
				break;
			}
		}
	}
/*	cout << "\nLista 1 após apagar: ";
	printList(l1);*/

	for (int i = l2.size() + 1; i >= 0; i--) {
		for (int j = offspring1.solution.size() + 1; j >= 0; j--){
			if (offspring1.solution[j] == i){
				l2.remove(offspring1.solution[j]);
				break;
			}
		}
	}
/*	cout << "Lista 2 após apagar: ";
	printList(l2);*/

	position = partition2 + 1;

	for (itr = l2.begin(); itr != l2.end(); itr++) {
		if (position >= size){
			offspring1.solution.insert(offspring1.solution.begin() + k, *itr);
			k++;
		}
		else{
			offspring1.solution.insert(offspring1.solution.end() - 1, *itr);
		}
		position++;
	}
/*	cout << "\nFilho 1: ";
	printSolution(offspring1.solution);*/
	
	position = partition2 + 1;
	k = 1;
	
	for (itr = l1.begin(); itr != l1.end(); itr++) {
		if (position >= size){
			offspring2.solution.insert(offspring2.solution.begin() + k, *itr);
			k++;
		}
		else{
			offspring2.solution.insert(offspring2.solution.end() - 1, *itr);
		}
		position++;
	}
/*	cout << "Filho 2: ";
	printSolution(offspring2.solution);*/

	offspring1.cost = costCalc(distM, offspring1.solution);
	offspring2.cost = costCalc(distM, offspring2.solution);

	pop.push_back(offspring1);
	pop.push_back(offspring2);
}

void Mutation(double **distM, vector<individual_t> &pop, int size)
{
	int tempElement = 0;
	int pos1 = 0;
	int pos2 = 0;

	for (int k = 0; k < pop.size(); k++){
		int prob = rand() % 4;
		if (prob <= 3) {
			pos1 = 1 + rand() % (size - 1);
			pos2 = 1 + rand() % (size - 1);
			pop[k].cost = costCalc(distM, pop[k].solution);
			tempElement = pop[k].solution[pos1];
			pop[k].solution[pos1] = pop[k].solution[pos2];
			pop[k].solution[pos2] = tempElement;
		}
	}

}

bool compareCosts(const individual_t &lhs, const individual_t &rhs)
{
    return lhs.cost < rhs.cost;
}