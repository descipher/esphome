#pragma once
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include <esp_timer.h>

#define ONE_SHOT_RX_TIME 375
#define ONE_SHOT_TX_TIME 250
#define RF_PREABLE 0x0000FFFF  // Uses MSB to achieve a preamble of 00000000000000001111

namespace esphome {
namespace gdo {

class RFTimer {
 public:
  esp_timer_handle_t rxTimer;
  esp_timer_handle_t txTimer;
  esp_timer_create_args_t rxtimerConfig;
  esp_timer_create_args_t txtimerConfig;
  uint32_t rx_delay_time{ONE_SHOT_RX_TIME};
  uint32_t tx_delay_time{ONE_SHOT_TX_TIME};
  uint32_t preamble{RF_PREABLE};
  static constexpr const char *const TAG = "GDO.RF";

  RFTimer() {
    rxtimerConfig.arg = this;
    rxtimerConfig.callback = reinterpret_cast<esp_timer_cb_t>(rx_pulse_cb);
    rxtimerConfig.dispatch_method = ESP_TIMER_TASK;
    rxtimerConfig.name = "rx_Timer";
    esp_timer_create(&rxtimerConfig, &rxTimer);
    txtimerConfig.arg = this;
    txtimerConfig.callback = reinterpret_cast<esp_timer_cb_t>(tx_pulse_cb);
    txtimerConfig.dispatch_method = ESP_TIMER_TASK;
    txtimerConfig.name = "tx_Timer";
    esp_timer_create(&txtimerConfig, &txTimer);
  }

  static void rx_pulse_cb(void *arg) {
    RFTimer *obj = (RFTimer *) arg;
    obj->rx_pulse();
  }

  static void tx_pulse_cb(void *arg) {
    RFTimer *obj = (RFTimer *) arg;
    obj->tx_pulse();
  }

  void start_rx_timer() { esp_timer_start_once(rxTimer, rx_delay_time); }
  void start_tx_timer() { esp_timer_start_periodic(txTimer, tx_delay_time); }
  void report_bits() { ESP_LOGD(TAG, "Bits: %d", this->bit_pos_); }
  void set_rx_pin(InternalGPIOPin *pin) { rx_pin_ = pin; };
  void set_tx_pin(InternalGPIOPin *pin) { tx_pin_ = pin; };
  void send_rf_cmd(uint64_t rf_cmd) {
    this->rf_tx_data_ = rf_cmd;
    this->rf_tx_data_ <<= 40;
    this->bit_pos_ = 0;
    this->clock_count_ = 0;
    this->tx_pin_->digital_write(false);
    delayMicroseconds(250);
    this->start_tx_timer();
  }

  void rx_pulse() {
    // ESP_LOGD(TAG,"Pulse");
    esp_timer_stop(rxTimer);
    bool value = rx_pin_->digital_read();
    this->rf_rx_data_ <<= 1;
    this->rf_rx_data_ |= value ? 1 : 0;
    this->bit_pos_++;
    ESP_LOGD(TAG, "Pulse:%08X", this->rf_rx_data_);
  }

  void tx_pulse() {
    bool value;
    //  Skip the 1st 250us
    if (this->clock_count_ == 0) {
      this->clock_count_++;
      return;
    }

    if (this->bit_pos_ >=64) {
      esp_timer_stop(txTimer);
      ESP_LOGD(TAG,"       DataIn: %s",this->rf_data_bits.c_str());
      ESP_LOGD(TAG,"ManchesterOut: %s",this->machester_bits.c_str());
      this->rf_data_bits.clear();
      this->machester_bits.clear();
      return;
    }

    if (this->bit_pos_ <= 20) {
      value = (this->preamble >> (31 - this->bit_pos_)) & 1;
    } else {
      value = (this->rf_tx_data_ >> (64 - (this->bit_pos_ - 20))) & 1;
    }

    if ((this->clock_count_ % 2) != 0) {  // not mid bit?
      this->tx_pin_->digital_write(!value);
      this->machester_bits.append(!value ? "1" : "0");
      this->rf_data_bits.append(value ? "1" : "0");
    } else {
      this->tx_pin_->digital_write(value);
      this->machester_bits.append(!value ? "1" : "0");
      this->bit_pos_++;
    }
    clock_count_++;
  }


  void tx_data(uint32_t rf_data, uint8_t bits) {

    // int8_t encode_v2(const uint32_t rolling, const uint64_t fixed, uint32_t data,
    //             const uint8_t frame_type, uint8_t *packet1, uint8_t *packet2)
    encode_v2(rolling, fixed, data, frame_type, &packet1, &packet2);

    //  Skip the 1st 250us
    if (this->clock_count_ == 0) {
      this->clock_count_++;
      return;
    }

    if (this->bit_pos_ >= bits) {
      esp_timer_stop(txTimer);
      ESP_LOGD(TAG,"       DataIn: %s",this->rf_data_bits.c_str());
      ESP_LOGD(TAG,"ManchesterOut: %s",this->machester_bits.c_str());
      return;
    }

    const bool value = (this->rf_tx_data_ >> this->bit_pos_) & 1;

    if ((this->clock_count_ % 2) != 0) {  // not mid bit?
      this->tx_pin_->digital_write(value);
      this->machester_bits.append(value ? "1" : "0");
      this->rf_data_bits.append(value ? "1" : "0");
    } else {
      this->tx_pin_->digital_write(!value);
      this->machester_bits.append(!value ? "1" : "0");
      this->bit_pos_++;
    }
    clock_count_++;
  }

 protected:
  uint8_t bit_pos_{0};
  InternalGPIOPin *rx_pin_;
  InternalGPIOPin *tx_pin_;
  uint64_t rf_rx_data_{0};
  uint64_t rf_tx_data_{0};
  uint8_t clock_count_{0};
  uint8_t packet1[16];
  uint8_t packet2[16];
  std::string machester_bits;
  std::string rf_data_bits;
};

class Manchester {
 public:
  // Encode table provide the next tx_pin transition state
  uint8_t tx_enc_tbl[2][4];
  uint64_t rf_rx_data;

 protected:
};

// class RFrx {
//  public:
//   // Setup must be called once, set up pin used to receive data
//   uint64_t rf_rx_data;

//   void rf_rx_setup(InternalGPIOPin *pin)
//   {
//     if (pin != nullptr) {
//       pin->setup();
//       pin->to_isr();
//       pin->pin_mode(gpio::FLAG_INPUT);
//       pin->attach_interrupt(RFStore::rx_isr_bits, &this->rf_isr_store_, gpio::INTERRUPT_RISING_EDGE);
//     }
//   }

//  protected:
//   void set_rf_rx_level_(bool rf_rx_level) { this->rf_rx_level_ = rf_rx_level; };
//   bool get_rf_rx_level_() { return this->rf_rx_level_; };
//   bool rf_rx_level_;
// };

// struct RFStore {
//   uint64_t rx_bits;
//   static void IRAM_ATTR rx_isr_bits(RFStore *arg) {
//     arg->rx_bits++;
//   }
// };

}  // namespace gdo
}  // namespace esphome
