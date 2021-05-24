#include "RoutePool.h"

RoutePool::RoutePool() : pool(vector<Route *>()) {}

RoutePool::~RoutePool() {}

int RoutePool::getHash(Route *route) {
    return 0;
}

bool RoutePool::isInPool(Route *route) {
    for (int i = 0; i < pool.size(); ++i) {
        if ((*route) == (*pool[i]))
            return true;
    }
    return false;
}

void RoutePool::update(Solution *sol) {
    for (auto r : sol->getRoutes()) {
        if (!isInPool(r))
            this->pool.push_back(new Route(*r));
    }
}

void RoutePool::updateSet(Solution *sol) {
    Route *route;
    pair<set<Route *>::iterator, bool> pointer;
    for (auto r : sol->getRoutes()) {
        route   = new Route(*r);
        pointer = this->setPool.insert(route);
        if (!pointer.second)
            delete route;
    }
}

void RoutePool::addInitialSol(Solution *sol) {
    for (auto r : sol->getRoutes()) {
        this->pool.push_back(new Route(*r));
    }
}

void RoutePool::mergePool(RoutePool *other) {
    Route *route;
    pair<set<Route *>::iterator, bool> pointer;
    for (auto r : other->setPool) {
        route   = new Route(*r);
        pointer = this->setPool.insert(route);
        if (!pointer.second) {
            delete route;
        }
    }
}

void RoutePool::setToList() {
    for (auto r : setPool) {
        this->pool.push_back(new Route(*r));
    }
}

void RoutePool::clear() {
    for (set<Route *>::iterator it = this->setPool.begin(); it != this->setPool.end(); it++) {
        delete *it;
    }

    for (vector<Route *>::iterator it = pool.begin(); it != pool.end(); it++) {
        delete *it;
    }

    setPool.clear();
    pool.clear();
}

void RoutePool::print() {
    cout << pool.size() << endl;

    for (int i = 0; i < this->pool.size(); ++i) {
        cout << i << " ";
        this->pool[i]->print();
    }
}