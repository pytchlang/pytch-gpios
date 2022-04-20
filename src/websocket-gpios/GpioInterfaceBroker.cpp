#include <functional>
#include <utility>
#include <memory>

#include "GpioInterfaceBroker.h"
#include "GpioJsonInterface.h"
#include "MessageTransmitChannel.h"
#include "GpioArray.h"
#include "Types.h"

using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////

GpioInterfaceBroker::GpioInterfaceBroker(std::shared_ptr<IGpioArray> gpios)
    : gpios_(std::move(gpios))
{
    gpios_->launch_input_monitor(
        std::bind(&GpioInterfaceBroker::maybe_send_pin_level_, this, _1, _2));
}

void GpioInterfaceBroker::maybe_send_pin_level_(PinId pin, PinLevel level)
{
    // TODO: We're only reading active_transmit_channel_, so
    // perhaps we don't need to lock?
    mutex_lock_t lock{mutex_};

    auto transmit_channel{active_transmit_channel_.lock()};
    if (transmit_channel)
    {
        const auto message
            = GpioJsonInterface::report_input_message(pin, level);

        transmit_channel->send(std::make_shared<std::string>(message));
    }
}
