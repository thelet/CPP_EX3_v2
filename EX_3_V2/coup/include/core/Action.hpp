// thelet.shevach@gmail.com
#pragma once
#include <optional>

namespace coup {

struct Action {
    enum class Type {
        Gather,  Tax,    Bribe,
        Arrest,  Sanction, Coup,
        
        Block,   Invest,  SpyPeek
    };

    Type          type;
    std::size_t   actor;                 // index in Game::roster()
    std::optional<std::size_t> target;   // some actions need a victim
};

} // namespace coup
