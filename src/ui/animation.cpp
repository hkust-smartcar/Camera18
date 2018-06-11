//
// Created by Daniel on 10/4/2018.
//

#include <ui/animation.h>

namespace ui {
    void Animation::animate(uint16_t duration, std::function<float(float)> &&easing_fn,
                                std::function<void(float)> &&application_fn) {



    }


    namespace easing {
        float linear(float t) {
            return t;
        }

        float quadIn(float t) {
            return t*t;
        }

        float quadOut(float t) {
            return t * (2-t);
        }

        float quadInOut(float t) {
            return t<.5 ? 2*t*t : -1+(4-2*t)*t;
        }
    }
}