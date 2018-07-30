//
// Created by Daniel on 19/2/2018.
//

#include <utility>

#include "ui/text_block.h"

std::string ui::TextBlock::getText() {
    return text;
}

void ui::TextBlock::setText(const std::string& text) {
    this->text = text;
    for (auto& listener: listener_stack) {
        listener(*this);
    }
}

void ui::TextBlock::onChange(std::function<void(ui::TextBlock&)> function) {
    listener_stack.push_back(function);
}

void ui::TextBlock::render() {
    text::TextEngine textEngine;
    textEngine.setFont(font_ptr);
    textEngine.setOverflow(overflow);
    textEngine.setTextWrap(text_wrap);
    textEngine.setColor(foreground_color);
    textEngine.setRegion(ui_region);
    textEngine.render(text);
}
