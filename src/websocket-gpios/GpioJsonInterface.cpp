#include <functional>
#include <sstream>

#include "GpioJsonInterface.h"

using namespace std::placeholders;

////////////////////////////////////////////////////////////////////////////////

GpioJsonInterface::GpioJsonInterface(std::shared_ptr<IGpioArray> gpios)
    : gpios_(gpios)
{
}

std::string GpioJsonInterface::do_commands(const std::string &message)
{
    std::vector<nlohmann::json> outcomes;

    try
    {
        const auto jMessage = nlohmann::json::parse(message);

        std::transform(
            jMessage.begin(),
            jMessage.end(),
            std::back_inserter(outcomes),
            std::bind(&GpioJsonInterface::do_one_command_, this, _1));
    }
    catch (const nlohmann::json::parse_error &err)
    {
        std::ostringstream oss;
        oss << "could not parse message \"";
        oss << message;
        oss << "\": " << err.what();
        outcomes.push_back(json_error_(0, oss.str()));
    }

    const nlohmann::json jOutcomes(outcomes);
    return jOutcomes.dump();
}
