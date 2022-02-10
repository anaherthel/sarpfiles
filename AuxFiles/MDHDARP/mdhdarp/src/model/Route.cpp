#include "Route.h"

#include <iostream>

using namespace std;

int Route::objCounter = 0;

Route::Route(Data *data, Vehicle vhc) : data(data),
                                        id(objCounter++),
                                        vehicle(vhc),
                                        extra(vhc.isExtra()),
                                        cost(0),
                                        neighborhoodStatus(INTRA_NEIGHBORHOOD_SIZE, true) {
    int depot = (vehicle.getId() >= data->v) ? (data->n << 1)
                                             : (data->n << 1) + vehicle.getId();
    penalCoef = extra ? PENAL_COST : 1;
    path.push_back(depot);
    path.push_back(depot);

    this->update();
}

Route::Route(const Route &other) : id(other.id),
                                   data(other.data),
                                   vehicle(other.vehicle),
                                   cost(other.cost),
                                   extra(other.extra),
                                   penalCoef(other.penalCoef),
                                   path(other.path),
                                   neighborhoodStatus(other.neighborhoodStatus) {
    this->update();
}

Route::~Route() {}

void Route::insertRequest(int id, int pos) {
    this->path.insert(this->path.begin() + pos, id);
}

int Route::removeRequest(int pos) {
    int id = this->path[pos];
    this->path.erase(this->path.begin() + pos, this->path.begin() + pos + 1);
    return id;
}

void Route::insertBlockByPos(int pos, vector<int> *block) {
    this->path.insert(this->path.begin() + pos, block->begin(), block->end());
}

vector<int> *Route::removeBlock(int start, int end) {
    vector<int> *removedNodes =
        new vector<int>(this->path.begin() + start, this->path.begin() + end);
    this->path.erase(this->path.begin() + start, this->path.begin() + end);
    return removedNodes;
}

vector<int> *Route::removeBlockByPos(int pos, int blockSize) {
    return removeBlock(pos, pos + blockSize);
}

vector<int> *Route::removeBlockById(int id, int blockSize) {
    int blockPos;
    for (int i = 0; i < this->getPathSize(); ++i) {
        if (id == this->path[i]) {
            blockPos = i;
            break;
        }
    }
    return removeBlock(blockPos, blockPos + blockSize);
}

void Route::updateDepot() {
    this->path[path.size() - 1] = this->path[0] =
        vehicle.getId() + (2 * data->n);
}

void Route::swapRequests(int idx1, int idx2) {
    swap(this->path[idx1], this->path[idx2]);
}

void Route::changeRequest(int idx, int idVertex) {
    this->path[idx] = idVertex;
}

void Route::changeVhc(Vehicle vhc) {
    setVehicle(vhc);
    changeRequest(0, data->n * 2 + vhc.getId());
    changeRequest(this->getPathSize() - 1, data->n * 2 + vhc.getId());
}

void Route::reinsertRequests(int idx1, int idx2, int idx3, int idx4) {
    if (idx1 < idx3) {
        if (idx2 < idx4)
            idx4--;

        if (idx2 < idx3)
            idx3--;

        idx3--;
    } else {
        if (idx2 < idx4)
            idx4--;

        if (idx1 >= idx4)
            idx4++;
    }

    vector<int> auxPath(this->getPathSize(), 0);

    for (int i = 0; i < this->getPathSize(); ++i)
        auxPath[i] = this->path[i];

    int pickup   = this->getRequestIdByPos(idx1);
    int delivery = this->getRequestIdByPos(idx2);

    for (int i = 0, j = 0; j < this->getPathSize(); ++i) {
        if ((auxPath[i] == pickup) || (auxPath[i] == delivery))
            continue;
        else if (idx3 == j) {
            this->path[j] = pickup;
            --i;
            ++j;
        } else if (idx4 == j) {
            this->path[j] = delivery;
            --i;
            ++j;
        } else {
            this->path[j] = auxPath[i];
            ++j;
        }
    }
}

