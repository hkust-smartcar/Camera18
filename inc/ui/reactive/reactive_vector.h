//
// Created by Daniel on 5/6/2018.
//

#ifndef LIBUI_REACTIVE_VECTOR_H
#define LIBUI_REACTIVE_VECTOR_H

#include <ui/reactive/reactive.h>
#include <vector>
#include <cstdarg>

namespace ui { namespace reactive {

template <class T>
class ReactiveVector: public ui::reactive::Reactive<std::vector<T>> {
public:

};

}}


#endif //LIBUI_REACTIVE_VECTOR_H
