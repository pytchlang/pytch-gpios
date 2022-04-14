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
