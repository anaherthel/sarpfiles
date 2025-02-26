/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#include "sarDataC.h"

#include <utility>
#include <vector>
#include <cstdio>
// #include "sarModelC.h"
// #include "bcModelSolutionC.hpp"
// #include "bcModelNetworkFlow.hpp"
// #include "bcNetworkFlowC.hpp"

using namespace std;

double distance(InputVertex& v1, InputVertex& v2)
{
   double x_sq = (v1._pos_x - v2._pos_x)*(v1._pos_x - v2._pos_x);
   double y_sq = (v1._pos_y - v2._pos_y)*(v1._pos_y - v2._pos_y);
   return sqrt(x_sq + y_sq);
}

double geodist(InputVertex& v1, InputVertex& v2)
{
    double PI = 3.141592, min, RRR = 6378.388, slat, flat, slong, flong, q1, q2, q3;
    int deg;
    
	deg = (int) v1._pos_x;
	min = v1._pos_x - deg;
	slat = v1._pos_x * PI / 180.0;
	
	deg = (int) v2._pos_x;
	min = v2._pos_x - deg;
	flat = v2._pos_x * PI/ 180.0;

	deg = (int) v1._pos_y;
	min = v1._pos_y - deg;
	slong = v1._pos_y * PI / 180.0;

	deg = (int) v2._pos_y;
	min = v2._pos_y - deg;
	flong = v2._pos_y * PI / 180;
   
    q1 = cos(flong - slong);
    q2 = cos(flat - slat);
    q3 = cos(flat + slat);

    return (RRR * acos( 0.5*((1.0+q1)*q2 - (1.0-q1)*q3)));
}

InputVertex* get_input_vertex(Vertex& vi, vector<InputVertex*>& depots, 
	vector<InputVertex*>& custpick, 
	vector<InputVertex*>& custdel, 
	vector<InputVertex*>& parcelpick, 
	vector<InputVertex*>& parceldel,
	int n, int m, int d, 
	bool ishead=false)
{
	if(vi._type == Customer)
	{
		if(ishead)
			return custpick[vi._id - d];
		else
			return custdel[vi._id - d];
	}
	else if(vi._type == Depot)
		return depots[vi._id];
	else
	{
		if(vi._demand < 0)
			return parceldel[vi._id - n - m - d];
		else
			return parcelpick[vi._id - n - d];
	}
}

double get_parcel_cost(int parcelid,
	vector<InputVertex*>& parcelpick, 
	vector<InputVertex*>& parceldel)
{
	return -2.74 - 0.83*distance(*parcelpick[parcelid], *parceldel[parcelid]);
}

void SarData::getInstanceType (string filename)
{
	instType.clear();
    string::size_type loc = filename.find_first_of("/");
    string::size_type loc2 = filename.find_last_of("/", filename.size());

    instType.append(filename, loc+1, loc2-loc-1 );
}

Graph::Graph(int n, int m, int d, string instType, vector<InputVertex>& input_vertices)
{
	vector<InputVertex*> depots, custpick, custdel, parcelpick, parceldel;
	double calcdist;

	for(auto& v : input_vertices)
	{
		if(v._type == Depot)
			depots.push_back(&v);
		else if(v._type == Customer && v._demand > 0)
			custpick.push_back(&v);
		else if(v._type == Customer && v._demand < 0)
			custdel.push_back(&v);
		else if(v._type == Parcel && v._demand > 0)
			parcelpick.push_back(&v);
		else if(v._type == Parcel && v._demand < 0)
			parceldel.push_back(&v);
	}

	// #depots
	int nextid = 0;
	for(auto invertex : depots)
	{
		_V.push_back(Vertex(nextid, 0, 0, invertex->_start_tw, invertex->_end_tw, Depot));
		nextid ++;
	}
	// #customers
	for(int i = 0; i < n; i++)
	{
		if (instType == "csarp")
			calcdist = 	distance(*custpick[i], *custdel[i]);
		else if (instType == "grubhub")
			calcdist = 	distance(*custpick[i], *custdel[i])/50;
		else if (instType == "sf_data")
			calcdist = 	geodist(*custpick[i], *custdel[i]);

		double service_time = 2*custpick[i]->_service_time + calcdist/41.0;
		_V.push_back(Vertex(nextid, service_time, 0, custpick[i]->_start_tw, custpick[i]->_start_tw, Customer));
		nextid ++;
	}

	// #parcels
	for(auto invertex : parcelpick)
	{
		_V.push_back(Vertex(nextid, invertex->_service_time, invertex->_demand, invertex->_start_tw, invertex->_end_tw, Parcel));
		nextid ++;
	}
	for(auto invertex : parceldel)
	{
		_V.push_back(Vertex(nextid, invertex->_service_time, invertex->_demand, invertex->_start_tw, invertex->_end_tw, Parcel));
		nextid ++;
	}

	for(auto& vi : _V)
	{
		for(auto& vj : _V)
		{
			if(vi._id != vj._id)
			{
				_A.push_back(make_pair(vi._id,vj._id));
				if(vj._type == Depot)
				{
					_cost[make_pair(vi._id,vj._id)] = 0.0;
					_time[make_pair(vi._id,vj._id)] = vi._service_time;
				}
				else
				{
					InputVertex* vout = get_input_vertex(vi, depots, custpick, custdel, 
						parcelpick, parceldel, n,  m,  d, false
					);
					InputVertex* vin = get_input_vertex(vj, depots, custpick, custdel, 
						parcelpick, parceldel, n,  m,  d, true
					);

					if (instType == "csarp"){
						calcdist = distance(*vout, *vin);
					}
						
					else if (instType == "grubhub"){
						cout << "in here:1 " << endl;

						calcdist = 	distance(*vout, *vin);
						calcdist = calcdist/50;
						cout << "in here:2 " << endl;
					}	
					else if (instType == "sf_data"){
						calcdist = 	geodist(*vout, *vin);
					}
					
					_cost[make_pair(vi._id,vj._id)] = 0.46*calcdist;
				
					if(vi._type == Parcel && vi._demand > 0)
						_cost[make_pair(vi._id,vj._id)] += get_parcel_cost(vi._id - n - d, parcelpick, parceldel);
					_time[make_pair(vi._id,vj._id)] = calcdist/41.0 + vi._service_time;
					
				}    
			}
		}
	}
	//custo fixo na FO relativo aos clientes
	_fixed_cost = -3.24*n;
	
	for(int i = 0; i < n; i++)
	{
		if (instType == "csarp")
			calcdist = distance(*custpick[i], *custdel[i]);
		else if (instType == "grubhub")
			calcdist = 	distance(*custpick[i], *custdel[i])/50;
		else if (instType == "sf_data")
			calcdist = 	geodist(*custpick[i], *custdel[i]);		

		_fixed_cost += (0.46 - 1.03)*calcdist;
	}
}

// SarData::SarData(const SarApplicationSpecificParam & applSpecParam, string instancePath)
// {
// 	readCSarpData(instancePath);
// }
SarData::SarData(string instancePath) //No parameters
{
	readCSarpData(instancePath);
}


