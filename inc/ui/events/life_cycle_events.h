//
// Created by Daniel on 4/3/2018.
//

#ifndef INNO14_D_2017_INNO_CYCLE_EVENTS_H
#define INNO14_D_2017_INNO_CYCLE_EVENTS_H


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


#endif //INNO14_D_2017_INNO_CYCLE_EVENTS_H
