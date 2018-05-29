//
// Created by Daniel on 20/2/2018.
//

#include "ui/font.h"

ui::Font::Font(const uint8_t *bitmaps, const ui::FONT_INFO *font_info, const ui::FONT_CHAR_INFO *descriptors) :
        bitmaps(bitmaps),
        font_info_ptr(font_info),
        descriptors(descriptors) {}

ui::Font::Font(const uint8_t *bitmaps, const ui::FONT_INFO *font_info, const ui::FONT_CHAR_INFO *descriptors,
               bool monospaced) :
        bitmaps(bitmaps),
        font_info_ptr(font_info),
        descriptors(descriptors),
        fontType(monospaced ? MONOSPACED : PROPORTIONAL) {}

const ui::FONT_CHAR_INFO &ui::Font::getCharInfo(char c) const {
    return descriptors[c - font_info_ptr->start_character];
}

const ui::FONT_INFO &ui::Font::getFontInfo() const {
    return *font_info_ptr;
}
