//
// Created by Daniel on 22/2/2018.
//

#ifndef INNO14_D_2017_INNO_FLASH_STORAGE_H
#define INNO14_D_2017_INNO_FLASH_STORAGE_H

#include <string>
#include <libbase/misc_utils.h>
#include <libbase/k60/flash.h>
/**
 * Flash Storage class to facilitate easier saving and loading to and from flash memory.
 *
 * To use this, first assign a Flash object pointer to flash_ptr. A reset is required to use for the first time, like:
 *
 * @code
 * FlashStorage::data = FlashStorage::Data{}
 * FlashStorage::save();
 * @endcode
 *
 * To save new changes to the flash memory, it is required to change the FlashStorage::Data struct in the header to
 * a struct that can store your necessary information, and use the code below to save the changes:
 *
 * @code
 * FlashStorage::data.name = new_value;
 * FlashStorage::save();
 * @endcode
 *
 * Loading is like saving, but the data from flash memory will overwrite the data as the static member.
 */
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
