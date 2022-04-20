#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>

#include "GpioArray.h"
#include "LoopbackGpioArray.h"
#include "GpioInterfaceBroker.h"

#if ENABLE_PIGPIO
#include "PiGpioArray.h"
#endif

#include <boost/asio.hpp>

namespace net = boost::asio;

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
    if (argc != 3)
        return usage();

    auto const address = net::ip::make_address("0.0.0.0");
    auto const port = static_cast<unsigned short>(std::atoi(argv[1]));

    std::string gpio_kind = argv[2];
    auto gpios = make_gpios(gpio_kind);
    if (gpios == nullptr)
        return usage();

    GpioInterfaceBroker broker{gpios};

    return EXIT_SUCCESS;
}
