/************************************
 * Garage
 * Door
 * Opener
 *
 * Copyright (C) 2022  Paul Wieland
 * Copyright (C) 2024  Mike La Spina
 *
 * GNU GENERAL PUBLIC LICENSE
 ************************************/

#pragma once
#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/preferences.h"
#include "callbacks.h"
#include "listeners.h"
#include "protocol.h"
#include "gdo_states.h"
#include "RF.h"

#define ESP_LOG1 ESP_LOGV
#define ESP_LOG2 ESP_LOGV

namespace esphome
{
    class InternalGPIOPin;
    namespace gdo
    {

        class GDOComponent;
        typedef Parented<GDOComponent> GDOClient;

        const float DOOR_POSITION_UNKNOWN = -1.0;
        const float DOOR_DELTA_UNKNOWN = -2.0;
        const uint16_t PAIRED_DEVICES_UNKNOWN = 0xFF;
        static const uint8_t RF_PACKET_LENGTH = 19;
        typedef uint8_t RFPacket[RF_PACKET_LENGTH];

        struct GDOStore
        {
            int obstruction_low_count = 0; // count obstruction low pulses

            static void IRAM_ATTR HOT isr_obstruction(GDOStore *arg)
            {
                arg->obstruction_low_count++;
            }
        };

        struct RFStore
        {
            RFTimer rf_timer;
            static void IRAM_ATTR HOT rx_isr_bits(RFStore *arg)
            {
                arg->rf_timer.start_rx_timer();
            }
        };

        struct Openings
        {
            uint16_t count;
            uint8_t flag;
        };

        struct PairedDeviceCount
        {
            PairedDevice kind;
            uint16_t count;
        };

        struct TimeToClose
        {
            uint16_t seconds;
        };

        using protocol::Args;
        using protocol::Result;

        class GDOComponent : public Component
        {
        public:
            void setup() override;
            void loop() override;
            void dump_config() override;
            void init_protocol();
            void obstruction_loop();

            float start_opening{-1};
            GDOListener<float> opening_duration{0};
            float start_closing{-1};
            GDOListener<float> closing_duration{0};

            GDOListener<uint16_t> openings{0}; // number of times the door has been opened
            GDOListener<uint16_t> paired_total{PAIRED_DEVICES_UNKNOWN};
            GDOListener<uint16_t> paired_remotes{PAIRED_DEVICES_UNKNOWN};
            GDOListener<uint16_t> paired_keypads{PAIRED_DEVICES_UNKNOWN};
            GDOListener<uint16_t> paired_wall_controls{PAIRED_DEVICES_UNKNOWN};
            GDOListener<uint16_t> paired_accessories{PAIRED_DEVICES_UNKNOWN};

            GDOListener<DoorState> door_state{DoorState::UNKNOWN};
            GDOListener<float> door_position{DOOR_POSITION_UNKNOWN};

            unsigned long door_start_moving{0};
            float door_start_position{DOOR_POSITION_UNKNOWN};
            float door_move_delta{DOOR_DELTA_UNKNOWN};

            GDOListener<LightState> light_state{LightState::UNKNOWN};
            GDOListener<LockState> lock_state{LockState::UNKNOWN};
            GDOListener<ObstructionState> obstruction_state{ObstructionState::UNKNOWN};
            GDOListener<MotorState> motor_state{MotorState::UNKNOWN};
            GDOListener<ButtonState> button_state{ButtonState::UNKNOWN};
            GDOListener<MotionState> motion_state{MotionState::UNKNOWN};
            GDOListener<LearnState> learn_state{LearnState::UNKNOWN};

            OnceCallbacks<void(DoorState)> on_door_state_;

            GDOListener<bool> sync_failed{false};

