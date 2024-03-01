#include "gdo_switch.h"
#include "../gdo_state.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gdo {

    static const char* const TAG = "gdo.switch";

    void GDOSwitch::dump_config()
    {
        LOG_SWITCH("", "GDO Switch", this);
        if (this->switch_type_ == SwitchType::GDO_LEARN) {
            ESP_LOGCONFIG(TAG, "  Type: Learn");
        }
    }

    void GDOSwitch::setup()
    {
        if (this->switch_type_ == SwitchType::GDO_LEARN) {
            this->parent_->subscribe_learn_state([=](LearnState state) {
                this->publish_state(state == LearnState::ACTIVE);
            });
        }
    }

    void GDOSwitch::write_state(bool state)
    {
        if (this->switch_type_ == SwitchType::GDO_LEARN) {
            if (state) {
                this->parent_->activate_learn();
            } else {
                this->parent_->inactivate_learn();
            }
        }
    }

} // namespace gdo
} // namespace esphome
