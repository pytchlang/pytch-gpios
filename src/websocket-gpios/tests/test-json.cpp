#include "../GpioJsonInterface.h"
#include "../LoopbackGpioArray.h"

#include "../vendor/catch2/catch.hpp"

using json = nlohmann::json;

struct LoopbackJson
{
    LoopbackJson()
        : gpios(std::make_shared<LoopbackGpioArray>()), interface(gpios)
    {
    }

    std::shared_ptr<IGpioArray> gpios;
    GpioJsonInterface interface;
};
