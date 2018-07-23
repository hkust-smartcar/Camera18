//
// Created by Daniel on 15/3/2018.
//

#ifndef INNO14_D_2017_INNO_MENU_H
#define INNO14_D_2017_INNO_MENU_H

#include <libsc/lcd.h>
#include <libsc/st7735r.h>
#include <libsc/joystick.h>
#include <libsc/servo.h>
#include <flash_storage.h>
#include <ui/menus/menu_group.h>
#include <ui/menus/menu_number.h>
#include <ui/fonts/blocky.h>
#include <adapters/context_adapter.h>
#include <adapters/application.h>
#include <global.h>


using namespace libsc;
using namespace libbase::k60;

class CameraMenu: public virtual Application {
public:
    void init() override;

    void start() override;

    static void show() {
        CameraMenu menu;
        menu.run();
    };

    template<class T>
    void changeValue(const std::string& name, T* target, T step, const std::string& format) {
        screen_adapter_ptr->clear();

        const char* f = format.c_str();
        volatile bool requireRendering = true;
        ui::TextBlock tb;
        tb.setColor(0xFFFF);
        tb.setFont(&humanist);

        tb.setText(name);
        tb.setRegion(0, 0, 128, 14);
        tb.render();

        tb.setRegion(0, 15, 128, 14);
        while (true) {
            bool is_delay = false;

            if (joystick->GetState() == libsc::Joystick::State::kDown) {
                *target -= step;
                requireRendering = true;
                is_delay = true;
            } else if (joystick->GetState() == libsc::Joystick::State::kRight) {
                //Increase
                *target += step;
                requireRendering = true;
                is_delay = true;
            } else if (joystick->GetState() == libsc::Joystick::State::kSelect) {
                //Confirm
                break;
            }

            if (requireRendering) {
                char t[26];

                screen_adapter_ptr->setRegion(tb.getRegion());
                screen_adapter_ptr->fill(0);

                sprintf(t, f, *target);
                tb.setText(t);
                tb.render();

                requireRendering = false;
            }

            if (is_delay)
                libsc::System::DelayMs(300);
        }
        libsc::System::DelayMs(300);
    }

    void drawLine(bool is_green, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
        uint16_t color = is_green ? uint16_t(0x7E0u) : uint16_t(0x001Fu);
        std::function<void(uint16_t x, uint16_t y)> put_pixel = [&](uint16_t x, uint16_t y){
            ui::Context::getScreen()->setRegion(x, y, 1, 1);
            ui::Context::getScreen()->fill(color);
        };

        //Bresenham's Line Drawing Algorithm
        int dx, dy, p, x, y, i = 1;

        dx=x1-x0;
        dy=y1-y0;

        x=x0;
        y=y0;

        p=2*dy-dx;

        do{
            put_pixel(uint16_t(x),uint16_t(y));
            while(p>=0){
                y++;
                p = p - 2*dx;
                put_pixel(uint16_t(x),uint16_t(y));
            }
            x++;
            p = p + 2*dy;
            i++;
        } while(i<=dx);
    }

    void showPlot() {
        screen_adapter_ptr->clear();

        uint16_t max_speed = FlashStorage::data.max_speed;
        uint16_t min_speed = FlashStorage::data.min_speed;
        float slope_param = FlashStorage::data.slope_param;
        float x_shift = FlashStorage::data.x_shift;

        double k = min_speed*pow(sin(40/(180*3.141592654)), 0.5);
        uint16_t y_range = max_speed - min_speed;
        uint16_t speed1[41];
        uint16_t speed3[41];

        //Populate arrays
        for (uint16_t i = 0; i < 41u; i++) {
            speed1[i] = i == 0 ? max_speed : (uint16_t) fmin(k / (pow(fabs(sin(i/(180*3.141592654))), 0.5)), max_speed);
            speed3[i] = (uint16_t) (((max_speed-min_speed) / (1 + exp((double) slope_param * ((i*(x_shift) / 40) - (x_shift-1))))) + min_speed);
        }

        //Draw non-green speed1
        for (uint16_t i = 0; i < 40u; i++) {
            //pair up with next i
            drawLine(false, uint16_t(i*(128/41)), uint16_t(100-(speed1[i]-min_speed)*100/y_range), uint16_t((i+1)*(128/41)), uint16_t(100-(speed1[i+1]-min_speed)*100/y_range));
        }

        //Draw green speed3
        for (uint16_t i = 0; i < 40u; i++) {
            //pair up with next i
            drawLine(true, uint16_t(i*(128/41)), uint16_t(100-(speed3[i]-min_speed)*100/y_range), uint16_t((i+1)*(128/41)), uint16_t(100-(speed3[i+1]-min_speed)*100/y_range));
        }

        ui::TextBlock tb;
        tb.setColor(0xFFFF);
        tb.setFont(&humanist);

        char t[26];

        sprintf(t, "Max speed: %d", max_speed);
        tb.setRegion(5, 102, 128, 10);
        tb.setText(t);
        tb.render();

        sprintf(t, "Min speed: %d", min_speed);
        tb.setRegion(5, 117, 128, 10);
        tb.setText(t);
        tb.render();

        sprintf(t, "Slope param: %.2f", slope_param);
        tb.setRegion(5, 132, 128, 10);
        tb.setText(t);
        tb.render();

        sprintf(t, "X shift: %.2f", x_shift);
        tb.setRegion(5, 147, 128, 10);
        tb.setText(t);
        tb.render();
    }
};


#endif //INNO14_D_2017_INNO_MENU_H
