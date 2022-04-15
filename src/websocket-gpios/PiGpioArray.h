#ifndef WEBSOCKET_GPIOS_PIGPIOARRAY_H_
#define WEBSOCKET_GPIOS_PIGPIOARRAY_H_

#include "GpioArray.h"

class PiGpioArray : public IGpioArray
{
public:
    PiGpioArray();
    virtual ~PiGpioArray();

    Outcome<void> reset() override;
    Outcome<PinLevel> set_as_input(PinId pin, PullKind pull_kind) override;
    Outcome<void> set_output(PinId pin, PinLevel level) override;
    Outcome<void> launch_input_monitor(PinLevelReportFun report_fun) override;

private:
    static void
    dispatch_gpio_alert_(int gpio, int level, uint32_t tick, void *closure);
    void handle_gpio_alert_(int gpio, int level, uint32_t tick);

    PinLevelReportFun report_fun_;
};

#endif // WEBSOCKET_GPIOS_PIGPIOARRAY_H_
