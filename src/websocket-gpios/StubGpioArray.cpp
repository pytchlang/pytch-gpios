#include "StubGpioArray.h"

static const Failure FailureOutcome_{"GPIOs are already in use"};

Outcome<void> StubGpioArray::reset()
{
    return FailureOutcome_;
}

Outcome<PinLevel>
StubGpioArray::set_as_input(PinId /* pin */, PullKind /* pull_kind */)
{
    return FailureOutcome_;
}

Outcome<void> StubGpioArray::set_output(PinId /* pin */, PinLevel /* level */)
{
    return FailureOutcome_;
}

Outcome<void>
StubGpioArray::launch_input_monitor(PinLevelReportFun /* report_fun */)
{
    return FailureOutcome_;
}
