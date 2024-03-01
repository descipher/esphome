#pragma once

#include "../gdo.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    enum SwitchType {
        GDO_LEARN
    };

    class GDOSwitch : public switch_::Switch, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;
        void set_switch_type(SwitchType switch_type_) { this->switch_type_ = switch_type_; }

        void write_state(bool state) override;

    protected:
        SwitchType switch_type_;
    };

} // namespace gdo
} // namespace esphome
