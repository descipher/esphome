#pragma once
#include "SoftwareSerial.h"  // Using espsoftwareserial https://github.com/plerup/espsoftwareserial
#include "esphome/core/optional.h"
#include "callbacks.h"
#include "listeners.h"
#include "protocol.h"
#include "gdo_states.h"
#include "gdo.h"
#include <esp_timer.h>

namespace esphome {

class Scheduler;
class InternalGPIOPin;

namespace gdo {
class GDOComponent;

namespace secplus2 {

using namespace esphome::gdo::protocol;

static const uint8_t PACKET_LENGTH = 19;
typedef uint8_t WirePacket[PACKET_LENGTH];
static const uint8_t RF_PACKET_LENGTH = 4;
typedef uint8_t RFPacket[RF_PACKET_LENGTH];

enum class CommandType : uint16_t {
  UNKNOWN = 0x000,
  GET_STATUS = 0x080,
  STATUS = 0x081,
  OBST_1 = 0x084,  // sent when an obstruction happens?
  OBST_2 = 0x085,  // sent when an obstruction happens?
  BATTERY_STATUS = 0x09d,
  PAIR_3 = 0x0a0,
  PAIR_3_RESP = 0x0a1,
  LEARN = 0x181,
  LOCK = 0x18c,
  DOOR_ACTION = 0x280,
  LIGHT = 0x281,
  MOTOR_ON = 0x284,
  MOTION = 0x285,
  GET_PAIRED_DEVICES = 0x307,    // nibble 0 for total, 1 wireless, 2 keypads, 3 wall, 4 accessories.
  PAIRED_DEVICES = 0x308,        // byte2 holds number of paired devices
  CLEAR_PAIRED_DEVICES = 0x30D,  // nibble 0 to clear remotes, 1 keypads, 2 wall, 3 accessories (offset from above)
  LEARN_1 = 0x391,
  PING = 0x392,
  PING_RESP = 0x393,
  PAIR_2 = 0x400,
  PAIR_2_RESP = 0x401,
  SET_TTC = 0x402,  // ttc_in_seconds = (byte1<<8)+byte2
  CANCEL_TTC = 0x408,
  TTC = 0x40a,  // Time to close
  GET_OPENINGS = 0x48b,
  OPENINGS = 0x48c,  // openings = (byte1<<8)+byte2
};

inline const char *CommandType_to_string(CommandType _e) {
  switch (_e) {
    case CommandType::UNKNOWN:
      return "UNKNOWN";
    case CommandType::GET_STATUS:
      return "GET_STATUS";
    case CommandType::STATUS:
      return "STATUS";
    case CommandType::OBST_1:
      return "OBST_1";
    case CommandType::OBST_2:
      return "OBST_2";
    case CommandType::BATTERY_STATUS:
      return "BATTERY_STATUS";
    case CommandType::PAIR_3:
      return "PAIR_3";
    case CommandType::PAIR_3_RESP:
      return "PAIR_3_RESP";
    case CommandType::LEARN:
      return "LEARN";
    case CommandType::LOCK:
      return "LOCK";
    case CommandType::DOOR_ACTION:
      return "DOOR_ACTION";
    case CommandType::LIGHT:
      return "LIGHT";
    case CommandType::MOTOR_ON:
      return "MOTOR_ON";
    case CommandType::MOTION:
      return "MOTION";
    case CommandType::GET_PAIRED_DEVICES:
      return "GET_PAIRED_DEVICES";
    case CommandType::PAIRED_DEVICES:
      return "PAIRED_DEVICES";
    case CommandType::CLEAR_PAIRED_DEVICES:
      return "CLEAR_PAIRED_DEVICES";
    case CommandType::LEARN_1:
      return "LEARN_1";
    case CommandType::PING:
      return "PING";
    case CommandType::PING_RESP:
      return "PING_RESP";
    case CommandType::PAIR_2:
      return "PAIR_2";
    case CommandType::PAIR_2_RESP:
      return "PAIR_2_RESP";
    case CommandType::SET_TTC:
      return "SET_TTC";
    case CommandType::CANCEL_TTC:
      return "CANCEL_TTC";
    case CommandType::TTC:
      return "TTC";
    case CommandType::GET_OPENINGS:
      return "GET_OPENINGS";
    case CommandType::OPENINGS:
      return "OPENINGS";
    default:
      return "UNKNOWN";
  }
}
inline CommandType to_CommandType(uint16_t _t, CommandType _unknown) {
  switch (_t) {
    case 0x000:
      return CommandType::UNKNOWN;
    case 0x080:
      return CommandType::GET_STATUS;
    case 0x081:
      return CommandType::STATUS;
    case 0x084:
      return CommandType::OBST_1;
    case 0x085:
      return CommandType::OBST_2;
    case 0x09d:
      return CommandType::BATTERY_STATUS;
    case 0x0a0:
      return CommandType::PAIR_3;
    case 0x0a1:
      return CommandType::PAIR_3_RESP;
    case 0x181:
      return CommandType::LEARN;
    case 0x18c:
      return CommandType::LOCK;
    case 0x280:
      return CommandType::DOOR_ACTION;
    case 0x281:
      return CommandType::LIGHT;
    case 0x284:
      return CommandType::MOTOR_ON;
    case 0x285:
      return CommandType::MOTION;
    case 0x307:
      return CommandType::GET_PAIRED_DEVICES;
    case 0x308:
      return CommandType::PAIRED_DEVICES;
    case 0x30D:
      return CommandType::CLEAR_PAIRED_DEVICES;
    case 0x391:
      return CommandType::LEARN_1;
    case 0x392:
      return CommandType::PING;
    case 0x393:
      return CommandType::PING_RESP;
    case 0x400:
      return CommandType::PAIR_2;
    case 0x401:
      return CommandType::PAIR_2_RESP;
    case 0x402:
      return CommandType::SET_TTC;
    case 0x408:
      return CommandType::CANCEL_TTC;
    case 0x40a:
      return CommandType::TTC;
    case 0x48b:
      return CommandType::GET_OPENINGS;
    case 0x48c:
      return CommandType::OPENINGS;
    default:
      return _unknown;
  }
}

inline bool operator==(const uint16_t cmd_i, const CommandType &cmd_e) { return cmd_i == static_cast<uint16_t>(cmd_e); }
inline bool operator==(const CommandType &cmd_e, const uint16_t cmd_i) { return cmd_i == static_cast<uint16_t>(cmd_e); }

enum class IncrementRollingCode {
  NO,
  YES,
};

struct Command {
  CommandType type;
  uint8_t nibble;
  uint8_t byte1;
  uint8_t byte2;

