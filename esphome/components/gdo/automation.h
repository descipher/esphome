
#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "gdo.h"

namespace esphome {
namespace gdo {

    class SyncFailed : public Trigger<> {
    public:
        explicit SyncFailed(GDOComponent* parent)
        {
            parent->subscribe_sync_failed([this](bool state) {
                if (state)
                    this->trigger();
            });
        }
    };

}
}
