//
// Created by Daniel on 20/2/2018.
//

#include "ui/text/text_engine.h"

bool ui::text::TextEngine::render(const std::string &string) {
    if (font_ptr == nullptr)
        return false; //You must set a font using `setFont(Font*)`

    uint32_t px_per_line = ui_region.w;
    std::string::const_iterator it = string.begin();

    //TODO Aligning center and right

    const uint8_t& space_width = font_ptr->getFontInfo().space_width;
    const uint8_t& tracking = font_ptr->getFontInfo().tracking;
    const auto& string_end = string.end();

    if (text_wrap == WRAP) {

        std::queue<char> word;
        uint32_t accumulated_length = 0;
        uint32_t word_length = 0;
        uint32_t x = 0;
        uint32_t y = 0;

        while (it <= string_end) {
            const char& c = *it;
            uint32_t carry_over_start = 0;

            if (c != ' ' && c != '\0' && c != '\n') {
                word.push(c);
                word_length += font_ptr->getCharInfo(c).width + tracking;
            } else {
                accumulated_length += word_length;
                if (c == '\n' || (accumulated_length > px_per_line && word_length <= px_per_line)) {
                    //Go to next line
                    y += font_ptr->getFontInfo().height;
                    x = 0;
                    accumulated_length = word_length;
                }

                if (word_length > px_per_line) {
                    carry_over_start = x;
                }

                //Print the word
                while (!word.empty()) {
                    const FONT_CHAR_INFO& char_info = font_ptr->getCharInfo(word.front());
                    if (x + char_info.width > px_per_line) {
                        y += font_ptr->getFontInfo().height;
                        x = 0;

                        if (word_length > px_per_line) {
                            accumulated_length = carry_over_start - x;
                            carry_over_start = 0;
                        }
                    }
                    drawCharacter(char_info, ui_region.x + x, ui_region.y + y);
                    x += tracking + char_info.width;
                    word.pop();
                }
                accumulated_length += space_width;
                x += space_width;
                word_length = 0;

                if (overflow == HIDDEN && y > ui_region.y + ui_region.h)
                    break;
            }
            it++;
        }

    } else {
        uint16_t reserved_space = 0;
        const FONT_CHAR_INFO& period_char_info = font_ptr->getCharInfo('.');
        if (text_wrap == ELLIPSIS) {
            uint16_t period_width = period_char_info.width;
            reserved_space = (uint16_t)3 * period_width + (uint16_t) (tracking * 2);
        }

        uint16_t current_character_width = 0;
        const FONT_CHAR_INFO* current_char_info = nullptr;
        uint32_t remaining_space = px_per_line - reserved_space;
        uint32_t accumulated_length = 0;
        while (*it == ' ' || (accumulated_length + (current_character_width = (*(current_char_info = &font_ptr->getCharInfo(*it))).width)
               < remaining_space && it < string_end)) {

            if (*it == ' ') {
                accumulated_length += space_width;
            } else if (*it == '\n') {
                return true;
            } else {
                drawCharacter(*current_char_info, ui_region.x + accumulated_length, ui_region.y);
                accumulated_length += current_character_width + tracking;
            }

            it++;
        }

        if (text_wrap == ELLIPSIS) {
            uint16_t ellipsis_chars_space = 0;
            std::string::const_iterator it0(it);
            while (it0 < string_end && ellipsis_chars_space <= remaining_space) {
                ellipsis_chars_space += font_ptr->getCharInfo(*it0).width + tracking;
                it0++;
            }
            ellipsis_chars_space = (uint16_t) std::max(int(ellipsis_chars_space - tracking), 0);
            if (ellipsis_chars_space <= reserved_space) {
                //draw rest characters
                while (it < string_end) {
                    const FONT_CHAR_INFO& char_info = font_ptr->getCharInfo(*it);
                    drawCharacter(char_info, ui_region.x + accumulated_length, ui_region.y);
                    accumulated_length += char_info.width + tracking;
                    it++;
                }
            } else {
                //draw 3 dots
                for (uint8_t i = 0; i < 3; i++) {
                    drawCharacter(period_char_info, ui_region.x + accumulated_length, ui_region.y);
                    accumulated_length += period_char_info.width + tracking;
                }
            }
        }
    }

    return true;
}

