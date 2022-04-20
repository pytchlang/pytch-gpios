#include <iostream>

#include "WebSocketListener.h"
#include "fail-if-error-macro.h"

namespace beast = boost::beast; // from <boost/beast.hpp>

////////////////////////////////////////////////////////////////////////////////

static void fail(beast::error_code ec, char const *what)
{
    std::cerr << what << ": " << ec.message() << "\n";
}
