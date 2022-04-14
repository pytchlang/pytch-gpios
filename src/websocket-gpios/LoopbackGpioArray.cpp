#include <stdexcept>

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
