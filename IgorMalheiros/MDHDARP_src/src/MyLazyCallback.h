#ifndef MY_LAZY_CALLBACK_2_H
#define MY_LAZY_CALLBACK_2_H

#include <ilcplex/ilocplex.h>
#include <vector>
#include <list>
#include <time.h>
#include "SubtourSep.h"
#include "Data.h"

/** Lazy callback */ 
class MyLazyCallback : public IloCplex::LazyConstraintCallbackI 
{
    public:
 
        //construtor da classe: recebe o environmet, a referencia da variavel e o numero de clusters
        MyLazyCallback(IloEnv env, const IloArray<IloArray<IloBoolVarArray> >& x_ref, Data* data); 

        //metodo que retorna uma copia do calback. Cplex exige este método.
        IloCplex::CallbackI* duplicateCallback() const;

        //no método main do callback está o código que será executado pelo cplex.
        void main(); 

        double getSepTime();

        inline int getNCutsSubtour(){return n_cuts_subtour;}
   
    private:

        //quando da construcao do objeto desta classe, x é usado para colocar todas as variaveis no vetor x_vars.
        IloArray <IloArray < IloBoolVarArray > > x;

        Data* data;

        clock_t sep_time;
        int n_cuts_subtour;

}; 

#endif
