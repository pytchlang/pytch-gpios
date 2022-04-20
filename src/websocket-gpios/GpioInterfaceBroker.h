#ifndef WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
#define WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H

#include <memory>
#include <mutex>

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

    std::shared_ptr<IGpioArray> gpios_;
    std::mutex mutex_;
};

#endif // WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
