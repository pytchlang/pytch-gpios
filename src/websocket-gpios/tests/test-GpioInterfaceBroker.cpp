#include "../GpioInterfaceBroker.h"
#include "../MessageTransmitChannel.h"

#include "../vendor/catch2/catch.hpp"

#include <memory>
#include <string>
#include <vector>
#include <mutex>

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

TEST_CASE("GpioInterfaceBroker")
{
}
