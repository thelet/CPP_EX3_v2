// you@example.com
#pragma once
#include <stdexcept>
#include <string>

namespace coup {

struct IllegalAction   : std::logic_error { using logic_error::logic_error; };
struct GameNotFinished : std::logic_error { using logic_error::logic_error; };
struct NotYourTurn     : IllegalAction    { using IllegalAction::IllegalAction; };
struct NoSuchPlayer    : IllegalAction    { using IllegalAction::IllegalAction; };
struct NotEnoughCoins  : IllegalAction    { using IllegalAction::IllegalAction; };

} // namespace coup
