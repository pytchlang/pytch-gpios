#ifndef WEBSOCKET_GPIOS_STUBGPIOARRAY_H_
#define WEBSOCKET_GPIOS_STUBGPIOARRAY_H_

#include "GpioArray.h"

class StubGpioArray : public IGpioArray
{
public:
    StubGpioArray() {}
    virtual ~StubGpioArray() {}

    Outcome<void> reset() override;
    Outcome<PinLevel> set_as_input(PinId pin, PullKind pull_kind) override;
    Outcome<void> set_output(PinId pin, PinLevel level) override;
    Outcome<void> launch_input_monitor(PinLevelReportFun report_fun) override;
};

#endif // WEBSOCKET_GPIOS_STUBGPIOARRAY_H_
