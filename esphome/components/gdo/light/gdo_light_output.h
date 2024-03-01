#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/light/light_output.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    class GDOLightOutput : public light::LightOutput, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;
        light::LightTraits get_traits() override;
        void write_state(light::LightState* state) override;
        void setup_state(light::LightState* state) override;
        void set_state(esphome::gdo::LightState state);
        light::LightState* get_state() { return this->light_state_; }

        void on_light_state(esphome::gdo::LightState state);

    protected:
        light::LightState* light_state_;
        bool has_initial_state_ = false;
    };

} // namespace gdo
} // namespace esphome
