#include <iostream>

#include "ClientSession.h"
#include "GpioInterfaceBroker.h"
#include "GpioJsonInterface.h"
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

void ClientSession::do_read_()
{
    ws_.async_read(buffer_, BIND_FRONT_THIS(&ClientSession::on_read_));
}

void ClientSession::on_read_(
    beast::error_code ec, std::size_t /* bytes_transferred */)
{
    // Bit rude if the client just drops the connection (eof / reset)
    // but we shouldn't treat it as an error.
    if (ec == websocket::error::closed || ec == net::error::eof
        || ec == net::error::connection_reset)
        return;

    FAIL_AND_RETURN_IF_EC(ec, "read");

    const auto message = beast::buffers_to_string(buffer_.data());
    const auto message_out = json_interface_->do_commands(message);
    send(std::make_shared<std::string>(message_out));

    buffer_.consume(buffer_.size());

    // TODO: For testing, allow a command which makes the server close
    // the connection.  If such a command received, do not call
    // do_read_() here.

    do_read_();
}

void ClientSession::send(const std::shared_ptr<const std::string> message)
{
    // This send() method can be called from any thread.  Ensure the
    // actual sending takes place on the correct strand.
    net::post(
        ws_.get_executor(),
        BIND_FRONT_THIS_1(&ClientSession::on_send_, message));
}
