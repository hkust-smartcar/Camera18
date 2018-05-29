//
// Created by Daniel on 2/3/2018.
//

#include "ui/toolbar.h"

namespace ui {

    Toolbar::Toolbar() {
        //Style setup
        assert(Context::font_repo["Humanist"] != nullptr);
        textBlockTitle.setFont(Context::font_repo["Humanist"]);
        textBlockTitle.setTextWrap(text::ELLIPSIS);
        textBlockTitle.setColor(Context::color_scheme.BODY);
        textBlockTitle.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, Context::full_screen.w - PADDING, 12);
        textBlockTitle.setText(this->name);
    }

    void Toolbar::render() {
        //Draw title bar
        for (uint8_t i = 0; i < TOOLBAR_HEIGHT; i++) {
            Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(ui_region.x, ui_region.y + i, Context::full_screen.w, 1));
            Context::lcd_ptr->FillColor(ColorUtil::rgb565Mix(
                    Context::color_scheme.BACKGROUND_LIGHTER,
                    Context::color_scheme.GRAY_LIGHTER,
                    (double) i / TOOLBAR_HEIGHT
            ));
        }
        Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(ui_region.x, ui_region.y + TOOLBAR_HEIGHT - 1, Context::full_screen.w, 1));
        Context::lcd_ptr->FillColor(Context::color_scheme.GRAY);

        textBlockTitle.setText(name);
        textBlockTitle.render();

        //Draw back icon
        if (has_back_arrow)
            Icons::drawCaret(ui_region.x + 4, ui_region.y + CARET_OFFSET, Context::color_scheme.BLACK, graphics::LEFT, 5);
    }

    void Toolbar::setHasBackArrow(bool has_back_arrow) {
        this->has_back_arrow = has_back_arrow;
    }

    void Toolbar::setName(std::string name) {
        this->name = std::move(name);
    }

    void Toolbar::reserveHSpace(Uint w) {
        textBlockTitle.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, Context::full_screen.w - PADDING - w, 12);
    }

}
