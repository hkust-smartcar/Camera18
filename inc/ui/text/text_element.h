//
// Created by Daniel on 21/2/2018.
//

#ifndef LIBUI_TEXT_ELEMENT_H
#define LIBUI_TEXT_ELEMENT_H

#include <cstdint>
#include <ui/font.h>
#include <ui/ui_base.h>

namespace ui {
    namespace text {
        enum TextAlign {
            LEFT,
            CENTER,
            RIGHT
        };

        /**
         * Different wrapping strategies.
         *
         * WRAP: Text will attempt to wrap.
         *
         * NO_WRAP: All available characters will be drawn on the same line.
         *
         * ELLIPSIS: All available characters will be drawn on the same line. Some space at the end will be reserved
         * to display "..."
         */
        enum TextWrap {
            WRAP,
            NO_WRAP,
            ELLIPSIS
        };

        enum Overflow {
            HIDDEN,
            VISIBLE
        };

        class TextElement: public virtual UIBase {
        public:
            /**
             * Sets text color in RGB565
             * @param color
             */
            void setColor(uint16_t color);

            /**
             * Retrieves text color in RGB565
             * @return color in RGB565
             */
            uint16_t getColor();

            /**
             * Sets the font to be used. Fonts may be retrieved using the FontRepo or by the address of a global Font
             * object.
             * @param font
             * @see ui::Font
             */
            void setFont(const Font* font);

            /**
             * Retrieves the font used.
             * @return
             */
            const Font* getFont();

            /**
             * Sets text wrapping strategy
             * @param wrapping_strategy
             * @see ui::text::TextWrap
             */
            void setTextWrap(TextWrap wrapping_strategy);

            /**
             * Retrieves the text wrapping strategy
             * @return
             */
            TextWrap getTextWrap();

            /**
             * Sets the text overflowing strategy
             * @param overflow
             */
            void setOverflow(Overflow overflow);

            /**
             * Retrieves the text overflowing strategy
             * @return
             */
            Overflow getOverflow();

        protected:
            const ui::Font* font_ptr = nullptr;
            uint16_t foreground_color = 0;
            TextWrap text_wrap = WRAP;
            Overflow overflow = VISIBLE;
        };
    }
}

#endif //LIBUI_TEXT_ELEMENT_H