#include "ADS.h"

ADS::ADS(Data *data, Statistics *stats, Solution *solution) : data(data),
                                                              stats(stats),
                                                              solution(solution) {
    vector<int> extraCap(data->R, UNLIMITED_CAPACITY);

    int totalVhc = data->v + 1;  // One extra vhc for  perturb

    vertexPosition.resize(totalVhc);
    pairPosition.resize(totalVhc);
    loadZeroIndexes.resize(totalVhc);
    singleSequence.resize((data->n << 1) + data->v);
    sequences.resize(totalVhc);
    load.resize(totalVhc);
    startSearch.resize(totalVhc);
    endSearch.resize(totalVhc);

    for (int r = 0; r < totalVhc; r++) {
        vertexPosition[r].resize((data->n << 1) + 2);
        pairPosition[r].resize((data->n << 1) + 2);
        sequences[r].resize((data->n << 1) + 2);
        load[r].resize((data->n << 1) + 2);
        startSearch[r].resize(data->n << 1);
        endSearch[r].resize(data->n << 1);

        for (int i = 0; i < (data->n << 1) + 2; ++i) {
            sequences[r][i].resize((data->n << 1) + 2);
            load[r][i].resize(data->R);
        }
    }

    for (int i = 0; i < ((data->n << 1) + data->v); ++i) {
        Request *req      = &data->requests[i];
        singleSequence[i] = Sequence(data, req);
    }
}

void ADS::updateSol(Solution *sol) { this->solution = sol; }

void ADS::update(int r, int L, int H) {
    this->updateVertexPosition(r);
    this->updatePairPositions(r);
    this->updateSequences(r, L, H);
    this->updateLoad(r);
    this->updateStartSearch(r);
    this->updateEndSearch(r);
}

void ADS::update() {
    int totalVhc = solution->getRoutesSize();  // One extra vhc for  perturb

    for (int r = 0; r < totalVhc; ++r)
        this->update(r, 0, this->solution->getRoute(r)->getPathSize());
}

void ADS::updateVertexPosition(int r) {
    Route *route = this->solution->getRoute(r);
    int reqId;

    this->vertexPosition[r].assign(vertexPosition[r].size(), -1);

    for (int i = 1; i < route->getPathSize() - 1; i++) {
        reqId                          = route->getRequestIdByPos(i);
        this->vertexPosition[r][reqId] = i;
    }
}

void ADS::updatePairPositions(int r) {
    Route *route = this->solution->getRoute(r);
    int reqId;

    pairPosition[r][0] = make_pair(0, route->getPathSize() - 1);

    for (int i = 1; i < route->getPathSize() - 1; ++i) {
        reqId = route->getRequestIdByPos(i);

        if (reqId < data->n)
            pairPosition[r][i] =
                make_pair(0, this->vertexPosition[r][reqId + data->n]);
        else
            pairPosition[r][i] =
                make_pair(1, this->vertexPosition[r][reqId - data->n]);
    }

    pairPosition[r][route->getPathSize() - 1] = make_pair(1, 0);
}

void ADS::updateSequences(int r, int L, int H) {
    Route *route = this->solution->getRoute(r);

    for (int i = 0; i < route->getPathSize(); ++i)
        sequences[r][i][i] =
            singleSequence[route->getRequestIdByPos(i)];

    for (int i = 0; i < H; ++i) {
        for (int j = max(L, i + 1); j < route->getPathSize(); ++j) {
            sequences[r][i][j] = Sequence::concatenate(
                data, sequences[r][i][j - 1], sequences[r][j][j]);
        }
    }
}

void ADS::updateLoad(int r) {
    int request_index, cumulative_load;
    bool is_zero_point;
    Route *route = this->solution->getRoute(r);
    vector<int> load_requests, curr_loads;

    loadZeroIndexes[r].clear();
    loadZeroIndexes[r].push_back(0);

    load[r][0] = vector<int>(data->R, 0);

    for (int i = 1; i < route->getPathSize() - 1; ++i) {
        request_index = route->getRequestIdByPos(i);
        // The load request vector
        load_requests = data->requests[request_index].getResourceRequest();
        // The current load in vertex before i
        curr_loads = load[r][i - 1];

        is_zero_point = true;
        for (int j = 0; j < data->R; ++j) {
            curr_loads[j] += load_requests[j];
            is_zero_point &= (!curr_loads[j]);
        }

        load[r][i] = curr_loads;

        if (is_zero_point) {
            loadZeroIndexes[r].push_back(i);
        }
    }

    load[r][route->getPathSize() - 1] = vector<int>(data->R, 0);
}

