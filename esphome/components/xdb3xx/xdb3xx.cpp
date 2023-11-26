#include "xdb3xx.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <cinttypes>

namespace esphome {
namespace xdb3xx {

void XDB3XXComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up XDB3XX...");
  uint8_t data[3];
  if (write_byte(XDB3XX_REG_CMD, XDB3XX_MODE_CONTINUOS, true)) {
    read_byte(XDB3XX_REG_CMD, data, true);
    ESP_LOGVV(TAG, "Config Mode Reg: %d", data[0]);
  } else {
    this->mark_failed();
    return;
  }
}

void XDB3XXComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "XDB3XX:");
  LOG_I2C_DEVICE(this);
  LOG_SENSOR("  ", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("  ", "Pressure", this->pressure_sensor_);
}
float XDB3XXComponent::get_setup_priority() const { return setup_priority::DATA; }

void XDB3XXComponent::update() {
  ESP_LOGV(TAG, "Sending conversion request...");
  float converted{0};
  this->read_pressure();
  this->read_temperature();
  double converted_pressure = get_pressure();
  if (this->pressure_sensor_ != nullptr) {
    const std::string unit = this->pressure_sensor_->get_unit_of_measurement().c_str();
    if (unit == "PSI") {
      converted_pressure = converted_pressure * FACTOR_KPA_TO_PSI;
    } else if (unit == "Bar") {
      converted_pressure = converted_pressure * FACTOR_KPA_TO_BAR;
    } else if (unit == "hPa") {
      converted_pressure = converted_pressure * FACTOR_KPA_TO_HPA;
    }
    this->pressure_sensor_->publish_state((float) converted_pressure);
  }
  if (this->temperature_sensor_ != nullptr) {
    this->temperature_sensor_->publish_state(this->get_temperature());
  }
}

void XDB3XXComponent::read_temperature() {
  uint8_t reg_tdata[3]{0};
  read_bytes(XDB3XX_REG_TDATA1, reg_tdata, 3);
  int32_t temperature_raw = ((reg_tdata[0] * (1 << 16)) + (reg_tdata[1] * (1 << 8)) + (reg_tdata[2]));
  float temperature = static_cast<float>(temperature_raw);

  // Temperature conversion 2s compliment to normalized data format for Celsius
  if (temperature_raw > (1 << 23)) {
    temperature = (temperature - (1 << 24)) / (1 << 16) + 25;
  } else {
    temperature = (temperature / (1 << 16)) + 25;
  }
  this->set_temperature(temperature);
}

void XDB3XXComponent::read_pressure() {
  uint8_t reg_pdata[3]{0};
  read_bytes(XDB3XX_REG_PDATA1, reg_pdata, 3);

  // Combine the three bytes into a 24-bit signed integer
  int32_t pressure_raw = (reg_pdata[0] * (1 << 16)) + (reg_pdata[1] * (1 << 8)) + reg_pdata[2];
  ESP_LOGV(TAG, "Raw Read: 0x%08" PRIX32, pressure_raw);
  float pressure;

  // Pressure conversion 2's complement to normalized data format for hPa
  if (pressure_raw >= (1 << 23)) {
    // Negative value, adjust, normalize and apply transfer function
    pressure = (static_cast<float>(pressure_raw) - ((1 << 24) / (1 << 23)) * pressure_max_) - pressure_min_;

    ESP_LOGV(TAG, "Negative Read: %f", pressure);
  } else {
    // Positive value, normalize and apply transfer function
    pressure = ((static_cast<float>(pressure_raw) / (1 << 23)) * pressure_max_) - pressure_min_;
    ESP_LOGV(TAG, "Positive Read: %f", pressure);
  }

  this->set_pressure(pressure);
}

}  // namespace xdb3xx
}  // namespace esphome
