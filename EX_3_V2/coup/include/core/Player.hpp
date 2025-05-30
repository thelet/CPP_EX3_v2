// you@example.com

#pragma once

#include <string>
#include "core/Action.hpp"

namespace coup {
    class Game;  // forward

    class Player {
    public:
        // Rule of Three: non-copyable, default destructor
        Player(Game& g, const std::string& n);
        Player(const Player&) = delete;
        Player& operator=(const Player&) = delete;
        virtual ~Player() = default;

        // Accessors
        const std::string& name()  const { return name_; }
        int                coins() const { return coins_; }
        virtual std::string role()  const = 0;

        // Game‐side coin bookkeeping
        void addCoins(int c)   { coins_ += c; }
        void spendCoins(int c);

        // Actions (wrap Game::perform)
        void gather();
        void tax();
        void bribe();
        void arrest(Player& target);
        void sanction(Player& target);
        void coup(Player& target);

        // Called by Game at start of each turn
        void onNewTurn();

        // Used internally by Game
        std::size_t lastArrested_{static_cast<std::size_t>(-1)};
        std::size_t sanctionedUntilTurn_{0};

    protected:
        Game&       game_;
        std::string name_;
        int         coins_{0};
    };

    // —— concrete roles ——

    class Governor : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "Governor"; }
        void undo(Player& taxed);
    };

    class Spy : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "Spy"; }
        int  peek(Player& target)                { return target.coins(); }
        void blockArrest(Player& target);
    };

    class Baron : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "Baron"; }
        void invest();
    };

    class General : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "General"; }
        void blockCoup(Player& actor);
    };

    class Judge : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "Judge"; }
        void undo(Player& briber);
    };

    class Merchant : public Player {
    public:
        using Player::Player;
        std::string role() const override { return "Merchant"; }
    };

} // namespace coup
