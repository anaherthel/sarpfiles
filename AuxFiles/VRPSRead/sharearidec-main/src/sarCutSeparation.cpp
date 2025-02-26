/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#include "sarCutSeparation.h"
#include <cfloat>
#include <vector>
#include <unordered_set>

LazyCutSeparationRoutine::LazyCutSeparationRoutine(SarData &dataStruct) : _dataStruct(dataStruct)
{
    _cutCount = 0;
    _cutRound = 0;
}

LazyCutSeparationRoutine::~LazyCutSeparationRoutine()
{
}

void LazyCutSeparationRoutine::updateDynamicValues(BcSolution &primalSol)
{
    int nbCustomers = _dataStruct.nbcustomers();
    int nbDepots = _dataStruct.nbdepots();
    int nbParcels = _dataStruct.nbparcels();
    int nbVertices = nbDepots + nbCustomers + 2*nbParcels;
    std::set<BcVar> xvarList;

    primalSol.extractVar("X", xvarList);
    _xvals = std::vector<std::vector<double>>(nbVertices, std::vector<double>(nbVertices, 0.0));
    for (std::set<BcVar>::const_iterator varIt = xvarList.begin();
         varIt != xvarList.end(); ++varIt)
        _xvals[varIt->id().first()][varIt->id().second()] += (*varIt).solVal();
}

bool LazyCutSeparationRoutine::solIsInteger()
{
    for(int i = 0; i < _xvals.size(); i++)
    {
        for(int j = 0; j < _xvals[i].size(); j++)
        {
            if(1e-6 <= _xvals[i][j] && _xvals[i][j] <= 1 - 1e-6)
                return false;
        }
    }
    return true;
}

vector<vector<int>> LazyCutSeparationRoutine::extractRoutes()
{
    int nbCustomers = _dataStruct.nbcustomers();
    int nbParcels = _dataStruct.nbparcels();
    int nbDepots = _dataStruct.nbdepots();
    Graph& G = _dataStruct.graph();
    vector<pair<int,int>>& A = G.arcs();
    
    vector<bool> visited(G.vertices().size(), false);
    vector<vector<int>> routes;
    for(auto arc : A)
    {
        int i = arc.first, j = arc.second;
        if (i < nbDepots && _xvals[i][j] >= 0.99)
        {
            routes.push_back(vector<int>(1, i));
        }
    }
    for(auto& route : routes)
    {
        int curvertex = route[0];
        while(true)
        {
            for(auto arc : A)
            {
                int i = arc.first, j = arc.second;
                if(i == curvertex 
                    && _xvals[i][j] >= 0.99
                    && !visited[j])
                {
                    curvertex = j; 
                    break;
                }
            }
            route.push_back(curvertex);
            if(curvertex == route[0])
                break;
            visited[curvertex] = true;
        }
    }
    return routes;
}

TWSeqInfo concatenate_seqinfos(TWSeqInfo& seq1, TWSeqInfo& seq2, double arctime)
{
    double delta = seq1.duration - seq1.time_warp + arctime;
    double deltawt = max(seq2.earliest - delta - seq1.latest, 0.0);
    double deltatw = max(seq1.earliest + delta - seq2.latest, 0.0);
    double duration = seq1.duration + seq2.duration + deltawt + arctime;
    double time_warp = seq1.time_warp + seq2.time_warp + deltatw;
    double earliest = max(seq2.earliest - delta, seq1.earliest) - deltawt;
    double latest = min(seq2.latest - delta, seq1.latest) + deltatw;
    return TWSeqInfo(earliest, latest, time_warp, duration);
}

bool LazyCutSeparationRoutine::feasibleWrtDuration(std::vector<int>& route)
{
    Graph& G = _dataStruct.graph();
    Vertex curvertex = G.getvertex(route[0]);

    TWSeqInfo seqinfo(curvertex._start_tw, curvertex._end_tw, 0.0, 0.0);
    double s = 0.0;
    for(int i = 1; i < route.size(); i++)
    {
        s += G.t(route[i-1],route[i]);
        curvertex = G.getvertex(route[i]);
        TWSeqInfo nextseqinfo(curvertex._start_tw, curvertex._end_tw, 0.0, 0.0);
        seqinfo = concatenate_seqinfos(seqinfo, nextseqinfo, G.t(route[i-1],route[i]));
    }

    // std::cout << "["; 
    // for(int j : route)
    //     std::cout << j << " ";
    // std::cout << "]" << seqinfo.duration << " " << s << std::endl;
    return seqinfo.duration <= 8.00001;
}

int LazyCutSeparationRoutine::operator()(BcFormulation formPtr,
                                           BcSolution &primalSol,
                                           double &maxViolation,
                                           std::list<BcConstr> &cutList)
{
    _cutRound++;

    updateDynamicValues(primalSol);
    if(solIsInteger())
    {
        vector<vector<int>> routes = extractRoutes();
        BcCutConstrArray lazyConstr(formPtr, "LAZY");
        lazyConstr.sense('L');
        for(auto& route : routes)
        {
            if(!feasibleWrtDuration(route))
            {
                const BcFormulation & spForm = *next(formPtr.colGenSubProblemList().begin(), route[0]);
                BcVarArray X(spForm, "X");
                BcConstr lazyCut = lazyConstr(_cutCount);
                lazyCut <= route.size() - 2.0;
                for(int i = 1; i < route.size(); i++)
                    lazyCut += X[route[i-1]][route[i]];
                cutList.push_back(lazyCut);
                _cutCount++;
            }
        }
    }

    return _cutCount;
}