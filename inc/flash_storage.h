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
        float P = 0;
        float I = 0;
        float D = 0;
    };

    static void save();
    static void load();


    static Data data;
    static libbase::k60::Flash* flash_ptr;
};


#endif //INNO14_D_2017_INNO_FLASH_STORAGE_H
