//
// Created by Daniel on 21/2/2018.
//

#include "ui/text/text_element.h"

void ui::text::TextElement::setColor(uint16_t color) {
    foreground_color = color;
}

uint16_t ui::text::TextElement::getColor() {
    return foreground_color;
}

void ui::text::TextElement::setFont(const ui::Font *font) {
    this->font_ptr = font;
}

const ui::Font *ui::text::TextElement::getFont() {
    return font_ptr;
}

void ui::text::TextElement::setTextWrap(ui::text::TextWrap wrapping_strategy) {
    this->text_wrap = wrapping_strategy;
}

ui::text::TextWrap ui::text::TextElement::getTextWrap() {
    return text_wrap;
}

void ui::text::TextElement::setOverflow(ui::text::Overflow overflow) {
    this->overflow = overflow;
}

ui::text::Overflow ui::text::TextElement::getOverflow() {
    return overflow;
}