#ifndef WEBSOCKET_GPIOS_MESSAGETRANSMITCHANNEL_H
#define WEBSOCKET_GPIOS_MESSAGETRANSMITCHANNEL_H

#include <memory>
#include <string>

////////////////////////////////////////////////////////////////////////////////

struct IMessageTransmitChannel
{
    virtual void send(const std::shared_ptr<const std::string> message) = 0;
};

#endif // WEBSOCKET_GPIOS_MESSAGETRANSMITCHANNEL_H
