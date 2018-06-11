//
// Created by Daniel on 4/3/2018.
//

#ifndef LIBUI_CYCLE_EVENTS_H
#define LIBUI_CYCLE_EVENTS_H

namespace ui {
    namespace events {
        class LifeCycleEvents {
        public:
            enum ExitStatus {
                SUCCESS = 0,
                FAILURE
            };

            virtual void onEnter() {};

            virtual void onExit() {};
        };
    }
}


#endif //LIBUI_CYCLE_EVENTS_H
