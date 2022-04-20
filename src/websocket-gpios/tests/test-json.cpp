#include "../GpioJsonInterface.h"
#include "../LoopbackGpioArray.h"

#include "../vendor/catch2/catch.hpp"

#include "json-requires.h"

#include <regex>

using json = nlohmann::json;

struct LoopbackJson
{
    LoopbackJson()
        : gpios(std::make_shared<LoopbackGpioArray>()), interface(gpios)
    {
    }

    json do_commands(const std::string &message)
    {
        const auto reply = interface.do_commands(message);
        const auto jReply = json::parse(reply);
        REQUIRE(jReply.is_array());
        return jReply;
    }

    std::shared_ptr<IGpioArray> gpios;
    GpioJsonInterface interface;
};

TEST_CASE("Zero-length array of commands")
{
    const auto jResp = LoopbackJson{}.do_commands("[]");
    JsonRequires::require_empty_array(jResp);
}

TEST_CASE("Valid reset command")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{"seqnum": 1234, "kind": "reset"}]
    )");
    JsonRequires::require_sole_ok(jResp, 1234);
}

TEST_CASE("Valid set-input command")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-input",
            "pin": 5,
            "pullKind": "pull-up"
        }]
    )");
    JsonRequires::require_sole_report_input(jResp, 1234, 5, 1);
}

TEST_CASE("Valid set-output command")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 4,
            "level": 1
        }]
    )");
    JsonRequires::require_sole_ok(jResp, 1234);
}

TEST_CASE("Multiple valid commands in one message")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 4,
            "level": 1
        }, {
            "seqnum": 1235,
            "kind": "set-input",
            "pin": 5,
            "pullKind": "pull-down"
        }]
    )");
    REQUIRE(jResp.size() == 2);
    JsonRequires::require_ok(jResp[0], 1234);
    JsonRequires::require_report_input(jResp[1], 1235, 5, 1);
}

TEST_CASE("Mixed-validity commands in one message")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 4,
            "level": 1
        }, {
            "seqnum": 1235,
            "kind": "dance"
        }, {
            "kind": "reset"
        }]
    )");
    REQUIRE(jResp.size() == 3);
    JsonRequires::require_ok(jResp[0], 1234);
    JsonRequires::require_error(jResp[1], 1235, "unknown command");
    JsonRequires::require_error(jResp[2], 0, "'seqnum' not found");
}

TEST_CASE("Multiple separate messages")
{
    // Need same interface to maintain state between messages:
    auto interface = LoopbackJson{};

    auto jResp = interface.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 4,
            "level": 1
        }]
    )");
    JsonRequires::require_sole_ok(jResp, 1234);

    jResp = interface.do_commands(R"(
        [{
            "seqnum": 1235,
            "kind": "set-input",
            "pin": 5,
            "pullKind": "pull-down"
        }]
    )");
    JsonRequires::require_sole_report_input(jResp, 1235, 5, 1);
}

TEST_CASE("Malformed message (not JSON)")
{
    const auto jResp = LoopbackJson{}.do_commands("not-json-urgh");
    JsonRequires::require_sole_error(jResp, 0, "could not parse");
}

TEST_CASE("Malformed command (missing field)")
{
    auto jResp = LoopbackJson{}.do_commands(R"(
        [{ "kind": "reset" }]
    )");
    JsonRequires::require_sole_error(jResp, 0, "'seqnum' not found");

    jResp = LoopbackJson{}.do_commands(R"(
        [{ "seqnum": 1234 }]
    )");
    JsonRequires::require_sole_error(jResp, 1234, "'kind' not found");
}

TEST_CASE("Malformed message (wrong top-level type)")
{
    auto jResp = LoopbackJson{}.do_commands(R"(
        { "seqnum": 1234, "kind": "reset" }
    )");
    JsonRequires::require_sole_error(
        jResp, 0, "must be JSON array, but got object");

    jResp = LoopbackJson{}.do_commands("42");
    JsonRequires::require_sole_error(
        jResp, 0, "must be JSON array, but got number");
}

TEST_CASE("Malformed message (wrong command type)")
{
    auto jResp = LoopbackJson{}.do_commands("[[0, 1, 2, 3]]");
    JsonRequires::require_sole_error(
        jResp, 0, "must be JSON object, but got array");

    jResp = LoopbackJson{}.do_commands("[0, 1, 2, 3]");
    REQUIRE(jResp.size() == 4);
    for (const auto &jReply : jResp)
        JsonRequires::require_error(
            jReply, 0, "must be JSON object, but got number");
}

TEST_CASE("Malformed command (wrong field type)")
{
    auto jResp = LoopbackJson{}.do_commands(R"(
        [{ "seqnum": true, "kind": "reset" }]
    )");
    JsonRequires::require_sole_error(jResp, 0, "must be number");

    jResp = LoopbackJson{}.do_commands(R"(
        [{ "seqnum": 42, "kind": 43 }]
    )");
    JsonRequires::require_sole_error(jResp, 42, "must be string");
}

TEST_CASE("Unknown command")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{ "seqnum": 10101, "kind": "dance" }]
    )");
    JsonRequires::require_sole_error(
        jResp, 10101, R"(unknown command "dance")");
}

TEST_CASE("Malformed set-input command (bad pullKind)")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-input",
            "pin": 5,
            "pullKind": "sideways"
        }]
    )");
    JsonRequires::require_sole_error(jResp, 1234, "unknown pullKind");
}

TEST_CASE("Bad set-input command (invalid pin)")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-input",
            "pin": 55,
            "pullKind": "no-pull"
        }]
    )");
    JsonRequires::require_sole_error(
        jResp, 1234, "cannot use pin 55 as an input");
}

TEST_CASE("Bad set-output command (invalid field values)")
{
    auto jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 555,
            "level": 1
        }]
    )");
    JsonRequires::require_sole_error(
        jResp, 1234, "cannot use pin 555 as an output");

    jResp = LoopbackJson{}.do_commands(R"(
        [{
            "seqnum": 1234,
            "kind": "set-output",
            "pin": 5,
            "level": 12
        }]
    )");
    JsonRequires::require_sole_error(
        jResp, 1234, "cannot set pin 5 to bad level 12");
}

TEST_CASE("Construct report-input message")
{
    const auto reply = GpioJsonInterface::report_input_message(17, 1);
    const auto jReply = json::parse(reply);
    JsonRequires::require_sole_report_input(jReply, 0, 17, 1);
}
