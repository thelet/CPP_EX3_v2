// thelet.shevach@gmail.com
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "core/Game.hpp"
#include "core/Player.hpp"

#include <vector>
#include <string>
using namespace coup;

//───────────────────────────────────────────────────────────────────────────────
// Advance the game until it's target's turn. Everybody else just gathers.
static void advanceTo(Game& g, const std::vector<Player*>& ps, Player* target) {
    while (g.turn() != target->name()) {
        const std::string cur = g.turn();
        bool acted = false;
        for (auto p : ps) {
            if (p->name() == cur) {
                p->gather();
                acted = true;
                break;
            }
        }
        // if turn not in our list, break to avoid infinite loop
        if (!acted) break;
    }
}
//───────────────────────────────────────────────────────────────────────────────

TEST_CASE("1. Gather increments by 1") {
    Game g;
    Governor a(g,"A"); Spy b(g,"B");
    std::vector<Player*> ps{&a,&b};
    CHECK(a.coins()==0);
    advanceTo(g,ps,&a);
    a.gather();
    CHECK(a.coins()==1);
    advanceTo(g,ps,&b);
    b.gather();
    CHECK(b.coins()==1);
}

TEST_CASE("2. Tax: normal=2, governor=3") {
    Game g;
    Governor gov(g,"G"); Spy s(g,"S");
    std::vector<Player*> ps{&gov,&s};
    advanceTo(g,ps,&gov);
    gov.tax();   // +3
    advanceTo(g,ps,&s);
    s.tax();     // +2
    CHECK(gov.coins()==3);
    CHECK(s.coins()==2);
}

TEST_CASE("3. Governor undo tax in same round") {
    Game g;
    Governor gov(g,"G"); Spy s(g,"S");
    std::vector<Player*> ps{&gov,&s};
    advanceTo(g,ps,&s);
    s.tax();
    advanceTo(g,ps,&gov);
    gov.undo(s);
    CHECK(s.coins()==0);
}

TEST_CASE("4. Judge cannot undo tax") {
    Game g;
    Judge j(g,"J"); Spy s(g,"S");
    std::vector<Player*> ps{&j,&s};
    advanceTo(g,ps,&j);
    j.gather();
    advanceTo(g,ps,&s);
    s.tax();
    advanceTo(g,ps,&j);
    CHECK_THROWS_AS(j.undo(s), IllegalAction);
}

TEST_CASE("5. Bribe gives extra turn; Judge can undo later") {
    Game g;
    Spy s(g,"S"); Judge j(g,"J");
    s.addCoins(4);
    std::vector<Player*> ps{&s,&j};
    advanceTo(g,ps,&s);
    s.bribe();       // spends 4, still S's turn
    s.gather();      // extra action
    CHECK(s.coins()==1);
    advanceTo(g,ps,&j);
    j.undo(s);
    CHECK(s.coins()==5);
}

TEST_CASE("6. Arrest cannot target same twice in a row") {
    Game g;
    Spy a(g,"A"); Spy b(g,"B");
    std::vector<Player*> ps{&a,&b};
    advanceTo(g,ps,&a);
    b.addCoins(2);
    a.arrest(b);
    CHECK(a.coins()==1);
    advanceTo(g,ps,&a);
    CHECK_THROWS_AS(a.arrest(b), IllegalAction);
}

TEST_CASE("7. Spy blocks next arrest") {
    Game g;
    Spy spy(g,"P"); Baron bar(g,"B");
    std::vector<Player*> ps{&spy,&bar};
    advanceTo(g,ps,&spy);
    spy.blockArrest(bar);
    advanceTo(g,ps,&bar);
    CHECK_THROWS_AS(bar.arrest(spy), IllegalAction);
}


TEST_CASE("8. Baron invest action") {
    Game g;
    Baron bar(g,"B");
    std::vector<Player*> ps{&bar};
    bar.addCoins(3);
    advanceTo(g,ps,&bar);
    bar.invest();
    CHECK(bar.coins()==6);
}

