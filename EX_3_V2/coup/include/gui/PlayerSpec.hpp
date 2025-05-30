// include/gui/PlayerSpec.hpp
#pragma once
#include <string>

namespace coup_gui {
struct PlayerSpec {
    std::string name;
    std::string role;   // “Governor”, “Spy”, …
};
} // namespace coup_gui
