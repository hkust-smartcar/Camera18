//
// Created by Daniel on 29/5/2018.
//

#ifndef LIBUI_BATTERY_METER_ADAPTER_H
#define LIBUI_BATTERY_METER_ADAPTER_H

namespace ui {
    namespace adapters {
        class BatteryMeterAdapterInterface {
        public:
            virtual float getVoltage() = 0;
        };
    }
}


#endif //LIBUI_BATTERY_METER_ADAPTER_H
