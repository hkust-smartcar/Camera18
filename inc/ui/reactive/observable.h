//
// Created by Daniel on 16/4/2018.
//

#ifndef LIBUI_OBSERVABLE_H
#define LIBUI_OBSERVABLE_H

#include "ui/reactive/observer_interface.h"
#include <algorithm>
#include <cassert>

namespace ui {
    namespace reactive {
        class Observable {
        public:
            void subscribe(ObserverInterface *o_ptr);
            int countObservers();
            void unsubscribe(ObserverInterface *o_ptr);
            void unsubscribeAll();
            bool hasChanged();
            void notifyAll(void *arg);

        protected:
            void clearChanged();
            void setChanged();

        private:
            bool has_changed = false;
            std::vector<ObserverInterface*> observers;
        };
    }
}


#endif //LIBUI_OBSERVABLE_H
