#ifndef WEBSOCKET_GPIOS_CLIENTSESSION_H_
#define WEBSOCKET_GPIOS_CLIENTSESSION_H_

#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

#include "MessageTransmitChannel.h"

class GpioInterfaceBroker;
class GpioJsonInterface;

////////////////////////////////////////////////////////////////////////////////

class ClientSession : public std::enable_shared_from_this<ClientSession>,
                      public IMessageTransmitChannel
{
public:
    explicit ClientSession(boost::asio::ip::tcp::socket &&socket);
    void run(GpioInterfaceBroker *interface_broker);
    void send(const std::shared_ptr<const std::string> msg) override;

private:
    void on_run_();

    boost::beast::websocket::stream<boost::beast::tcp_stream> ws_;
    std::shared_ptr<GpioJsonInterface> json_interface_;
};

#endif // WEBSOCKET_GPIOS_CLIENTSESSION_H_
