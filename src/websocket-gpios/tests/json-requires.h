#ifndef WEBSOCKET_GPIOS_TESTS_JSON_REQUIRES_H_
#define WEBSOCKET_GPIOS_TESTS_JSON_REQUIRES_H_

#include "../vendor/catch2/catch.hpp"

#include "../vendor/nlohmann/json.hpp"

#include <regex>

using json = nlohmann::json;

struct JsonRequires
{
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
            jReply[0], expected_seqnum, expected_pin, expected_level);
    }

    static void require_error(
        const json &jResp,
        SeqNum expected_seqnum,
        const std::string expected_detail_regex_str)
    {
        REQUIRE(jResp.size() == 3);
        REQUIRE(jResp["seqnum"].get<SeqNum>() == expected_seqnum);
        REQUIRE(jResp["kind"].get<std::string>() == "error");

        std::regex expected_error_regex{expected_detail_regex_str};
        std::string detail = jResp["errorDetail"].get<std::string>();
        INFO("errorDetail \""
             << detail
             << "\" should match regex \""
             << expected_detail_regex_str << "\"");
        REQUIRE(std::regex_search(detail, expected_error_regex));
    }

    static void require_sole_error(
        const json &jReply,
        SeqNum expected_seqnum,
        const std::string expected_detail_regex_str)
    {
        INFO("reply: " << jReply);
        REQUIRE(jReply.is_array());
        REQUIRE(jReply.size() == 1);
        require_error(jReply[0], expected_seqnum, expected_detail_regex_str);
    }
};

#endif // WEBSOCKET_GPIOS_TESTS_JSON_REQUIRES_H_
