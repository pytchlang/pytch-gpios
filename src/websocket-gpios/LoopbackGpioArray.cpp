#include <stdexcept>
#include <sstream>
#include <cassert>

#include "LoopbackGpioArray.h"

////////////////////////////////////////////////////////////////////////////////

LoopbackGpioArray::PinState_
LoopbackGpioArray::PinState_from_PullKind_(PullKind pull_kind)
{
    switch (pull_kind)
    {
    case PullKind::PULL_UP:
        return PinState_::INPUT_PU;
    case PullKind::PULL_DOWN:
        return PinState_::INPUT_PD;
    case PullKind::NO_PULL:
        return PinState_::INPUT_NP;
    default:
        throw new std::invalid_argument("bad pull-kind");
    }
}

Outcome<void> LoopbackGpioArray::reset()
{
    mutex_lock_t lock{mutex_};
    ++seqnum_;

    pin4_state_ = PinState_::INPUT_NP;
    pin5_state_ = PinState_::INPUT_NP;

    return Success<void>{};
}

Outcome<PinLevel> LoopbackGpioArray::set_as_input(PinId pin, PullKind pull_kind)
{
    mutex_lock_t lock{mutex_};
    ++seqnum_;

    switch (pin)
    {
    case 4:
        pin4_state_ = PinState_from_PullKind_(pull_kind);
        return Success{pin4_level_()};
    case 5:
        pin5_state_ = PinState_from_PullKind_(pull_kind);
        return Success{pin5_level_()};
    default:
        std::ostringstream oss;
        oss << "cannot use pin " << static_cast<int>(pin) << " as an input";
        return Failure{oss.str()};
    }
}

Outcome<void> LoopbackGpioArray::set_output(PinId pin, PinLevel level)
{
    mutex_lock_t lock{mutex_};
    ++seqnum_;

    PinState_ *state{nullptr};

    switch (pin)
    {
    case 4:
        state = &pin4_state_;
        break;
    case 5:
        state = &pin5_state_;
        break;
    default:
        std::ostringstream oss;
        oss << "cannot use pin " << static_cast<int>(pin) << " as an output";
        return Failure{oss.str()};
    }

    assert(state != nullptr);

    switch (level)
    {
    case 0:
        *state = PinState_::DRIVEN_LOW;
        return Success<void>{};
    case 1:
        *state = PinState_::DRIVEN_HIGH;
        return Success<void>{};
    default:
        std::ostringstream oss;
        oss << "cannot set pin " << pin << " to bad level " << level;
        return Failure{oss.str()};
    }
}

PinLevel LoopbackGpioArray::pin_level_(
    PinState_ read_pin_state, PinState_ drive_pin_state)
{
    // Is read-pin an output?  Report output level if so.
    //
    if (read_pin_state == PinState_::DRIVEN_LOW)
        return 0;
    if (read_pin_state == PinState_::DRIVEN_HIGH)
        return 1;

    // Is drive-pin being driven by the other end of the connection?
    // Report its drive level if so.
    //
    if (drive_pin_state == PinState_::DRIVEN_LOW)
        return 0;
    if (drive_pin_state == PinState_::DRIVEN_HIGH)
        return 1;

    // Is read-pin pulled?  Report pull level if so.
    //
    if (read_pin_state == PinState_::INPUT_PU)
        return 1;
    if (read_pin_state == PinState_::INPUT_PD)
        return 0;

    // Indeterminate; pick something.
    //
    return 0;
}

PinLevel LoopbackGpioArray::pin4_level_()
{
    return pin_level_(pin4_state_, pin5_state_);
}

PinLevel LoopbackGpioArray::pin5_level_()
{
    return pin_level_(pin5_state_, pin4_state_);
}

bool LoopbackGpioArray::state_is_input_(PinState_ pin_state)
{
    switch (pin_state)
    {
    case PinState_::INPUT_PU:
    case PinState_::INPUT_PD:
    case PinState_::INPUT_NP:
        return true;
    default:
        return false;
    }
}

bool LoopbackGpioArray::pin4_is_input_()
{
    return state_is_input_(pin4_state_);
}

bool LoopbackGpioArray::pin5_is_input_()
{
    return state_is_input_(pin5_state_);
}

const std::chrono::milliseconds LoopbackGpioArray::Poll_Interval_{20};
