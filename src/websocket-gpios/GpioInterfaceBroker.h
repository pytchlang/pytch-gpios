#ifndef WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
#define WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H

#include <memory>

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
};

#endif // WEBSOCKET_GPIOS_GPIOINTERFACEBROKER_H
