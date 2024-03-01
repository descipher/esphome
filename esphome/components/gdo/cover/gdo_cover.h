#pragma once

#include "../gdo.h"
#include "../gdo_states.h"
#include "esphome/components/cover/cover.h"
#include "esphome/core/component.h"

namespace esphome {
namespace gdo {

    class GDOCover : public cover::Cover, public GDOClient, public Component {
    public:
        void dump_config() override;
        void setup() override;

        cover::CoverTraits get_traits() override;
        void on_door_state(DoorState state, float position);

    protected:
        void control(const cover::CoverCall& call) override;
    };

} // namespace gdo
} // namespace esphome
