//
// Created by Daniel on 20/2/2018.
//

#ifndef INNO14_D_2017_INNO_TEXT_ENGINE_H
#define INNO14_D_2017_INNO_TEXT_ENGINE_H

#include <libsc/lcd.h>
#include <ui/context.h>
#include <ui/font.h>
#include <queue>
#include <algorithm>
#include "text_element.h"

namespace ui {
    namespace text {
        /**
         * TextEngine utilizes a libsc::Lcd object. The text engine can render text using a specific font file generated
         * with The Dot Factory with slight modifications. Features include: text wrapping and text color
         * configurations. This rendering can be further abstracted by utilizing the UI element TextBlock.
         *
         * @example
         * The following program will render a sentence on the screen.
         * @code
         * ui::text::TextEngine textEngine;
         * textEngine.setColor(Lcd::kWhite);
         * textEngine.setRegion(Lcd::Rect(0, 0, 128, 15));
         * textEngine.setFont(&ui::fonts::blocky);
         * textEngine.setTextWrap(ui::text::ELLIPSIS);
         * textEngine.render("The quick brown fox jumps over a lazy dog.");
         * @endcode
         *
         * @see ui::Font
         * @see ui::TextBlock
         */
        class TextEngine: public virtual TextElement {
        public:

            struct Line {
                /**
                 * The occupied width as it would on the screen on the line.
                 */
                Uint width;
                std::string::const_iterator line_start;
            };

            /**
             * Renders text within region using the specified font. No background color is printed, so this method will
             * print slower than printing the foreground and background colors of the text.
             *
             * <h2>Text wrapping</h2>
             *
             * - WRAP strategy: Go to next line and wrap when there is no space left on the same line. If the word is
             * longer than the line, all available characters will be drawn in the current line, leaving the rest for
             * the next line. There will be no strategy for handling hyphenated words.
             *
             * - NO_WRAP strategy: Print as many characters as it can on a single line.
             *
             * - ELLIPSIS strategy: Print as many characters as it can on a single line, with the last 3 characters be
             * "...".
             *
             * <h2>Text overflow</h2>
             *
             * - VISIBLE: Will print outside the border in the y-direction.
             *
             * - HIDDEN: Will crop the characters if they land outside the border in the y-direction.
             *
             * @param string
             * @return A boolean value of true if rendering is successful
             */
            bool render(const std::string &string);

            /**
             * Retrieves the lines as it would get displayed on the screen.
             * @return
             */
            std::vector<Line> getLines(const std::string &string);

            /**
             * Retrieves the occupied height in px as it would on the screen of the string.
             * @param string
             * @return
             */
            Uint getParagraphHeight(const std::string &string);

        private:
            void drawCharacter(char& c, Uint x, Uint y);

            void drawCharacter(const FONT_CHAR_INFO& c, Uint x, Uint y);
        };
    }
}


#endif //INNO14_D_2017_INNO_TEXT_ENGINE_H
