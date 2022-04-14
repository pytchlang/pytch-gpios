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

private:
    enum class PinState_
    {
        INPUT_PU,
        INPUT_PD,
        INPUT_NP,
        DRIVEN_LOW,
        DRIVEN_HIGH
    };

    PinState_ PinState_from_PullKind_(PullKind pull_kind);

    PinState_ pin4_state_ = PinState_::INPUT_NP;
    PinState_ pin5_state_ = PinState_::INPUT_NP;
};

#endif // WEBSOCKET_GPIOS_LOOPBACKGPIOARRAY_H_
