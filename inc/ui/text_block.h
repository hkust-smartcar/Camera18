//
// Created by Daniel on 19/2/2018.
//

#ifndef INNO14_D_2017_INNO_TEXT_BLOCK_H
#define INNO14_D_2017_INNO_TEXT_BLOCK_H


#include <string>
#include <utility>
#include <functional>
#include <vector>
#include "ui/text/text_engine.h"
#include "ui/ui_element.h"
#include "ui/font.h"

namespace ui {
    class TextBlock: public UIElement, public text::TextElement {
    public:

        /**
         * Retrieves the text displayed
         * @return
         */
        std::string getText();

        /**
         * Sets the text displayed
         * @param text
         */
        void setText(const std::string& text);

        /**
         * Listen for change in displayed text
         * @param function
         */
        void onChange(std::function<void(TextBlock&)> function);

        void render() override;

    private:
        std::string text;
        std::vector<std::function<void(TextBlock&)>> listener_stack = {};
    };
}


#endif //INNO14_D_2017_INNO_TEXT_BLOCK_H
