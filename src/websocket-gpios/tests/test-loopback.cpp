#include <chrono>
#include <mutex>
#include <thread>

#include "../LoopbackGpioArray.h"

#if ENABLE_PIGPIO
#include "../PiGpioArray.h"
#endif // ENABLE_PIGPIO

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

// Keep the recorders alive indefinitely because we have no mechanism
// for terminating the monitor threads which are launched, and they
// might send callbacks to the InputRecorder objects.
//
static std::vector<std::shared_ptr<InputRecorder>> recorders;

static void test_loopback(IGpioArray &gpios, PinId read_pin, PinId drive_pin)
{
    recorders.emplace_back(std::make_shared<InputRecorder>());
    auto recorder = recorders.back();

    gpios.reset();

    gpios.launch_input_monitor([recorder](PinId pin, PinLevel level)
                               { recorder->update_pin(pin, level); });

    auto outcome = gpios.set_as_input(drive_pin, PullKind::NO_PULL);
    REQUIRE(std::holds_alternative<Success<PinLevel>>(outcome));

    {
        INFO("reads low if pull-down")
        auto outcome = gpios.set_as_input(read_pin, PullKind::PULL_DOWN);
        REQUIRE(std::holds_alternative<Success<PinLevel>>(outcome));
        REQUIRE(recorder->pin_eventually_has_level(read_pin, 0));
    }

    {
        INFO("reads high if pull-up")
        auto outcome = gpios.set_as_input(read_pin, PullKind::PULL_UP);
        REQUIRE(std::holds_alternative<Success<PinLevel>>(outcome));
        REQUIRE(recorder->pin_eventually_has_level(read_pin, 1));
    }

    {
        INFO("reads low when other end driven low")
        auto outcome = gpios.set_output(drive_pin, 0);
        REQUIRE(std::holds_alternative<Success<void>>(outcome));
        REQUIRE(recorder->pin_eventually_has_level(read_pin, 0));
    }

    {
        INFO("reads high when other end driven high")
        auto outcome = gpios.set_output(drive_pin, 1);
        REQUIRE(std::holds_alternative<Success<void>>(outcome));
        REQUIRE(recorder->pin_eventually_has_level(read_pin, 1));
    }
}

// Keep the GpioArrays alive indefinitely because we have no mechanism
// for terminating the monitor threads which are launched, and they
// might attempt to unlock destroyed mutexes.
//
static std::vector<std::shared_ptr<IGpioArray>> gpio_arrays;

TEST_CASE("LoopbackGpioArray behaves")
{
    gpio_arrays.emplace_back(std::make_shared<LoopbackGpioArray>());
    auto &gpios(*gpio_arrays.back());
    test_loopback(gpios, 4, 5);
    test_loopback(gpios, 5, 4);
}

#if ENABLE_PIGPIO

TEST_CASE("PiGpioArray behaves")
{
    gpio_arrays.emplace_back(std::make_shared<PiGpioArray>());
    auto &gpios(*gpio_arrays.back());
    test_loopback(gpios, 4, 5);
    test_loopback(gpios, 5, 4);
}

#endif // ENABLE_PIGPIO
