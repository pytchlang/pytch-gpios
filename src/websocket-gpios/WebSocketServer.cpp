#include <iostream>

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
