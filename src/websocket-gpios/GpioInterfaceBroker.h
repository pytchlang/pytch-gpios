#ifndef WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
#define WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H

#include <memory>
#include <mutex>

#include "Types.h"

////////////////////////////////////////////////////////////////////////////////

class IGpioArray;
class GpioJsonInterface;
class IMessageTransmitChannel;

class GpioInterfaceBroker
{
public:
    explicit GpioInterfaceBroker(std::shared_ptr<IGpioArray> gpios);

    std::shared_ptr<GpioJsonInterface> issue_json_interface(
        std::shared_ptr<IMessageTransmitChannel> message_transmit_channel);

private:
    using mutex_lock_t = std::lock_guard<std::mutex>;

    // If there is an active message-sink, send a message to that sink
    // describing the pin level.  If no active message-sink, do nothing.
    void maybe_send_pin_level_(PinId pin, PinLevel level);

    std::shared_ptr<IGpioArray> gpios_;
    std::mutex mutex_;
    std::weak_ptr<IMessageTransmitChannel> active_transmit_channel_;
};

#endif // WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
