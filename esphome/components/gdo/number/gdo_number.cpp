#include "gdo_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gdo {

    using protocol::SetClientID;
    using protocol::SetRollingCodeCounter;

    float normalize_client_id(float client_id)
    {
        uint32_t int_value = static_cast<uint32_t>(client_id);
        if ((int_value & 0xFFF) != 0x539) {
            client_id = ceil((client_id - 0x539) / 0x1000) * 0x1000 + 0x539;
        }
        return client_id;
    }

    static const char* const TAG = "gdo.number";

    void GDONumber::dump_config()
    {
        LOG_NUMBER("", "GDO Number", this);
        if (this->number_type_ == GDO_CLIENT_ID) {
            ESP_LOGCONFIG(TAG, " Type: Client ID");
        } else if (this->number_type_ == GDO_ROLLING_CODE_COUNTER) {
            ESP_LOGCONFIG(TAG, "  Type: Rolling Code Counter");
        } else if (this->number_type_ == GDO_OPENING_DURATION) {
            ESP_LOGCONFIG(TAG, "  Type: Opening Duration");
        } else if (this->number_type_ == GDO_CLOSING_DURATION) {
            ESP_LOGCONFIG(TAG, "  Type: Closing Duration");
        }
    }

    void GDONumber::setup()
    {
        float value;
        this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
        if (!this->pref_.load(&value)) {
            if (this->number_type_ == GDO_CLIENT_ID) {
                value = ((random_uint32() + 1) % 0x7FF) << 12 | 0x539; // max size limited to be precisely convertible to float
            } else {
                value = 0;
            }
        } else {
            if (this->number_type_ == GDO_CLIENT_ID) {
                uint32_t int_value = static_cast<uint32_t>(value);
                if ((int_value & 0xFFF) != 0x539) {
                    value = ((random_uint32() + 1) % 0x7FF) << 12 | 0x539; // max size limited to be precisely convertible to float
                    this->pref_.save(&value);
                }
            }
        }
        this->control(value);

        if (this->number_type_ == GDO_ROLLING_CODE_COUNTER) {
            this->parent_->subscribe_rolling_code_counter([=](uint32_t value) {
                this->update_state(value);
            });
        } else if (this->number_type_ == GDO_OPENING_DURATION) {
            this->parent_->subscribe_opening_duration([=](float value) {
                this->update_state(value);
            });
        } else if (this->number_type_ == GDO_CLOSING_DURATION) {
            this->parent_->subscribe_closing_duration([=](float value) {
                this->update_state(value);
            });
        }
    }

    void GDONumber::set_number_type(NumberType number_type_)
    {
        this->number_type_ = number_type_;
        if (this->number_type_ == GDO_OPENING_DURATION || this->number_type_ == GDO_CLOSING_DURATION) {
            this->traits.set_step(0.1);
            this->traits.set_min_value(0.0);
            this->traits.set_max_value(180.0);
        } else if (this->number_type_ == GDO_ROLLING_CODE_COUNTER) {
            this->traits.set_max_value(0xfffffff);
        } else if (this->number_type_ == GDO_CLIENT_ID) {
            this->traits.set_step(0x1000);
            this->traits.set_min_value(0x539);
            this->traits.set_max_value(0x7ff539);
        }
    }

    void GDONumber::update_state(float value)
    {
        if (value == this->state) {
            return;
        }
        this->pref_.save(&value);
        this->publish_state(value);
    }

    void GDONumber::control(float value)
    {
        if (this->number_type_ == GDO_ROLLING_CODE_COUNTER) {
            this->parent_->call_protocol(SetRollingCodeCounter { static_cast<uint32_t>(value) });
        } else if (this->number_type_ == GDO_OPENING_DURATION) {
            this->parent_->set_opening_duration(value);
        } else if (this->number_type_ == GDO_CLOSING_DURATION) {
            this->parent_->set_closing_duration(value);
        } else if (this->number_type_ == GDO_CLIENT_ID) {
            value = normalize_client_id(value);
            this->parent_->call_protocol(SetClientID { static_cast<uint32_t>(value) });
        }
        this->update_state(value);
    }

} // namespace gdo
} // namespace esphome
