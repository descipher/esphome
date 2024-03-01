#include "gdo_lock.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gdo {

    static const char* const TAG = "gdo.lock";

    void GDOLock::dump_config()
    {
        LOG_LOCK("", "GDO Lock", this);
        ESP_LOGCONFIG(TAG, "  Type: Lock");
    }

    void GDOLock::setup()
    {
        this->parent_->subscribe_lock_state([=](LockState state) {
            this->on_lock_state(state);
        });
    }

    void GDOLock::on_lock_state(LockState state)
    {
        if (state == LockState::LOCKED && this->state == lock::LockState::LOCK_STATE_LOCKED) {
            return;
        }
        if (state == LockState::UNLOCKED && this->state == lock::LockState::LOCK_STATE_UNLOCKED) {
            return;
        }

        auto call = this->make_call();
        if (state == LockState::LOCKED) {
            call.set_state(lock::LockState::LOCK_STATE_LOCKED);
        } else if (state == LockState::UNLOCKED) {
            call.set_state(lock::LockState::LOCK_STATE_UNLOCKED);
        }
        this->control(call);
    }

    void GDOLock::control(const lock::LockCall& call)
    {
        auto state = *call.get_state();

        if (state == lock::LockState::LOCK_STATE_LOCKED) {
            this->parent_->lock();
        } else if (state == lock::LockState::LOCK_STATE_UNLOCKED) {
            this->parent_->unlock();
        }

        this->publish_state(state);
    }

} // namespace gdo
} // namespace esphome
