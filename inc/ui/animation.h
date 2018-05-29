//
// Created by Daniel on 10/4/2018.
//

#ifndef INNO14_D_2017_INNO_ANIMATE_H
#define INNO14_D_2017_INNO_ANIMATE_H

#include <functional>
#include <libsc/system.h>

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


#endif //INNO14_D_2017_INNO_ANIMATE_H
