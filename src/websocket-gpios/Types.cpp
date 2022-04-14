#include <sstream>
#include <stdexcept>
#include <string>

#include "Types.h"

PullKind PullKind_from_string(const std::string &pull_kind_str)
{
    if (pull_kind_str == "pull-up")
        return PullKind::PULL_UP;
    if (pull_kind_str == "pull-down")
        return PullKind::PULL_DOWN;
    if (pull_kind_str == "no-pull")
        return PullKind::NO_PULL;

    std::ostringstream oss;
    oss << "unknown pullKind \"" << pull_kind_str << "\"";
    throw std::invalid_argument(oss.str());
}
