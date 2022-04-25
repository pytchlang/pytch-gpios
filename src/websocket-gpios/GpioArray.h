#ifndef WEBSOCKET_GPIOS_GPIOARRAY_H_
#define WEBSOCKET_GPIOS_GPIOARRAY_H_

#include <cstdint>
#include <string>

#include "Types.h"

struct IGpioArray
{
    virtual ~IGpioArray() {}

    virtual Outcome<void> reset() = 0;

    virtual Outcome<PinLevel> set_as_input(PinId pin, PullKind pull_kind) = 0;

    virtual Outcome<void> set_output(PinId pin, PinLevel level) = 0;

    virtual Outcome<void> launch_input_monitor(PinLevelReportFun report) = 0;

    virtual const std::string &kind() const = 0;
};

#endif // WEBSOCKET_GPIOS_GPIOARRAY_H_
