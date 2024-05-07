#include "gdo_light_output.h"
#include "../gdo_states.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gdo {

    using namespace esphome::light;

    static const char* const TAG = "gdo.light";

    void GDOLightOutput::dump_config()
    {
        ESP_LOGCONFIG(TAG, "GDO Light");
    }

    void GDOLightOutput::setup()
    {
        this->parent_->subscribe_light_state([=](LightState state) {
            this->on_light_state(state);
        });
    }

    void GDOLightOutput::on_light_state(esphome::gdo::LightState state)
    {
        if (this->light_state_) {
            this->has_initial_state_ = true;
            set_state(state);
        }
    }

    void GDOLightOutput::set_state(esphome::gdo::LightState state)
    {
        bool is_on = state == LightState::ON;
        this->light_state_->current_values.set_state(is_on);
        this->light_state_->remote_values.set_state(is_on);
        this->light_state_->publish_state();
    }

    void GDOLightOutput::setup_state(light::LightState* light_state)
    {
        esphome::gdo::LightState state = this->parent_->get_light_state();
        this->light_state_ = light_state;
        this->set_state(state);
    }

    LightTraits GDOLightOutput::get_traits()
    {
        auto traits = LightTraits();
        traits.set_supported_color_modes({ light::ColorMode::ON_OFF });
        return traits;
    }

    void GDOLightOutput::write_state(light::LightState* state)
    {
        if (!this->has_initial_state_)
            return;
        bool binary;
        state->current_values_as_binary(&binary);
        if (binary) {
            this->parent_->light_on();
        } else {
            this->parent_->light_off();
        }
    }

} // namespace gdo
} // namespace esphome