  Command() : type(CommandType::UNKNOWN) {}
  Command(CommandType type_, uint8_t nibble_ = 0, uint8_t byte1_ = 0, uint8_t byte2_ = 0)
      : type(type_), nibble(nibble_), byte1(byte1_), byte2(byte2_) {}
};

class Secplus2 : public Protocol {
 public:
  void setup(GDOComponent *gdo, Scheduler *scheduler, InternalGPIOPin *rx_pin, InternalGPIOPin *tx_pin,
             InternalGPIOPin *rf_rx_pin, InternalGPIOPin *rf_tx_pin);
  void loop();
  void dump_config();
  void sync();
  void light_action(LightAction action);
  void lock_action(LockAction action);
  void door_action(DoorAction action);

  Result call(Args args);

  const Traits &traits() const { return this->traits_; }

 protected:
  void increment_rolling_code_counter(int delta = 1);
  void set_rolling_code_counter(uint32_t counter);
  void set_client_id(uint64_t client_id);

  optional<Command> read_command();
  void handle_command(const Command &cmd);

  void send_command(Command cmd, IncrementRollingCode increment = IncrementRollingCode::YES);
  void send_command(Command cmd, IncrementRollingCode increment, std::function<void()> &&on_sent);
  void encode_packet(Command cmd, WirePacket &packet);
  bool transmit_packet();
  void encode_rf_packets(Command cmd, RFPacket &packet1, RFPacket &packet2);
  bool transmit_rf_packets();

  void door_command(DoorAction action);

  void query_status();
  void query_openings();
  void query_paired_devices();
  void query_paired_devices(PairedDevice kind);
  void clear_paired_devices(PairedDevice kind);
  void activate_learn();
  void inactivate_learn();

  void print_packet(const char *prefix, const WirePacket &packet) const;
  optional<Command> decode_packet(const WirePacket &packet) const;

  void sync_helper(uint32_t start, uint32_t delay, uint8_t tries);

  LearnState learn_state_{LearnState::UNKNOWN};

  GDOListener<uint32_t> rolling_code_counter_{0};
  uint64_t client_id_{0x539};

  bool transmit_pending_{false};
  uint32_t transmit_pending_start_{0};
  bool transmit_rf_pending_{false};
  WirePacket tx_packet_;
  OnceCallbacks<void()> on_command_sent_;

  Traits traits_;

  SoftwareSerial sw_serial_;

  InternalGPIOPin *tx_pin_;
  InternalGPIOPin *rx_pin_;
  RFPacket rf_packet1_;
  RFPacket rf_packet2_;
  GDOComponent *gdo_;
  Scheduler *scheduler_;
};
}  // namespace secplus2
}  // namespace gdo
}  // namespace esphome