void ADS::updateStartSearch(int r) {
    int u, v;
    int V        = data->n << 1;
    int last_pos = this->solution->getRoute(r)->getPathSize() - 2;

    for (u = 0; u < V; ++u) {
        startSearch[r][u] = 1;

        for (int pos = last_pos; pos >= 1; --pos) {
            v = this->solution->getRoute(r)->getRequestIdByPos(pos);
            if (!data->NCA[u][v]) {
                startSearch[r][u] = pos;
                break;
            }
        }
    }
}

void ADS::updateEndSearch(int r) {
    int u, v;
    int V        = data->n << 1;
    int last_pos = this->solution->getRoute(r)->getPathSize() - 1;

    for (int u = 0; u < V; ++u) {
        endSearch[r][u] = last_pos + 1;

        for (int pos = 1; pos < last_pos; ++pos) {
            v = this->solution->getRoute(r)->getRequestIdByPos(pos);
            if (!data->NCB[u][v]) {
                endSearch[r][u] = pos + 1;
                break;
            }
        }
    }
}

double ADS::evalRemoveRequest(int r, int idxPickup) {
    Route *route    = this->solution->getRoute(r);
    int idxDelivery = this->pairPosition[r][idxPickup].second;
    Vehicle *vhc    = route->getVehicle();
    Sequence seq;

    // If is an extra vhc
    if (route->getPenalCoef() > 1) {
        // If the delivery is exactly after the pickup
        if ((idxPickup + 1) == idxDelivery) {
            seq = Sequence::concatenate(
                data, this->sequences[r][0][idxPickup - 1],
                this->sequences[r][idxDelivery + 1][route->getPathSize() - 1]);
        } else {
            seq = Sequence::concatenate(
                data, this->sequences[r][0][idxPickup - 1],
                this->sequences[r][idxPickup + 1][idxDelivery - 1]);
            seq = Sequence::concatenate(
                data, seq,
                this->sequences[r][idxDelivery + 1][route->getPathSize() - 1]);
        }
    } else {
        int idPickupBefore =
            this->solution->getRoute(r)->getRequestIdByPos(idxPickup - 1);
        int idPickup =
            this->solution->getRoute(r)->getRequestIdByPos(idxPickup);
        int idPickupAfter =
            this->solution->getRoute(r)->getRequestIdByPos(idxPickup + 1);

        int idDeliveryBefore =
            this->solution->getRoute(r)->getRequestIdByPos(idxDelivery - 1);
        int idDelivery =
            this->solution->getRoute(r)->getRequestIdByPos(idxDelivery);
        int idDeliveryAfter =
            this->solution->getRoute(r)->getRequestIdByPos(idxDelivery + 1);

        // If the delivery is exactly after the pickup
        if ((idxPickup + 1) == idxDelivery) {
            return (route->getCost() +
                    data->c[idPickupBefore][idDeliveryAfter] -
                    data->c[idPickupBefore][idPickup] -
                    data->c[idPickup][idDelivery] -
                    data->c[idDelivery][idDeliveryAfter]);

        } else {
            return (route->getCost() + data->c[idPickupBefore][idPickupAfter] +
                    data->c[idDeliveryBefore][idDeliveryAfter] -
                    data->c[idPickupBefore][idPickup] -
                    data->c[idPickup][idPickupAfter] -
                    data->c[idDeliveryBefore][idDelivery] -
                    data->c[idDelivery][idDeliveryAfter]);
        }
    }

    return seq.getCost(vhc);
}

