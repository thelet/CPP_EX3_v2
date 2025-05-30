#include "gui/SFMLWindow.hpp"
#include "gui/BoardWidget.hpp"
#include "core/Player.hpp" 
#include "core/Game.hpp"

#include <sstream>
using coup_gui::SFMLWindow;
using coup::Player;

SFMLWindow::SFMLWindow(coup::Game& g) : game_(g)
{
    font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    board_ = std::make_unique<BoardWidget>(font_);
}

/*──────── convenience ───────*/
void SFMLWindow::postMessage(const std::string& txt){ message_=txt; }

bool SFMLWindow::isSanctioned(const Player& p) const
{
     return p.sanctionedUntilTurn_ > game_.tick();  
}

/*──────── panel refresh ───────*/
void SFMLWindow::updatePanels(){
    const auto& roster = game_.roster(); 
    for(std::size_t i=0;i<6;++i){
        auto& card = board_->card(i);
        if(i>=roster.size() || !game_.alive(i)){
            card.setColor(sf::Color(30,30,30));
            continue;
        }
        const Player& pl = *roster[i];
        auto  base = sf::Color(90,90,90);
        if(i==game_.turnIndex()) base = sf::Color(200,50,200);
        if(isSanctioned(pl))     base = sf::Color(50,50,50);
        card.setColor(base);
        card.setTitle(pl.name()+" – "+pl.role());
        card.setCoins(pl.coins());
    }
}

/*──────── main loop ───────*/
int SFMLWindow::run(sf::RenderWindow& win)
{
    while(win.isOpen()){
        sf::Event ev;
        while(win.pollEvent(ev)){
            if(ev.type==sf::Event::Closed) win.close();
            if(ev.type==sf::Event::MouseButtonPressed){
                board_->handleClick(ev.mouseButton, *this);
            }
        }
        updatePanels();
        win.clear(sf::Color::Black);
        board_->draw(win);
        sf::Text bar(message_, font_, 16); bar.setPosition(10,750);
        win.draw(bar);
        win.display();
    }
    return 0;

}
