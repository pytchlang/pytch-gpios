#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>

#include "GpioArray.h"
#include "GpioInterfaceBroker.h"
#include "LoopbackGpioArray.h"
#include "WebSocketListener.h"

#if ENABLE_PIGPIO
#include "PiGpioArray.h"
#endif

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>

namespace net = boost::asio;
using tcp = net::ip::tcp;

////////////////////////////////////////////////////////////////////////////////

int usage()
{
    std::cerr << "Usage: websocket-gpio-server <port> <kind>\n\n"
              << "where 'kind' must be one of:\n"
              << "    loopback\n"
#if ENABLE_PIGPIO
              << "    pigpio\n"
#endif
              << "\nExample:\n"
              << "    websocket-gpio-server 8055 loopback\n";

    return EXIT_FAILURE;
}

std::shared_ptr<IGpioArray> make_gpios(std::string kind)
{
    if (kind == "loopback")
        return std::make_shared<LoopbackGpioArray>();

#if ENABLE_PIGPIO
    if (kind == "pigpio")
        return std::make_shared<PiGpioArray>();
#endif

    return nullptr;
}

int main(int argc, char *argv[])
{
    BOOST_LOG_TRIVIAL(info) << "starting";

    if (argc != 3)
        return usage();

    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi(argv[1]));

    std::string gpio_kind = argv[2];
    auto gpios = make_gpios(gpio_kind);
    if (gpios == nullptr)
        return usage();

    GpioInterfaceBroker broker{gpios};

    net::io_context ioc;
    const tcp::endpoint tcp_endpoint{address, port};

    std::make_shared<WebSocketListener>(&ioc, tcp_endpoint, &broker)->run();

    net::signal_set signals(ioc, SIGINT, SIGTERM);
    signals.async_wait(
        [&ioc](boost::system::error_code const &, int)
        {
            // Stop the io_context. This will cause run() to return
            // immediately, eventually destroying the io_context and
            // any remaining handlers in it.
            ioc.stop();
        });

    ioc.run();

    return EXIT_SUCCESS;
}
