//
// Created by Daniel on 16/4/2018.
//

#include "ui/reactive/observable.h"

void ui::reactive::Observable::subscribe(ObserverInterface *o_ptr) {
    assert(o_ptr != nullptr);
    if (std::find(observers.begin(), observers.end(), o_ptr) == observers.end()) {
        observers.push_back(o_ptr);
    }
}

int ui::reactive::Observable::countObservers() {
    return (int) observers.size();
}

void ui::reactive::Observable::unsubscribe(ObserverInterface *o_ptr) {
    if (o_ptr == nullptr)
        return;

    observers.erase(std::remove(observers.begin(), observers.end(), o_ptr), observers.end());
}

void ui::reactive::Observable::unsubscribeAll() {
    observers.clear();
}

bool ui::reactive::Observable::hasChanged() {
    return has_changed;
}


void ui::reactive::Observable::notifyAll(void *arg) {
    if (!has_changed)
        return;

    for (ObserverInterface* o_ptr: observers) {
        o_ptr->update(this, arg);
    }
    clearChanged();
}

void ui::reactive::Observable::clearChanged() {
    has_changed = false;
}

void ui::reactive::Observable::setChanged() {
    has_changed = true;
}