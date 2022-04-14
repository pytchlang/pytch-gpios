#ifndef WEBSOCKET_GPIOS_LOOPBACKGPIOARRAY_H_
#define WEBSOCKET_GPIOS_LOOPBACKGPIOARRAY_H_

#include "GpioArray.h"

class LoopbackGpioArray : public IGpioArray
{
public:
    LoopbackGpioArray() {}
    virtual ~LoopbackGpioArray() {}

    Outcome<void> reset() override;
    Outcome<PinLevel> set_as_input(PinId pin, PullKind pull_kind) override;
    Outcome<void> set_output(PinId pin, PinLevel level) override;
    Outcome<void> launch_input_monitor(PinLevelReportFun report_fun) override;
};

#endif // WEBSOCKET_GPIOS_LOOPBACKGPIOARRAY_H_
