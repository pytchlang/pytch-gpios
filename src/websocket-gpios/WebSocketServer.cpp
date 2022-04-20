#include <iostream>
#include <memory>
#include <string>

#include "GpioArray.h"
#include "LoopbackGpioArray.h"

#if ENABLE_PIGPIO
#include "PiGpioArray.h"
#endif

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