void SarData::readCSarpData(string path_file)
{
	FILE* file = fopen(path_file.c_str(), "r");

	getInstanceType(path_file);

	double service_time;
	fscanf(file, "%d %lf %d %d", &_d, &service_time, &_n, &_m);
	service_time /= 60.0;
	_K = 1;

	int depotcount;
	if(instType == "csarp")
		depotcount = _d;
	else if (instType == "grubhub" || instType == "sf_data")
		depotcount = 1;

	for(int i = 0; i <= (depotcount + 2*(_n + _m) - 1); i++)
	{
		int id, demand, none;
		double x, y, start_tw, end_tw;
		if(instType == "csarp" || instType == "grubhub")
		{
			fscanf(file, "%d %lf %lf %d %lf %lf", &id, &x, &y,
				&demand, &start_tw, &end_tw);
		}

		else if (instType == "sf_data")
		{
			fscanf(file, "%d %lf %lf %*d %d %lf %lf", &id, &x, &y,
				&demand, &start_tw, &end_tw);
		}

		start_tw /= 60.0;    
		end_tw /= 60.0;
		enum VertexType type;
		if(demand == 0)
			type = Depot;
		// else if(i <= 2*_n - 1)
		else if(abs(demand) > 2)
			type = Customer;
		else
			type = Parcel;
		
		_input_vertices.push_back(InputVertex(id, x, y, service_time, demand, start_tw, end_tw, type));
	}

	_G = Graph(_n, _m, _d, instType, _input_vertices);
	fclose(file);
}
/*******************************************************************/
// Vertex

// Vertex::Vertex(const int & id, const double & coordx, const double & coordy):
//   _vId(id),_coordx(coordx),_coordy(coordy)
// {
// }

// int Vertex::getId() const
// {
//   return _vId;
// }

// double Vertex::getCoordx() const
// {
//   return _coordx;
// }

// double Vertex::getCoordy() const
// {
//   return _coordy;
// }

// double Vertex::computeDistFrom(Vertex * v, int roundMode) const
// {
//   double dist = sqrt( pow(_coordx - v->_coordx, 2) + pow(_coordy - v->_coordy, 2));
//   if (roundMode == DISTANCES_ROUND_MODE_NO)
//       return dist;
//   if (roundMode == DISTANCES_ROUND_MODE_DOWN)
//       return floor(dist);
//   else if (roundMode == DISTANCES_ROUND_MODE_NEAREST_INTEGER)
//       return round(dist);

//   return floor(dist * 10) / 10.0;
// }

// std::ostream& Vertex::print(std::ostream& os) const {
//   os << "Vertex " << _vId << "( " << _coordx << " , " << _coordy << " )" << std::endl;
//   return os;
// }



// /******************************************************************/
// // Site

// Site::Site(const int & id, const double & coordx, const double & coordy,
//            const int & demand, const int & serviceTime,
//            const int & timeWindowBegin, const int & timeWindowEnd) :
//   Vertex(id,coordx,coordy), _demand(demand), _serviceTime(serviceTime),
//   _timeWindowBegin(timeWindowBegin), _timeWindowEnd(timeWindowEnd),
//   _explDistToSite()
// {
// }

// Site::Site(const int & id, std::vector<double>  distances,
//            const int & demand, const int & serviceTime,
//            const int & timeWindowBegin, const int & timeWindowEnd) :
//   Vertex(id, 0.0, 0.0), _demand(demand), _serviceTime(serviceTime),
//   _timeWindowBegin(timeWindowBegin), _timeWindowEnd(timeWindowEnd),
//   _explDistToSite(std::move(distances))
// {
// }

// double Site::getExplDistToSite(const int & siteId) const
// {
//   return _explDistToSite[siteId];
// }

// int Site::getDemand() const
// {
//   return _demand;
// }

// void Site::setDemand(const int & demand)
// {
//   _demand = demand;
// }

// int Site::getServiceTime() const
// {
//   return _serviceTime;
// }

// int Site::getTimeWindowBegin() const
// {
//   return _timeWindowBegin;
// }

// int Site::getTimeWindowEnd() const
// {
//   return _timeWindowEnd;
// }

// void Site::setServiceTime(const int & serviceTime)
// {
//   _serviceTime = serviceTime;
// }

// std::ostream& Site::print(std::ostream& os) const {
//   os << "  Site " << std::endl;
//   Vertex::print(os);
//   os <<"  Customer with demand: " << _demand << std::endl;
//   return os;
// }

// /*******************************************************************/
// // DepotVehTypePair

// DepotVehTypePair::DepotVehTypePair(const int & id, const double & coordx, const double & coordy,
//                                    const int & minNumVehicles, const int & maxNumVehicles,
//                                    const int & capacity, const double & maxDistance,
//                                    const double & fixedCost, const double & costFactor):
//   Vertex(id,coordx,coordy), _minNumVehicles(minNumVehicles), _maxNumVehicles(maxNumVehicles),
//   _capacity(capacity), _maxDistance(maxDistance), _fixedCost(fixedCost),
//   _costFactor(costFactor), _canVisitAllSites(true), _explDistToSite(), _canVisitSite(), _greatestCommonDivisor(1)
// {
// }

// DepotVehTypePair::DepotVehTypePair(const int & id, std::vector<double>  distances,
//                                    const int & minNumVehicles, const int & maxNumVehicles,
//                                    const int & capacity, const double & maxDistance,
//                                    const double & fixedCost, const double & costFactor):
//   Vertex(id, 0.0, 0.0), _minNumVehicles(minNumVehicles), _maxNumVehicles(maxNumVehicles),
//   _capacity(capacity), _maxDistance(maxDistance), _fixedCost(fixedCost),
//   _costFactor(costFactor), _canVisitAllSites(true), _explDistToSite(std::move(distances)), _canVisitSite(),
//   _greatestCommonDivisor(0)
// {
// }

// double DepotVehTypePair::getExplDistToSite(const int & siteId) const
// {
//   return _explDistToSite[siteId];
// }

// bool DepotVehTypePair::canVisitSite(const int & siteId) const
// {
//   return (_canVisitAllSites || _canVisitSite[siteId]);
// }

// void DepotVehTypePair::setVisitedSites(const std::vector<bool> & canVisitSite)
// {
//   _canVisitAllSites = false;
//   _canVisitSite = canVisitSite;
// }

// int DepotVehTypePair::getCapacity() const
// {
//   return _capacity;
// }

// void DepotVehTypePair::setCapacity(const int & value)
// {
//     _capacity = value;
// }

// void DepotVehTypePair::decreaseMaxNumVehicles()
// {
//   _maxNumVehicles -= 1;
// }

// int DepotVehTypePair::maxNumVehicles() const
// {
//   return _maxNumVehicles;
// }

// int DepotVehTypePair::minNumVehicles() const
// {
//   return _minNumVehicles;
// }

// void DepotVehTypePair::setMinNumVehicles(const int & value)
// {
//   if (_minNumVehicles < value)
//     _minNumVehicles = value;
// }

// double DepotVehTypePair::getMaxDistance() const
// {
//   return _maxDistance;
// }

// double DepotVehTypePair::fixedCost() const
// {
//   return _fixedCost;
// }

// double DepotVehTypePair::costFactor() const
// {
//   return _costFactor;
// }

// void DepotVehTypePair::setGreatestCommonDivisor(int divisor)
// {
//     _greatestCommonDivisor = divisor;
// }

// int DepotVehTypePair::greatestCommonDivisor() const
// {
//     return _greatestCommonDivisor;
// }

