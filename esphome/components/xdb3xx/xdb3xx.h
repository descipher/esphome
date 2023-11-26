#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace xdb3xx {

/*
PDATA1 - Signed, 2 's complement: stores calibrated pressure sensor data
When RAW_P = 1, store the ADC output of the main signal channel
when RAW_P = 0, store the corrected sensor data
Code_P = Data0x06*2^16+ Data0x07*2^8+ Data0x08

TDATA
Signed，2’s complement:
When RAW_T = 1, stores the ADC output of temperature channel
When RAW_T = 0, stores the calibrated temperature data
LSB=1/2^16℃. Real Temperature =TDATA/2^16+25 ℃
*/

static const char *const TAG = "xdb3xx.sensor";
static const uint8_t XDB306_I2C_ADDRESS = 0x6D;      // Reference only
static const uint8_t XDB3XX_MODE_CONFIG = 0x00;      // Config mode
static const uint8_t XDB3XX_MODE_SINGLE = 0x01;      // Config  - single mode low power
static const uint8_t XDB3XX_MODE_CONTINUOS = 0x03;   // Config continuos mode (default)
static const uint8_t XDB3XX_STATUS = 0x02;           // Status bit 0 - Set after data updated cleared after read
static const uint8_t XDB3XX_MODE_PROG = 0x33;        // Enter EEPROM programing mode
static const uint8_t XDB3XX_REG_CMD = 0x30;          // Command register address
static const uint8_t XDB3XX_REG_PDATA1 = 0x06;       // Pressure data address <23:16>
static const uint8_t XDB3XX_REG_PDATA2 = 0x07;       // Pressure data address <15:8>
static const uint8_t XDB3XX_REG_PDATA3 = 0x08;       // Pressure data address <07:0>
static const uint8_t XDB3XX_REG_TDATA1 = 0x09;       // Temperature data address <23:16>
static const uint8_t XDB3XX_REG_TDATA2 = 0x0A;       // Temperature data address <15:8>
static const uint8_t XDB3XX_REG_TDATA3 = 0x0B;       // Temperature data address <07:0>
static const float FACTOR_KPA_TO_PSI = 0.1450377;
static const float FACTOR_KPA_TO_BAR = 0.01;
static const float FACTOR_KPA_TO_HPA = 10;


/// This class implements support for the XDB3XX Temperature+Pressure i2c sensor.
class XDB3XXComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }
  void set_pressure_sensor(sensor::Sensor *pressure_sensor) { pressure_sensor_ = pressure_sensor; }
  void read_temperature();
  void read_pressure();
  float get_pressure() { return this->pressure_value_; }
  float get_temperature() { return this->temperature_value_; }
  void set_pressure(float val) { this->pressure_value_ = val; }
  void set_temperature(float val) { this->temperature_value_ = val; }
  void set_pressure_min(int val) { this->pressure_min_ = val; }
  void set_pressure_max(int val) { this->pressure_max_ = val; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  int pressure_min_{0};
  int pressure_max_{0};
  float pressure_value_{0};
  float temperature_value_{0};

  sensor::Sensor *temperature_sensor_{nullptr};
  sensor::Sensor *pressure_sensor_{nullptr};
};

}  // namespace xdb3xx
}  // namespace esphome