void Route::rmvInsertRequests(int idx1, int idx2, int idx3, int idPickup) {
    int idx4 = idx3;

    if (idx1 < idx3) {
        if (idx2 < idx4)
            idx4--;

        if (idx2 < idx3)
            idx3--;

        idx3--;
    } else {
        if (idx2 < idx4)
            idx4--;

        if (idx1 >= idx4)
            idx4++;
    }

    vector<int> auxPath(this->getPathSize(), 0);

    for (int i = 0; i < this->getPathSize(); ++i)
        auxPath[i] = this->path[i];

    int pickup   = this->getRequestIdByPos(idx1);
    int delivery = this->getRequestIdByPos(idx2);

    int idDelivery = idPickup + data->n;

    for (int i = 0, j = 0; j < this->getPathSize(); ++i) {
        if ((auxPath[i] == pickup) || (auxPath[i] == delivery))
            continue;
        else if (idx3 == j) {
            this->path[j] = idPickup;
            --i;
            ++j;
        } else if (idx4 == j) {
            this->path[j] = idDelivery;
            --i;
            ++j;
        } else {
            this->path[j] = auxPath[i];
            ++j;
        }
    }
}

void Route::update() {
    calcCost();
    for (auto neighborhood : intraList) {
        this->setNeighborhoodStatus(neighborhood, true);
    }
}

void Route::calcCost() {
    this->cost = 0;
    int u, v;
    for (int i = 0; i < getPathSize() - 1; ++i) {
        u = path[i];
        v = path[i + 1];
        this->cost += data->c[u][v];
    }
    this->cost = this->cost * this->penalCoef;
}

void Route::print() {
    // printPath();
    // puts("ROUTE\n");
    // vehicle.print();
    // printf("Cost of: %.3lf\n", cost);
    // double begin = this->sequences[0][this->getPathSize()-1].getEarliest();

    // for(int i = 0; i < getPathSize(); ++i)
    //   printf("%d ->(%d %d %d %d) --- I'm a %c and my pair is in position %d
    //   ---- [%.4lf , %.4lf] -- Arrived at %.4lf -- TW: %.4lf\n", path[i],
    //   load[i][0], load[i][1], load[i][2], load[i][3], pairPosition[i].first ?
    //   'D' : 'P', pairPosition[i].second,
    //   this->getRequestByPos(i)->getEarliestTW(),
    //   this->getRequestByPos(i)->getLatestTW(),
    //   this->sequences[0][i].getStartServiceTime(this->getRequestByPos(i),
    //   begin), this->sequences[0][i].timewarp );
    //    //printf("%d ", path->at(i));

    // puts("\n");

    // printf("TW WARP: %.4f\n",
    // this->sequences[0][this->getPathSize()-1].getTimewarp()); puts(
    // this->feasible ? "This route is feasible for all constraints\n" : "This
    // route is NOT feasible for all constraints\n");
}

void Route::printPath() {
    // printf("Vhc %d: from %d (%.2lf, %.2lf) -- [%d %d %d %d] %d\n",
    //         vehicle.getId(), vehicle.getDepot(), vehicle.getX(),
    //         vehicle.getY(), vehicle.getCapacity()[0],
    //         vehicle.getCapacity()[1], vehicle.getCapacity()[2],
    //         vehicle.getCapacity()[3], penalCoef);

    printf("Cost: %.4lf - Path size: %d - Invalid: %d\n", this->getCost(),
           this->getPathSize(), this->getPenalCoef());
    for (int i = 0; i < getPathSize(); ++i) {
        printf("%d ", this->path[i]);
        // this->getRequestByPos(i)->print(); puts("");
    }

    puts("");
}

void Route::clear() {
    path.clear();

    this->cost = 0;

    int depot = (vehicle.getId() >= data->v) ? (data->n << 1)
                                             : (data->n << 1) + vehicle.getId();

    path.push_back(depot);
    path.push_back(depot);

    this->update();
}

bool Route::operator==(const Route &r) const {
    int mid_this = (path.size()) / 2;
    int mid_r    = (r.path.size()) / 2;

    return (cost == r.cost && path.size() == r.path.size() &&
            path[1] == r.path[1] &&
            path[path.size() - 2] == r.path[path.size() - 2] &&
            path[mid_this] == r.path[mid_r]);
}

bool Route::operator<(const Route &r) const {
    int mid_this = (path.size()) / 2;
    int mid_r    = (r.path.size()) / 2;

    if (cost == r.cost) {
        if (path.size() == r.path.size()) {
            if (path[1] == r.path[1]) {
                if (path[path.size() - 2] == r.path[path.size() - 2]) {
                    return path[mid_this] < r.path[mid_r];
                } else {
                    return path[path.size() - 2] < r.path[path.size() - 2];
                }
            } else {
                return path[1] < r.path[1];
            }
        } else {
            return path.size() < r.path.size();
        }
    } else {
        return cost < r.cost;
    }
    return cost < r.cost;
}