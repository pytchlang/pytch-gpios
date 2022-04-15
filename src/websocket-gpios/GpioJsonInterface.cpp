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

nlohmann::json
GpioJsonInterface::do_one_command_(const nlohmann::json &jCommand)
{
    // If we fail to extract seqnum from command, zero is a reasonable
    // value to include in the error report.
    SeqNum seqnum = 0;
    try
    {
        seqnum = jCommand.at("seqnum").get<SeqNum>();
        const auto kind = jCommand.at("kind").get<std::string>();

        if (kind == "reset")
            return do_reset_(seqnum);
        else if (kind == "set-input")
            return do_set_input_(seqnum, jCommand);
        else if (kind == "set-output")
            return do_set_output_(seqnum, jCommand);
        else
        {
            std::ostringstream oss;
            oss << "unknown command \"" << kind << "\"";
            return json_error_(seqnum, oss.str());
        }
    }
    catch (const std::exception &err)
    {
        return json_error_(seqnum, err.what());
    }
}

nlohmann::json GpioJsonInterface::do_reset_(SeqNum seqnum)
{
    const auto outcome = gpios_->reset();
    if (std::holds_alternative<Success<void>>(outcome))
        return json_ok_(seqnum);
    else
        return json_error_(seqnum, std::get<Failure>(outcome).message);
}

nlohmann::json
GpioJsonInterface::do_set_input_(SeqNum seqnum, const nlohmann::json &jCommand)
{
    const auto pin = jCommand.at("pin").get<PinId>();
    const auto pull_kind_str = jCommand.at("pullKind").get<std::string>();
    const auto pull_kind = PullKind_from_string(pull_kind_str);

    const auto outcome = gpios_->set_as_input(pin, pull_kind);

    if (const auto success = std::get_if<Success<PinLevel>>(&outcome))
        return json_report_input_(seqnum, pin, success->value);
    else
        return json_error_(seqnum, std::get<Failure>(outcome).message);
}

nlohmann::json
GpioJsonInterface::do_set_output_(SeqNum seqnum, const nlohmann::json &jCommand)
{
    const auto pin = jCommand.at("pin").get<PinId>();
    const auto level = jCommand.at("level").get<PinLevel>();
    const auto outcome = gpios_->set_output(pin, level);

    if (std::holds_alternative<Success<void>>(outcome))
        return json_ok_(seqnum);
    else
        return json_error_(seqnum, std::get<Failure>(outcome).message);
}

nlohmann::json GpioJsonInterface::json_ok_(SeqNum seqnum)
{
    nlohmann::json jResponse;
    jResponse["seqnum"] = seqnum;
    jResponse["kind"] = "ok";
    return jResponse;
}

nlohmann::json
GpioJsonInterface::json_report_input_(SeqNum seqnum, PinId pin, PinLevel level)
{
    nlohmann::json jResponse;
    jResponse["seqnum"] = seqnum;
    jResponse["kind"] = "report-input";
    jResponse["pin"] = pin;
    jResponse["level"] = level;
    return jResponse;
}

nlohmann::json
GpioJsonInterface::json_error_(SeqNum seqnum, const std::string &message)
{
    nlohmann::json jResponse;
    jResponse["seqnum"] = seqnum;
    jResponse["kind"] = "error";
    jResponse["errorDetail"] = message;
    return jResponse;
}
