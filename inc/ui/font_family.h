//
// Created by Daniel on 21/2/2018.
//

#ifndef LIBUI_FONT_FAMILY_H
#define LIBUI_FONT_FAMILY_H


#include <vector>
#include "ui/font.h"

namespace ui {
    class FontFamily {
    public:

        enum struct FontWeight {
            NORMAL,
            BOLDER,
            LIGHTER
        };

        enum struct FontStyle {
            NORMAL,
            ITALIC
        };

        struct Config {
            const Font* normal;
            const Font* italic;
            const Font* bolder;
            const Font* lighter;
            const Font* lighter_italic;
            const Font* bolder_italic;
        };

        const Font* normal = nullptr;
        const Font* italic = nullptr;
        const Font* bolder = nullptr;
        const Font* lighter = nullptr;
        const Font* lighter_italic = nullptr;
        const Font* bolder_italic = nullptr;

        /**
         * Order to search for fall back font
         */
        const std::vector<const Font**> search_order = {
                &normal,
                &lighter,
                &bolder,
                &italic,
                &lighter_italic,
                &bolder_italic
        };

        explicit FontFamily(Config config) {
            this->normal = config.normal;
            this->italic = config.italic;
            this->bolder = config.bolder;
            this->lighter = config.lighter;
            this->lighter_italic = config.lighter_italic;
            this->bolder_italic = config.bolder_italic;
        }

        const Font* getFallbackFont();

        const Font* getNormal();
        const Font* getItalic();
        const Font* getBolder();
        const Font* getLighter();
        const Font* getLighterItalic();
        const Font* getBolderItalic();
        const Font* getFont(FontWeight fontWeight, FontStyle fontStyle);
    };
}

#endif //LIBUI_FONT_FAMILY_H
