#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/number/number.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    enum NumberType {
        GDO_CLIENT_ID,
        GDO_ROLLING_CODE_COUNTER,
        GDO_OPENING_DURATION,
        GDO_CLOSING_DURATION,
    };

    class GDONumber : public number::Number, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;
        void set_number_type(NumberType number_type);
        // other esphome components that persist state in the flash have HARDWARE priority
        // ensure we get initialized before them, so that the state doesn't get invalidated
        // by components that might be added in the future
        float get_setup_priority() const override { return setup_priority::HARDWARE + 1; }

        void update_state(float value);
        void control(float value) override;

    protected:
        NumberType number_type_;
        ESPPreferenceObject pref_;
    };

} // namespace gdo
} // namespace esphome
