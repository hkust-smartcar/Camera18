//
// Created by Daniel on 10/4/2018.
//

#ifndef LIBUI_ANIMATE_H
#define LIBUI_ANIMATE_H

#include <functional>

namespace ui {

    //TODO thread

    class Animation {
        static void animate(uint16_t duration, std::function<float(float)>&& easing_fn, std::function<void(float)>&& application_fn);
    };

    namespace easing {
        float linear(float t);
        float quadIn(float t);
        float quadOut(float t);
    }

}


#endif //LIBUI_ANIMATE_H
