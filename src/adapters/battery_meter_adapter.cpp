//
// Created by Daniel on 30/5/2018.
//

#include "adapters/battery_meter_adapter.h"

float adapters::BatteryMeterAdapter::getVoltage() {
    return battery_meter_ptr->GetVoltage();
}
