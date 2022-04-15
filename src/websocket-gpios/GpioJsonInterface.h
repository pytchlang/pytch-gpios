#ifndef WEBSOCKET_GPIOS_GPIOJSONINTERFACE_H_
#define WEBSOCKET_GPIOS_GPIOJSONINTERFACE_H_

#include <memory>
#include <string>
#include "GpioArray.h"
#include "vendor/nlohmann/json.hpp"

class GpioJsonInterface
{
public:
    GpioJsonInterface(std::shared_ptr<IGpioArray> gpios);
    std::string do_commands(const std::string &message);
    static std::string report_input_message(PinId pin, PinLevel level);

private:
    using json = nlohmann::json;

    std::shared_ptr<IGpioArray> gpios_;
};

#endif // WEBSOCKET_GPIOS_GPIOJSONINTERFACE_H_
