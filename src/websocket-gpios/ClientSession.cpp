#include <iostream>

#include "ClientSession.h"
#include "GpioInterfaceBroker.h"
#include "bind-macro.h"
#include "fail-if-error-macro.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

////////////////////////////////////////////////////////////////////////////////

static void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}

ClientSession::ClientSession(net::ip::tcp::socket &&socket)
    : ws_(std::move(socket))
{
}

void ClientSession::run(GpioInterfaceBroker *interface_broker)
{
    json_interface_
        = interface_broker->issue_json_interface(shared_from_this());

    // We need to be executing within a strand to perform async
    // operations on the websocket I/O objects in this session.
    net::dispatch(ws_.get_executor(), BIND_FRONT_THIS(&ClientSession::on_run_));
}

void ClientSession::on_run_()
{
    ws_.set_option(
        websocket::stream_base::timeout::suggested(beast::role_type::server));

    ws_.async_accept(BIND_FRONT_THIS(&ClientSession::on_accept_));
}

void ClientSession::on_accept_(beast::error_code ec)
{
    FAIL_AND_RETURN_IF_EC(ec, "accept");
    do_read_();
}
