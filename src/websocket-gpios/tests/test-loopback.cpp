#include <thread>
#include <chrono>
#include <mutex>

#include "../LoopbackGpioArray.h"

#include "../vendor/catch2/catch.hpp"

class InputRecorder
{
public:
    void update_pin(PinId pin, PinLevel level);
    bool pin_eventually_has_level(PinId pin, PinLevel expected_level);

private:
    using mutex_lock_t = std::lock_guard<std::mutex>;
    std::unordered_map<PinId, PinLevel> pin_levels_;
    std::mutex mutex_;
};

void InputRecorder::update_pin(PinId pin, PinLevel level)
{
    mutex_lock_t lock{mutex_};
    pin_levels_.insert_or_assign(pin, level);
}

bool InputRecorder::pin_eventually_has_level(PinId pin, PinLevel expected_level)
{
    bool got_expected = false;

    for (int i = 0; i != 100; ++i)
    {
        {
            mutex_lock_t lock{mutex_};

            if (auto entry = pin_levels_.find(pin);
                entry != pin_levels_.end() && entry->second == expected_level)
            {
                got_expected = true;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return got_expected;
}
