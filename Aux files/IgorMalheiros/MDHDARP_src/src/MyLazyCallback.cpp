#include "MyLazyCallback.h"
#include <iostream>
#include <vector>
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

//construtor da classe
MyLazyCallback::MyLazyCallback(IloEnv env, const IloArray<IloArray<IloBoolVarArray> >& x_ref, Data* data) : IloCplex::LazyConstraintCallbackI(env), x(x_ref), data(data)
{} 

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyLazyCallback::duplicateCallback() const { 
    return new (getEnv()) MyLazyCallback(getEnv(), x, data); 
}

//código do callback que é executado pelo cplex.
void MyLazyCallback::main() {
    int n_nodes = (2*data->n);
    std::vector<IloConstraint> cons; 

    vector<vector<int>> solX(n_nodes, vector<int>(n_nodes, 0));

    for(int i = 0; i < n_nodes; i++){
        for(int j = i+1; j < n_nodes; j++){
            double val = 0;
            if(!data->A_[i][j]){ //If arc is valid
                for(int k = 0; k < data->v; ++k){
                    val += getValue(x[i][j][k]);
                }
                if(val >= 0.9){
                    solX[i][j] += 1;
                    solX[j][i] += 1;
                } 
            }if(!data->A_[j][i]){
                double val = 0;
                for(int k = 0; k < data->v; ++k){
                    val += getValue(x[j][i][k]);
                }
                if(val >= 0.9){
                    solX[i][j] += 1;
                    solX[j][i] += 1;
                } 
            }
        }
    }


    //////////////////////////////////////////////////////////////////////////
    //SUBTOUR
    clock_t t_ini = clock();
    SubtourSep sep;
    vector<vector<int>> cutsets = sep.separateInteger(solX, n_nodes);
    sep_time += clock() - t_ini;

    for(auto& S : cutsets){
        IloExpr cut(getEnv());
        for(int v : S){
            for(int u : S){
                if(!data->A_[v][u]){
                    for(int k = 0; k < data->v; ++k){
                        cut += x[v][u][k];
                    }    
                }
            }
        }
        cons.push_back(cut <= (int)S.size() - 1);
    }
    //adiciona as violacoes 
    for(int i = 0; i < cons.size(); i++){
        add(cons[i]).end();
        //cout << "adding subtour in lazy\n";
    }
    n_cuts_subtour += cons.size();
}


double MyLazyCallback::getSepTime(){
    return (double)(sep_time)/CLOCKS_PER_SEC;
}