// std::ostream& DepotVehTypePair::print(std::ostream& os) const
// {
//   os << "  Depot with max. " << _maxNumVehicles << " vehicles";
//   if (_minNumVehicles >= 0)
//     os << " with lower bound on the num. of vehicles " << _minNumVehicles << std::endl;
//   if (_capacity >= 0)
//     os << " with capacity " << _capacity << std::endl;
//   if (_maxDistance >= 0)
//     os << " with maximum distance " << _maxDistance << std::endl;
//   if (_costFactor != 1.0)
//     os << " with cost factor " << _costFactor << std::endl;
//   if (_fixedCost != 0.0)
//     os << " with fixedCost " << _fixedCost << std::endl;
//   return os;
//   Vertex::print(os);
//   return os;
// }


// /*******************************************************************/
// //Data structure

// SarData::SarData(const SarApplicationSpecificParam & applSpecParam) :
//         _fileName(), _depotVehTypePairPts(), _custPts(), _roundMode(applSpecParam.roundMode()),
//         _distanceIsExplicit(false), _instanceHasTimeWindows(false),
//         _unlimitedFleet(applSpecParam.unlimitedFleet()), _setOneInstance(false),
//         _model(applSpecParam.model()), _nbDiffDelQuantities(applSpecParam.nbDiffDeliveryQuantitiesPerCustomer()),
//         _maxK(), _avgRoutesLength(), _maxDemand(0.0f), _timeWindowRatio()
// {
// }

// SarData::~SarData()
// {
//   clearMemory();
// }

// void SarData::clearMemory()
// {
//   for (auto depPtrIt = _depotVehTypePairPts.begin(); depPtrIt != _depotVehTypePairPts.end(); ++depPtrIt)
//     delete *depPtrIt;
//   _depotVehTypePairPts.clear();

//   for (auto custPtrIt = _custPts.begin(); custPtrIt != _custPts.end(); ++custPtrIt)
//     delete *custPtrIt;
//   _custPts.clear();
// }

// bool SarData::readVRPTWData(const std::string & inputFileName)
// {
// 	std::ifstream ifs(inputFileName.c_str(), std::ios::in);
// 	if (!ifs) {
// 		std::cout << "cannot find input file: " << inputFileName << std::endl;
// 		exit(1);
// 	}
// 	if (ifs.eof()) {
// 		std::cout << "empty input file" << std::endl;
// 		exit(1);
// 	}

// 	std::string line;
// 	std::getline(ifs, line);

// 	switch (line.at(0)) {
// 		case 'r':
// 		case 'R':
// 		case 'c':
// 		case 'C':
// 			break;
// 		default:
// 			return false;
// 	}
// 	std::getline(ifs, line);
// 	std::getline(ifs, line);
// 	std::getline(ifs, line);
// 	int maxNbVehicles, capacity;
// 	ifs >> maxNbVehicles >> capacity;

// 	if ((maxNbVehicles <= 0) || (capacity <= 0) || ifs.eof() || ifs.bad())
// 		return false;

// 	std::getline(ifs, line);
// 	std::getline(ifs, line);
// 	std::getline(ifs, line);
// 	std::getline(ifs, line);

// 	while (true) {
// 		int customerId = -1, demand, readyTime, dueDate, serviceTime;
// 		double xCoord, yCoord;
// 		ifs >> customerId >> xCoord >> yCoord >> demand >> readyTime >> dueDate >> serviceTime;
// 		if ((customerId < 0) || ifs.eof() || ifs.bad())
// 			break;
// 		if (customerId == 0)
// 		{
// 			int minNbVehicles = 0;
// 			_depotVehTypePairPts.push_back(new DepotVehTypePair(0, xCoord, yCoord, minNbVehicles, maxNbVehicles,
// 																capacity, dueDate));
// 		}
// 		else
// 		{
// 			_custPts.push_back(new Site(customerId - 1, xCoord, yCoord, demand, serviceTime, readyTime, dueDate));
// 		}
// 	}
// 	if (_depotVehTypePairPts.empty() || _custPts.empty())
// 		return false;

// 	calculateGreatestCommonDivisors();

// 	cout << "VRPTW data file detected" << endl;

// 	return true;
// }

// #include  <random>
// #include  <iterator>

// template<typename Iter, typename RandomGenerator>
// Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
//     std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
//     std::advance(start, dis(g));
//     return start;
// }

// template<typename Iter>
// Iter select_randomly(Iter start, Iter end) {
//     static std::random_device rd;
//     static std::mt19937 gen(rd());
//     return select_randomly(start, end, gen);
// }

// void SarData::readData(const std::string & inputFileName)
// {
// 	if (readVRPTWData(inputFileName))
// 	{
// 		_instanceHasTimeWindows = true;
//         calculateShortestPathMatrix();

//         double sumTWSpan = 0.0f;

//         for (const auto & customer : _custPts)
//         {
//             sumTWSpan += (double)(customer->getTimeWindowEnd()-customer->getTimeWindowBegin());

//             if (_maxDemand < customer->getDemand())
//                 _maxDemand = customer->getDemand();
//         }

//         _timeWindowRatio = (sumTWSpan/(double)_custPts.size())/_depotVehTypePairPts[0]->getMaxDistance();

// 		return;
// 	}
// 	else
// 		clearMemory();

// 	_fileName = inputFileName;

// 	int slashPos = (int)(_fileName.size() -1 );

// 	while (_fileName[slashPos] != '/')
// 	{
// 	    slashPos -= 1;
// 	}

// 	if (_fileName[slashPos+1] == 'S' && _fileName[slashPos+2] == 'D')
// 	    _setOneInstance = true;

// 	FILE *f = fopen(_fileName.c_str(), "r");
// 	if (f == NULL) {
// 		cout << "Error : cannot open file " << inputFileName << endl;
// 		exit(1);
// 	}

// 	unsigned int dimension = 0;
// 	unsigned int temp;
// 	int capacity(0);
// 	char *lineBuf = new char [500];

// 	bool isCoordinateData = false;
// 	bool isFullMatrix = false;
// 	bool isLowerDiagRow = false;
// 	bool isUpperDiagRow = false;
// 	bool isUpperRow = false;
// 	bool isLowerRow = false;

// 	while (fgets(lineBuf, 500, f))
// 	{
// 		if (strncmp(lineBuf, "DIMENSION",strlen("DIMENSION")) == 0)
// 		{
// 			if ((sscanf(lineBuf, "DIMENSION : %d", &dimension) != 1)
// 					&& (sscanf(lineBuf, "DIMENSION: %d", &dimension) != 1))
// 			{
// 				cerr << "Error : cannot read DIMENSION" << endl;
// 				exit(1);
// 			}
// 		}
// 		if (strncmp(lineBuf, "CAPACITY",strlen("CAPACITY")) == 0)
// 		{
// 			if ((sscanf(lineBuf, "CAPACITY : %d", &capacity) != 1)
// 					&& (sscanf(lineBuf, "CAPACITY: %d", &capacity) != 1))
// 			{
// 				cerr << "Error : cannot read CAPACITY" << endl;
// 				exit(1);
// 			}
// 		}

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT: LOWER_ROW", strlen("EDGE_WEIGHT_FORMAT: LOWER_ROW")) == 0 )
// 			isLowerRow = true;
// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT : LOWER_ROW", strlen("EDGE_WEIGHT_FORMAT : LOWER_ROW")) == 0 )
// 			isLowerRow = true;

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT: UPPER_ROW", strlen("EDGE_WEIGHT_FORMAT: UPPER_ROW")) == 0 )
// 			isUpperRow = true;
// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT : UPPER_ROW", strlen("EDGE_WEIGHT_FORMAT : UPPER_ROW")) == 0 )
// 			isUpperRow = true;

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT: FULL_MATRIX", strlen("EDGE_WEIGHT_FORMAT: FULL_MATRIX")) == 0 )
// 			isFullMatrix = true;
// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT : FULL_MATRIX", strlen("EDGE_WEIGHT_FORMAT : FULL_MATRIX")) == 0 )
// 			isFullMatrix = true;

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT: LOWER_DIAG_ROW", strlen("EDGE_WEIGHT_FORMAT: LOWER_DIAG_ROW")) == 0 )
// 			isLowerDiagRow = true;
// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW", strlen("EDGE_WEIGHT_FORMAT : LOWER_DIAG_ROW")) == 0 )
// 			isLowerDiagRow = true;

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT: UPPER_DIAG_ROW", strlen("EDGE_WEIGHT_FORMAT: UPPER_DIAG_ROW")) == 0 )
// 			isUpperDiagRow = true;
// 		if (strncmp(lineBuf, "EDGE_WEIGHT_FORMAT : UPPER_DIAG_ROW", strlen("EDGE_WEIGHT_FORMAT : UPPER_DIAG_ROW")) == 0 )
// 			isUpperDiagRow = true;

