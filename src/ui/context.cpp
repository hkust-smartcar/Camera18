//
// Created by Daniel on 20/2/2018.
//

#include "ui/context.h"

namespace ui {
    adapters::ContextAdapterInterface* Context::context_adapter_ptr = nullptr;

    ColorScheme Context::color_scheme;

    std::map<EventType, EventDispatchChain*> Context::listener_map;

    void Context::setColorScheme(ColorScheme colorScheme) {
        Context::color_scheme = colorScheme;
    }

    void Context::addEventListener(EventType event, std::function<void(E&)>* cb_ptr) {
        if (listener_map.find(event) == listener_map.end()) {
            listener_map[event] = new EventDispatchChain;
        }
        listener_map[event]->prepend(cb_ptr);
    }

    void Context::removeEventListener(EventType event, std::function<void(E&)>* cb_ptr) {
        //Check if the function is already in map
        if ( listener_map.find(event) != listener_map.end() ) {
            listener_map[event]->removeHandler(cb_ptr);
        }
    }

    void Context::triggerListeners(EventType event, E e) {
        if ( listener_map.find(event) != listener_map.end() ) {
            listener_map[event]->dispatchEvent(&e);
        }
    }

    void Context::prepareListenerEvents() {
        context_adapter_ptr->prepareListenerEvents();
    }

    std::map<std::string, Font*> Context::font_repo;

    void Context::addFontToRepo(const std::string &name, Font* font_ptr) {
        font_repo[name] = font_ptr;
    }

    void Context::removeFontFromRepo(const std::string &name) {
        if ( font_repo.find(name) != font_repo.end() ) {
            font_repo.erase(name);
        }
    }

    void Context::clearFontRepo() {
        font_repo.clear();
    }

    void Context::attachAdapter(ui::adapters::ContextAdapterInterface* context_adapter_interface_ptr) {
        context_adapter_ptr = context_adapter_interface_ptr;
    }

    adapters::ScreenAdapterInterface *Context::getScreen() {
        return context_adapter_ptr->getScreen();
    }

    adapters::JoystickAdapterInterface *Context::getJoystick() {
        return context_adapter_ptr->getJoystick();
    }

    adapters::BatteryMeterAdapterInterface *Context::getBatteryMeter() {
        return context_adapter_ptr->getBatteryMeter();
    }

    void Context::setGUIRotation(adapters::ContextAdapterInterface::RotationalChange rotational_change) {
        context_adapter_ptr->setGUIRotation(rotational_change);
    }

    void Context::remapJoystickDirections(JoystickState up, JoystickState down, JoystickState left, JoystickState right,
                                          JoystickState select, JoystickState idle) {
        context_adapter_ptr->remapJoystickDirections(up, down, left, right, select, idle);
    }

    uint32_t Context::getSystemTime() {
        return context_adapter_ptr->getSystemTime();
    }

    adapters::ContextAdapterInterface* Context::getContext() {
        return context_adapter_ptr;
    }
}
