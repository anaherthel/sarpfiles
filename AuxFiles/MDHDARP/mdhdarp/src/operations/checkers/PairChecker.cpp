#include "PairChecker.h"

PairChecker::PairChecker(vector<int> *path,
                         Data *data,
                         Vehicle *vehicle) {
    this->data    = data;
    this->path    = path;
    this->vehicle = vehicle;
    pairPosition.assign(path->size(), -1);

    if (path->at(0) == path->at(path->size() - 1)) {
        pairPosition[0]                = path->size() - 1;
        pairPosition[path->size() - 1] = 0;
    }

    for (int i = 1; i < path->size() - 1; ++i) {
        for (int j = i + 1; j < path->size() - 1; ++j) {
            if (path->at(i) == path->at(j) + data->n ||
                path->at(i) == path->at(j) - data->n) {
                pairPosition[i] = j;
                pairPosition[j] = i;
            }
        }
    }
}

bool PairChecker::check() {
    int vertex;
    if (pairPosition[0] != path->size() - 1 ||
        pairPosition[path->size() - 1] != 0) {
        return false;
    }
    for (int i = 1; i < path->size() - 1; ++i) {
        vertex = path->at(i);
        if (pairPosition[i] == -1) {
            return false;
        }
        if ((vertex < data->n && i >= pairPosition[i]) ||
            (vertex >= data->n && i < pairPosition[i])) {
            cout << vertex << " " << i << " " << pairPosition[i] << endl;
            return false;
        }
    }
    return true;
}