// 		if (strncmp(lineBuf, "NODE_COORD_SECTION", strlen("NODE_COORD_SECTION")) == 0 )
// 		{
// 			isCoordinateData = true;
// 			break;
// 		}

// 		if (strncmp(lineBuf, "EDGE_WEIGHT_SECTION", strlen("EDGE_WEIGHT_SECTION")) == 0 )
// 			break;
// 	}

// 	if (dimension == 0)
// 	{
// 		cout << "Error : no DIMENSION specified" << endl;
// 		exit(1);
// 	}

// 	if (capacity == 0)
// 	{
// 		cout << "Error : no CAPACITY specified" << endl;
// 		exit(1);
// 	}

// 	_depotVehTypePairPts.resize(1, NULL);
// 	_custPts.resize(dimension - 1, NULL);
// 	int n, d;
// 	double x, y;
//     double x_depot = -1, y_depot = -1;
// 	std::vector<double> depot_dists;

// 	if(isCoordinateData)
// 	{
// 		while (fgets(lineBuf, 500, f))
// 		{
// 			if (sscanf(lineBuf,"%d %lf %lf",&n,&x,&y) == 3)
// 			{
// 				if (n == 1)
// 				{
// 					x_depot = x;
// 					y_depot = y;
// 				}
// 				else if ((n > 0) && (n <= dimension))
// 				{
// 						_custPts[n-2] = new Site(n-2, x, y);
// 				}
// 			}
// 			if (strncmp(lineBuf, "DEMAND_SECTION", strlen("DEMAND_SECTION")) == 0 )
// 				break;
// 		}
// 	}
// 	else
// 	{
// 		std::vector<std::vector<double> > distVectors(dimension, std::vector<double>(dimension, 0));

// 		if(isFullMatrix)
// 		{
// 			for (int i = 0; i < dimension; ++i)
// 			{
// 				double valueRead;
// 				for (int j = 0; j < dimension; ++j)
// 					if(fscanf(f, "%lf", &valueRead))
// 						distVectors[i][j] = valueRead;
// 			}
// 		}

// 		if(isLowerRow)
// 		{
// 			for (int i = 0; i < dimension; ++i)
// 			{
// 				double valueRead;
// 				for (int j = 0; j < i; ++j)
// 					if(fscanf(f, "%lf", &valueRead))
// 						distVectors[i][j] = distVectors[j][i] = valueRead;
// 			}
// 		}
// 		else if(isUpperRow)
// 		{
// 			for (int i = 0; i < dimension; ++i)
// 			{
// 				double valueRead;
// 				for (int j = i+1; j < dimension; ++j)
// 					if(fscanf(f, "%lf", &valueRead))
// 						distVectors[i][j] = distVectors[j][i] = valueRead;
// 			}
// 		}
// 		else if(isLowerDiagRow)
// 		{
// 			for (int i = 0; i < dimension; ++i)
// 			{
// 				double valueRead;
// 				for (int j = 0; j <= i; ++j)
// 					if(fscanf(f, "%lf", &valueRead))
// 						distVectors[i][j] = distVectors[j][i] = valueRead;
// 			}
// 		}
// 		else if(isUpperDiagRow)
// 		{
// 			for (int i = 0; i < dimension; ++i)
// 			{
// 				double valueRead;
// 				for (int j = i; j < dimension; ++j)
// 					if(fscanf(f, "%lf", &valueRead))
// 						distVectors[i][j] = distVectors[j][i] = valueRead;
// 			}
// 		}

// 		for (int i = 0; i < dimension; ++i)
// 		{
// 			std::vector<double> dists(distVectors[i].begin()+1, distVectors[i].end());
// 			if (i == 0)
// 			{
// 				depot_dists = dists;
// 			}
// 			else
// 			{
// 				_custPts[i-1] = new Site(i-1, dists);
// 			}
// 		}

// 		while(fgets(lineBuf, 500, f))
// 		{
// 			if (strncmp(lineBuf, "DEMAND_SECTION", strlen("DEMAND_SECTION")) == 0 )
// 			{
// 				break;
// 			}
// 		}
// 	}

// 	std::cout << "Number of vertices read : " << dimension << std::endl;

// 	double totalDemand = 0;
// 	while (fgets(lineBuf, 500, f))
// 	{
// 		if (sscanf(lineBuf,"%d %d",&n,&d) == 2)
// 		{
// 			if (n == 1)
// 				continue;
// 			if ((n > 0) && (n <= dimension))
// 			{
// 				_custPts[n-2]->setDemand(d);
// 				totalDemand += d;
// 			}
// 		}
// 		if (strncmp(lineBuf, "DEPOT_SECTION", strlen("DEPOT_SECTION")) == 0 )
// 			break;
// 	}

// 	int minNumberOfVehicles = (int)std::ceil(totalDemand/capacity);

//     int dimvehicle = minNumberOfVehicles;
//     if(_unlimitedFleet)
//     	dimvehicle = (int)dimension; //dimvehicle *= 2;
//     if(isCoordinateData)
//     {
//     	_depotVehTypePairPts[0]
// 							 = new DepotVehTypePair(0, x_depot, y_depot, 0, dimvehicle, capacity, -1);
//     }
//     else
//     {
//     		_depotVehTypePairPts[0]
//     					= new DepotVehTypePair(0, depot_dists, 0, dimvehicle, capacity, -1);
//     }

// 	fgets(lineBuf, 500, f);
// 	std::cout << "vehicle created : number = " << dimvehicle << "; capacity = " << capacity << std::endl;

// 	if (_depotVehTypePairPts[0] == NULL)
// 	{
// 		cerr << "Error : depot is not defined" << endl;
// 		exit(1);
// 	}
// 	for (int custId = 0; custId < dimension - 1; ++custId)
// 		if (_custPts[custId] == NULL)
// 		{
// 			cerr << "Error : customer " << custId + 2 << " is not defined" << endl;
// 			exit(1);
// 		}
// 		else if (getDemand(custId) < 0)
// 		{
// 			cerr << "Demand of customer " << custId + 2 << " is negative or not defined" << endl;
// 			exit(1);
// 		}

// 	fclose(f);

