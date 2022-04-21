#include <functional>
#include <memory>
#include <utility>

#include "GpioArray.h"
#include "GpioInterfaceBroker.h"
#include "GpioJsonInterface.h"
#include "MessageTransmitChannel.h"
#include "StubGpioArray.h"
#include "Types.h"

using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////

GpioInterfaceBroker::GpioInterfaceBroker(std::shared_ptr<IGpioArray> gpios)
    : gpios_(std::move(gpios))
{
    gpios_->launch_input_monitor(
        std::bind(&GpioInterfaceBroker::maybe_send_pin_level_, this, _1, _2));
}

std::shared_ptr<GpioJsonInterface> GpioInterfaceBroker::issue_json_interface(
    std::shared_ptr<IMessageTransmitChannel> message_transmit_channel)
{
    mutex_lock_t lock{mutex_};

    bool real_array_available = (active_transmit_channel_.lock() == nullptr);

    std::shared_ptr<IGpioArray> gpios;
    if (real_array_available)
    {
        active_transmit_channel_ = message_transmit_channel;
        gpios = gpios_;
    }
    else
        gpios = std::make_shared<StubGpioArray>();

    return std::make_shared<GpioJsonInterface>(gpios);
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
