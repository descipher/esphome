#pragma once
#include "SoftwareSerial.h" // Using espsoftwareserial https://github.com/plerup/espsoftwareserial
#include "esphome/core/optional.h"
#include "callbacks.h"
#include "listeners.h"
#include "protocol.h"
#include "gdo_states.h"

namespace esphome {

class Scheduler;
class InternalGPIOPin;

namespace gdo {
    namespace dry_contact {

        using namespace esphome::gdo::protocol;

        class DryContact : public Protocol {
        public:
            void setup(GDOComponent* gdo, Scheduler* scheduler, InternalGPIOPin* rx_pin, InternalGPIOPin* tx_pin);
            void loop();
            void dump_config();

            void sync();

            void light_action(LightAction action);
            void lock_action(LockAction action);
            void door_action(DoorAction action);

            Result call(Args args);

            const Traits& traits() const { return this->traits_; }

        protected:
            Traits traits_;

            InternalGPIOPin* tx_pin_;
            InternalGPIOPin* rx_pin_;

            GDOComponent* gdo_;
            Scheduler* scheduler_;
        };

    } // namespace secplus1
} // namespace gdo
} // namespace esphome
