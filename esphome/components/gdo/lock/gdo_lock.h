#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/lock/lock.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    class GDOLock : public lock::Lock, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;

        void on_lock_state(LockState state);
        void control(const lock::LockCall& call) override;
    };

} // namespace gdo
} // namespace esphome
