#include "../StubGpioArray.h"

#include "../vendor/catch2/catch.hpp"

template <typename T> static void require_failure(const Outcome<T> &outcome)
{
    REQUIRE(std::holds_alternative<Failure>(outcome));
}