//  For Shift
double ADS::evalInsertionRequest(MoveInfo move_info, int pickup_id) {
    int route_idx   = move_info.route_2;
    int route_pos_1 = move_info.route_2_pos_1;
    int route_pos_2 = move_info.route_2_pos_2;

    Route *route    = this->solution->getRoute(route_idx);
    int delivery_id = pickup_id + data->n;

    if (data->A_[delivery_id][route->getRequestIdByPos(route_pos_2)] ||
        (route_pos_1 != route_pos_2 &&
         (data->A_[route->getRequestIdByPos(route_pos_2 - 1)][delivery_id] ||
          data->A_[pickup_id][route->getRequestIdByPos(route_pos_1)])))
        return INFEASIBLE;

    Vehicle *vhc = route->getVehicle();
    Sequence seq;

    seq = Sequence::concatenate(data, this->sequences[route_idx][0][route_pos_1 - 1],
                                singleSequence[pickup_id]);

    if (route_pos_1 != route_pos_2) {
        seq = Sequence::concatenate(data, seq,
                                    this->sequences[route_idx][route_pos_1][route_pos_2 - 1]);
    }

    seq = Sequence::concatenate(data, seq, singleSequence[delivery_id]);
    seq = Sequence::concatenate(
        data, seq, this->sequences[route_idx][route_pos_2][route->getPathSize() - 1]);

    if (!seq.isFeasible(vhc))
        return INFEASIBLE;

    return seq.getCost(vhc);
}

//  Feasible Shift Single
bool ADS::checkInsertionVertice(int r, int idx1, int idPickup) {
    Route *route = this->solution->getRoute(r);
    Vehicle *vhc = route->getVehicle();

    if (data->A_[route->getRequestIdByPos(idx1 - 1)][idPickup])
        return false;

    Sequence seq;

    seq = Sequence::concatenate(this->data, this->sequences[r][0][idx1 - 1],
                                singleSequence[idPickup]);

    if (idx1 == route->getPathSize() - 1)
        return seq.isFeasible(vhc);

    seq = Sequence::concatenate(
        this->data, seq, this->sequences[r][idx1][route->getPathSize() - 1]);

    return (seq.timewarp == 0);
}

//  For Reinsertion
double ADS::evalRelocateIntra(MoveInfo move_info) {
    int r                = move_info.route_1;
    int pickup_idx_rem   = move_info.route_1_pos_1;
    int delivery_idx_rem = move_info.route_1_pos_2;
    int pickup_idx_ins   = move_info.route_2_pos_1;
    int delivery_idx_ins = move_info.route_2_pos_2;

    Route *route = this->solution->getRoute(r);

    if (pickup_idx_rem == pickup_idx_ins &&
        delivery_idx_rem == delivery_idx_ins)
        return route->getCost();

    vector<pair<int, pair<int, char>>> indexes;

    int pickup_id   = route->getRequestIdByPos(pickup_idx_rem);
    int delivery_id = pickup_id + data->n;
    Vehicle *vhc    = route->getVehicle();

    indexes.push_back(make_pair(pickup_idx_ins, make_pair(pickup_id, 'I')));
    indexes.push_back(make_pair(delivery_idx_ins, make_pair(delivery_id, 'I')));
    indexes.push_back(make_pair(pickup_idx_rem, make_pair(1000, 'R')));
    indexes.push_back(make_pair(delivery_idx_rem, make_pair(1000, 'R')));

    sort(indexes.begin(), indexes.begin() + 4);

    int pos = 0;
    int len = indexes.size();
    int prox;

    Sequence seq;

    for (int i = 0; i < len; ++i) {
        prox = indexes[i].first;

        if (pos <= prox - 1) {
            if (i == 0)
                seq = this->sequences[r][pos][prox - 1];
            else
                seq = Sequence::concatenate(data, seq,
                                            this->sequences[r][pos][prox - 1]);
        }

        if (indexes[i].second.second == 'R')
            pos = prox + 1;
        else {
            if (indexes[i].second.first < data->n)
                seq = Sequence::concatenate(
                    data, seq,
                    this->sequences[r][pickup_idx_rem][pickup_idx_rem]);
            else
                seq = Sequence::concatenate(
                    data, seq,
                    this->sequences[r][delivery_idx_rem][delivery_idx_rem]);

            pos = prox;
        }
    }

    seq = Sequence::concatenate(
        data, seq, this->sequences[r][pos][route->getPathSize() - 1]);

    if (!seq.isFeasible(vhc))
        return INFEASIBLE;

    return seq.getCost(vhc);
}

