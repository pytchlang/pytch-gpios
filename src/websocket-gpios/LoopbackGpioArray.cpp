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
