//
// Created by Daniel on 17/2/2018.
//

#include <utility>

#include "ui/menus/menu_action.h"

ui::MenuAction::MenuAction() {
    //Style setup
    assert(Context::font_repo["Humanist"] != nullptr);
    textBlockName.setFont(Context::font_repo["Humanist"]);
    textBlockName.setTextWrap(text::ELLIPSIS);
    textBlockName.setColor(Context::color_scheme.BODY);
    textBlockName.setText(this->name);
}

int ui::MenuAction::run() {
    onEnter();
    onExit();
    return SUCCESS;
}

void ui::MenuAction::select() {
    this->is_selected = true;
}

void ui::MenuAction::deselect() {
    this->is_selected = false;
}

void ui::MenuAction::render() {
    /**
     * Remember to set font pointer in Context.
     *
     * @see ui::Context::font_repo
     */

    Context::lcd_ptr->SetRegion(ui_region);
    Context::lcd_ptr->FillColor(is_selected ? Context::color_scheme.PRIMARY_LIGHTER : Context::color_scheme.BACKGROUND_LIGHT);
    textBlockName.setRegion(ui_region.x + PADDING, ui_region.y + TEXT_OFFSET, ui_region.w - PADDING, ITEM_HEIGHT);
    textBlockName.setText(name);
    textBlockName.render();
}

void ui::MenuAction::setName(std::string name) {
    this->name = std::move(name);
}
