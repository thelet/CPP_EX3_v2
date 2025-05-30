#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include "gui/BoardWidget.hpp"
#include "core/Game.hpp"  
#include "core/Player.hpp"

namespace coup_gui {

class SFMLWindow {
public:
    explicit SFMLWindow(coup::Game& g);
    int run(sf::RenderWindow& win);

private:
    bool isSanctioned(const coup::Player&) const;
    void postMessage(const std::string&);
    void updatePanels();

    coup::Game&                  game_;
    sf::Font                     font_;
    std::unique_ptr<BoardWidget> board_;
    std::string                  message_;
};

} // namespace coup_gui
