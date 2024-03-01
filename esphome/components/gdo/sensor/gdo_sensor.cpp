#include "gdo_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gdo {

    static const char* const TAG = "gdo.sensor";

    void GDOSensor::setup()
    {
        if (this->gdo_sensor_type_ == GDOSensorType::GDO_OPENINGS) {
            this->parent_->subscribe_openings([=](uint16_t value) {
                this->publish_state(value);
            });
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_DEVICES_TOTAL) {
            this->parent_->subscribe_paired_devices_total([=](uint16_t value) {
                this->publish_state(value);
            });
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_REMOTES) {
            this->parent_->subscribe_paired_remotes([=](uint16_t value) {
                this->publish_state(value);
            });
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_KEYPADS) {
            this->parent_->subscribe_paired_keypads([=](uint16_t value) {
                this->publish_state(value);
            });
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_WALL_CONTROLS) {
            this->parent_->subscribe_paired_wall_controls([=](uint16_t value) {
                this->publish_state(value);
            });
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_ACCESSORIES) {
            this->parent_->subscribe_paired_accessories([=](uint16_t value) {
                this->publish_state(value);
            });
        }
    }

    void GDOSensor::dump_config()
    {
        LOG_SENSOR("", "GDO Sensor", this);
        if (this->gdo_sensor_type_ == GDOSensorType::GDO_OPENINGS) {
            ESP_LOGCONFIG(TAG, "  Type: Openings");
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_DEVICES_TOTAL) {
            ESP_LOGCONFIG(TAG, "  Type: Paired Devices");
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_REMOTES) {
            ESP_LOGCONFIG(TAG, "  Type: Paired Remotes");
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_KEYPADS) {
            ESP_LOGCONFIG(TAG, "  Type: Paired Keypads");
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_WALL_CONTROLS) {
            ESP_LOGCONFIG(TAG, "  Type: Paired Wall Controls");
        } else if (this->gdo_sensor_type_ == GDOSensorType::GDO_PAIRED_ACCESSORIES) {
            ESP_LOGCONFIG(TAG, "  Type: Paired Accessories");
        }
    }

} // namespace gdo
} // namespace esphome
