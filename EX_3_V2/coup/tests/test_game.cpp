// tests/test_game.cpp
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "core/Game.hpp"
#include "core/Player.hpp"


using namespace coup;

// Helper to set up a game with named role pairs
static void setupGame(Game &g, std::vector<std::unique_ptr<Player>> &players,
                      std::initializer_list<std::pair<std::string,std::string>> specs) {
    for (auto &pr : specs) {
        const auto &name = pr.first;
        const auto &role = pr.second;
        if (role == "Governor")    players.emplace_back(std::make_unique<Governor>(g,name));
        if (role == "Spy")          players.emplace_back(std::make_unique<Spy>(g,name));
        if (role == "Baron")        players.emplace_back(std::make_unique<Baron>(g,name));
        if (role == "General")      players.emplace_back(std::make_unique<General>(g,name));
        if (role == "Judge")        players.emplace_back(std::make_unique<Judge>(g,name));
        if (role == "Merchant")     players.emplace_back(std::make_unique<Merchant>(g,name));
    }
}

TEST_CASE("Initial turn order and players list") {
    Game g;
    std::vector<std::unique_ptr<Player>> players;
    setupGame(g, players, {{"A","Spy"},{"B","Baron"},{"C","General"}});
    auto names = g.players();
    CHECK(names == std::vector<std::string>{"A","B","C"});
    CHECK(g.turn() == "A");
}

TEST_CASE("Gather and Tax actions update coins and advance turn") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g, P, {{"P1","Merchant"},{"P2","Spy"}});
    auto &p1 = dynamic_cast<Merchant&>(*P[0]);
    auto &p2 = dynamic_cast<Spy&>(*P[1]);

    CHECK(p1.coins() == 0);
    p1.gather(); // +1
    CHECK(p1.coins() == 1);
    CHECK(g.turn() == "P2");

    p2.tax(); // +2
    CHECK(p2.coins() == 2);
    CHECK(g.turn() == "P1");
}

TEST_CASE("Bribe grants extra turn and costs 4 coins") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"X","Judge"},{"Y","Baron"}});
    auto &x = dynamic_cast<Judge&>(*P[0]);
    CHECK(x.coins()==0);
    // give Judge coins
    for(int i=0;i<4;i++){ x.gather(); g.perform({Action::Type::Gather,0,{}}); }
    CHECK(x.coins()==4);
    x.bribe();
    CHECK(x.coins()==0);
    // still Judge's turn because bribe
    CHECK(g.turn()=="X");
    // now next action consumes and advances
    x.gather();
    CHECK(g.turn()=="Y");
}

TEST_CASE("Arrest takes 1 coin, cannot arrest same twice, Merchant pays 2") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"M","Merchant"},{"S","Spy"}});
    auto &m = dynamic_cast<Merchant&>(*P[0]);
    auto &s = dynamic_cast<Spy&>(*P[1]);
    // give Spy 1 coin
    s.gather();
    CHECK(s.coins()==1);
    m.arrest(s);
    CHECK(m.coins()==1);
    CHECK(s.coins()==0);
    // cannot arrest same again
    CHECK_THROWS_AS(m.arrest(s), IllegalAction);
}

TEST_CASE("Sanction blocks gather and tax, costs 3 coins, Baron rebate, Judge extra fee") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"B","Baron"},{"J","Judge"}});
    auto &b = dynamic_cast<Baron&>(*P[0]);
    auto &j = dynamic_cast<Judge&>(*P[1]);
    // give Baron coins
    for(int i=0;i<3;i++){ b.gather(); g.perform({Action::Type::Gather,0,{}});} 
    CHECK(b.coins()==3);
    b.sanction(j);
    // sanction costs 3, then rebate 1 for Baron
    CHECK(b.coins()==1);
    CHECK_THROWS_AS(j.gather(), IllegalAction);
    CHECK_THROWS_AS(j.tax(), IllegalAction);
    // after J's turn advances, sanction expires
}