//  Feasible Reinsertion Single
bool ADS::checkReinsertionVertice(int r, int pickup_idx_rem,
                                  int delivery_idx_rem, int pickup_idx_ins) {
    if (pickup_idx_rem == pickup_idx_ins)
        return true;

    Route *route = this->solution->getRoute(r);

    vector<pair<int, char>> indexes;

    Vehicle *vhc = route->getVehicle();

    indexes.push_back(make_pair(pickup_idx_ins, 'I'));
    indexes.push_back(make_pair(pickup_idx_rem, 'R'));
    indexes.push_back(make_pair(delivery_idx_rem, 'R'));

    sort(indexes.begin(), indexes.begin() + 3);

    int pos = 0;
    int len = indexes.size();
    int prox;

    Sequence seq;

    for (int i = 0; i < len; ++i) {
        prox = indexes[i].first;

        if (pos <= prox - 1) {
            if (i == 0)
                seq = this->sequences[r][pos][prox - 1];
            else
                seq = Sequence::concatenate(data, seq,
                                            this->sequences[r][pos][prox - 1]);
        }

        if (indexes[i].second == 'R')
            pos = prox + 1;
        else {
            seq = Sequence::concatenate(
                data, seq,
                this->sequences[r][pickup_idx_rem][pickup_idx_rem]);
            pos = prox;
        }
    }

    if (pos != route->getPathSize() - 1)
        seq = Sequence::concatenate(
            data, seq, this->sequences[r][pos][route->getPathSize() - 1]);

    return (seq.timewarp == 0);
}

double ADS::evalExchangeInter(MoveInfo move_info) {
    int route_1_idx = move_info.route_1;
    int route_2_idx = move_info.route_2;

    Route *route_1 = this->solution->getRoute(route_1_idx);
    Route *route_2 = this->solution->getRoute(route_2_idx);

    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    int route_1_pickup_id   = route_1->getRequestIdByPos(route_1_pos_1);
    int route_1_delivery_id = route_1_pickup_id + data->n;
    int route_2_pickup_id   = route_2->getRequestIdByPos(route_2_pos_1);
    int route_2_delivery_id = route_2_pickup_id + data->n;

    Vehicle *vhc_1 = route_1->getVehicle();
    Vehicle *vhc_2 = route_2->getVehicle();

    if ((route_1->getPenalCoef() == 1 &&
         (data->A_[route_1->getRequestIdByPos(route_1_pos_1 - 1)][route_2_pickup_id] ||
          data->A_[route_2_delivery_id][route_1->getRequestIdByPos(route_1_pos_2 + 1)] ||
          ((route_1_pos_1 + 1) != route_1_pos_2) &&
              (data->A_[route_2_pickup_id][route_1->getRequestIdByPos(route_1_pos_1 + 1)] ||
               data->A_[route_1->getRequestIdByPos(route_1_pos_2 - 1)][route_2_delivery_id]))) ||
        (route_2->getPenalCoef() == 1 &&
         (data->A_[route_2->getRequestIdByPos(route_2_pos_1 - 1)][route_1_pickup_id] ||
          data->A_[route_1_delivery_id][route_2->getRequestIdByPos(route_2_pos_2 + 1)] ||
          ((route_2_pos_1 + 1) != route_2_pos_2) &&
              (data->A_[route_1_pickup_id][route_2->getRequestIdByPos(route_2_pos_1 + 1)] ||
               data->A_[route_2->getRequestIdByPos(route_2_pos_2 - 1)][route_1_delivery_id])))) {
        return INFEASIBLE;
    }

    Sequence seq_1;
    Sequence seq_2;
    // Seq 1
    seq_1 = Sequence::concatenate(data, this->sequences[route_1_idx][0][route_1_pos_1 - 1],
                                  singleSequence[route_2_pickup_id]);

    // If the delivery is exactly after the pickup
    if ((route_1_pos_1 + 1) != route_1_pos_2)
        seq_1 = Sequence::concatenate(data, seq_1,
                                      this->sequences[route_1_idx][route_1_pos_1 + 1][route_1_pos_2 - 1]);

    seq_1 = Sequence::concatenate(data, seq_1, singleSequence[route_2_delivery_id]);
    seq_1 = Sequence::concatenate(
        data, seq_1, this->sequences[route_1_idx][route_1_pos_2 + 1][route_1->getPathSize() - 1]);

    if (route_1->getPenalCoef() == 1 && !seq_1.isFeasible(vhc_1))
        return INFEASIBLE;

    // Seq 2
    seq_2 = Sequence::concatenate(data, this->sequences[route_2_idx][0][route_2_pos_1 - 1],
                                  singleSequence[route_1_pickup_id]);

    // If the delivery is exactly after the pickup
    if ((route_2_pos_1 + 1) != route_2_pos_2)
        seq_2 = Sequence::concatenate(data, seq_2,
                                      this->sequences[route_2_idx][route_2_pos_1 + 1][route_2_pos_2 - 1]);

    seq_2 = Sequence::concatenate(data, seq_2, singleSequence[route_1_delivery_id]);
    seq_2 = Sequence::concatenate(
        data, seq_2, this->sequences[route_2_idx][route_2_pos_2 + 1][route_2->getPathSize() - 1]);

    if (route_2->getPenalCoef() == 1 && !seq_2.isFeasible(vhc_2))
        return INFEASIBLE;

    return (seq_1.getCost(vhc_1) + seq_2.getCost(vhc_2));
}

