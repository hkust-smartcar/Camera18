//
// Created by Daniel on 22/2/2018.
//

#ifndef INNO14_D_2017_INNO_FLASH_STORAGE_H
#define INNO14_D_2017_INNO_FLASH_STORAGE_H

#include <string>
#include <libbase/misc_utils.h>
#include <libbase/k60/flash.h>

class FlashStorage {
public:
    struct Data {
        uint16_t max_speed = 1200;
        uint16_t min_speed = 750;
        float slope_param = 8.5f;
        float x_shift = 1.85f;
        uint16_t constant_speed = 750;
        bool is_using_constant_mode = false;
    };

    static void save();
    static void load();


    static Data data;
    static libbase::k60::Flash* flash_ptr;
};


#endif //INNO14_D_2017_INNO_FLASH_STORAGE_H
