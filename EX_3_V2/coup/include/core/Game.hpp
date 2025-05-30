// thelet.shevach@gmail.com
#pragma once

#include <vector>
#include <string>
#include <optional>
#include "core/Action.hpp"
#include "util/Exceptions.hpp"

namespace coup {

class Player;              // forward

class Game {
    /* ── state ─────────────────────────────────────────────── */
    std::vector<Player*> roster_;      // players in join order
    std::vector<bool>    alive_;       // still in the game
    std::size_t          turnIdx_{0};  // whose turn (index into roster_)
    std::size_t          tick_{0};     // “full-turns” counter


    std::optional<Action> pending_;

    /* lastBlockable_ – remembers the most-recent Tax / Bribe / Coup
       and stays valid until that same actor’s *next* turn.          */
    struct Remembered {
        Action      act;
        std::size_t expiresOnTurnIdx;   // when original actor plays again
    };
    std::optional<Remembered> lastBlockable_;

    /* ── internal helpers ───────────────────────────────────── */
    Player&       playerAt(std::size_t i);
    const Player& playerAt(std::size_t i) const;
    void          nextTurn();                 // advance to next living player
    void          enforce10CoinRule(Player&); // throw if ≥10 coins
    void          recordBlockable(const Action&); // fill lastBlockable_

public:
    explicit Game() = default;
    ~Game() = default;

    void registerPlayer(Player* p);    // called from Player ctor

    /* ---- public API used by Demo / GUI --------------------- */
    std::vector<std::string> players() const;   // living players
    const std::string&       turn()    const;   // whose turn name
    std::string              winner()  const;   // last survivor

    const std::vector<Player*>& roster()  const { return roster_; }
    bool                        alive(std::size_t i) const { return alive_.at(i); }
    std::size_t                 turnIndex()  const { return turnIdx_; }
    std::size_t                 tick()       const { return tick_; }




    /* ---- engine services ----------------------------------- */
    std::size_t indexOf(const Player& p) const;

    void perform(const Action& a);     // do an action (Player wrappers call)
    void block  (const Action& b);     // Governor / Judge / General

    /* role-specific helpers (Spy, Baron, …) ------------------ */
    void governorUndoTax(Player& gov, Player& taxed);
    void baronInvest     (Player& baron);
    void spyPeek         (Player& spy, Player& target);
    void spyBlockArrest  (Player& spy, Player& target);
};

} // namespace coup
