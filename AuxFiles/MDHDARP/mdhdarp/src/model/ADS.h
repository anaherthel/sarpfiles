#ifndef ADS_H
#define ADS_H

#include <cstring>
#include <map>
#include <set>
#include <vector>

#include "Data.h"
#include "MoveInfo.h"
#include "Sequence.h"
#include "Solution.h"
#include "Statistics.h"
#include "Utils.h"

using namespace std;
class ADS {
   private:
    Data *data;
    Statistics *stats;

    void updateVertexPosition(int r);
    void updateSequences(int r, int L, int H);
    void updatePairPositions(int r);
    void updateLoad(int r);
    void updateStartSearch(int r);
    void updateEndSearch(int r);

   public:
    Solution *solution;

    vector<vector<int> > vertexPosition;
    vector<vector<vector<Sequence> > > sequences;
    vector<Sequence> singleSequence;
    vector<vector<pair<bool, int> > > pairPosition;  //0 is pickup, 1 is delivery
    vector<vector<vector<int> > > load;
    vector<vector<int> > loadZeroIndexes;
    vector<vector<int> > startSearch;
    vector<vector<int> > endSearch;

    // Memoization Solution Descriptor
    map<double, int> MSD;
    map<double, int>::iterator itMSD;

    // Memoization Move Descriptor
    map<double, vector<MoveDesc> > MMDIntra;
    map<double, vector<MoveDesc> >::iterator itMMDIntra;

    map<pair<double, double>, vector<MoveDesc> > MMDInter;
    map<pair<double, double>, vector<MoveDesc> >::iterator itMMDInter;

    ADS(Data *data, Statistics *stats, Solution *solution);

    void updateSol(Solution *sol);
    void update();
    void update(int r, int L, int H);

    double evalRemoveRequest(int r, int idxPickup);
    double evalRemoveBlock(int r, int pos1, int pos2);
    double evalInsertionRequest(MoveInfo move_info, int idPickup);
    double evalExchangeInter(MoveInfo move_info);
    double evalTwoOpt(MoveInfo move_info);
    double evalExchangeVehicle(MoveInfo move_info);
    double evalExchangeBlock(MoveInfo move_info);
    double evalInsertionBlock(MoveInfo move_info);
    double evalExchangeIntra(MoveInfo move_info);
    double evalRelocateIntra(MoveInfo move_info);

    bool checkInsertionVertice(int r, int idx1, int idPickup);
    bool checkReinsertionVertice(int r, int idx1, int idx2, int idx3);

    // MSD methods
    bool useMSD(int routesSize);
    bool isInMSD(double key);
    void insertMSD(double key, int ils_start);
    int getMSD(double key);

    // MMD methods
    bool useMMD(int route_1, int route_2);
    void insertMMD(double key, MoveDesc move_desc);
    void insertMMD(pair<double, double> key, MoveDesc move_desc);
    MoveDesc getMMD(NEIGHBORHOOD neighborhood, double key);
    MoveDesc getMMD(NEIGHBORHOOD neighborhood, pair<double, double> key);

    void printADS(int r);
};

#endif