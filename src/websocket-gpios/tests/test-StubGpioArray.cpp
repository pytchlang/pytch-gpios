#include "../StubGpioArray.h"

#include "../vendor/catch2/catch.hpp"

template <typename T> static void require_failure(const Outcome<T> &outcome)
{
    REQUIRE(std::holds_alternative<Failure>(outcome));
}

TEST_CASE("StubGpioArray rejects all methods")
{
    StubGpioArray gpios;

    {
        INFO("reset() should fail");
        require_failure(gpios.reset());
    }

    {
        INFO("set_as_input() should fail");
        require_failure(gpios.set_as_input(1, PullKind::PULL_DOWN));
    }

    {
        INFO("set_output() should fail");
        require_failure(gpios.set_output(5, 1));
    }

    {
        INFO("launch_input_monitor() should fail");
        require_failure(gpios.launch_input_monitor([](PinId, PinLevel) {}));
    }
}