TEST_CASE("Coup eliminates or can be blocked by General") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"G1","General"},{"G2","Governor"}});
    auto &gen = dynamic_cast<General&>(*P[0]);
    auto &gov = dynamic_cast<Governor&>(*P[1]);
    // give Governor 7 coins
    for(int i=0;i<7;i++){ gov.gather(); g.perform({Action::Type::Gather,1,{}});} 
    CHECK(gov.coins()==7);
    // Governor coup General
    gov.coup(gen);
    CHECK_THROWS_AS(gen.gather(), IllegalAction);
    // now only one left → winner
    CHECK(g.winner() == "Governor");

    // test block by General
    setupGame(g,P,{{"G1","General"},{"B","Baron"}});
    auto &b = dynamic_cast<Baron&>(*P[1]);
    for(int i=0;i<7;i++){ b.gather(); g.perform({Action::Type::Gather,1,{}});}    
    // Baron attempts coup General
    b.coup(gen);
    // General blocks
    gen.block(b);
    // both remain alive
    auto names = g.players();
    CHECK(names.size()==2);
}

TEST_CASE("Forced coup at 10+ coins") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"U","Spy"},{"V","Merchant"}});
    auto &u = dynamic_cast<Spy&>(*P[0]);
    // give U 10 coins
    for(int i=0;i<10;i++){ u.gather(); g.perform({Action::Type::Gather,0,{}});}  
    CHECK(u.coins()==10);
    CHECK_THROWS_AS(u.gather(), IllegalAction);
    // must coup
}

TEST_CASE("Role-specific: Governor undo tax") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"G","Governor"},{"S","Spy"}});
    auto &gov = dynamic_cast<Governor&>(*P[0]);
    auto &spy = dynamic_cast<Spy&>(*P[1]);
    spy.gather(); //1
    gov.tax();    // +3
    // Governor can undo
    gov.undo(spy);
    CHECK(spy.coins()==0);
}

TEST_CASE("Role-specific: Spy peek and blockArrest") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"S","Spy"},{"M","Merchant"}});
    auto &spy = dynamic_cast<Spy&>(*P[0]);
    auto &m   = dynamic_cast<Merchant&>(*P[1]);
    m.gather();
    int coins = spy.peek(m);
    CHECK(coins==1);
    spy.blockArrest(m);
    // next M.arrest(spy) should fail
}

TEST_CASE("Role-specific: Baron invest and rebate on sanction") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"B","Baron"},{"G","Spy"}});
    auto &bar = dynamic_cast<Baron&>(*P[0]);
    bar.gather(); bar.gather(); bar.gather();
    bar.invest();
    CHECK(bar.coins()==6);
    // sanction rebate
    bar.sanction(dynamic_cast<Spy&>(*P[1]));
    CHECK(bar.coins()==4);
}

TEST_CASE("Role-specific: General block coup, refund on arrest") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"G","General"},{"T","Baron"}});
    auto &gen = dynamic_cast<General&>(*P[0]);
    auto &bar = dynamic_cast<Baron&>(*P[1]);
    bar.gather(); for(int i=0;i<6;i++) { g.perform({Action::Type::Gather,1,{}});} // bar->7
    bar.coup(gen);
    gen.block(bar);
    // test arrest refund
    gen.gather(); // 1
    bar.arrest(gen);
    CHECK(gen.coins()==1);
}

TEST_CASE("Role-specific: Judge cancel bribe, extra fee on sanction") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"J","Judge"},{"B","Baron"}});
    auto &j = dynamic_cast<Judge&>(*P[0]);
    auto &b = dynamic_cast<Baron&>(*P[1]);
    // give B coins
    for(int i=0;i<4;i++){ b.gather(); g.perform({Action::Type::Gather,1,{}});}  
    b.bribe();
    CHECK_THROWS_AS(b.bribe(), NotYourTurn);
    j.undo(b);
    CHECK(b.coins()==4);
}

TEST_CASE("Role-specific: Merchant extra coin at start of turn") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"M","Merchant"},{"S","Spy"}});
    auto &m = dynamic_cast<Merchant&>(*P[0]);
    // give m 3 ahead of his turn
    m.gather(); m.gather(); m.gather();
    g.perform({Action::Type::Gather,1,{}}); // spy turn gathers
    // on new Merchant turn he gets +1
    CHECK(m.coins()==4);
}

TEST_CASE("Winner only when one left; otherwise throws") {
    Game g;
    std::vector<std::unique_ptr<Player>> P;
    setupGame(g,P,{{"A","Spy"},{"B","Baron"}});
    CHECK_THROWS_AS(g.winner(), GameNotFinished);
    // eliminate B
    auto &a = dynamic_cast<Spy&>(*P[0]);
    auto &b = dynamic_cast<Baron&>(*P[1]);
    for(int i=0;i<7;i++){ a.gather(); g.perform({Action::Type::Gather,0,{}});}    
    a.coup(b);
    CHECK(g.winner()=="A");
}

