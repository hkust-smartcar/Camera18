//
// Created by Daniel on 21/2/2018.
//

#ifndef LIBUI_UBUNTU_MONO_FAMILY_H
#define LIBUI_UBUNTU_MONO_FAMILY_H

#include <ui/font_family.h>
#include <ui/fonts/ubuntu_mono.h>
#include <ui/fonts/ubuntu_mono_italic.h>

namespace ui {
    namespace font_families {
        class UbuntuMono: public FontFamily {
        public:
            //TODO fix this
            UbuntuMono(): FontFamily(Config {
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr,
                    nullptr
            }) {}
        };

        extern const UbuntuMono ubuntu_mono;
    }
}


#endif //LIBUI_UBUNTU_MONO_FAMILY_H