// Exchange only one vertex
double ADS::evalExchangeIntra(MoveInfo move_info) {
    int route_idx = move_info.route_1;
    int pos_1     = min(move_info.route_1_pos_1, move_info.route_1_pos_2);
    int pos_2     = max(move_info.route_1_pos_1, move_info.route_1_pos_2);

    Route *route = this->solution->getRoute(route_idx);

    Vehicle *vhc = route->getVehicle();

    int vertex_1_id = route->getRequestIdByPos(pos_1);
    int vertex_2_id = route->getRequestIdByPos(pos_2);

    Sequence seq_1 = this->singleSequence[vertex_1_id];
    Sequence seq_2 = this->singleSequence[vertex_2_id];

    Sequence seq;

    if (pos_1 < pos_2) {
        seq = this->sequences[route_idx][0][pos_1 - 1];
        seq = Sequence::concatenate(data, seq, seq_2);

        if (pos_1 + 1 != pos_2)
            seq = Sequence::concatenate(
                data, seq, this->sequences[route_idx][pos_1 + 1][pos_2 - 1]);

        seq = Sequence::concatenate(data, seq, seq_1);
        seq = Sequence::concatenate(
            data, seq, this->sequences[route_idx][pos_2 + 1][route->getPathSize() - 1]);
    }

    if (!seq.isFeasible(vhc))
        return INFEASIBLE;

    return seq.getCost(vhc);
}

double ADS::evalTwoOpt(MoveInfo move_info) {
    int route_1_idx = move_info.route_1;
    int route_2_idx = move_info.route_2;

    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_2_pos_1 = move_info.route_2_pos_1;

    Route *route_1 = this->solution->getRoute(route_1_idx);
    Route *route_2 = this->solution->getRoute(route_2_idx);

    if (data->A_[route_1->getRequestIdByPos(route_1_pos_1)][route_2->getRequestIdByPos(route_2_pos_1 + 1)] ||
        data->A_[route_2->getRequestIdByPos(route_2_pos_1)][route_1->getRequestIdByPos(route_1_pos_1 + 1)])
        return INFEASIBLE;

    Sequence seq_1;
    Sequence seq_2;

    if (route_2_pos_1 + 1 == route_2->getPathSize() - 1)
        seq_1 = sequences[route_1_idx][0][route_1_pos_1];
    else
        seq_1 = Sequence::concatenate(
            data, this->sequences[route_1_idx][0][route_1_pos_1],
            this->sequences[route_2_idx][route_2_pos_1 + 1][route_2->getPathSize() - 2]);

    seq_1 = Sequence::concatenate(data, seq_1,
                                  this->sequences[route_1_idx][route_1->getPathSize() - 1]
                                                 [route_1->getPathSize() - 1]);

    if (!seq_1.isFeasible(route_1->getVehicle()))
        return INFEASIBLE;

    if (route_1_pos_1 + 1 == route_1->getPathSize() - 1)
        seq_2 = this->sequences[route_2_idx][0][route_2_pos_1];
    else
        seq_2 = Sequence::concatenate(
            data, this->sequences[route_2_idx][0][route_2_pos_1],
            this->sequences[route_1_idx][route_1_pos_1 + 1][route_1->getPathSize() - 2]);

    seq_2 = Sequence::concatenate(data, seq_2,
                                  this->sequences[route_2_idx][route_2->getPathSize() - 1]
                                                 [route_2->getPathSize() - 1]);

    if (!seq_2.isFeasible(route_2->getVehicle()))
        return INFEASIBLE;

    return seq_1.getCost(route_1->getVehicle()) +
           seq_2.getCost(route_2->getVehicle());
}

