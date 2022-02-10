#include "ArcsChecker.h"

ArcsChecker::ArcsChecker(vector<int> *path,
                         Data *data,
                         Vehicle *vehicle) {
    this->data    = data;
    this->path    = path;
    this->vehicle = vehicle;
}

bool ArcsChecker::check() {
    int u, v;
    u = path->at(0);
    for (int i = 1; i < path->size(); ++i) {
        v = path->at(i);
        if (data->A_[u][v]) {
            cout << endl;
            data->requests[u].print();
            data->requests[v].print();
            cout << "arc: " << data->t[u][v] << endl;
            return false;
        }
        u = v;
    }
    return true;
}