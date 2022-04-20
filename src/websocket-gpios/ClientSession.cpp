#include "ClientSession.h"
#include "GpioInterfaceBroker.h"
#include "bind-macro.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

////////////////////////////////////////////////////////////////////////////////

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