double ADS::evalExchangeVehicle(MoveInfo move_info) {
    int route_1_idx = move_info.route_1;
    int route_2_idx = move_info.route_2;

    Route *route_1 = this->solution->getRoute(route_1_idx);
    Route *route_2 = this->solution->getRoute(route_2_idx);

    Vehicle *vhc_1 = route_1->getVehicle();
    Vehicle *vhc_2 = route_2->getVehicle();

    Sequence seq_1 = Sequence::concatenate(
        data, singleSequence[(data->n << 1) + vhc_2->getId()],
        this->sequences[route_1_idx][1][route_1->getPathSize() - 2]);

    seq_1 = Sequence::concatenate(
        data, seq_1, singleSequence[(data->n << 1) + vhc_2->getId()]);

    if (!seq_1.isFeasible(vhc_2))
        return INFEASIBLE;

    Sequence seq_2 = Sequence::concatenate(
        data, singleSequence[(data->n << 1) + vhc_1->getId()],
        this->sequences[route_2_idx][1][route_2->getPathSize() - 2]);

    seq_2 = Sequence::concatenate(
        data, seq_2, singleSequence[(data->n << 1) + vhc_1->getId()]);

    if (!seq_2.isFeasible(vhc_1))
        return INFEASIBLE;

    return seq_1.getCost(vhc_2) + seq_2.getCost(vhc_1);
}

double ADS::evalExchangeBlock(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;
    int route_2_pos_2 = move_info.route_2_pos_2;

    Route *route_1 = this->solution->getRoute(route_1_idx);
    Route *route_2 = this->solution->getRoute(route_2_idx);

    Vehicle *vhc_1 = route_1->getVehicle();
    Vehicle *vhc_2 = route_2->getVehicle();

    if ((route_1->getPenalCoef() == 1 && route_2->getPenalCoef() == 1) &&
        (data->A_[route_1->getRequestIdByPos(route_1_pos_1 - 1)][route_2->getRequestIdByPos(route_2_pos_1)] ||
         data->A_[route_2->getRequestIdByPos(route_2_pos_2)][route_1->getRequestIdByPos(route_1_pos_2 + 1)] ||
         data->A_[route_2->getRequestIdByPos(route_2_pos_1 - 1)][route_1->getRequestIdByPos(route_1_pos_1)] ||
         data->A_[route_1->getRequestIdByPos(route_1_pos_2)][route_2->getRequestIdByPos(route_2_pos_2 + 1)]))
        return INFEASIBLE;

    Sequence seq_1 =
        Sequence::concatenate(data, this->sequences[route_1_idx][0][route_1_pos_1 - 1],
                              this->sequences[route_2_idx][route_2_pos_1][route_2_pos_2]);
    seq_1 = Sequence::concatenate(
        data, seq_1,
        this->sequences[route_1_idx][route_1_pos_2 + 1][route_1->getPathSize() - 1]);

    if (route_1->getPenalCoef() == 1 && !seq_1.isFeasible(vhc_1))
        return INFEASIBLE;

    Sequence seq_2 =
        Sequence::concatenate(data, this->sequences[route_2_idx][0][route_2_pos_1 - 1],
                              this->sequences[route_1_idx][route_1_pos_1][route_1_pos_2]);
    seq_2 = Sequence::concatenate(
        data, seq_2,
        this->sequences[route_2_idx][route_2_pos_2 + 1][route_2->getPathSize() - 1]);

    if (route_2->getPenalCoef() == 1 && !seq_2.isFeasible(vhc_2))
        return INFEASIBLE;

    return seq_1.getCost(vhc_1) + seq_2.getCost(vhc_2);
}

double ADS::evalRemoveBlock(int r, int pos1, int pos2) {
    Route *route = this->solution->getRoute(r);
    Vehicle *vhc = route->getVehicle();

    Sequence seq = Sequence::concatenate(
        data, this->sequences[r][0][pos1 - 1],
        this->sequences[r][pos2 + 1][route->getPathSize() - 1]);

    return seq.getCost(vhc);
}

