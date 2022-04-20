#ifndef WEBSOCKET_GPIOS_WEBSOCKETLISTENER_H_
#define WEBSOCKET_GPIOS_WEBSOCKETLISTENER_H_

#include <memory>

#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>

class GpioInterfaceBroker;

////////////////////////////////////////////////////////////////////////////////

class WebSocketListener : public std::enable_shared_from_this<WebSocketListener>
{
public:
    WebSocketListener(
        boost::asio::io_context *ioc,
        boost::asio::ip::tcp::endpoint endpoint,
        GpioInterfaceBroker *interface_broker);

    void run();

private:
    void do_accept_();
    void on_accept_(
        boost::beast::error_code ec, boost::asio::ip::tcp::socket socket);

    boost::asio::io_context &ioc_;
    boost::asio::ip::tcp::acceptor acceptor_;
    GpioInterfaceBroker &interface_broker_;
};

#endif // WEBSOCKET_GPIOS_WEBSOCKETLISTENER_H_
