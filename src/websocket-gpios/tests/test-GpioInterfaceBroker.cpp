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
#include <thread>
#include <chrono>

using json = nlohmann::json;

struct RecordingMessageChannel : public IMessageTransmitChannel
{
    void send(const std::shared_ptr<const std::string> message) override
    {
        mutex_lock_t lock{mutex_};
        messages.push_back(*message);
    }

    void poll_until_n_messages(size_t n_messages)
    {
        size_t got_n_messages;
        for (int i = 0; i != 100; ++i)
        {
            {
                mutex_lock_t lock{mutex_};
                got_n_messages = messages.size();
                if (got_n_messages == n_messages)
                    break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        REQUIRE(got_n_messages == n_messages);
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

    // Wait for initial input-level reports to come in.
    recorder->poll_until_n_messages(2);

    // We should get two more reports when we make a change:
    gpios->set_as_input(4, PullKind::PULL_UP);
    recorder->poll_until_n_messages(4);

    // And only one more (because pin 5 no longer an input) when we
    // change pin 5 to an output:
    gpios->set_output(5, 0);
    recorder->poll_until_n_messages(5);

    // The following test for the content of the input-level reports is
    // fragile.  It relies on the fact that we're given report-input
    // messages in order of pin id.
    JsonRequires::require_sole_report_input(
        json::parse(recorder->messages[2]), 0, 4, 1);
    JsonRequires::require_sole_report_input(
        json::parse(recorder->messages[4]), 0, 4, 0);

    // Assigning a fresh shared-ptr to "recorder" should destroy the
    // existing RecordingMessageChannel, meaning that the broker should
    // let us get a new non-stub interface.
    recorder = std::make_shared<RecordingMessageChannel>();
    json_ifc = broker.issue_json_interface(recorder);
    reply = json_ifc->do_commands(reset_message);
    JsonRequires::require_sole_ok(json::parse(reply), 1234);
}