TEST_CASE("9. Baron sanctioned") {
    Game g;
    Spy a(g,"A"); Baron b(g,"B");
    std::vector<Player*> ps{&a,&b};
    a.addCoins(3);
    advanceTo(g,ps,&a);
    a.sanction(b);
    // a paid 3 then got rebate 1 => net -3 from start 3 => 0
    CHECK(a.coins()==0);
}

TEST_CASE("10. General refunded upon arrest") {
    Game g;
    Spy a(g,"A"); General g1(g,"G");
    std::vector<Player*> ps{&a,&g1};
    advanceTo(g,ps,&a);
    g1.addCoins(1);
    a.arrest(g1);
    // g1 lost 1 then refunded => 1
    CHECK(g1.coins()==1);
}



TEST_CASE("11. Merchant loses 2 when arrested") {
    Game g;
    Spy a(g,"A"); Merchant m(g,"M");
    std::vector<Player*> ps{&a,&m};
    advanceTo(g,ps,&a);
    a.addCoins(1);
    m.addCoins(2);
    a.arrest(m);
    // m pays 2, a gets non
    CHECK(a.coins()==1);
    CHECK(m.coins()==0);
}


TEST_CASE("12. Governor cannot undo bribe") {
    Game g;
    Governor gov(g,"Gov"); Spy s(g,"S");
    std::vector<Player*> ps{&gov,&s};
    s.addCoins(4);
    advanceTo(g,ps,&s);
    s.bribe();
    advanceTo(g,ps,&gov);
    CHECK_THROWS_AS(gov.undo(s), IllegalAction);
}

TEST_CASE("13. Judge cannot block tax") {
    Game g;
    Judge j(g,"J"); Spy s(g,"S");
    std::vector<Player*> ps{&j,&s};
    advanceTo(g,ps,&s);
    s.tax();
    advanceTo(g,ps,&j);
    CHECK_THROWS_AS(j.undo(s), IllegalAction);
}


TEST_CASE("14. Cannot spend more coins than you have") {
    Game g;
    Spy s(g,"S");
    std::vector<Player*> ps{&s};
    advanceTo(g,ps,&s);
    CHECK_THROWS_AS(s.coup(s), IllegalAction);
}


TEST_CASE("15. Bribe grants exactly one extra action") {
    Game g;
    Spy s(g,"S"); General g1(g,"G1");
    std::vector<Player*> ps{&s,&g1};
    s.addCoins(4);
    advanceTo(g,ps,&s);
    s.bribe();
    s.gather();
    CHECK_THROWS_AS(s.gather(), NotYourTurn);
}

TEST_CASE("16. Governor undo must happen in same round") {
    Game g;
    Governor gov(g,"Gov"); Spy s(g,"S");
    std::vector<Player*> ps{&gov,&s};
    advanceTo(g,ps,&s);
    s.tax();
    advanceTo(g,ps,&gov);
    gov.undo(s);
    // next round
    advanceTo(g,ps,&s);
    advanceTo(g,ps,&gov);
    CHECK_THROWS_AS(gov.undo(s), IllegalAction);
}


TEST_CASE("17. Winner only when one remains") {
    Game g;
    Spy a(g,"A"), b(g,"B");
    std::vector<Player*> ps{&a,&b};
    CHECK_THROWS_AS(g.winner(), GameNotFinished);
    a.addCoins(7);
    advanceTo(g,ps,&a);
    a.coup(b);
    CHECK(g.winner()=="A");
}

TEST_CASE("18. Spy peek returns correct coins") {
    Game g;
    Spy spy(g,"S"); Baron bar(g,"B");
    std::vector<Player*> ps{&spy,&bar};
    bar.addCoins(3);
    advanceTo(g,ps,&spy);
    CHECK(spy.peek(bar)==3);
}



