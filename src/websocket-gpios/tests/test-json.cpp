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

    json do_commands(const std::string &message)
    {
        const auto reply = interface.do_commands(message);
        const auto jReply = json::parse(reply);
        REQUIRE(jReply.is_array());
        return jReply;
    }

    static void require_empty_array(const json &jReply)
    {
        REQUIRE(jReply.size() == 0);
    }

    std::shared_ptr<IGpioArray> gpios;
    GpioJsonInterface interface;
};
