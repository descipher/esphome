/************************************
 * Rage
 * Against
 * The
 * Garage
 * Door
 * Opener
 *
 * Copyright (C) 2022  Paul Wieland
 * Copyright (C) 2024  Mike La Spina
 *
 * GNU GENERAL PUBLIC LICENSE
 ************************************/

#pragma once
#include <cstdint>

namespace esphome
{
    namespace gdo
    {
        enum class DoorState : uint8_t
        {
            UNKNOWN = 0,
            OPEN = 1,
            CLOSED = 2,
            STOPPED = 3,
            OPENING = 4,
            CLOSING = 5
        };
        inline const char *DoorState_to_string(DoorState _e)
        {
            switch (_e)
            {
            case DoorState::UNKNOWN:
                return "UNKNOWN";
            case DoorState::OPEN:
                return "OPEN";
            case DoorState::CLOSED:
                return "CLOSED";
            case DoorState::STOPPED:
                return "STOPPED";
            case DoorState::OPENING:
                return "OPENING";
            case DoorState::CLOSING:
                return "CLOSING";
            default:
                return "UNKNOWN";
            }
        }
        inline DoorState to_DoorState(uint8_t _t, DoorState _unknown)
        {
            switch (_t)
            {
            case 0:
                return DoorState::UNKNOWN;
            case 1:
                return DoorState::OPEN;
            case 2:
                return DoorState::CLOSED;
            case 3:
                return DoorState::STOPPED;
            case 4:
                return DoorState::OPENING;
            case 5:
                return DoorState::CLOSING;
            default:
                return _unknown;
            }
        }
        enum class LightState : uint8_t
        {
            OFF = 0,
            ON = 1,
            UNKNOWN = 2
        };
        inline const char *LightState_to_string(LightState _e)
        {
            switch (_e)
            {
            case LightState::OFF:
                return "OFF";
            case LightState::ON:
                return "ON";
            case LightState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline LightState to_LightState(uint8_t _t, LightState _unknown)
        {
            switch (_t)
            {
            case 0:
                return LightState::OFF;
            case 1:
                return LightState::ON;
            case 2:
                return LightState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        LightState light_state_toggle(LightState state);

        enum class LockState : uint8_t
        {
            UNLOCKED = 0,
            LOCKED = 1,
            UNKNOWN = 2
        };
        inline const char *LockState_to_string(LockState _e)
        {
            switch (_e)
            {
            case LockState::UNLOCKED:
                return "UNLOCKED";
            case LockState::LOCKED:
                return "LOCKED";
            case LockState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline LockState to_LockState(uint8_t _t, LockState _unknown)
        {
            switch (_t)
            {
            case 0:
                return LockState::UNLOCKED;
            case 1:
                return LockState::LOCKED;
            case 2:
                return LockState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        LockState lock_state_toggle(LockState state);

        enum class MotionState : uint8_t
        {
            CLEAR = 0,
            DETECTED = 1,
            UNKNOWN = 2
        };
        inline const char *MotionState_to_string(MotionState _e)
        {
            switch (_e)
            {
            case MotionState::CLEAR:
                return "CLEAR";
            case MotionState::DETECTED:
                return "DETECTED";
            case MotionState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline MotionState to_MotionState(uint8_t _t, MotionState _unknown)
        {
            switch (_t)
            {
            case 0:
                return MotionState::CLEAR;
            case 1:
                return MotionState::DETECTED;
            case 2:
                return MotionState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class ObstructionState : uint8_t
        {
            OBSTRUCTED = 0,
            CLEAR = 1,
            UNKNOWN = 2
        };
        inline const char *ObstructionState_to_string(ObstructionState _e)
        {
            switch (_e)
            {
            case ObstructionState::OBSTRUCTED:
                return "OBSTRUCTED";
            case ObstructionState::CLEAR:
                return "CLEAR";
            case ObstructionState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline ObstructionState to_ObstructionState(uint8_t _t, ObstructionState _unknown)
        {
            switch (_t)
            {
            case 0:
                return ObstructionState::OBSTRUCTED;
            case 1:
                return ObstructionState::CLEAR;
            case 2:
                return ObstructionState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class MotorState : uint8_t
        {
            OFF = 0,
            ON = 1,
            UNKNOWN = 2
        };
        inline const char *MotorState_to_string(MotorState _e)
        {
            switch (_e)
            {
            case MotorState::OFF:
                return "OFF";
            case MotorState::ON:
                return "ON";
            case MotorState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline MotorState to_MotorState(uint8_t _t, MotorState _unknown)
        {
            switch (_t)
            {
            case 0:
                return MotorState::OFF;
            case 1:
                return MotorState::ON;
            case 2:
                return MotorState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class ButtonState : uint8_t
        {
            PRESSED = 0,
            RELEASED = 1,
            UNKNOWN = 2
        };
        inline const char *ButtonState_to_string(ButtonState _e)
        {
            switch (_e)
            {
            case ButtonState::PRESSED:
                return "PRESSED";
            case ButtonState::RELEASED:
                return "RELEASED";
            case ButtonState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline ButtonState to_ButtonState(uint8_t _t, ButtonState _unknown)
        {
            switch (_t)
            {
            case 0:
                return ButtonState::PRESSED;
            case 1:
                return ButtonState::RELEASED;
            case 2:
                return ButtonState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class BatteryState : uint8_t
        {
            UNKNOWN = 0,
            CHARGING = 6,
            FULL = 8
        };
        inline const char *BatteryState_to_string(BatteryState _e)
        {
            switch (_e)
            {
            case BatteryState::UNKNOWN:
                return "UNKNOWN";
            case BatteryState::CHARGING:
                return "CHARGING";
            case BatteryState::FULL:
                return "FULL";
            default:
                return "UNKNOWN";
            }
        }
        inline BatteryState to_BatteryState(uint8_t _t, BatteryState _unknown)
        {
            switch (_t)
            {
            case 0:
                return BatteryState::UNKNOWN;
            case 6:
                return BatteryState::CHARGING;
            case 8:
                return BatteryState::FULL;
            default:
                return _unknown;
            }
        }

        enum class LearnState : uint8_t
        {
            INACTIVE = 0,
            ACTIVE = 1,
            UNKNOWN = 2
        };
        inline const char *LearnState_to_string(LearnState _e)
        {
            switch (_e)
            {
            case LearnState::INACTIVE:
                return "INACTIVE";
            case LearnState::ACTIVE:
                return "ACTIVE";
            case LearnState::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline LearnState to_LearnState(uint8_t _t, LearnState _unknown)
        {
            switch (_t)
            {
            case 0:
                return LearnState::INACTIVE;
            case 1:
                return LearnState::ACTIVE;
            case 2:
                return LearnState::UNKNOWN;
            default:
                return _unknown;
            }
        }

        LearnState learn_state_toggle(LearnState state);

        enum class PairedDevice : uint8_t
        {
            ALL = 0,
            REMOTE = 1,
            KEYPAD = 2,
            WALL_CONTROL = 3,
            ACCESSORY = 4,
            UNKNOWN = 0xff
        };
        inline const char *PairedDevice_to_string(PairedDevice _e)
        {
            switch (_e)
            {
            case PairedDevice::ALL:
                return "ALL";
            case PairedDevice::REMOTE:
                return "REMOTE";
            case PairedDevice::KEYPAD:
                return "KEYPAD";
            case PairedDevice::WALL_CONTROL:
                return "WALL_CONTROL";
            case PairedDevice::ACCESSORY:
                return "ACCESSORY";
            case PairedDevice::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline PairedDevice to_PairedDevice(uint8_t _t, PairedDevice _unknown)
        {
            switch (_t)
            {
            case 0:
                return PairedDevice::ALL;
            case 1:
                return PairedDevice::REMOTE;
            case 2:
                return PairedDevice::KEYPAD;
            case 3:
                return PairedDevice::WALL_CONTROL;
            case 4:
                return PairedDevice::ACCESSORY;
            case 0xff:
                return PairedDevice::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class LightAction : uint8_t
        {
            OFF = 0,
            ON = 1,
            TOGGLE = 2,
            UNKNOWN = 3
        };
        inline const char *LightAction_to_string(LightAction _e)
        {
            switch (_e)
            {
            case LightAction::OFF:
                return "OFF";
            case LightAction::ON:
                return "ON";
            case LightAction::TOGGLE:
                return "TOGGLE";
            case LightAction::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline LightAction to_LightAction(uint8_t _t, LightAction _unknown)
        {
            switch (_t)
            {
            case 0:
                return LightAction::OFF;
            case 1:
                return LightAction::ON;
            case 2:
                return LightAction::TOGGLE;
            case 3:
                return LightAction::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class LockAction : uint8_t
        {
            UNLOCK = 0,
            LOCK = 1,
            TOGGLE = 2,
            UNKNOWN = 3
        };
        inline const char *LockAction_to_string(LockAction _e)
        {
            switch (_e)
            {
            case LockAction::UNLOCK:
                return "UNLOCK";
            case LockAction::LOCK:
                return "LOCK";
            case LockAction::TOGGLE:
                return "TOGGLE";
            case LockAction::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline LockAction to_LockAction(uint8_t _t, LockAction _unknown)
        {
            switch (_t)
            {
            case 0:
                return LockAction::UNLOCK;
            case 1:
                return LockAction::LOCK;
            case 2:
                return LockAction::TOGGLE;
            case 3:
                return LockAction::UNKNOWN;
            default:
                return _unknown;
            }
        }

        enum class DoorAction : uint8_t
        {
            CLOSE = 0,
            OPEN = 1,
            TOGGLE = 2,
            STOP = 3,
            UNKNOWN = 4
        };
        inline const char *DoorAction_to_string(DoorAction _e)
        {
            switch (_e)
            {
            case DoorAction::CLOSE:
                return "CLOSE";
            case DoorAction::OPEN:
                return "OPEN";
            case DoorAction::TOGGLE:
                return "TOGGLE";
            case DoorAction::STOP:
                return "STOP";
            case DoorAction::UNKNOWN:
                return "UNKNOWN";
            default:
                return "UNKNOWN";
            }
        }
        inline DoorAction to_DoorAction(uint8_t _t, DoorAction _unknown)
        {
            switch (_t)
            {
            case 0:
                return DoorAction::CLOSE;
            case 1:
                return DoorAction::OPEN;
            case 2:
                return DoorAction::TOGGLE;
            case 3:
                return DoorAction::STOP;
            case 4:
                return DoorAction::UNKNOWN;
            default:
                return _unknown;
            }
        }

    } // namespace gdo
} // namespace esphome