// 	delete [] lineBuf;

// 	_distanceIsExplicit = !isCoordinateData;

// 	if (!_instanceHasTimeWindows)
// 	{
// 	    calculateGreatestCommonDivisors();
//         calculateShortestPathMatrix();
// 	}

// 	std::cout << "End of reading." << std::endl;
// }

// double SarData::computeInitialUpperBound()
// {
//   double costUpperBound = 0;
//   int nbCustomers = _custPts.size();
//   int nbDepots = _depotVehTypePairPts.size();
//   for (int custId = 0; custId < nbCustomers; ++custId)
//     {
//       for (int custId2 = custId + 1; custId2 < nbCustomers; ++custId2)
//         costUpperBound += getCustToCustDistance(0, custId, custId2);
//       for (int depotId = 0; depotId < nbDepots; ++depotId)
//         costUpperBound += 2 * getDepotToCustDistance(depotId, custId);
//     }
//   return costUpperBound;
// }

// int SarData::getTotalDemand() const
// {
//   int nbCustomers = _custPts.size();
//   int totalDemand = 0;
//   for (int custId = 0; custId < nbCustomers; ++custId)
//     totalDemand += getDemand(custId);
//   return totalDemand;
// }

// bool SarData::printSolutionAndCheckIfFeasibleXD(BcSolution solution, const bool & bestSolution) const
// {
// 	std::cout << "------------------------------------------ " << std::endl;
// 	if (!solution.defined())
// 	{
// 		std::cout << "Solution is not defined!" << std::endl;
// 		std::cout << "------------------------------------------ " << std::endl;
// 		return false;
// 	}
// 	if (bestSolution)
// 		std::cout << "Best found solution of value " << solution.cost() << " : " << std::endl;
// 	else
// 		std::cout << "New solution of value " << solution.cost() << " : " << std::endl;

// 	solution = solution.next();   /// skip master solution, go straight to the subproblem solutions

// 	std::vector<std::vector<BcSolution> > solsByVehType(_depotVehTypePairPts.size());

// 	while (solution.defined())
// 	{
// 		int vehicleType = solution.formulation().id().first();
// 		solsByVehType[vehicleType].push_back(solution);
// 		solution = solution.next();
// 	}

// 	bool solutionIsFeasible = true;
// 	for (int vehType = 0; vehType < _depotVehTypePairPts.size(); ++vehType)
// 	{
// 		if (solsByVehType[vehType].empty())
// 			continue;

// 		BcFormulation spForm(solsByVehType[vehType].front().formulation());
// 		const BcNetwork network(spForm.network());
// 		BcVarArray dVar(spForm, "D");

// 		int vehicleNumber = 0;
// 		DepotVehTypePair * depVehTypePairPtr = _depotVehTypePairPts[vehType];
// 		std::vector<BcSolution>::iterator solIt;
// 		for (solIt = solsByVehType[vehType].begin(); solIt != solsByVehType[vehType].end(); ++solIt)
// 		{
// 			std::cout << "Vehicle " << ++vehicleNumber << " of type " << vehType+1 <<  " (" << solIt->cost() << ") : ";
// 			if(solIt->orderedIds().size() == 0)
// 			{
// 				std::cout << "\n";
// 				continue;
// 			}
// 			std::vector<int>::const_iterator arcIdIt = solIt->orderedIds().begin();

// 			int prevVertId = network.getArc(*arcIdIt).tail().ref();
// 			if (prevVertId != 0)
// 			{
// 				solutionIsFeasible = false; /// first vertex is not depot
// 				std::cout << prevVertId;
// 			}
// 			else
// 			{
// 				std::cout << "depot";
// 			}

// 			double cumTime = 0, cumDemand = 0;
// 			for (arcIdIt = solIt->orderedIds().begin(); arcIdIt != solIt->orderedIds().end(); ++arcIdIt)
// 			{
// 				int vertexId = network.getArc(*arcIdIt).head().ref();
// 				bool vertexIsDepot = (vertexId == 3*_custPts.size()+1);
// 				if(!vertexIsDepot)
// 				{
// 					if(vertexId % _custPts.size() == 0)
// 						vertexId = _custPts.size();
// 					else
// 						vertexId = vertexId % _custPts.size();
// 				}

// 				//split detected
// 				if(vertexId == prevVertId)
// 				{
// 				   prevVertId = vertexId;
// 				   continue;
// 				}
// 				else if(!vertexIsDepot)
// 				{
// 					cumDemand += solIt->getVarVal(dVar[vertexId]);
// 				}

// 				bool prevVertexIsDepot = (prevVertId == 0);
// 				std::cout << " -> ";
// 				double distance = 0;
// 				if (vertexIsDepot)
// 					distance = getDepotToCustDistance(vehType, prevVertId - 1);
// 				else if (prevVertexIsDepot)
// 					distance = getDepotToCustDistance(vehType, vertexId - 1);
// 				else
// 					distance = getCustToCustDistance(vehType, prevVertId - 1, vertexId - 1);
//      			cumTime += distance;
// 				if (vertexIsDepot)
// 					std::cout << "depot";
// 				else
// 					std:cout << vertexId;
// 				prevVertId = vertexId;
// 			}

// 			if ((prevVertId > 0) && (prevVertId <= _custPts.size()))
// 				solutionIsFeasible = false; /// last vertex is not depot
// 			std::cout << ", demand = " << cumDemand << ", time = " << cumTime << std::endl;
// 			if (cumDemand > depVehTypePairPtr->getCapacity())
// 				solutionIsFeasible = false; /// capacity is exceeded
// 		}
// 	}
// 	std::cout << "Solution is " << ((solutionIsFeasible) ? "" : "NOT ") << "feasible" << std::endl;
// 	std::cout << "------------------------------------------ " << std::endl;

// 	return solutionIsFeasible;
// }

// bool SarData::printSolutionAndCheckIfFeasibleXY(BcSolution solution, const bool & bestSolution) const
// {
// 	std::vector< std::vector<int> > splits;
// 	splits.resize( _custPts.size() + 1 );

// 	std::cout << "------------------------------------------ " << std::endl;
// 	if (!solution.defined())
// 	{
// 		std::cout << "Solution is not defined!" << std::endl;
// 		std::cout << "------------------------------------------ " << std::endl;
// 		return false;
// 	}
// 	if (bestSolution)
// 		std::cout << "Best found solution of value " << solution.cost() << " : " << std::endl;
// 	else
// 		std::cout << "New solution of value " << solution.cost() << " : " << std::endl;

// 	solution = solution.next();   /// skip master solution, go straight to the subproblem solutions

// 	std::vector<std::vector<BcSolution> > solsByVehType(_depotVehTypePairPts.size());

// 	while (solution.defined())
// 	{
// 		int vehicleType = solution.formulation().id().first();
// 		solsByVehType[vehicleType].push_back(solution);
// 		solution = solution.next();
// 	}

// 	bool solutionIsFeasible = true;
// 	for (int vehType = 0; vehType < _depotVehTypePairPts.size(); ++vehType)
// 	{
// 		if (solsByVehType[vehType].empty())
// 			continue;

// 		BcFormulation spForm(solsByVehType[vehType].front().formulation());
// 		const BcNetwork network(spForm.network());

