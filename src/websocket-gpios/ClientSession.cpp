#include "ClientSession.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;

////////////////////////////////////////////////////////////////////////////////

ClientSession::ClientSession(net::ip::tcp::socket &&socket)
    : ws_(std::move(socket))
{
}
