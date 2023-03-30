#include "geiger.h"

namespace esphome {
namespace geiger {

static const char *const TAG = "Geiger";
const char *const getData = "{\"get\":\"data\"}\n";
const char *const getConfig = "{\"get\":\"config\"}\n";

// Used to send IP address to a GRGC1 model
network::IPAddress addr;

void GEIGER::loop() {

  if (this->model_ == GEIGER_MODEL_GRGC1) {
    uint8_t data;
    while (this->available() > 0) {
      if (this->read_byte(&data)) {
        this->rx_buff_ += (char) data;
        // Test if buffer full or EOL
        if (this->rx_buff_.back() == static_cast<char>(10) || this->rx_buff_.length() >= 128) {
          this->get_gc_data_(); // Process completed line
        }
      }
    }
  }
}

void GEIGER::setup() {

  ESP_LOGCONFIG(TAG, "Setting up Geiger Counter ...");

  if (this->model_ == GEIGER_MODEL_GRGC1) {
    this->write_byte(10); // Flush remote buffer data                                                                                                                                                                                                                 if any
    this->write_str(getConfig);
    addr = network::get_ip_address();
    #ifdef USE_TIME
      setup_time_();
    #endif
  }
}

void GEIGER::dump_config() {
  ESP_LOGCONFIG(TAG, "geiger:");
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    ESP_LOGCONFIG(TAG, "Model: GRGC1");
    ESP_LOGCONFIG(TAG, "Sound: %s", gccfg.sound_ ? "enabled" : "disabled");
    ESP_LOGCONFIG(TAG, "Tube: %d", gccfg.tube_ );
  }
}

void GEIGER::update() {

  #ifdef USE_TIME
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    if (this->time_id_.has_value()) {
      auto *time_id = *this->time_id_;
      time::ESPTime now = time_id->now();
      if (now.is_valid()) {
        // We are good to go
      } else { time_id->timestamp_now();}
    }
  #endif
  this->write_str(getData);
  }

}

float GEIGER::get_setup_priority() const { return setup_priority::AFTER_WIFI; }

void GEIGER::set_gc_clock(uint8_t hour, uint8_t minute, uint8_t second, uint8_t month, uint8_t day , uint16_t year) {
  char td_buff[24];
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    tx_buff_.clear();
    gcConfig_.clear();
    sprintf(td_buff, "%02d/%02d/%04d",month ,day ,year);
    gcConfig_["date"] = td_buff;
    sprintf(td_buff, "%02d:%02d:%02d",hour ,minute ,second);
    gcConfig_["time"] = td_buff;
    serializeJson(gcConfig_, tx_buff_);
    this->write_str(tx_buff_.c_str());
    this->write_byte(10);
  }
}

bool GEIGER::get_gc_data_() {
  bool success = false;
  uint16_t cpm=0, cps=0, voltage = 0;
  double dose = 0;

  if (this->model_ == GEIGER_MODEL_GRGC1) {
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(gcData_,rx_buff_);
    this->rx_buff_.clear();
    if (error) {
      ESP_LOGE(TAG, "A JSON parsing error occured with the serial data: ","%d",error);
    } else {
      bool success = true;
      gcDataObj_ = gcData_.as<JsonObject>();
    }

    if (gcDataObj_.containsKey("cpm")) {
      cpm = gcDataObj_["cpm"].as<unsigned int>();
      cps = gcDataObj_["cps"].as<unsigned int>();
      dose = gcDataObj_["dose"].as<double>();
      voltage = gcDataObj_["voltage"].as<unsigned int>();


      if (this->geiger_cpm_ != nullptr) {
        this->geiger_cpm_->publish_state(cpm);
      }
      if (this->geiger_cps_ != nullptr) {
        this->geiger_cps_->publish_state(cps);
      }
      if (this->geiger_dose_ != nullptr) {
        this->geiger_dose_->publish_state(dose);
      }
      if (this->geiger_voltage_ != nullptr) {
        this->geiger_voltage_->publish_state(voltage);
      }
    }

    // Configuration and initial time setup
    if (gcConfigured_ == false) {
          this->set_gc_config_();
          this->send_local_time();
          gcConfigured_ = true;
    }

  }
  return success;
}

bool GEIGER::set_gc_config_() {
  bool success = false;
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    tx_buff_.clear();
    gcConfig_.clear();
    gcConfig_["sound"] = gccfg.sound_ ? 1 : 0;
    gcConfig_["tube"] = gccfg.tube_;
    gcConfig_["ip"] = addr.str().c_str();
    serializeJson(gcConfig_, this->tx_buff_);
    this->write_str(this->tx_buff_.c_str());
    this->write_byte(10);
    this->write_str(getConfig);   // Request a configuration update
    bool success = true;
  }
  return success;
}

/* Time */
#ifdef USE_TIME

void GEIGER::send_local_time() {
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    if (this->time_id_.has_value()) {
      auto *time_id = *this->time_id_;
      time::ESPTime tm = time_id->now();
      if (tm.is_valid()) {
        this->set_gc_clock(tm.hour, tm.minute, tm.second, tm.day_of_month, tm.month, tm.year);
        ESP_LOGD(TAG, "Using time component to set geiger clock: %02d:%02d:%02d %02d/%02d/%d", tm.hour, tm.minute, tm.second, tm.day_of_month ,tm.month, tm.year);
      }
    } else {
      ESP_LOGI(TAG, "`time_id` is not configured: cannot not sync geiger clock.");
    }
  }
}

void GEIGER::setup_time_() {
  if (this->model_ == GEIGER_MODEL_GRGC1) {
    if (this->time_id_.has_value()) {
      this->send_local_time();
      auto *time_id = *this->time_id_;
      time_id->add_on_time_sync_callback([this] { this->send_local_time(); });
    } else {
      ESP_LOGI(TAG, "`time_id` is not configured: will not sync geiger clock.");
    }
  }
}
#endif

}  // namespace geiger
}  // namespace esphome
