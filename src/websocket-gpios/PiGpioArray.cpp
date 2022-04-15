#include <pigpio.h>

#include "PiGpioArray.h"

////////////////////////////////////////////////////////////////////////////////

PiGpioArray::PiGpioArray()
{
    // TODO: Any way to report this failing?
    gpioInitialise();
}

PiGpioArray::~PiGpioArray()
{
    // This will not return until the alert-emitting thread has
    // successfully cancelled.  The next time another instance calls
    // gpioInitialise(), the alert functions will all be cleared.
    //
    // TODO: Any way to report this failing?
    gpioTerminate();
}

Outcome<void> PiGpioArray::reset()
{
    // As per
    //
    //   https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf#page=102
    //
    // omitting reserved GPIOs 0 and 1, and only going up to the
    // maximum GPIO brought out to the header (27).
    //
    static std::vector<std::pair<int, int>> power_up_states
        = {{2, PI_PUD_UP},    {3, PI_PUD_UP},    {4, PI_PUD_UP},
           {5, PI_PUD_UP},    {6, PI_PUD_UP},    {7, PI_PUD_UP},
           {8, PI_PUD_UP},    {9, PI_PUD_DOWN},  {10, PI_PUD_DOWN},
           {11, PI_PUD_DOWN}, {12, PI_PUD_DOWN}, {13, PI_PUD_DOWN},
           {14, PI_PUD_DOWN}, {15, PI_PUD_DOWN}, {16, PI_PUD_DOWN},
           {17, PI_PUD_DOWN}, {18, PI_PUD_DOWN}, {19, PI_PUD_DOWN},
           {20, PI_PUD_DOWN}, {21, PI_PUD_DOWN}, {22, PI_PUD_DOWN},
           {23, PI_PUD_DOWN}, {24, PI_PUD_DOWN}, {25, PI_PUD_DOWN},
           {26, PI_PUD_DOWN}, {27, PI_PUD_DOWN}};

    for (const auto &pin_state : power_up_states)
    {
        auto outcome = gpioSetMode(pin_state.first, PI_INPUT);
        if (outcome != 0)
            return Failure{"failed to set pin to INPUT"};
        outcome = gpioSetPullUpDown(pin_state.first, pin_state.second);
        if (outcome != 0)
            return Failure{"failed to set pull-up/-down of pin"};
    }

    return Success<void>{};
}

Outcome<PinLevel> PiGpioArray::set_as_input(PinId pin, PullKind pull_kind)
{
    // TODO: Reject reserved pins 0, 1, >27.

    gpioSetMode(pin, PI_INPUT);

    unsigned pull_up_down;
    switch (pull_kind)
    {
    case PullKind::PULL_DOWN:
        pull_up_down = PI_PUD_DOWN;
        break;
    case PullKind::PULL_UP:
        pull_up_down = PI_PUD_UP;
        break;
    case PullKind::NO_PULL:
        pull_up_down = PI_PUD_OFF;
        break;
    default:
        return Failure{"unknown pull-kind"};
    }

    gpioSetPullUpDown(pin, pull_up_down);
    int level = gpioRead(pin);

    // TODO(ben): Set "glitch function" to give us debouncing.

    gpioSetAlertFuncEx(pin, dispatch_gpio_alert_, this);

    return Success<PinLevel>{level};
}
