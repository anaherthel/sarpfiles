#include "MyCutCallback.h"
#include "NodeInfo.h"
#include "SubtourSep.h"
#include <iostream>
#include <vector>
#include <limits.h>
#include <stdlib.h>
#include <algorithm>

using namespace std;

//construtor da classe
MyCutCallback::MyCutCallback(IloEnv env, const IloArray <IloArray < IloBoolVarArray > >& x_ref, Data* data) : IloCplex::UserCutCallbackI(env), x(x_ref), 
data(data), sep_time(0), n_cuts(0)
{} 

//retorna uma cópia do callback. Este método é uma exigência do CPLEX.
IloCplex::CallbackI* MyCutCallback::duplicateCallback() const 
{ 
    return new (getEnv()) MyCutCallback(getEnv(), x, data); 
}

//código do callback que é executado pelo cplex.
void MyCutCallback::main() 
{
    NodeInfo *nodeData = dynamic_cast<NodeInfo*>(getNodeData()); 
    int depth;
    if(!nodeData){  
        depth = 0;
    }else{
        depth = nodeData->getDepth();
    }

    if(depth > 10)
        return;

    int n_nodes = (2*data->n);
    std::vector<IloConstraint> cons; 
    vector<vector<double>> solX(n_nodes, vector<double>(n_nodes, 0));

    for(int i = 0; i < n_nodes; i++){
        for(int j = i+1; j < n_nodes; j++){
            if(!data->A_[i][j]){ //If arc is valid
                for(int k = 0; k < data->v; ++k){
                    solX[i][j] += getValue(x[i][j][k]);
                    solX[j][i] += getValue(x[i][j][k]);
                }
            }if(!data->A_[j][i]){ //If arc is valid
                for(int k = 0; k < data->v; ++k){
                    solX[i][j] += getValue(x[j][i][k]);
                    solX[j][i] += getValue(x[j][i][k]);
                }
            }
        }
    }
    
    SubtourSep sep;
    clock_t t_ini = clock();
    //vector<vector<int>> cutsets = sep.separate(solX, n_nodes-2);
    vector<vector<int>> cutsets = sep.separate(solX, n_nodes);
    sep_time += clock() - t_ini; 

    //versao 1 do corte
    for(auto& S : cutsets){
        double lhs=0;
        IloExpr cut(getEnv());

        for(int v : S){
            for(int u : S){
                if(!data->A_[v][u]){
                    for(int k = 0; k < data->v; ++k){
                        cut += x[v][u][k];
                        lhs += getValue(x[v][u][k]);
                    }
                }
            }
        }
        cons.push_back(cut <= (int)S.size() - 1);
    }

    //adiciona as violacoes
    for(int i = 0; i < cons.size(); i++){
        cout << "adding subtour in cut" << endl;
        //   cout << cons[i] << endl;
        add(cons[i]).end();
    }
    n_cuts += cons.size();
}

double MyCutCallback::getSepTime()
{
    return (double)(sep_time)/CLOCKS_PER_SEC;
}
