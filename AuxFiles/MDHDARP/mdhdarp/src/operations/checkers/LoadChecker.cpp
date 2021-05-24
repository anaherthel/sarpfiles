#include "LoadChecker.h"

LoadChecker::LoadChecker(vector<int> *path,
                         Data *data,
                         Vehicle *vehicle) {
    this->data    = data;
    this->path    = path;
    this->vehicle = vehicle;

    Sequence next_sequence;
    Request *request = &data->requests[path->at(0)];
    sequence         = Sequence(data, request);

    for (int i = 1; i < path->size(); ++i) {
        request       = &data->requests[path->at(i)];
        next_sequence = Sequence(data, request);
        sequence      = Sequence::concatenate(data, sequence, next_sequence);
    }
}

bool LoadChecker::check() {
    for (int r = 0; r < vehicle->getResources(); ++r) {
        if (max(0, sequence.load_max[r] - vehicle->getCapacity()[r])) {
            return false;
        }
    }
    return true;
}