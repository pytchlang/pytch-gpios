#ifndef WEBSOCKET_GPIOS_CLIENTSESSION_H_
#define WEBSOCKET_GPIOS_CLIENTSESSION_H_

#include <memory>

#include "MessageTransmitChannel.h"

////////////////////////////////////////////////////////////////////////////////

class ClientSession : public std::enable_shared_from_this<ClientSession>,
                      public IMessageTransmitChannel
{
};

#endif // WEBSOCKET_GPIOS_CLIENTSESSION_H_
