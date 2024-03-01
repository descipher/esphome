#pragma once
#include <functional>
#include <utility>
#include <vector>

namespace esphome {
namespace gdo {

    template <typename T>
    class GDOListener {
    public:
        GDOListener(const T& value)
            : value_(value)
        {
        }

        template <typename U>
        GDOListener& operator=(U value)
        {
            if (value != this->value_) {
                this->value_ = value;
                this->notify();
            }
            return *this;
        }

        T const* operator&() const { return &this->value_; }
        T const& operator*() const { return this->value_; }

        template <typename GDOListeners>
        void subscribe(GDOListeners&& listener)
        {
            this->listeners_.push_back(std::forward<GDOListeners>(listener));
        }

        void notify() const
        {
            for (const auto& listener : this->listeners_) {
                listener(this->value_);
            }
        }

    private:
        T value_;
        std::vector<std::function<void(T)>> listeners_;
    };

} // namespace gdo
} // namespace esphome
