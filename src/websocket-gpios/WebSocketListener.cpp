#include <iostream>

#include "ClientSession.h"
#include "WebSocketListener.h"
#include "bind-macro.h"
#include "fail-if-error-macro.h"

#include <boost/log/trivial.hpp>

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace net = boost::asio;    // from <boost/asio.hpp>

////////////////////////////////////////////////////////////////////////////////

static void fail(beast::error_code ec, char const *what)
{
    BOOST_LOG_TRIVIAL(error) << what << ": " << ec.message();
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

    // If we get to this point then all the above succeeded.
    setup_succeeded_ = true;
}

void WebSocketListener::run()
{
    do_accept_();
}

void WebSocketListener::do_accept_()
{
    // The new connection gets its own strand
    acceptor_.async_accept(
        net::make_strand(ioc_),
        BIND_FRONT_THIS(&WebSocketListener::on_accept_));
}

void WebSocketListener::on_accept_(
    beast::error_code ec, boost::asio::ip::tcp::socket socket)
{
    if (ec)
        fail(ec, "accept");
    else
    {
        auto session = std::make_shared<ClientSession>(std::move(socket));
        session->run(&interface_broker_);
    }

    do_accept_();
}