// 		int vehicleNumber = 0;
// 		DepotVehTypePair * depVehTypePairPtr = _depotVehTypePairPts[vehType];
// 		std::vector<BcSolution>::iterator solIt;
// 		for (solIt = solsByVehType[vehType].begin(); solIt != solsByVehType[vehType].end(); ++solIt)
// 		{
// 			std::cout << "Vehicle " << ++vehicleNumber << " of type " << vehType+1 <<  " (" << solIt->cost() << ") : ";
// 			if (solIt->orderedIds().size() == 0)
// 			{
// 				std::cout << "\n";
// 				continue;
// 			}
// 			std::vector<int>::const_iterator arcIdIt = solIt->orderedIds().begin();
//                         const std::vector<std::vector<double> > & resConsVec = solIt->resConsumption();

// 			int prevVertId = network.getArc(*arcIdIt).tail().ref();

// 			if (prevVertId != 0)
// 			{
// 				solutionIsFeasible = false; /// first vertex is not depot
// 				std::cout << prevVertId;
// 			}
// 			else
// 			{
// 				std::cout << "depot";
// 			}

// 			double cumTime = 0, cumDemand = 0;
// 			int vertexPos = 1;
// 			for (arcIdIt = solIt->orderedIds().begin(); arcIdIt != solIt->orderedIds().end(); ++arcIdIt, vertexPos++)
// 			{
// 				int vertexId = network.getArc(*arcIdIt).head().ref();
// 				if (vertexId >= _custPts.size()+1 && (_model == EXTREME_ROUTE_XY || _model == EXTREME_ROUTE_XYS))
// 				     vertexId -= _custPts.size();
// 				bool vertexIsDepot = (vertexId == _custPts.size()+1);

// 				int d = resConsVec[vertexPos][0] - cumDemand;
// 				bool isSplit = !vertexIsDepot && (d != getDemand(vertexId - 1));
// 				if( !vertexIsDepot ){
// 				     splits[vertexId].push_back(vehicleNumber);
// 				     splits[vertexId].push_back(d);
// 				}

// 				bool prevVertexIsDepot = (prevVertId == 0);
// 				std::cout << " -> ";
// 				double distance = 0;
// 				if (vertexIsDepot)
// 				     distance = getDepotToCustDistance(vehType, prevVertId - 1);
// 				else if (prevVertexIsDepot)
// 				     distance = getDepotToCustDistance(vehType, vertexId - 1);
// 				else
// 				     distance = getCustToCustDistance(vehType, prevVertId - 1, vertexId - 1);
// 				cumTime += distance;
// 				cumDemand += d;
// 				if (vertexIsDepot){
// 				     std::cout << "depot";
// 				}
// 				else
// 				{
// 				     std::cout << vertexId << (isSplit ? "*" : "");
// 				}
// 				prevVertId = vertexId;
// 			}

// 			if ((prevVertId > 0) && (prevVertId <= _custPts.size()))
// 			     solutionIsFeasible = false; /// last vertex is not depot
// 			std::cout << ", demand = " << cumDemand << ", time = " << cumTime << std::endl;
// 			if (cumDemand > depVehTypePairPtr->getCapacity())
// 			     solutionIsFeasible = false; /// capacity is exceeded
// 		}
// 	}
//         std::cout << "Splits: " << std::endl;
// 	for(int vertexId = 1; vertexId <= _custPts.size(); vertexId++ )
// 	{
// 	     // if there is no information on vertex vector, something is wrong...
// 	     if( splits[vertexId].size() < 2 )
// 	     {
// 		  std::cout << "ERROR: client " << vertexId << " is not visited!" << std::endl;
// 		  solutionIsFeasible = false; 
// 	     }

// 	     //if vertex is visited only once and full demand delivered, no need to print it
// 	     if( splits[vertexId].size() == 2 || splits[vertexId][2] == getDemand(vertexId - 1) )
// 		  continue;

// 	     //if there are splits, print them and sum deliveries
// 	     int dem = 0;
// 	     std::cout << "  v" << vertexId << " (dem= " << getDemand(vertexId - 1) << ")   =>    ";
// 	     for(int i = 0; i < (int)splits[vertexId].size(); i += 2)
// 	     {
// 		  std::cout << "R" << splits[vertexId][i] << " (" << splits[vertexId][i+1] << ")   ";
// 		  dem += splits[vertexId][i+1];
// 	     }
// 	     std::cout << "  Delivered=" << dem  << std::endl;

// 	     if( dem < getDemand(vertexId - 1) )
// 	     {
// 		  solutionIsFeasible = false;
// 		  std::cout << "ERROR: client doesn't receive full demand" << std::endl;
//              }
// 	}

// 	std::cout << "Solution is " << ((solutionIsFeasible) ? "" : "NOT ") << "feasible" << std::endl;
// 	std::cout << "------------------------------------------ " << std::endl;

// 	return solutionIsFeasible;
// }

// bool SarData::printSolutionAndCheckIfFeasibleKPathBased(BcSolution solution, const bool & bestSolution) const
// {
//     std::vector< std::vector<int> > splits;
//     splits.resize( _custPts.size() + 1 );

//     std::cout << "------------------------------------------ " << std::endl;
//     if (!solution.defined())
//     {
//         std::cout << "Solution is not defined!" << std::endl;
//         std::cout << "------------------------------------------ " << std::endl;
//         return false;
//     }
//     if (bestSolution)
//         std::cout << "Best found solution of value " << solution.cost() << " : " << std::endl;
//     else
//         std::cout << "New solution of value " << solution.cost() << " : " << std::endl;

//     solution = solution.next();   /// skip master solution, go straight to the subproblem solutions

//     std::vector<std::vector<BcSolution> > solsByVehType(_depotVehTypePairPts.size());

//     while (solution.defined())
//     {
//         int vehicleType = solution.formulation().id().first();
//         solsByVehType[vehicleType].push_back(solution);
//         solution = solution.next();
//     }

//     int numCols = 0;
//     bool solutionIsFeasible = true;

//     int columnsListIndexer = 0;
//     std::vector<AuxColumn> columnsList;

//     for (int vehType = 0; vehType < _depotVehTypePairPts.size(); ++vehType)
//     {
//         if (solsByVehType[vehType].empty())
//             continue;

//         BcFormulation spForm(solsByVehType[vehType].front().formulation());
//         const BcNetwork network(spForm.network());

//         int vehicleNumber = 0;
//         DepotVehTypePair * depVehTypePairPtr = _depotVehTypePairPts[vehType];
//         std::vector<BcSolution>::iterator solIt;

//         numCols += (int)solsByVehType[vehType].size();

//         for (solIt = solsByVehType[vehType].begin(); solIt != solsByVehType[vehType].end(); ++solIt)
//         {
//             //std::cout << "Vehicle " << ++vehicleNumber << " of type " << vehType+1 <<  " (" << solIt->cost() << ") : ";
//             if(solIt->orderedIds().size() == 0)
//             {
//                 std::cout << "\n";
//                 continue;
//             }

//             columnsList.emplace_back(columnsListIndexer,
//                                      _depotVehTypePairPts[vehType]->getCapacity(),
//                                      solIt->getMultiplicity(),
//                                      solIt->cost());

//             std::vector<int>::const_iterator arcIdIt = solIt->orderedIds().begin();
//             const std::vector<std::vector<double> > & resConsVec = solIt->resConsumption();

//             int prevVertId = network.getArc(*arcIdIt).tail().ref();
//             int prevCustId = (prevVertId <= _custPts.size()) ? prevVertId - 1 : prevVertId - 2 - _custPts.size();

