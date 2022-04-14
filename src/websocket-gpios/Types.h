#ifndef WEBSOCKET_GPIOS_TYPES_H_
#define WEBSOCKET_GPIOS_TYPES_H_

#include <cstdint>
#include <string>

////////////////////////////////////////////////////////////////////////////////

using SeqNum = uint64_t;

////////////////////////////////////////////////////////////////////////////////

enum class PullKind : uint8_t
{
    PULL_UP,
    PULL_DOWN,
    NO_PULL
};

PullKind PullKind_from_string(const std::string &pull_kind_str);

#endif // WEBSOCKET_GPIOS_TYPES_H_
