//
// Created by Daniel on 5/6/2018.
//

#ifndef LIBUI_OBSERVABLE_VALUE_H
#define LIBUI_OBSERVABLE_VALUE_H

#include "ui/reactive/observable.h"

namespace ui {
    namespace reactive {

        template <class T>
        class ObservableValueInterface: public virtual Observable {

        public:
            virtual T get() = 0;

        };
    }
}

#endif //LIBUI_OBSERVABLE_VALUE_H