//             if (prevVertId != 0)
//             {
//                 solutionIsFeasible = false; /// first vertex is not depot
//                 std::cout << prevVertId;
//             }
//             else
//             {
//                 //std::cout << "depot";
//             }

//             double cumTime = 0;
//             int cumDemand = 0;
//             int vertexPos = 1;
//             for (arcIdIt = solIt->orderedIds().begin(); arcIdIt != solIt->orderedIds().end(); ++arcIdIt, vertexPos++)
//             {
//                 int vertexId = network.getArc(*arcIdIt).head().ref();
//                 int custId = (vertexId <= _custPts.size()) ?  vertexId - 1 : vertexId - 2 - _custPts.size();
//                 bool vertexIsDepot = (vertexId == _custPts.size() + 1);
//                 bool prevVertexIsDepot = (prevVertId == 0);
//                 //std::cout << " -> ";

//                 double distance = 0;
//                 if (vertexIsDepot)
//                     distance = getDepotToCustDistance(vehType, prevCustId);
//                 else if (prevVertexIsDepot)
//                     distance = getDepotToCustDistance(vehType, custId);
//                 else
//                     distance = getCustToCustDistance(vehType, prevCustId, custId);

//                 cumTime += floor(distance * 10) / 10;
//                 if (_instanceHasTimeWindows)
//                 {
//                     if (!vertexIsDepot && (cumTime < getTimeWindowBegin(custId)))
//                         cumTime = getTimeWindowBegin(custId);

//                     if (!vertexIsDepot && (cumTime > getTimeWindowEnd(custId)))
//                     {
//                         solutionIsFeasible = false;
//                         //std::cout << " !!!!!!! ";
//                     }

//                     if (vertexIsDepot && (cumTime > getMaxDistance(0)))
//                     {
//                         //std::cout << " !!!!!!! ";
//                         solutionIsFeasible = false;
//                     }
//                 }

//                 if (!vertexIsDepot)
//                 {
//                     double serviceTime = getServiceTime(custId);
//                     if (serviceTime > 0)
//                         cumTime += serviceTime;
//                     cumDemand += (vertexId <= _custPts.size()) ? getGreatestCommonDivisor(vehType) : getDemand(custId);
//                 }
//                 if (vertexIsDepot){
//                     //std::cout << "depot";
//                 }
// 				else {
// 					//std::cout << custId + 1;
//                     columnsList[columnsListIndexer]._setIds.push_back(custId);
// 				}

// 				prevVertId = vertexId;
// 				prevCustId = custId;
//             }
//             ++columnsListIndexer;

//             if ((prevVertId > 0) && (prevVertId <= _custPts.size()))
//                 solutionIsFeasible = false; /// last vertex is not depot
//             //std::cout << " demand = " << cumDemand << ", time = " << cumTime << std::endl;
//         }
//     }

//     if (numCols != (int)columnsList.size())
//     {
//         std::cout << "Something weird is happening!" << std::endl;
//         std::cout << "------------------------------------------ " << std::endl;
//         return false;
//     }

//     if (solutionIsFeasible)
//         solutionIsFeasible = printAndBuildSolutionAndCheckIfFeasibleGeneric(columnsList);

//     std::cout << "Solution is " << ((solutionIsFeasible) ? "" : "NOT ") << "feasible" << std::endl;
//     std::cout << "------------------------------------------ " << std::endl;

//     return solutionIsFeasible;
// }

// bool SarData::printAndBuildSolutionAndCheckIfFeasibleGeneric(std::vector<AuxColumn> & solutionColumns) const
// {
//     bool solutionIsCapacityFeasible = false;

//     int numCols = (int)solutionColumns.size();
//     int numElemSets = (int)_custPts.size();

//     double totalDemand = 0.0f;

//     for (int elemSet = 0; elemSet < numElemSets; ++elemSet)
//         totalDemand += _custPts[elemSet]->getDemand();

//     double edgeBigM = ceil(2*totalDemand);

//     std::vector<std::vector<double>> flowGraph( numCols+numElemSets+2,
//                                                 std::vector<double> ( numCols+numElemSets+2, 0.0f ) );

//     int sourceIdx = numCols+numElemSets;
//     int sinkIdx = numCols+numElemSets+1;

//     for (int i = 0; i < numCols; ++i)
//     {
//         flowGraph[sourceIdx][i] = solutionColumns[i]._capacity * solutionColumns[i]._multiplicity;
//     }

//     for (int i = 0; i < numElemSets; ++i)
//     {
//         flowGraph[numCols+i][sinkIdx] = _custPts[i]->getDemand();
//     }

//     for (const auto & col : solutionColumns)
//     {
//         for (const auto & setId : col._setIds)
//         {
//             flowGraph[col._index][numCols+setId] = edgeBigM;
//         }
//     }

//     //std::cout << "(int)flowGraph.size(): " << (int)flowGraph.size() << std::endl;
//     //for (auto line : flowGraph)
//     //{
//     //    for (auto col : line)
//     //    {
//     //        std::cout << col << " ";
//     //    }
//     //    std::cout << std::endl;
//     //}

//     bcp_rcsp::MinCutDigraph sepGraph( flowGraph );

//     sepGraph.findMinCut(sourceIdx, sinkIdx);

//     if (sepGraph.getMaxFlow() > totalDemand - 0.000001)
//     {
//         //std::cout << "The flow graph is covered and the solution is feasible!" << std::endl;
//         solutionIsCapacityFeasible = true;
//         int vehicleUsage = 0;
//         int delivery;

//         for (auto & col : solutionColumns)
//         {
//             for (const auto & setId : col._setIds)
//             {
//                 delivery = (int)sepGraph.getArcFlow(col._index, numCols+setId);
//                 vehicleUsage += delivery;
//                 col._deliveries.push_back(delivery);
//             }
//             printf("Vehicle %.2d (%d/%d), cost %.1f : depot -> ",
//                    col._index+1,
//                    vehicleUsage,
//                     //(int)sepGraph.getArcFlow(sourceIdx, col._index),
//                    col._capacity,
//                    col._cost);

//             for (const auto & setId : col._setIds)
//             {
//                 printf("%d (%d/%d) -> ", setId+1, (int)sepGraph.getArcFlow(col._index, numCols+setId),
//                        getDemand(setId));
//             }
//             printf("depot\n");
//             vehicleUsage = 0;
//         }
//     }
//     return solutionIsCapacityFeasible;
// }

// bool SarData::printSolutionAndCheckIfFeasibleMultiDelivery(BcSolution solution, const bool & bestSolution) const
// {
//     std::vector< std::vector<int> > splits;
//     splits.resize( _custPts.size() + 1 );
//     int nbCustomers = (int)_custPts.size();
//     bool multiArcModel = _model == STRONG_K_PATH_CUTS_BASED_MULTI_ARC;
//     int sourceVertId = 0;
//     int sinkVertId = (multiArcModel) ? 2 * nbCustomers + 1 : nbCustomers * _nbDiffDelQuantities + 1;

//     std::cout << "------------------------------------------ " << std::endl;
//     if (!solution.defined())
//     {
//         std::cout << "Solution is not defined!" << std::endl;
//         std::cout << "------------------------------------------ " << std::endl;
//         return false;
//     }
//     if (bestSolution)
//         std::cout << "Best found solution of value " << solution.cost() << " : " << std::endl;
//     else
//         std::cout << "New solution of value " << solution.cost() << " : " << std::endl;

