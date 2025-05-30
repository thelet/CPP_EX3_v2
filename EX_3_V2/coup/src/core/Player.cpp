// you@example.com
#include "core/Player.hpp"
#include "core/Game.hpp"

using namespace coup;

/*──────── ctor ───────*/
Player::Player(Game& g, const std::string& n)
        : game_{g}, name_{n}
{
    game_.registerPlayer(this);
}

/*──────── bookkeeping helpers ───*/
void Player::spendCoins(int c) {
    if(coins_ < c) throw NotEnoughCoins("Not enough coins");
    coins_ -= c;
}

void Player::onNewTurn() {
    /* Merchant passive bonus */
    if(role()=="Merchant" && coins_>=3) coins_ += 1;
    /* clear sanction flag if its time passed */
    if(sanctionedUntilTurn_ && game_.turn() == name_)
        sanctionedUntilTurn_ = 0;
}

/*──────── generic action wrappers ─────*/
static Action make(std::size_t idx, Action::Type t,
                   std::optional<std::size_t> tgt = std::nullopt)
{
    return Action{t, idx, tgt};
}

void Player::gather()               { game_.perform( make(game_.indexOf(*this), Action::Type::Gather) ); }
void Player::tax()                  { game_.perform( make(game_.indexOf(*this), Action::Type::Tax   ) ); }
void Player::bribe()                { game_.perform( make(game_.indexOf(*this), Action::Type::Bribe ) ); }
void Player::arrest(Player& t)      { game_.perform( make(game_.indexOf(*this), Action::Type::Arrest , game_.indexOf(t)) ); }
void Player::sanction(Player& t)    { game_.perform( make(game_.indexOf(*this), Action::Type::Sanction, game_.indexOf(t)) ); }
void Player::coup(Player& t)        { game_.perform( make(game_.indexOf(*this), Action::Type::Coup   , game_.indexOf(t)) ); }

/*──────── role-specific helpers ─────*/

/* Governor – undo someone’s Tax.
   Can be called at ANY time (not only his own turn).                  */
void Governor::undo(Player& taxed) {
    game_.block( make(game_.indexOf(*this), Action::Type::Block) );
}

/* Judge – undo a Bribe. */
void Judge::undo(Player& briber) {
    game_.block( make(game_.indexOf(*this), Action::Type::Block) );
}

/* Spy – stop someone from being arrested next turn                   */
void Spy::blockArrest(Player& tgt)  { game_.spyBlockArrest(*this,tgt); }

/* Baron – invest finishes the turn                                  */
void Baron::invest() {
    game_.baronInvest(*this);
    game_.perform( make(game_.indexOf(*this), Action::Type::Gather) ); // dummy to advance turn
}

/* General – pay 5 coins to save a Coup victim                       */
void General::blockCoup(Player& actor) {
    game_.block( make(game_.indexOf(*this), Action::Type::Block) );
}