            void set_output_gdo_pin(InternalGPIOPin *pin) { this->output_gdo_pin_ = pin; }
            void set_input_gdo_pin(InternalGPIOPin *pin) { this->input_gdo_pin_ = pin; }
            void set_input_obst_pin(InternalGPIOPin *pin) { this->input_obst_pin_ = pin; }
            void set_rf_tx_pin(InternalGPIOPin *pin) { this->rf_tx_pin_ = pin; }
            void set_rf_rx_pin(InternalGPIOPin *pin) { this->rf_rx_pin_ = pin; }

            Result call_protocol(Args args);

            void received(const DoorState door_state);
            void received(const LightState light_state);
            void received(const LockState lock_state);
            void received(const ObstructionState obstruction_state);
            void received(const LightAction light_action);
            void received(const MotorState motor_state);
            void received(const ButtonState button_state);
            void received(const MotionState motion_state);
            void received(const LearnState light_state);
            void received(const Openings openings);
            void received(const TimeToClose ttc);
            void received(const PairedDeviceCount pdc);
            void received(const BatteryState pdc);

            // door
            void door_toggle();
            void door_open();
            void door_close();
            void door_stop();

            void door_action(DoorAction action);
            void ensure_door_action(DoorAction action, uint32_t delay = 1500);
            void door_move_to_position(float position);
            void set_door_position(float door_position) { this->door_position = door_position; }
            void set_opening_duration(float duration);
            void set_closing_duration(float duration);
            void schedule_door_position_sync(float update_period = 500);
            void door_position_update();
            void cancel_position_sync_callbacks();

            // light
            void light_toggle();
            void light_on();
            void light_off();
            LightState get_light_state() const;

            // lock
            void lock_toggle();
            void lock();
            void unlock();

            // Learn & Paired
            void activate_learn();
            void inactivate_learn();
            void query_paired_devices();
            void query_paired_devices(PairedDevice kind);
            void clear_paired_devices(PairedDevice kind);

            // button functionality
            void query_status();
            void query_openings();
            void sync();

            // children subscriptions
            void subscribe_rolling_code_counter(std::function<void(uint32_t)> &&f);
            void subscribe_opening_duration(std::function<void(float)> &&f);
            void subscribe_closing_duration(std::function<void(float)> &&f);
            void subscribe_openings(std::function<void(uint16_t)> &&f);
            void subscribe_paired_devices_total(std::function<void(uint16_t)> &&f);
            void subscribe_paired_remotes(std::function<void(uint16_t)> &&f);
            void subscribe_paired_keypads(std::function<void(uint16_t)> &&f);
            void subscribe_paired_wall_controls(std::function<void(uint16_t)> &&f);
            void subscribe_paired_accessories(std::function<void(uint16_t)> &&f);
            void subscribe_door_state(std::function<void(DoorState, float)> &&f);
            void subscribe_light_state(std::function<void(LightState)> &&f);
            void subscribe_lock_state(std::function<void(LockState)> &&f);
            void subscribe_obstruction_state(std::function<void(ObstructionState)> &&f);
            void subscribe_motor_state(std::function<void(MotorState)> &&f);
            void subscribe_button_state(std::function<void(ButtonState)> &&f);
            void subscribe_motion_state(std::function<void(MotionState)> &&f);
            void subscribe_sync_failed(std::function<void(bool)> &&f);
            void subscribe_learn_state(std::function<void(LearnState)> &&f);

        protected:

            protocol::Protocol *protocol_;
            bool obstruction_from_status_{false};
            bool wire_tx_pending_{false};
            bool rf_rx_packet_pending_{false};
            RFPacket rf_tx_packet_;
            GDOStore isr_store_{};
            RFStore rf_isr_store_;
            InternalGPIOPin *output_gdo_pin_{nullptr};
            InternalGPIOPin *input_gdo_pin_{nullptr};
            InternalGPIOPin *input_obst_pin_{nullptr};
            InternalGPIOPin *rf_tx_pin_{nullptr};;
            InternalGPIOPin *rf_rx_pin_{nullptr};;

        }; // GDOComponent

    } // namespace gdo
} // namespace esphome