//     solution = solution.next();   /// skip master solution, go straight to the subproblem solutions

//     std::vector<std::vector<BcSolution> > solsByVehType(_depotVehTypePairPts.size());

//     while (solution.defined())
//     {
//         int vehicleType = solution.formulation().id().first();
//         solsByVehType[vehicleType].push_back(solution);
//         solution = solution.next();
//     }

//     int numCols = 0;
//     bool solutionIsFeasible = true;

//     int columnsListIndexer = 0;
//     std::vector<AuxColumn> columnsList;

//     for (int vehType = 0; vehType < _depotVehTypePairPts.size(); ++vehType)
//     {
//         if (solsByVehType[vehType].empty())
//             continue;

//         BcFormulation spForm(solsByVehType[vehType].front().formulation());
//         const BcNetwork network(spForm.network());

//         int vehicleNumber = 0;
//         DepotVehTypePair * depVehTypePairPtr = _depotVehTypePairPts[vehType];
//         std::vector<BcSolution>::iterator solIt;

//         numCols += (int)solsByVehType[vehType].size();

//         for (solIt = solsByVehType[vehType].begin(); solIt != solsByVehType[vehType].end(); ++solIt)
//         {
//             std::cout << "Vehicle " << ++vehicleNumber << " of type " << vehType+1 <<  " (" << solIt->cost() << ") : ";
//             if (solIt->orderedIds().empty())
//             {
//                 std::cout << "\n";
//                 continue;
//             }

//             columnsList.emplace_back(columnsListIndexer,
//                                      _depotVehTypePairPts[vehType]->getCapacity(),
//                                      solIt->getMultiplicity(),
//                                      solIt->cost());

//             int prevVertId = network.getArc(solIt->orderedIds().front()).tail().ref();
//             int prevCustId = -1;

//             if (prevVertId != sourceVertId)
//             {
//                 solutionIsFeasible = false; /// first vertex is not depot
//                 std::cout << prevVertId;
//             }
//             else
//             {
//                 std::cout << "depot";
//             }

//             double cumTime = 0;
//             int cumDemand = 0;
//             int vertexPos = 1;
//             for (auto arcId : solIt->orderedIds())
//             {
//                 int vertexId = network.getArc(arcId).head().ref();
//                 bool vertexIsDepot = vertexId == sinkVertId;
//                 int custId = (vertexIsDepot) ? -1 /// -1 means depot
//                                              : (multiArcModel) ? (vertexId - 1) / 2
//                                                                : (vertexId - 1) / _nbDiffDelQuantities;
//                 bool prevVertexIsDepot = (prevVertId == sourceVertId);
//                 std::cout << " -> ";

//                 double distance = 0;
//                 if (vertexIsDepot)
//                     distance = getDepotToCustDistance(vehType, prevCustId);
//                 else if (prevVertexIsDepot)
//                     distance = getDepotToCustDistance(vehType, custId);
//                 else
//                     distance = getCustToCustDistance(vehType, prevCustId, custId);

//                 cumTime += floor(distance * 10) / 10;
//                 if (_instanceHasTimeWindows)
//                 {
//                     if (!vertexIsDepot && (cumTime < getTimeWindowBegin(custId)))
//                         cumTime = getTimeWindowBegin(custId);

//                     if (!vertexIsDepot && (cumTime > getTimeWindowEnd(custId)))
//                     {
//                         solutionIsFeasible = false;
//                         //std::cout << " !!!!!!! ";
//                     }

//                     if (vertexIsDepot && (cumTime > getMaxDistance(0)))
//                     {
//                         //std::cout << " !!!!!!! ";
//                         solutionIsFeasible = false;
//                     }
//                 }

//                 if (!vertexIsDepot)
//                 {
//                     double serviceTime = getServiceTime(custId);
//                     if (serviceTime > 0)
//                         cumTime += serviceTime;
//                     const BcArcInfo * arcInfoPtr = ((NetworkFlow *)network)->getArcInfoPtr(arcId);
//                     int resId = (_instanceHasTimeWindows) ? 1 : 0;
//                     cumDemand += arcInfoPtr->resConsumption[resId];
//                 }
//                 if (vertexIsDepot){
//                     std::cout << "depot";
//                 }
//                 else {
//                     std::cout << custId + 1;
//                     columnsList[columnsListIndexer]._setIds.push_back(custId);
//                 }

//                 prevVertId = vertexId;
//                 prevCustId = custId;
//             }
//             ++columnsListIndexer;

//             if ((prevVertId > 0) && (prevVertId <= _custPts.size()))
//                 solutionIsFeasible = false; /// last vertex is not depot
//             std::cout << ", demand = " << cumDemand << ", time = " << cumTime << std::endl;
//         }
//     }

//     if (numCols != (int)columnsList.size())
//     {
//         std::cout << "Something weird is happening!" << std::endl;
//         std::cout << "------------------------------------------ " << std::endl;
//         return false;
//     }

//     if (solutionIsFeasible)
//         solutionIsFeasible = printAndBuildSolutionAndCheckIfFeasibleGeneric(columnsList);

//     std::cout << "Solution is " << ((solutionIsFeasible) ? "" : "NOT ") << "feasible" << std::endl;
//     std::cout << "------------------------------------------ " << std::endl;

//     return solutionIsFeasible;
// }

// void SarData::calculateGreatestCommonDivisors()
// {
//     for (int vehType = 0; vehType < _depotVehTypePairPts.size(); ++vehType)
//     {
//         int result = _depotVehTypePairPts[vehType]->getCapacity();
//         for (int custId = 0; custId < _custPts.size(); ++custId)
//             result = gcd(result, _custPts[custId]->getDemand());
//         _depotVehTypePairPts[vehType]->setGreatestCommonDivisor(result);
//     }
// }

// void SarData::calculateShortestPathMatrix()
// {
//     double distance;

//     _shortestPathMatrix.resize( (int)_custPts.size()+1, std::vector<double>((int)_custPts.size()+1, 0.0));
//     for (int i = 0; i < (int)_custPts.size(); ++i)
//     {
//         for (int j = i+1; j < (int)_custPts.size(); ++j)
//         {
//             distance = getCustToCustDistance(0, i, j);
//             _shortestPathMatrix[i][j] = distance;
//             _shortestPathMatrix[j][i] = distance;
//         }
//     }
//     for (int i = 0; i < (int)_custPts.size(); ++i)
//     {
//         distance = getDepotToCustDistance(0, i);
//         _shortestPathMatrix[(int)_custPts.size()][i] = distance;
//         _shortestPathMatrix[i][(int)_custPts.size()] = distance;
//     }

//     if (_instanceHasTimeWindows || _roundMode == DISTANCES_ROUND_MODE_NEAREST_INTEGER)
//     {
//         for (int k = 0; k < (int)_shortestPathMatrix.size(); ++k)
//         {
//             for (int i = 0; i < (int)_shortestPathMatrix.size(); ++i)
//             {
//                 for (int j = 0; j < (int)_shortestPathMatrix.size(); ++j)
//                 {
//                     if (_shortestPathMatrix[i][j] > _shortestPathMatrix[i][k] + _shortestPathMatrix[k][j])
//                     {
//                         _shortestPathMatrix[i][j] = _shortestPathMatrix[i][k] + _shortestPathMatrix[k][j];
//                     }
//                 }
//             }
//         }
//     }
// }
