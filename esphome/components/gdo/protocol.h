#pragma once

#include "gdo_states.h"

namespace esphome
{

    class Scheduler;
    class InternalGPIOPin;

    namespace gdo
    {

        class GDOComponent;

        namespace protocol
        {

            const uint32_t HAS_DOOR_OPEN = 1 << 0;  // has idempotent open door command
            const uint32_t HAS_DOOR_CLOSE = 1 << 1; // has idempotent close door command
            const uint32_t HAS_DOOR_STOP = 1 << 2;  // has idempotent stop door command
            const uint32_t HAS_DOOR_STATUS = 1 << 3;

            const uint32_t HAS_LIGHT_TOGGLE = 1 << 10; // some protocols might not support this

            const uint32_t HAS_LOCK_TOGGLE = 1 << 20;

            class Traits
            {
                uint32_t value;

            public:
                Traits()
                    : value(0)
                {
                }

                bool has_door_open() const { return this->value & HAS_DOOR_OPEN; }
                bool has_door_close() const { return this->value & HAS_DOOR_CLOSE; }
                bool has_door_stop() const { return this->value & HAS_DOOR_STOP; }
                bool has_door_status() const { return this->value & HAS_DOOR_STATUS; }

                bool has_light_toggle() const { return this->value & HAS_LIGHT_TOGGLE; }

                bool has_lock_toggle() const { return this->value & HAS_LOCK_TOGGLE; }

                void set_features(uint32_t feature) { this->value |= feature; }
                void clear_features(uint32_t feature) { this->value &= ~feature; }

                static uint32_t all()
                {
                    return HAS_DOOR_CLOSE | HAS_DOOR_OPEN | HAS_DOOR_STOP | HAS_DOOR_STATUS | HAS_LIGHT_TOGGLE | HAS_LOCK_TOGGLE;
                }
            };

            struct SetRollingCodeCounter
            {
                uint32_t counter;
            };
            struct GetRollingCodeCounter
            {
            };
            struct SetClientID
            {
                uint64_t client_id;
            };
            struct QueryStatus
            {
            };
            struct QueryOpenings
            {
            };
            struct ActivateLearn
            {
            };
            struct InactivateLearn
            {
            };
            struct QueryPairedDevices
            {
                PairedDevice kind;
            };
            struct QueryPairedDevicesAll
            {
            };
            struct ClearPairedDevices
            {
                PairedDevice kind;
            };

            class Args
            {
            public:
                union
                {
                    SetRollingCodeCounter set_rolling_code_counter;
                    GetRollingCodeCounter get_rolling_code_counter;
                    SetClientID set_client_id;
                    QueryStatus query_status;
                    QueryOpenings query_openings;
                    ActivateLearn activate_learn;
                    InactivateLearn inactivate_learn;
                    QueryPairedDevices query_paired_devices;
                    QueryPairedDevicesAll query_paired_devices_all;
                    ClearPairedDevices clear_paired_devices;
                } value;

                enum class Tag
                {
                    void_,
                    set_rolling_code_counter,
                    get_rolling_code_counter,
                    set_client_id,
                    query_status,
                    query_openings,
                    activate_learn,
                    inactivate_learn,
                    query_paired_devices,
                    query_paired_devices_all,
                    clear_paired_devices
                } tag;

                Args()
                    : tag(Tag::void_)
                {
                }
                Args(SetRollingCodeCounter &&arg)
                    : tag(Tag::set_rolling_code_counter)
                {
                    value.set_rolling_code_counter = std::move(arg);
                }
                Args(GetRollingCodeCounter &&arg)
                    : tag(Tag::get_rolling_code_counter)
                {
                    value.get_rolling_code_counter = std::move(arg);
                }
                Args(SetClientID &&arg)
                    : tag(Tag::set_client_id)
                {
                    value.set_client_id = std::move(arg);
                }
                Args(QueryStatus &&arg)
                    : tag(Tag::query_status)
                {
                    value.query_status = std::move(arg);
                }
                Args(QueryOpenings &&arg)
                    : tag(Tag::query_openings)
                {
                    value.query_openings = std::move(arg);
                }
                Args(ActivateLearn &&arg)
                    : tag(Tag::activate_learn)
                {
                    value.activate_learn = std::move(arg);
                }
                Args(InactivateLearn &&arg)
                    : tag(Tag::inactivate_learn)
                {
                    value.inactivate_learn = std::move(arg);
                }
                Args(QueryPairedDevices &&arg)
                    : tag(Tag::query_paired_devices)
                {
                    value.query_paired_devices = std::move(arg);
                }
                Args(QueryPairedDevicesAll &&arg)
                    : tag(Tag::query_paired_devices_all)
                {
                    value.query_paired_devices_all = std::move(arg);
                }
                Args(ClearPairedDevices &&arg)
                    : tag(Tag::clear_paired_devices)
                {
                    value.clear_paired_devices = std::move(arg);
                }
            };

            struct RollingCodeCounter
            {
                GDOListener<uint32_t> *value;
            };

            class Result
            {
            public:
                union
                {
                    RollingCodeCounter rolling_code_counter;
                } value;

                enum class Tag
                {
                    void_,
                    rolling_code_counter
                } tag;

                Result()
                    : tag(Tag::void_)
                {
                }

                Result(RollingCodeCounter &&arg)
                    : tag(Tag::rolling_code_counter)
                {
                    value.rolling_code_counter = std::move(arg);
                }
            };

            class Protocol
            {
            public:
                virtual void setup(GDOComponent *gdo, Scheduler *scheduler, InternalGPIOPin *rx_pin, InternalGPIOPin *tx_pin, InternalGPIOPin* rf_rx_pin, InternalGPIOPin* rf_tx_pin);
                virtual void loop();
                virtual void dump_config();

                virtual void sync();

                virtual const Traits &traits() const;

                virtual void light_action(LightAction action);
                virtual void lock_action(LockAction action);
                virtual void door_action(DoorAction action);

                virtual protocol::Result call(protocol::Args args);
            };

        }
    } // namespace gdo
} // namespace esphome
