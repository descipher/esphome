#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    enum SensorType {
        GDO_SENSOR_MOTION,
        GDO_SENSOR_OBSTRUCTION,
        GDO_SENSOR_MOTOR,
        GDO_SENSOR_BUTTON
    };

    class GDOBinarySensor : public binary_sensor::BinarySensor, public GDOClient, public Component {
    public:
        void setup() override;
        void dump_config() override;
        void set_binary_sensor_type(SensorType binary_sensor_type) { this->binary_sensor_type_ = binary_sensor_type; }

    protected:
        SensorType binary_sensor_type_;
    };

} // namespace gdo
} // namespace esphome
