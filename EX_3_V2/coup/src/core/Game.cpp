// you@example.com
#include "core/Game.hpp"
#include "core/Player.hpp"
#include <algorithm>

using namespace coup;

/* ── join / lookup ─────────────────────────────────────────── */
void Game::registerPlayer(Player* p) {
    roster_.push_back(p);
    alive_.push_back(true);
}

Player& Game::playerAt(std::size_t i)              { return *roster_.at(i); }
const Player& Game::playerAt(std::size_t i) const  { return *roster_.at(i); }

std::size_t Game::indexOf(const Player& p) const {
    auto it = std::find(roster_.begin(), roster_.end(), &p);
    if(it == roster_.end()) throw NoSuchPlayer("player not in roster");
    return std::distance(roster_.begin(), it);
}

/* ── turn rotation ─────────────────────────────────────────── */
void Game::nextTurn() {
    do { turnIdx_ = (turnIdx_ + 1) % roster_.size(); } while(!alive_[turnIdx_]);
    ++tick_;

    /* when the *actor* of lastBlockable_ gets the turn again → expire */
    if(lastBlockable_ && lastBlockable_->expiresOnTurnIdx == turnIdx_)
        lastBlockable_.reset();

    roster_[turnIdx_]->onNewTurn();
}

/* ── forced Coup when ≥10 coins ────────────────────────────── */
void Game::enforce10CoinRule(Player& p) {
    if(p.coins() >= 10) throw IllegalAction("Holding 10 coins – must coup");
}

/* remember a Tax / Bribe / Coup until actor’s next turn ----- */
void Game::recordBlockable(const Action& a) {
    lastBlockable_ = Remembered{a, static_cast<std::size_t>(turnIdx_)};
}

/* ── perform ------------------------------------------------- */
void Game::perform(const Action& a) {
    if(!alive_.at(a.actor))                         throw IllegalAction("Eliminated");
    if(a.actor != turnIdx_ && a.type != Action::Type::Block)
                                                    throw NotYourTurn("Wait for your turn");

    Player& actor = playerAt(a.actor);
    enforce10CoinRule(actor);

    switch(a.type) {
    case Action::Type::Gather:
        // cannot gather if currently sanctioned
        if (actor.sanctionedUntilTurn_ >  tick_) {
            throw IllegalAction("Player is sanctioned and cannot gather");
        }
        actor.addCoins(1);
        break;
    
    case Action::Type::Tax:
        // cannot tax if currently sanctioned
        if (actor.sanctionedUntilTurn_ > tick_) {
            throw IllegalAction("Player is sanctioned and cannot tax");
        }
        actor.addCoins(actor.role() == "Governor" ? 3 : 2);
        recordBlockable(a);
        break;

    case Action::Type::Bribe:
        actor.spendCoins(4);
        recordBlockable(a);          // Judge may undo later
        return;                      // extra action, keep same turnIdx_


    case Action::Type::Arrest: {
        if (!a.target) {
            throw IllegalAction("Need target");
        }
        Player& tgt   = playerAt(*a.target);
        // cannot arrest the same target twice in a row
        if (actor.lastArrested_ == *a.target) {
            throw IllegalAction("Cannot arrest same target twice");
        }

        const std::string &role = tgt.role();
        if (role == "Merchant") {
            // Merchant loses 2 coins; arresting player gets no coin.
            tgt.spendCoins(2);
        }
        else if (role == "General") {
            // General loses nothing; arresting player still earns 1 coin.
            actor.addCoins(1);
        }
        else {
            // All others lose 1, and arresting player gains 1.
            tgt.spendCoins(1);
            actor.addCoins(1);
        }

        actor.lastArrested_ = *a.target;
        break;
    }


    case Action::Type::Sanction:{
        if(!a.target)                       throw IllegalAction("Need target");
        Player& tgt = playerAt(*a.target);
        actor.spendCoins(3);
        tgt.sanctionedUntilTurn_ = tick_ + roster_.size();
        if(tgt.role()=="Baron")  tgt.addCoins(1);
        if(tgt.role()=="Judge")  actor.spendCoins(1);
        break;}

    case Action::Type::Coup:{
        if(!a.target)                       throw IllegalAction("Need target");
        actor.spendCoins(7);
        alive_.at(*a.target) = false;       // out of the game
        recordBlockable(a);                 // General may block
        break;}

    default:
        throw IllegalAction("Unsupported action");
    }

    pending_.reset();
    nextTurn();
}

/* ── block / undo ------------------------------------------- */
void Game::block(const Action& b) {
    /* verify role tries to block something valid ------------------ */
    auto matches = [&](const Action& src){
        return src.actor != b.actor; // can’t block yourself
    };

    const Action* targetAct = nullptr;

    if(pending_ && matches(*pending_))          targetAct = &*pending_;
    else if(lastBlockable_ && matches(lastBlockable_->act))
                                              targetAct = &lastBlockable_->act;

    if(!targetAct) throw IllegalAction("Nothing to block");

    Player& blocker = playerAt(b.actor);
    Player& actor   = playerAt(targetAct->actor);

    switch(targetAct->type)
    {
    case Action::Type::Tax:
        if(blocker.role()!="Governor") throw IllegalAction("Only Governor");
        actor.spendCoins(actor.role()=="Governor"?3:2);
        break;

    case Action::Type::Bribe:
        if(blocker.role()!="Judge")    throw IllegalAction("Only Judge");
        actor.addCoins(4);
        break;

    case Action::Type::Coup:
        if(blocker.role()!="General")  throw IllegalAction("Only General");
        blocker.spendCoins(5);
        alive_.at(targetAct->target.value()) = true; // revive victim
        break;

    default: throw IllegalAction("Cannot block this action");
    }

    pending_.reset();
    lastBlockable_.reset();
}

/* ── role helpers ------------------------------------------- */
void Game::governorUndoTax(Player& gov, Player& taxed){
    if(gov.role()!="Governor") throw IllegalAction("Not a governor");
    taxed.spendCoins(taxed.role()=="Governor"?3:2);
}
void Game::baronInvest(Player& baron){
    baron.spendCoins(3);
    baron.addCoins(5);
}
void Game::spyPeek(Player&, Player&){/* nothing */}
void Game::spyBlockArrest(Player& spy, Player& tgt){
    tgt.lastArrested_ = indexOf(spy);
}

/* ── living players & winner -------------------------------- */
std::vector<std::string> Game::players() const {
    std::vector<std::string> out;
    for(std::size_t i=0;i<roster_.size();++i)
        if(alive_[i]) out.push_back(roster_[i]->name());
    return out;
}

const std::string& Game::turn() const { return roster_.at(turnIdx_)->name(); }

std::string Game::winner() const {
    std::string name;
    for(std::size_t i=0;i<roster_.size();++i)
        if(alive_[i]){
            if(!name.empty()) throw GameNotFinished("Game still active");
            name = roster_[i]->name();
        }
    if(name.empty()) throw NoSuchPlayer("No players!");
    return name;
}