void ui::text::TextEngine::drawCharacter(char &c, uint32_t x, uint32_t y) {
    //draw character
    if (font_ptr != nullptr) {
        const FONT_CHAR_INFO& char_info = font_ptr->getCharInfo(c);
        drawCharacter(char_info, x, y);
    }
}

void ui::text::TextEngine::drawCharacter(const ui::FONT_CHAR_INFO& c, uint32_t x, uint32_t y) {
    adapters::ScreenAdapterInterface* screen_ptr = Context::getScreen();
    
    /*
     * Character 1+ bytes wide
     * 0b01100000
     * 0b10010000
     * 0b10010000
     * 0b01100000
     * 0b00000000
     *
     * Scan bytes from left to right, line by line, and draw consecutive pixels
     */
    uint8_t bytes_width = (uint8_t) ceil(c.width / 8.0);
    uint8_t character_height = font_ptr->getFontInfo().height;
    const uint8_t* bitmap_offset = font_ptr->bitmaps + c.offset;


    for (uint8_t yi = 0; yi < character_height; yi++) {
        if (overflow == HIDDEN && y + yi > ui_region.y + ui_region.h)
            break;

        uint8_t consecutive_px = 0;
        uint8_t xi = 0;
        for (uint8_t byte_i = 0; byte_i < bytes_width; byte_i++) {
            for (uint8_t bi = 0; bi < 8; bi++, xi++) {
                uint16_t bit_mask = 1u << (7 - bi);
                if ((*bitmap_offset & bit_mask) >> (7 - bi)) {
                    consecutive_px++;
                } else if (consecutive_px) {
                    screen_ptr->setRegion(x + xi - consecutive_px, y + yi, consecutive_px, 1);
                    screen_ptr->fill(foreground_color);
                    consecutive_px = 0;
                }
                if (xi - c.width == 0) //Already printed last character, scanning 1 pixel afterwards
                    break;
            }
            bitmap_offset ++;
        }
        if (consecutive_px > 0) {
            screen_ptr->setRegion(x + xi - consecutive_px, y + yi, consecutive_px, 1);
            screen_ptr->fill(foreground_color);
        }
    }

}

std::vector<ui::text::TextEngine::Line> ui::text::TextEngine::getLines(const std::string &string) {
    std::vector<ui::text::TextEngine::Line> lines;
    auto it = string.begin();
    auto current_line_start = it;

    ui::text::TextEngine::Line current_line{0, current_line_start};
    uint32_t accumulated_length = 0;
    const uint8_t& space_width = font_ptr->getFontInfo().space_width;
    const uint8_t& tracking = font_ptr->getFontInfo().tracking;
    std::deque<uint8_t> char_lengths;

    std::function<uint8_t()> get_word_length = [&]() {
        uint8_t length = 0;
        for (uint8_t char_length: char_lengths) {
            length += char_length;
        }
        return length;
    };

    std::function<void()> next_line = [&](){
        lines.push_back(current_line);
        current_line = {accumulated_length, current_line_start};
        accumulated_length = 0;
        current_line_start = it + 1;
    };

    while (it != string.end()) {
        const char& c = *it;

        if (c == '\n') {
            next_line();
        } else if (c == ' ') {
            //Clear char_lengths
            uint8_t word_length = get_word_length();
            accumulated_length += space_width + tracking + word_length + tracking * (char_lengths.size() - 1);
            if (accumulated_length > ui_region.w) {
                if (word_length < accumulated_length)
                    next_line();
                else {
                    //chop characters

                }
            }
        } else {
            const FONT_CHAR_INFO& char_info = font_ptr->getCharInfo(c);
            char_lengths.push_back(char_info.width);
        }

        it++;
    }

    next_line();

    return lines;
}

uint32_t ui::text::TextEngine::getParagraphHeight(const std::string &string) {
    return 0;
}
