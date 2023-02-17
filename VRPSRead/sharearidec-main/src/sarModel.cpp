/*
 *  BaPCod  -  a generic Branch-And-Price Code
 *
 *  VERSION %I% (%G%)
 *
 *  The Split delivery vehicle routing application was developed by Ruslan Sadykov and Teobaldo Bulhoes
 *  All Rights Reserved.
 */

#include "bcModelingLanguageC.hpp"
#include "bcModelRCSPSolver.hpp"
#include "bcModelNonPublicCuts.hpp"
#include "sarModelC.h"
#include "sarDataC.h"
#include "sarCutSeparation.h"

#include <vector>

BcSolverOracleFunctor * genRCSPSolver(BcFormulation & spForm, SarData & data,
                                                           SarApplicationSpecificParam & appParam)
{
    int nbCustomers = data.nbcustomers();
    int nbDepots = data.nbdepots();
    int nbParcels = data.nbparcels();
    int depotId = spForm.id().first();
    int setting = appParam.setting();
    Graph& G = data.graph();
    vector<pair<int,int>>& A = G.arcs();
    vector<Vertex>& V = G.vertices();

    BcVarArray xVar(spForm, "X");

    int nbPackingSets =  nbCustomers + 2*nbParcels;
    BcNetwork network(spForm, PathAttrMask, nbPackingSets, nbPackingSets);
    int sinkId = 1 + nbCustomers + 2*nbParcels;

    BcNetworkResource timeResource = BcNetworkResource(network, 0);
    timeResource.setAsMainResource();
    BcNetworkResource custResource = BcNetworkResource(network, 1);
    BcNetworkResource durationResource = BcNetworkResource(network, 2);
  
    double max_route_duration = 8.0;
    int max_nb_cust_with_parcels = (setting == ScSp || setting == ScMp)  ? 1 : nbCustomers;

    std::vector<BcVertex> verticesVector;
    for (int vertexId = 0; vertexId <= 1 + nbCustomers + 2*nbParcels; vertexId++)
    {
        BcVertex bcVertex = network.createVertex();
        verticesVector.push_back(bcVertex);
        if (vertexId == 0)     /// source
        {
            network.setPathSource(bcVertex);
        } else if (vertexId ==  sinkId)     /// sink
        {
            network.setPathSink(bcVertex);
        } else {
            bcVertex.setElementaritySet(vertexId - 1);
            bcVertex.setPackingSet(vertexId - 1);
        }
        bool isDepot = (vertexId == 0 || vertexId == sinkId);
        int vid = isDepot ? depotId : vertexId + nbDepots - 1;
        Vertex& v = G.getvertex(vid);
        timeResource.setVertexConsumptionLB(bcVertex, v._start_tw);
        timeResource.setVertexConsumptionUB(bcVertex, v._end_tw);
        custResource.setVertexConsumptionLB(bcVertex, 0);
        custResource.setVertexConsumptionUB(bcVertex, nbCustomers);
        durationResource.setVertexConsumptionLB(bcVertex, 0);
        durationResource.setVertexConsumptionUB(bcVertex, max_route_duration);
        for(int parcel_id = 1; parcel_id <= nbParcels; parcel_id++)
        {
            int res_lb = 0, res_ub = 1;
            if(isDepot)
               res_ub = 0;
            else if(vertexId == nbCustomers + nbParcels + parcel_id) //deliveryids[parcel_id]
               res_ub = 0;
            else if ((vertexId > nbCustomers) && (vertexId <= nbCustomers + nbParcels) 
                && (vertexId != nbCustomers + parcel_id) 
                && (setting == McSp || setting == ScSp))
               res_ub = 0;
            bcVertex.setBinaryResourceConsumptionLB(parcel_id - 1, res_lb);
            bcVertex.setBinaryResourceConsumptionUB(parcel_id - 1, res_ub);
            // if(depotId == 1)
            //     std::cout << "setting bounds for vertex " << vertexId << " res:" << parcel_id - 1
            //     << " lb:" << res_lb << " ub:" << res_ub << std::endl;
        }
    }
     for(int parcelIdx = 0; parcelIdx < nbParcels; parcelIdx++)
        network.setSpecialResourceNonDisposable(parcelIdx);
    //arcs leaving the depot
    //to customers
    for(int custIdx = 0; custIdx < nbCustomers; custIdx++)
    {
        BcArc bcArc = network.createArc(0, 1 + custIdx, 0);
        int tailId = depotId, headId = custIdx + nbDepots;
        timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
        durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
        custResource.setArcConsumption(bcArc, 1.0);
        bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
    }
    //to pickup nodes
    for(int parcelIdx = 0; parcelIdx < nbParcels; parcelIdx++)
    {
        BcArc bcArc = network.createArc(0, nbCustomers + parcelIdx + 1, 0);
        int tailId = depotId, headId = parcelIdx + nbDepots + nbCustomers;
        timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
        durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
        bcArc.addBinaryResourceConsumption(parcelIdx, 1);
        // if(depotId == 1)
        //     std::cout << "setting consumption 1 for arc " << 0 << " " << nbCustomers + parcelIdx + 1 <<
        //  " and res=" << parcelIdx << std::endl;
        bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
    }

    //arcs leaving customers
    for(int custIdx1 = 0; custIdx1 < nbCustomers; custIdx1++)
    {
        //to depot
        BcArc bcArc = network.createArc(1 + custIdx1, sinkId, 0);
        int tailId = custIdx1 + nbDepots, headId = depotId;
        timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
        durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
        bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        //to other customers
        for(int custIdx2 = 0; custIdx2 < nbCustomers; custIdx2++)
        {
            if(custIdx1 != custIdx2)
            {
                BcArc bcArc = network.createArc(1 + custIdx1, 1 + custIdx2, 0);
                int tailId = custIdx1 + nbDepots, headId = custIdx2 + nbDepots;
                timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
                durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
                custResource.setArcConsumption(bcArc, 1.0);
                bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
            }
        }
        //to pickup nodes
        for(int parcelIdx = 0; parcelIdx < nbParcels; parcelIdx++)
        {
            BcArc bcArc = network.createArc(1 + custIdx1, nbCustomers + parcelIdx + 1, 0);
            int tailId = custIdx1 + nbDepots, headId = parcelIdx + nbDepots + nbCustomers;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            custResource.setArcConsumption(bcArc, -nbCustomers);
            bcArc.addBinaryResourceConsumption(parcelIdx, 1);
        //     if(depotId == 1)
        //       std::cout << "setting consumption 1 for arc " << 1 + custIdx1 << " " << nbCustomers + parcelIdx + 1 <<
        //  " and res=" << parcelIdx << std::endl;
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
        //to delivery nodes
        for(int parcelIdx = 0; parcelIdx < nbParcels; parcelIdx++)
        {
            BcArc bcArc = network.createArc(1 + custIdx1, nbCustomers + nbParcels + parcelIdx + 1, 0);
            int tailId = custIdx1 + nbDepots, headId = parcelIdx + nbDepots + nbCustomers + nbParcels;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            custResource.setArcConsumption(bcArc, -nbCustomers);
            bcArc.addBinaryResourceConsumption(parcelIdx, -1);
        //     if(depotId == 1)
        //        std::cout << "setting consumption -1 for arc " << 1 + custIdx1 << " " << nbCustomers + nbParcels + parcelIdx + 1 <<
        //  " and res=" << parcelIdx << std::endl;
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
    }

    //arcs leaving pickup nodes
    for(int parcelIdx1 = 0; parcelIdx1 < nbParcels; parcelIdx1++)
    {
        //to customers
        for(int custIdx = 0; custIdx < nbCustomers; custIdx++)
        {
            BcArc bcArc = network.createArc(nbCustomers + parcelIdx1 + 1, 1 + custIdx, 0);
            int tailId = parcelIdx1 + nbDepots + nbCustomers, headId = custIdx + nbDepots;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            custResource.setArcConsumption(bcArc, nbCustomers - max_nb_cust_with_parcels + 1.0);
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
        //to other pickup nodes
        if(setting != ScSp && setting != McSp)
        {
            for(int parcelIdx2 = 0; parcelIdx2 < nbParcels; parcelIdx2++)
            {
                BcArc bcArc = network.createArc(nbCustomers + parcelIdx1 + 1, nbCustomers + parcelIdx2 + 1, 0);
                int tailId = parcelIdx1 + nbDepots + nbCustomers, headId = parcelIdx2 + nbDepots + nbCustomers;
                timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
                durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
                bcArc.addBinaryResourceConsumption(parcelIdx2, 1);
                bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
            }
        }
    }

    //arcs leaving delivery nodes
    for(int parcelIdx1 = 0; parcelIdx1 < nbParcels; parcelIdx1++)
    { 
        //to customers
        for(int custIdx = 0; custIdx < nbCustomers; custIdx++)
        {
            BcArc bcArc = network.createArc(nbCustomers + nbParcels + parcelIdx1 + 1, 1 + custIdx, 0);
            int tailId = parcelIdx1 + nbDepots + nbCustomers + nbParcels, headId = custIdx + nbDepots;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            custResource.setArcConsumption(bcArc, 1.0);
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
        //to other delivery nodes
        for(int parcelIdx2 = 0; parcelIdx2 < nbParcels; parcelIdx2++)
        {
            if(parcelIdx1 == parcelIdx2)
                continue;
            BcArc bcArc = network.createArc(nbCustomers + nbParcels + parcelIdx1 + 1, nbCustomers + nbParcels + parcelIdx2 + 1, 0);
            int tailId = parcelIdx1 + nbDepots + nbCustomers + nbParcels, headId = parcelIdx2 + nbDepots + nbCustomers + nbParcels;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            bcArc.addBinaryResourceConsumption(parcelIdx2, -1);
            // if(depotId == 1)
            //    std::cout << "setting consumption -1 for arc " << nbCustomers + nbParcels + parcelIdx1 + 1 <<
            //    " " << nbCustomers + nbParcels + parcelIdx2 + 1 <<
            //         " and res=" << parcelIdx2 << std::endl;
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
        //to pickup nodes
        for(int parcelIdx2 = 0; parcelIdx2 < nbParcels; parcelIdx2++)
        {
            if(parcelIdx1 == parcelIdx2)
                continue;
            BcArc bcArc = network.createArc(nbCustomers + nbParcels + parcelIdx1 + 1, nbCustomers +  parcelIdx2 + 1, 0);
            int tailId = parcelIdx1 + nbDepots + nbCustomers + nbParcels, headId = parcelIdx2 + nbDepots + nbCustomers;
            timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
            durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
            bcArc.addBinaryResourceConsumption(parcelIdx2, 1);
            // if(depotId == 1)
            //         std::cout << "setting consumption 1 for arc " << nbCustomers + nbParcels + parcelIdx1 + 1 << " " 
            //         << nbCustomers +  parcelIdx2 + 1 <<
            //         " and res=" << parcelIdx2 << std::endl;
            bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
        }
        //to depot
        BcArc bcArc = network.createArc(nbCustomers + nbParcels + parcelIdx1 + 1, sinkId, 0);
        int tailId = parcelIdx1 + nbDepots + nbCustomers + nbParcels, headId = depotId;
        timeResource.setArcConsumption(bcArc, G.t(tailId, headId));
        durationResource.setArcConsumption(bcArc, G.t(tailId, headId));
        bcArc.addVarAssociation(xVar(tailId, headId), 1.0);
    }

    // std::vector<std::vector<double> > distanceMatrix(nbCustomers, std::vector<double>(nbCustomers, 10e15));
    // for (int custId1 = 0; custId1 < nbCustomers; ++custId1)
    // {
    //     distanceMatrix[custId1][custId1] = 0;
    //     for (int custId2 = custId1 + 1; custId2 < nbCustomers; ++custId2)
    //     {
    //         distance = data.getCustToCustPreprDistance(depVehTpPairId, custId1, custId2);
    //         distanceMatrix[custId2][custId1] = distanceMatrix[custId1][custId2] = distance;
    //     }
    // }
    // /// this function is valid for covering sets too
    // network.setElemSetsDistanceMatrix(distanceMatrix);


    BcRCSPFunctor * functorPtr = new BcRCSPFunctor(spForm);
    //functorPtr->setParamTesting(appParam.testingParam());
    //functorPtr->setParamPrintLevel(2);

    return functorPtr;
}


void buildModel(SarData & data, BcModel & sarModel, SarApplicationSpecificParam & appParam)
{
    /* Compute an initial upper bound */
    BcObjectiveArray objective(sarModel);
    objective.setMinMaxStatus(BcObjStatus::minFloat);
    double cutoff = appParam.cutOffValue();

    objective() <= cutoff;
    objective().setArtCostValue(cutoff);

    int nbCustomers = data.nbcustomers();
    int nbParcels = data.nbparcels();
    int nbDepots = data.nbdepots();
    Graph& G = data.graph();
    vector<pair<int,int>>& A = G.arcs();

    BcMasterArray master(sarModel);

    BcConstrArray custDegreeConstr(master(), "CUSTDEG");
    for (int custId = 0; custId < nbCustomers; ++custId)
    {
        custDegreeConstr(custId) == 1.0;
    }
    BcConstrArray parcelDegreeConstr(master(), "PARCELDEG");
    for (int parcelId = 0; parcelId < nbCustomers; ++parcelId)
    {
        parcelDegreeConstr(parcelId) <= 1.0;
    }

    BcBranchingConstrArray vehNumberBranching(master(), "VNB", SelectionStrategy::MostFractional, 1.0);
    vehNumberBranching(0);

    BcColGenSpArray vehicleTypeSp(sarModel);

    for(int depotId = 0; depotId < nbDepots; depotId++)
    {
        BcFormulation spForm = vehicleTypeSp(depotId);
        spForm >= 0;
        spForm <= 1;

        BcVarArray xVar(spForm, "X");
        xVar.type('I');
        xVar.defineIndexName(MultiIndexNames('i'));
        xVar.priorityForMasterBranching(1);
        xVar.priorityForSubproblemBranching(-1);

        for (auto& arc : A)
        {
            BcVar bcVar = xVar(arc.first, arc.second);
            objective() += G.c(arc.first, arc.second) * bcVar;

            Vertex& head = G.getvertex(arc.second);
            if(head._type == Customer)
            {
                int cstrIdx = arc.second - nbDepots;
                custDegreeConstr[cstrIdx] += bcVar;
            }
            else if(head._type == Parcel && head._demand > 0)
            {
                int cstrIdx = arc.second - nbDepots - nbCustomers;
                parcelDegreeConstr[cstrIdx] += bcVar;
            }
          
            if(head._type == Depot)
                vehNumberBranching[0] += bcVar;
        }

        spForm.attach(genRCSPSolver(spForm, data, appParam));
    }

    BcLimMemRankOneCutConstrArray limMemRank1Cuts(master());

    BcCutConstrArray lazyConstr(master(), "LAZY", 'C', SelectionStrategy::MostViolated);
    // capacityConstr.setRootPriorityLevel(capCutPriority);
    LazyCutSeparationRoutine * separationRoutinePtr = new LazyCutSeparationRoutine(data);
    lazyConstr.attach(separationRoutinePtr);
}
