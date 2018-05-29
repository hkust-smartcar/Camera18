//
// Created by Daniel on 21/2/2018.
//

#include "ui/ui_base.h"

void ui::UIBase::setRegion(libsc::Lcd::Rect rect) {
    ui_region = rect;
}

void ui::UIBase::setRegion(Uint x, Uint y, Uint w, Uint h) {
    ui_region = libsc::Lcd::Rect(x, y, w, h);
}

libsc::Lcd::Rect ui::UIBase::getRegion() {
    return ui_region;
}

void ui::UIBase::setVisibility(Visibility visibility) {
    this->visibility = visibility;
}

ui::Visibility ui::UIBase::getVisibility() {
    return visibility;
}

void ui::UIBase::render() {

}
