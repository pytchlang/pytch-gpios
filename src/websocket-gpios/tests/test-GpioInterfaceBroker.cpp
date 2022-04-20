#include "../GpioInterfaceBroker.h"
#include "../MessageTransmitChannel.h"
#include "../GpioJsonInterface.h"
#include "../LoopbackGpioArray.h"

#include "../vendor/catch2/catch.hpp"

#include "json-requires.h"

#include <memory>
#include <string>
#include <vector>
#include <mutex>

using json = nlohmann::json;

struct RecordingMessageChannel : public IMessageTransmitChannel
{
    void send(const std::shared_ptr<const std::string> message) override
    {
        mutex_lock_t lock{mutex_};
        messages.push_back(*message);
    }

    using mutex_lock_t = std::lock_guard<std::mutex>;
    std::mutex mutex_;
    std::vector<std::string> messages;
};

// Allow this to be long-lived to avoid threads calling into
// destroyed objects.
static std::shared_ptr<IGpioArray> gpios;

TEST_CASE("GpioInterfaceBroker")
{
    const auto reset_message = R"([{"seqnum": 1234, "kind": "reset"}])";

    gpios = std::make_shared<LoopbackGpioArray>();
    GpioInterfaceBroker broker{gpios};

    // The first request to issue a JSON interface should give us
    // a real interface.
    auto recorder = std::make_shared<RecordingMessageChannel>();
    auto json_ifc = broker.issue_json_interface(recorder);
    auto reply = json_ifc->do_commands(reset_message);
    JsonRequires::require_sole_ok(json::parse(reply), 1234);

    // A request to issue a second interface should give a stub one.
    auto json_ifc_1 = broker.issue_json_interface(recorder);
    reply = json_ifc_1->do_commands(reset_message);
    JsonRequires::require_sole_error(
        json::parse(reply), 1234, "already in use");

    // Assigning a fresh shared-ptr to "recorder" should destroy the
    // existing RecordingMessageChannel, meaning that the broker should
    // let us get a new non-stub interface.
    recorder = std::make_shared<RecordingMessageChannel>();
    json_ifc = broker.issue_json_interface(recorder);
    reply = json_ifc->do_commands(reset_message);
    JsonRequires::require_sole_ok(json::parse(reply), 1234);
}
