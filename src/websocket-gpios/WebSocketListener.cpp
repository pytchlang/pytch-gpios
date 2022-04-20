#include <iostream>

#include "WebSocketListener.h"
#include "fail-if-error-macro.h"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>

////////////////////////////////////////////////////////////////////////////////

static void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

////////////////////////////////////////////////////////////////////////////////

WebSocketListener::WebSocketListener(
    net::io_context *ioc,
    net::ip::tcp::endpoint endpoint,
    GpioInterfaceBroker *interface_broker)
    : ioc_(*ioc), acceptor_(*ioc), interface_broker_(*interface_broker)
{
    beast::error_code ec;

    acceptor_.open(endpoint.protocol(), ec);
    FAIL_AND_RETURN_IF_EC(ec, "open");

    acceptor_.set_option(net::socket_base::reuse_address(true), ec);
    FAIL_AND_RETURN_IF_EC(ec, "set_option");

    acceptor_.bind(endpoint, ec);
    FAIL_AND_RETURN_IF_EC(ec, "bind");

    acceptor_.listen(net::socket_base::max_listen_connections, ec);
    FAIL_AND_RETURN_IF_EC(ec, "listen");
}
