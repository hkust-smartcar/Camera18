//
// Created by Daniel on 5/6/2018.
//

#ifndef LIBUI_REACTIVE_H
#define LIBUI_REACTIVE_H

#include <functional>
#include "ui/reactive/observable_value_interface.h"

namespace ui { namespace reactive {

template<typename T, typename... U>
size_t getAddress(std::function<T(U...)> f) {
    typedef T(fnType)(U...);
    fnType ** fnPointer = f.template target<fnType*>();
    return (size_t) *fnPointer;
}

/**
 * Reactive class.
 * @tparam T Type of the reactive data.
 */
template<class T>
class Reactive: public virtual ObservableValueInterface<T> {
public:
    typedef std::function<void(Reactive<T>*, T, T)> ChangeListener;

    Reactive() {
        binding_listener = [&](Reactive<T>* _this, T newValue, T oldValue) {
            set(newValue);
        };

        doubly_binding_listener = [&](Reactive<T>* _this, T newValue, T oldValue) {
            bound_target->value = newValue;
        };
    }

    explicit Reactive(T t): Reactive() {
        value = t;
    }

    void addListener(ChangeListener* listener_ptr) {
        if (std::find(listeners.begin(), listeners.end(), listener_ptr) == listeners.end()) {
            listeners.push_back(listener_ptr);
        }
    }

    void removeListener(ChangeListener* listener_ptr) {
        listeners.erase(std::remove(listeners.begin(), listeners.end(), listener_ptr), listeners.end());
    }

    /**
     * Retrieves the stored value
     * @return Stored value.
     */
    T get() override {
        return value;
    }

    /**
     * Updates the value and notifies all observers of the change.
     * @param t The new value.
     */
    virtual void set(T t) {
        T oldValue = value;

        value = t;
        this->setChanged();
        this->notifyAll(nullptr);

        for (ChangeListener* listener: listeners) {
            (*listener)(this, t, oldValue);
        }
    }

    /**
     * Checks whether the instance is in a one-way, or two-way binding relationship.
     * @return a boolean value indicating whether the instance is bound.
     */
    bool isBound() {
        return bound_target != nullptr;
    }

    /**
     * Checks whether the instance is in a one-way binding relationship.
     * @return a boolean value indicating whether the instance is bound.
     */
    bool isSinglyBound() {
        return bound_target != nullptr && bound_target->bound_target != this;
    }

    /**
     * Checks whether the instance is in a two-way binding relationship.
     * @return a boolean value indicating whether the instance is bound.
     */
    bool isDoublyBound() {
        return bound_target != nullptr && bound_target->bound_target == this;
    }

    /**
     * One-way binds value of this instance to the value of another instance.
     * @param reactive_ptr
     * @return a boolean value of whether the action was successful.
     */
    bool bindTo(Reactive<T>* reactive_ptr) {
        if (bound_target == nullptr) {
            reactive_ptr->addListener(&binding_listener);
            bound_target = reactive_ptr;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Removes one-way binding with the bound target.
     */
    void unbind() {
        if (bound_target != nullptr) {
            bound_target->removeListener(&binding_listener);
            bound_target = nullptr;
        }
    }

    /**
     * Two-way binds value of this instance with value of another instance. So when the value of this instance,
     * or that of the other instance is changed, the counter-part will update its value to the latest one.
     * @param reactive_ptr
     * @return a boolean value of whether the action was successful.
     */
    bool doublyBindWith(Reactive<T>* reactive_ptr) {
        if (bound_target == nullptr && reactive_ptr->bound_target == nullptr) {
            reactive_ptr->addListener(&binding_listener);
            reactive_ptr->bound_target = this;
            addListener(&doubly_binding_listener);
            bound_target = reactive_ptr;
            is_doubly_binding_master = true;
            return true;
        } else {
            return false;
        }
    }

    /**
     * Removes two-way binding with the bound target.
     */
    void doublyUnbind() {
        if (bound_target != nullptr && bound_target->bound_target != nullptr) {
            if (is_doubly_binding_master) {
                bound_target->removeListener(&binding_listener);
                bound_target->bound_target = nullptr;
                removeListener(&doubly_binding_listener);
                bound_target = nullptr;
                is_doubly_binding_master = false;
            } else {
                bound_target->doublyUnbind();
            }
        }
    }

protected:
    /**
     * Stored change listeners to be called when the instance updates.
     */
    std::vector<ChangeListener*> listeners;

    /**
     * The value of the Reactive instance.
     */
    T value;

private:
    /**
     * The one-way binding target to update the instance value from.
     */
    Reactive<T>* bound_target = nullptr;

    /**
     * Boolean value indicating whether this instance is the master in a two-way binding relationship.
     */
    bool is_doubly_binding_master = false;

    /**
     * The change listener owned by the instance to be used in one-way binding relationship.
     */
    ChangeListener binding_listener;

    /**
     * The change listener owned by the instance to be used in two-way binding relationship.
     */
    ChangeListener doubly_binding_listener;
};

}}

#endif //LIBUI_REACTIVE_H
