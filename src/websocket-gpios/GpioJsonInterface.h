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
    json do_one_command_(const json &command);

    json do_reset_(SeqNum seqnum);

    static json json_ok_(SeqNum seqnum);
    static json json_report_input_(SeqNum seqnum, PinId pin, PinLevel level);
    static json json_error_(SeqNum seqnum, const std::string &message);

    std::shared_ptr<IGpioArray> gpios_;
};

#endif // WEBSOCKET_GPIOS_GPIOJSONINTERFACE_H_
