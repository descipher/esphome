#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    enum GDOSensorType {
        GDO_OPENINGS,
        GDO_PAIRED_DEVICES_TOTAL,
        GDO_PAIRED_REMOTES,
        GDO_PAIRED_KEYPADS,
        GDO_PAIRED_WALL_CONTROLS,
        GDO_PAIRED_ACCESSORIES
    };

    class GDOSensor : public sensor::Sensor, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;
        void set_gdo_sensor_type(GDOSensorType gdo_sensor_type_) { this->gdo_sensor_type_ = gdo_sensor_type_; }

    protected:
        GDOSensorType gdo_sensor_type_;
    };

} // namespace gdo
} // namespace esphome