double ADS::evalInsertionBlock(MoveInfo move_info) {
    int route_1_idx   = move_info.route_1;
    int route_2_idx   = move_info.route_2;
    int route_1_pos_1 = move_info.route_1_pos_1;
    int route_1_pos_2 = move_info.route_1_pos_2;
    int route_2_pos_1 = move_info.route_2_pos_1;

    Route *route_rm = this->solution->getRoute(route_1_idx);
    Route *route    = this->solution->getRoute(route_2_idx);
    Vehicle *vhc    = route->getVehicle();

    if (data->A_[route->getRequestIdByPos(route_2_pos_1 - 1)][route_rm->getRequestIdByPos(route_1_pos_1)] ||
        data->A_[route_rm->getRequestIdByPos(route_1_pos_2)][route->getRequestIdByPos(route_2_pos_1)])
        return INFEASIBLE;

    Sequence seq =
        Sequence::concatenate(data, this->sequences[route_2_idx][0][route_2_pos_1 - 1],
                              this->sequences[route_1_idx][route_1_pos_1][route_1_pos_2]);
    seq = Sequence::concatenate(
        data, seq, this->sequences[route_2_idx][route_2_pos_1][route->getPathSize() - 1]);

    if (!seq.isFeasible(vhc))
        return INFEASIBLE;

    return seq.getCost(vhc);
}

bool ADS::useMSD(int routesSize) {
    return MSD_ACTIVATE && routesSize <= data->v;
}

bool ADS::isInMSD(double key) {
    return MSD.find(key) != MSD.end();
}

void ADS::insertMSD(double key, int ils_start) {
    MSD[key] = ils_start;
}

int ADS::getMSD(double key) {
    itMSD = MSD.find(key);
    return itMSD->second;
}

bool ADS::useMMD(int route_1, int route_2) {
    return (MMD_ACTIVATE && route_1 < data->v && route_2 < data->v);
}

void ADS::insertMMD(double key, MoveDesc move_desc) {
    // routes with key equal to 0.0 aren't allowed
    if (key <= EPS) {
        return;
    }
    int neighborhood = move_desc.moveInfo.neighborhood;
    // Search key
    itMMDIntra = MMDIntra.find(key);
    // Insert if this key exist
    if (itMMDIntra != MMDIntra.end()) {
        itMMDIntra->second[neighborhood] = move_desc;
        // Insert if this key doesn't exist
    } else {
        MMDIntra[key].assign(INTRA_NEIGHBORHOOD_SIZE, MoveDesc());  // Dummy move
        MMDIntra[key][neighborhood] = move_desc;
    }
}

void ADS::insertMMD(pair<double, double> key, MoveDesc move_desc) {
    // routes with key equal to 0.0 aren't allowed
    if (key.first <= EPS || key.second <= EPS) {
        return;
    }
    int neighborhood = move_desc.moveInfo.neighborhood;
    // Search key
    itMMDInter = MMDInter.find(key);
    // Insert if this key exist
    if (itMMDInter != MMDInter.end()) {
        itMMDInter->second[neighborhood] = move_desc;
        // Insert if this key doesn't exist
    } else {
        MMDInter[key].assign(PAIR_ROUTES_MOVES_TOTAL, MoveDesc());  // Dummy move
        MMDInter[key][neighborhood] = move_desc;
    }
}

MoveDesc ADS::getMMD(NEIGHBORHOOD neighborhood, double key) {
    itMMDIntra = MMDIntra.find(key);
    if (itMMDIntra == MMDIntra.end()) {
        return MoveDesc();
    }
    return itMMDIntra->second[neighborhood];
}

MoveDesc ADS::getMMD(NEIGHBORHOOD neighborhood, pair<double, double> key) {
    itMMDInter = MMDInter.find(key);
    if (itMMDInter == MMDInter.end()) {
        return MoveDesc();
    }
    return itMMDInter->second[neighborhood];
}

void ADS::printADS(int r) {
    puts("============ ADS ================");
    Route *route = this->solution->getRoute(r);
    Vehicle *vhc = route->getVehicle();
    route->printPath();

    // printf("%d\n", vhc->getId());

    puts("Pair Positions:");
    for (int i = 0; i < route->getPathSize(); ++i)
        printf("(%d-%d) ", this->pairPosition[r][i].first, this->pairPosition[r][i].second);

    puts("\nLoadZero:");
    for (int i = 0; i < loadZeroIndexes[r].size(); ++i)
        printf("%d ", this->loadZeroIndexes[r][i]);
    puts("\nSequences:");
    for (int i = 0; i < route->getPathSize(); i++) {
        for (int j = i; j < route->getPathSize(); j++) {
            printf("%.2lf ", sequences[r][i][j].timewarp);
        }
        puts("");
    }
    printf("%4.lf\n", this->sequences[r][0][route->getPathSize() - 1].timewarp);
    puts("\n============ END ADS ================");
}