#ifndef WEBSOCKET_GPIOS_TYPES_H_
#define WEBSOCKET_GPIOS_TYPES_H_

#include <cstdint>
#include <functional>
#include <string>
#include <variant>

////////////////////////////////////////////////////////////////////////////////

template <typename T> struct Success
{
    T value;
    explicit Success(const T &v) : value(v) {}
};

template <> struct Success<void>
{
};

struct Failure
{
    std::string message;
};

template <typename T> using Outcome = std::variant<Success<T>, Failure>;

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

////////////////////////////////////////////////////////////////////////////////

// These would fit in a uint8, but that's annoying to work with in
// stream operations when, e.g., creating error messages.
using PinId = int;
using PinLevel = int;

////////////////////////////////////////////////////////////////////////////////

using PinLevelReportFun = std::function<void(PinId, PinLevel)>;

#endif // WEBSOCKET_GPIOS_TYPES_H_
