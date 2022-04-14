#include "../Types.h"

#include "../vendor/catch2/catch.hpp"

TEST_CASE("PullKind_from_string parses valid PullKind from string")
{
    REQUIRE(PullKind_from_string("pull-up") == PullKind::PULL_UP);
    REQUIRE(PullKind_from_string("pull-down") == PullKind::PULL_DOWN);
    REQUIRE(PullKind_from_string("no-pull") == PullKind::NO_PULL);
}
