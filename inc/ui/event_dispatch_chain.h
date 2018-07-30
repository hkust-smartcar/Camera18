//
// Created by Daniel on 21/7/2018.
//

#ifndef LIBUI_EVENT_CHAIN_H
#define LIBUI_EVENT_CHAIN_H


#include <functional>
#include "e.h"

namespace ui {
    class EventDispatchChain {
    public:
        EventDispatchChain* next_ptr = nullptr;
        std::function<void(E& event_obj)>* event_handler_ptr = nullptr;

        /**
         * If the event is not consumed, it will try to dispatch the events one by one in the chain from the latest
         * added event handlers to the earliests.
         * @param event_ptr
         */
        void dispatchEvent(E* event_ptr);

        /**
         * Inserts a higher priority event dispatcher into the stack to be trigger earlier.
         *
         * The current event dispatcher will move deeper into the stack.
         * @param event_handler_ptr
         */
        void prepend(std::function<void(E& event_obj)>* event_handler_ptr);

        /**
         * Removes this dispatcher in the chain.
         *
         * The function actually copies the next element to itself and removes the next element in effect and changes
         * its event handler pointer to nullptr if there is no next in chain.
         */
        void removeSelf();

        /**
         * Removes the event handler in the chain recursively by the supplied event handler pointer.
         * @param event_handler_ptr
         */
        void removeHandler(std::function<void(E& event_obj)>* event_handler_ptr);
    };
}


#endif //LIBUI_EVENT_CHAIN_H
