#include <functional>
#include <utility>
#include <memory>

#include "GpioInterfaceBroker.h"
#include "GpioArray.h"

using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////

GpioInterfaceBroker::GpioInterfaceBroker(std::shared_ptr<IGpioArray> gpios)
    : gpios_(std::move(gpios))
{
    gpios_->launch_input_monitor(
        std::bind(&GpioInterfaceBroker::maybe_send_pin_level_, this, _1, _2));
}
