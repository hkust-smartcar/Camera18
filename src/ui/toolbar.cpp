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
        textBlockTitle.setText(this->name);
    }

    void Toolbar::render() {
        adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
        
        //Draw title bar
        for (uint8_t i = 0; i < TOOLBAR_HEIGHT; i++) {
            screen_ptr->setRegion(ui_region.x, ui_region.y + i, screen_ptr->getWidth(), 1);
            screen_ptr->fill(ColorUtil::rgb565Mix(
                    Context::color_scheme.BACKGROUND_LIGHT,
                    Context::color_scheme.GRAY_LIGHTER,
                    (double) i / TOOLBAR_HEIGHT
            ));
        }
        screen_ptr->setRegion(ui_region.x, ui_region.y + TOOLBAR_HEIGHT - 1, screen_ptr->getWidth(), 1);
        screen_ptr->fill(Context::color_scheme.GRAY);

        textBlockTitle.setText(name);
        textBlockTitle.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, Context::getScreen()->getWidth() - PADDING, 12);
        textBlockTitle.render();

        //Draw back icon
        if (has_back_arrow)
            Icons::drawCaret(ui_region.x + 4, ui_region.y + CARET_OFFSET, Context::color_scheme.BODY, graphics::LEFT, 5);
    }

    void Toolbar::setHasBackArrow(bool has_back_arrow) {
        this->has_back_arrow = has_back_arrow;
    }

    void Toolbar::setName(std::string name) {
        this->name = std::move(name);
    }

    void Toolbar::reserveHSpace(uint32_t w) {
        textBlockTitle.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, Context::getScreen()->getWidth() - PADDING - w, 12);
    }

}
