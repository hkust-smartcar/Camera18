//
// Created by Daniel on 16/4/2018.
//

#ifndef LIBUI_OBSERVER_H
#define LIBUI_OBSERVER_H

#include <string>
#include "ui/reactive/observable.h"

namespace ui {
    namespace reactive {
        class Observable;

        class ObserverInterface {
        public:
            virtual void update(Observable* o, void* arg) = 0;
        };
    }
}


#endif //LIBUI_OBSERVER_H
