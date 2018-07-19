//
// Created by Daniel on 20/2/2018.
//

#ifndef LIBUI_PERIPHERALS_H
#define LIBUI_PERIPHERALS_H


#include <ui/color_schemes/color_scheme.h>
#include <ui/ui_base.h>
#include <ui/e.h>
#include <map>
#include <functional>
#include <ui/adapters/context_adapters/context_adapter_interface.h>
#include "font.h"

namespace ui {
    class Context {
    public:
        static ui::adapters::ContextAdapterInterface* context_adapter_ptr;

        static ColorScheme color_scheme;

        /**
         * Sets color scheme
         * @param colorScheme
         */
        static void setColorScheme(ColorScheme colorScheme);

        /**
         * Relays all UI listeners to lower level libraries, so that removing listeners is possible
         */
        static std::map<std::pair<Event, std::function<void(E& event_obj)>*>, std::function<void(E& event_obj)>*>
                listener_map;

        /**
         * Prepares listener relays.
         *
         * Caution! This will delete and re-instantiate various peripheral pointers.
         */
        static void prepareListenerEvents();

        /**
         * Adds an event listener
         * @param event
         * @param cb_ptr
         */
        static void addEventListener(Event event, std::function<void(E& event_obj)>* cb_ptr);
        static void removeEventListener(Event event, std::function<void(E& event_obj)>* cb_ptr);
        static void triggerListeners(Event event, E e);
        static std::map<std::string, Font*> font_repo;
        static void addFontToRepo(const std::string &name, Font* font_ptr);
        static void removeFontFromRepo(const std::string &name);
        static void clearFontRepo();

        static void attachAdapter(ui::adapters::ContextAdapterInterface* context_adapter_interface_ptr);
        static adapters::ContextAdapterInterface* getContext();
        static adapters::ScreenAdapterInterface* getScreen();
        static adapters::JoystickAdapterInterface* getJoystick();
        static adapters::BatteryMeterAdapterInterface* getBatteryMeter();
        static void remapJoystickDirections(JoystickState up, JoystickState down, JoystickState left,
                                            JoystickState right, JoystickState select = JoystickState::SELECT,
                                            JoystickState idle = JoystickState::IDLE);
        static void setGUIRotation(adapters::ContextAdapterInterface::RotationalChange rotational_change);
        static uint32_t getSystemTime();
    };
}



#endif //LIBUI_PERIPHERALS_H
