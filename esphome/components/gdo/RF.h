#pragma once
#include "gdo.h"
#include "esphome/core/gpio.h"
#include "esphome/core/log.h"
#include "esphome/core/scheduler.h"
#include "esphome/core/helpers.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include <esp_timer.h>

#define RX_DELAY_TIME 375
#define TX_DELAY_TIME 250
#define PACKET_SIZE_BITS 40
#define PREAMBLE_BITS 20
#define PACKET_ID_BITS 2
#define RF_PREABLE 0x0000FFFF  // Uses MSB to achieve a preamble of 00000000000000001111

namespace esphome {
namespace gdo {

static const uint8_t RF_PACKET_LENGTH = 4;
typedef uint8_t RFPacket[RF_PACKET_LENGTH];

class RF {
 public:
  uint32_t preamble{RF_PREABLE};
  esp_timer_handle_t rxTimer;
  esp_timer_handle_t txTimer;
  esp_timer_create_args_t rxtimerConfig;
  esp_timer_create_args_t txtimerConfig;
  uint32_t rx_delay_time{RX_DELAY_TIME};
  uint32_t tx_delay_time{TX_DELAY_TIME};

  volatile bool rf_tx_active;
  static constexpr const char *const TAG = "gdo.rf";

  void init_rf_timer() {
    this->txtimerConfig.arg = this;
    this->txtimerConfig.callback = reinterpret_cast<esp_timer_cb_t>(tx_pulse_cb);
    this->txtimerConfig.dispatch_method = ESP_TIMER_TASK;
    this->txtimerConfig.name = "txTimer";
    esp_timer_create(&this->txtimerConfig, &this->txTimer);
  }
  static void rx_pulse_cb(RF *arg) {
    esp_timer_stop(arg->rxTimer);
    bool value = arg->rf_rx_pin_->digital_read();
    arg->rf_rx_data_[0] <<= 1;
    arg->rf_rx_data_[0] |= value ? 1 : 0;
    arg->bit_pos_++;
  }

  static void tx_pulse_cb(RF *arg) { arg->tx_packet(); }

  void start_rx_timer() { esp_timer_start_once(this->rxTimer, this->rx_delay_time); }
  void start_tx_timer() { esp_timer_start_periodic(this->txTimer, this->tx_delay_time); }
  void stop_rx_timer() { esp_timer_stop(this->rxTimer); }
  void stop_tx_timer() { esp_timer_stop(this->txTimer); }
  bool is_tx_active() { return esp_timer_is_active(this->txTimer); }
  void report_bits() { ESP_LOGD(TAG, "Bits: %d", this->bit_pos_); }
  void set_rx_pin(InternalGPIOPin *pin) { rf_rx_pin_ = pin; };
  void set_tx_pin(InternalGPIOPin *pin) { rf_tx_pin_ = pin; };
  inline void send_rf_cmd(volatile RFPacket packet1, volatile RFPacket packet2, const uint8_t packet_size,
                   const uint8_t size) {
    this->packet_number_ = 0;
    this->packet_delay_counter_ = 0;
    this->packet_size_ = packet_size;
    this->rf_tx_data_[0] = 0;
    this->rf_tx_data_[1] = 0;

    for (int i = 0; i < size; i++) {
      this->rf_tx_data_[0] |= static_cast<uint64_t>(packet1[i]) << (8 * i);
      this->rf_tx_data_[1] |= static_cast<uint64_t>(packet2[i]) << (8 * i);
    }
    this->bit_pos_ = 0;
    this->clock_count_ = 0;
    this->rf_tx_pin_->digital_write(false);
    this->start_tx_timer();
    this->rf_tx_active = true;
  }

  inline void tx_packet() {
    bool value;
    if (this->clock_count_ == 0) {
      // Init clock counter to mid bit
      this->clock_count_++;
      return;
    }

    if (this->bit_pos_ > this->packet_size_ + PREAMBLE_BITS + PACKET_ID_BITS - 1) {
      if (this->packet_number_ > 0) {
        esp_timer_stop(txTimer);
        this->rf_tx_active = false;
        this->rf_tx_pin_->digital_write(false);
        return;
      } else {
        if (this->packet_delay_counter_ > 120) {
          this->packet_number_++;
          this->bit_pos_ = 0;
          this->clock_count_ = 0;
          return;
        }
        this->rf_tx_pin_->digital_write(false);
        this->packet_delay_counter_++;
        return;
      }
    }

    if (this->bit_pos_ < 20) {
      value = (this->preamble >> (31 - this->bit_pos_)) & 1;
    } else {
      if (this->bit_pos_ <  22) {
        value = (this->packet_number_ >> (21 - this->bit_pos_)) & 1;
      } else {
        value = (this->rf_tx_data_[this->packet_number_] >> (63 - (this->bit_pos_) - 22)) & 1;
      }
    }

    if ((this->clock_count_ % 2) != 0) {  // not mid bit?
      this->rf_tx_pin_->digital_write(!value);
    } else {
      this->rf_tx_pin_->digital_write(value);
      this->bit_pos_++;
    }
    this->clock_count_++;
  }

 protected:
  volatile uint8_t bit_pos_{0};
  InternalGPIOPin *rf_rx_pin_;
  InternalGPIOPin *rf_tx_pin_;
  volatile uint64_t rf_rx_data_[2];
  volatile uint64_t rf_tx_data_[2];
  volatile uint16_t clock_count_{0};
  uint8_t packet1[16];
  uint8_t packet2[16];
  uint8_t packet_number_{0};
  uint8_t packet_size_{40};
  uint16_t packet_delay_counter_{0};
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
