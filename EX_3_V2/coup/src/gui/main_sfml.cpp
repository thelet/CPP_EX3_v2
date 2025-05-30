#include <SFML/Graphics.hpp>
#include "gui/StartScreen.hpp"
#include "gui/SFMLWindow.hpp"
#include "core/Player.hpp"
#include "core/Game.hpp"

int main(){
    sf::RenderWindow win(sf::VideoMode(800,800),"Coup");

    coup_gui::StartScreen start(win);
    auto specs = start.choose();              // modal

    coup::Game g;
    for(auto& s:specs){
        if(s.role=="Governor")  new coup::Governor(g,s.name);
        else if(s.role=="Spy")  new coup::Spy(g,s.name);
        else if(s.role=="Baron")new coup::Baron(g,s.name);
        else if(s.role=="General")new coup::General(g,s.name);
        else if(s.role=="Judge") new coup::Judge(g,s.name);
        else                     new coup::Merchant(g,s.name);
    }

    sf::RenderWindow gameWin(sf::VideoMode(800,800),"Coup");
    coup_gui::SFMLWindow gui(g);
    return gui.run(gameWin);
}
