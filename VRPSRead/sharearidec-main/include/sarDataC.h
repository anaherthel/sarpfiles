/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#ifndef SarDataClasses_h
#define SarDataClasses_h

#include "bcModelingLanguageC.hpp"
#include "sarParameters.h"

enum VertexType {Parcel, Customer, Depot};
enum Scenario {ScSp, ScMp, McSp, McMp};

#include <cmath>

using namespace std;

struct InputVertex
{
public:

    int _id;
    double _pos_x;
    double _pos_y;
    double _service_time;
    int _demand;
    double _start_tw;
    double _end_tw;
    enum VertexType _type;

    InputVertex(int id, double x, double y, double s, int d, double st, double et, enum VertexType type) :
        _id(id), _pos_x(x), _pos_y(y), _service_time(s), _demand(d), _start_tw(st), _end_tw(et), _type(type)
    {}
};

struct Vertex
{
public:

    int _id;
    double _service_time;
    int _demand;
    double _start_tw;
    double _end_tw;
    enum VertexType _type;

    Vertex(int id, double s, int d, double st, double et, enum VertexType type) :
        _id(id), _service_time(s), _demand(d), _start_tw(st), _end_tw(et), _type(type)
    {}
};

// Directed graph
class Graph
{
    vector<Vertex> _V;
    vector<pair<int,int>> _A;
    map<pair<int, int>, double> _cost;
    map<pair<int, int>, double> _time;
    double _fixed_cost; 

public:
    Graph(int n, int m, int d, vector<InputVertex>& input_vertices);
    Graph(){};

    vector<pair<int,int>>& arcs() {return _A;} 
    std::vector<Vertex>& vertices(){ return _V; }

    double c(int i, int j)
    { 
        if(find(_A.begin(), _A.end(), make_pair(i, j)) == _A.end()) 
            return 10000.0;
        return _cost[make_pair(i, j)]; 
    }
    double t(int i, int j)
    { 
        if(find(_A.begin(), _A.end(), make_pair(i, j)) == _A.end()) 
            return 10000.0;
        return _time[make_pair(i, j)]; 
    }
    Vertex& getvertex(int vertexId) {return _V[vertexId];} 
       
};

class SarData
{
    int _n; //number of customers
    int _m; //number of parcels
    int _d; //number of depots
    int _K; //num vehicles available
    Graph _G;
    vector<InputVertex> _input_vertices;

public:
    SarData(const SarApplicationSpecificParam & applSpecParam, string instancePath);
    ~SarData(){};

    void readCSarpData(std::string path_file);

    Graph& graph() {return _G;}
    int nbcustomers() {return _n;} 
    int nbdepots() {return _d;}
    int nbparcels() {return _m;}
    int nbveh() {return _K;}
};

#endif /// SarDataClasses_h
