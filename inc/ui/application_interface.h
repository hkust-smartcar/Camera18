//
// Created by Daniel on 10/6/2018.
//

#ifndef LIBUI_APPLICATION_INTERFACE_H
#define LIBUI_APPLICATION_INTERFACE_H


namespace ui {
    class ApplicationInterface {
    public:
        virtual void init() = 0;
        virtual void start() = 0;
        virtual void done() = 0;
        virtual void run() = 0;
    };
}


#endif //LIBUI_APPLICATION_INTERFACE_H
