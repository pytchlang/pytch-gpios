#include "../GpioJsonInterface.h"
#include "../LoopbackGpioArray.h"

#include "../vendor/catch2/catch.hpp"

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

    static void require_empty_array(const json &jReply)
    {
        REQUIRE(jReply.size() == 0);
    }

    static void require_ok(const json &jResp, SeqNum expected_seqnum)
    {
        REQUIRE(jResp.size() == 2);
        REQUIRE(jResp["seqnum"].get<SeqNum>() == expected_seqnum);
        REQUIRE(jResp["kind"].get<std::string>() == "ok");
    }

    static void require_report_input(
        const json &jResp,
        SeqNum expected_seqnum,
        PinId expected_pin,
        PinLevel expected_level)
    {
        REQUIRE(jResp.size() == 4);
        REQUIRE(jResp["seqnum"].get<SeqNum>() == expected_seqnum);
        REQUIRE(jResp["kind"].get<std::string>() == "report-input");
        REQUIRE(jResp["pin"].get<PinId>() == expected_pin);
        REQUIRE(jResp["level"].get<PinLevel>() == expected_level);
    }

    static void require_sole_ok(const json &jReply, SeqNum expected_seqnum)
    {
        REQUIRE(jReply.size() == 1);
        require_ok(jReply[0], expected_seqnum);
    }

    static void require_sole_report_input(
        const json &jReply,
        SeqNum expected_seqnum,
        PinId expected_pin,
        PinLevel expected_level)
    {
        REQUIRE(jReply.size() == 1);
        require_report_input(
            jReply[0],
            expected_seqnum, expected_pin, expected_level);
    }

    std::shared_ptr<IGpioArray> gpios;
    GpioJsonInterface interface;
};

TEST_CASE("Zero-length array of commands")
{
    const auto jResp = LoopbackJson{}.do_commands("[]");
    LoopbackJson::require_empty_array(jResp);
}

TEST_CASE("Valid reset command")
{
    const auto jResp = LoopbackJson{}.do_commands(R"(
        [{"seqnum": 1234, "kind": "reset"}]
    )");
    LoopbackJson::require_sole_ok(jResp, 1234);
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
    LoopbackJson::require_sole_report_input(jResp, 1234, 5, 1);
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
    LoopbackJson::require_sole_ok(jResp, 1234);
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
    LoopbackJson::require_ok(jResp[0], 1234);
    LoopbackJson::require_report_input(jResp[1], 1235, 5, 1);
}
