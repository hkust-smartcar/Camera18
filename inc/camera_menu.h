//
// Created by Daniel on 15/3/2018.
//

#ifndef INNO14_D_2017_INNO_MENU_H
#define INNO14_D_2017_INNO_MENU_H

#include <libsc/lcd.h>
#include <libsc/st7735r.h>
#include <libsc/joystick.h>
#include <libsc/servo.h>
#include <flash_storage.h>
#include <ui/menus/menu_group.h>
#include <ui/color_schemes/light.h>
#include <ui/menus/menu_number.h>
#include <ui/fonts/humanist.h>
#include <ui/fonts/blocky.h>
#include <adapters/context_adapter.h>
#include <adapters/application.h>
#include <global.h>


using namespace libsc;
using namespace libbase::k60;

using namespace libsc;
using namespace libbase::k60;

class CameraMenu: public virtual Application {
public:
    void init() override;

    void start() override;

    static void show() {
        CameraMenu menu;
        menu.run();
    };
};


#endif //INNO14_D_2017_INNO_MENU_H
