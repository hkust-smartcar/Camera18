//
// Created by Daniel on 22/2/2018.
//

#include "ui/icons.h"

void ui::Icons::drawCaret(Uint x, Uint y, uint16_t color, ui::Direction direction, uint16_t size) {
    Direction draw_order[4][2] = {
            {RIGHT, DOWN},
            {UP, LEFT},
            {RIGHT, UP},
            {DOWN, LEFT}
    };

    bool is_vertical_order = direction == LEFT || direction == RIGHT;

    drawTriangle(x, y, color, draw_order[direction][0], size);

    if (is_vertical_order)
        y += size - 1;
    else
        x += size - 1;

    drawTriangle(x, y, color, draw_order[direction][1], size);
}

void ui::Icons::drawTriangle(Uint x, Uint y, uint16_t color, ui::Direction direction, uint16_t size) {
    Position growth[4] = {
            {1, -1},
            {-1, 1},
            {-1, -1},
            {1, 1}
    };

    for (uint16_t yi = 0; yi < size; yi++) {
        uint16_t x_count = growth[direction].y > 0 ? yi + uint16_t(1) : size - yi;
        uint16_t x_offset = growth[direction].x > 0 ? size - x_count : (uint16_t) 0;
        Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x + x_offset, y + yi, x_count, 1));
        Context::lcd_ptr->FillColor(color);
    }
}

void ui::Icons::drawBatteryGauge(Uint x, Uint y, uint16_t color, uint16_t width, Uint height) {
    //Draw borders
    Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x, y, width - 1u, 1));
    Context::lcd_ptr->FillColor(color);
    Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x, y + height - 1, width - 1u, 1));
    Context::lcd_ptr->FillColor(color);
    Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x, y + 1, 1, height - 2));
    Context::lcd_ptr->FillColor(color);
    Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x + width - 2, y + 1, 1, height - 2));
    Context::lcd_ptr->FillColor(color);
    Context::lcd_ptr->SetRegion(libsc::Lcd::Rect(x + width - 1, y + height / 3, 1, height/3));
    Context::lcd_ptr->FillColor(color);
}
