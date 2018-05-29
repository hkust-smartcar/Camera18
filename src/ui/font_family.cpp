//
// Created by Daniel on 21/2/2018.
//

#include "ui/font_family.h"

const ui::Font* ui::FontFamily::getFallbackFont() {
    for (const Font** font_ptr_ptr: search_order) {
        if (*font_ptr_ptr != nullptr)
            return *font_ptr_ptr;
    }
    return nullptr;
}

const ui::Font *ui::FontFamily::getNormal() {
    if (normal != nullptr)
        return normal;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getItalic() {
    if (italic != nullptr)
        return italic;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getBolder() {
    if (bolder != nullptr)
        return bolder;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getLighter() {
    if (lighter != nullptr)
        return lighter;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getLighterItalic() {
    if (lighter_italic != nullptr)
        return lighter_italic;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getBolderItalic() {
    if (bolder_italic != nullptr)
        return bolder_italic;
    return getFallbackFont();
}

const ui::Font *ui::FontFamily::getFont(ui::FontFamily::FontWeight fontWeight, ui::FontFamily::FontStyle fontStyle) {
    if (fontWeight == FontWeight::NORMAL) {
        return fontStyle == FontStyle::ITALIC ? getItalic() : getNormal();
    } else if (fontWeight == FontWeight::BOLDER) {
        return fontStyle == FontStyle::ITALIC ? getBolderItalic() : getBolder();
    } else {
        return fontStyle == FontStyle::ITALIC ? getLighterItalic() : getItalic();
    }
}
