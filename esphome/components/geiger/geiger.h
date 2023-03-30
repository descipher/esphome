#pragma once
#include <ESP8266mDNS.h>
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/json/json_util.h"
#include "esphome/components/network/ip_address.h"
#include "esphome/components/network/util.h"
#include "esphome/components/sensor/sensor.h"
#ifdef USE_TIME
#include "esphome/components/time/real_time_clock.h"
#endif
#include "esphome/components/wifi/wifi_component.h"
#include "esphome/components/uart/uart.h"
#include <vector>


namespace esphome {
namespace geiger {

enum GEIGERModel {
  GEIGER_MODEL_GRGC1,
  GEIGER_MODEL_GRGC2,
};

enum GEIGERTube {
  GEIGER_TUBE_UNKNOWN,
  GEIGER_TUBE_SBM20,
  GEIGER_TUBE_SI29BG,
  GEIGER_TUBE_SBM19,
  GEIGER_TUBE_LND712,
  GEIGER_TUBE_SBM20M,
  GEIGER_TUBE_SI22G,
  GEIGER_TUBE_STS5,
  GEIGER_TUBE_SI3BG,
  GEIGER_TUBE_SBM21,
  GEIGER_TUBE_SBT9,
  GEIGER_TUBE_SI1G,
  GEIGER_TUBE_J305,
  GEIGER_TUBE_M4011,
};

/// Component for reading radiation measurements from geiger counters.
class GEIGER : public PollingComponent, public uart::UARTDevice {
 public:
  /** Manually select the GEIGER model.
   *
   * Valid values are:
   *
   *  - GEIGER_MODEL_PULSE
   *  - GEIGER_MODEL_GRGC1
   *  - GEIGER_MODEL_GRGC2
   *
   * @param model The GEIGER model.
   */


  void * set_config_callback(void * context){
    return reinterpret_cast<GEIGER*>(context)->parse_json();
  }

  void * parse_json(){ return nullptr; }

  void set_geiger_model(GEIGERModel model) { model_ = model; }
  void set_geiger_tube(GEIGERTube tube) { gccfg.tube_ = tube; }
  void set_geiger_cpm(sensor::Sensor *geiger_cpm) { geiger_cpm_ = geiger_cpm; }
  void set_geiger_cps(sensor::Sensor *geiger_cps) { geiger_cps_ = geiger_cps; }
  void set_geiger_dose(sensor::Sensor *geiger_dose) { geiger_dose_ = geiger_dose; }
  void set_geiger_voltage(sensor::Sensor *geiger_voltage) { geiger_voltage_ = geiger_voltage; }
  void set_geiger_sound(bool sound) { gccfg.sound_ = sound; }
  void set_pulse_pin(InternalGPIOPin *pin) { pin_ = pin; }
  float get_CPM2uSVh(uint8_t tube, uint32_t cpm);
  float get_detector_factor(uint8_t factor);
  float get_setup_priority() const override;
  void loop() override;
  void setup() override;
  void dump_config() override;
  void update() override;

  #ifdef USE_TIME
    /** Set the `time::RealTimeClock` implementation. */
    void set_time_id(time::RealTimeClock *time_id) { this->time_id_ = time_id; }
    /** Attempts to sync the local time (via `time_id`) to the Geiger device. */
    void send_local_time();
  #endif
  void set_gc_clock(uint8_t hour, uint8_t minute, uint8_t second, uint8_t month, uint8_t day , uint16_t year);

protected:
  bool get_gc_data_();
  bool set_gc_config_();
  bool gcConfigured_;
  InternalGPIOPin *pin_;
  std::string tx_buff_;
  std::string rx_buff_;
  JsonObject gcDataObj_;
  StaticJsonDocument<128> gcData_;
  StaticJsonDocument<128> gcConfig_;
  GEIGERModel model_{GEIGER_MODEL_GRGC1};
  sensor::Sensor *geiger_cpm_{nullptr};
  sensor::Sensor *geiger_cps_{nullptr};
  sensor::Sensor *geiger_dose_{nullptr};
  sensor::Sensor *geiger_voltage_{nullptr};

  #ifdef USE_TIME
    /** Initializes time sync callbacks to support syncing current time to the BedJet. */
    void setup_time_();
    optional<time::RealTimeClock *> time_id_{};
  #endif

  struct GRGC1_Config {
    char ssid_[32];
    char host_[32];
    char ip_[16];
    uint32_t unitID_;
    bool sound_;
    uint8_t tube_;
    } gccfg;

};

}  // namespace geiger
}  // namespace esphome
