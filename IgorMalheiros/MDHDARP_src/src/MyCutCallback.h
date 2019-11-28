#ifndef MY_CUT_CALLBACK_H
#define MY_CUT_CALLBACK_H

#include <ilcplex/ilocplex.h>
#include <vector>
#include <list>
#include <time.h>
//#include "SubtourSep.h"
//#include "Util.h"
#include "Data.h"
//#include "Exact.h"

/** CUT callback */ 
class MyCutCallback : public IloCplex::UserCutCallbackI 
{
    public:

        //construtor da classe: recebe o environmet, a referencia da variavel e o numero de clusters
        MyCutCallback(IloEnv env, const IloArray <IloArray < IloBoolVarArray > >& x_ref, Data* data); 

        //metodo que retorna uma copia do calback. Cplex exige este método.
        IloCplex::CallbackI* duplicateCallback() const;

        //no método main do callback está o código que será executado pelo cplex.
        void main(); 

        double getSepTime();
        inline int getNCuts(){return n_cuts;}

    private:

        //quando da construcao do objeto desta classe, x é usado para colocar todas as variaveis no vetor x_vars.
        IloArray <IloArray < IloBoolVarArray > > x;

        Data* data;

        clock_t sep_time;
        int n_cuts;
}; 

#endif
