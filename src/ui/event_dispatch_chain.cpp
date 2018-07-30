//
// Created by Daniel on 21/7/2018.
//

#include "ui/event_dispatch_chain.h"

void ui::EventDispatchChain::dispatchEvent(ui::E *event_ptr) {
    if (!event_ptr->isConsumed()) {
        if (event_handler_ptr != nullptr)
            (*event_handler_ptr)(*event_ptr);
        if (next_ptr != nullptr)
            next_ptr->dispatchEvent(event_ptr);
    }
}


void ui::EventDispatchChain::prepend(std::function<void(ui::E & event_obj)> *event_handler_ptr) {
    auto* _this = new EventDispatchChain;
    _this->next_ptr = this->next_ptr;
    _this->event_handler_ptr = this->event_handler_ptr;

    this->next_ptr = _this;
    this->event_handler_ptr = event_handler_ptr;
}

void ui::EventDispatchChain::removeSelf() {
    if (next_ptr != nullptr) {
        EventDispatchChain* n_ptr = next_ptr;

        next_ptr = n_ptr->next_ptr;
        event_handler_ptr = n_ptr->event_handler_ptr;

        delete n_ptr;
    } else {
        event_handler_ptr = nullptr;
    }
}

void ui::EventDispatchChain::removeHandler(std::function<void(ui::E & event_obj)> *event_handler_ptr) {
    if (this->event_handler_ptr == event_handler_ptr)
        removeSelf();
    else if (next_ptr != nullptr)
        next_ptr->removeHandler(event_handler_ptr);
}
