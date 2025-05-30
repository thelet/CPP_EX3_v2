// include/gui/StartScreen.hpp
#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include "gui/PlayerSpec.hpp"

namespace coup_gui {

class StartScreen {
public:
    explicit StartScreen(sf::RenderWindow& window);          // <- note
    std::vector<PlayerSpec> choose();                        // modal loop

private:
    sf::RenderWindow& win_;
    sf::Font          font_;

    struct Entry {
        sf::RectangleShape box;
        sf::Text           text;
        bool               active{false};
    };
    std::vector<Entry> nameBoxes_;
    std::vector<Entry> roleBoxes_;
    int                playerCount_{2};

    void draw();
    void handleEvent(const sf::Event&);
};

} // namespace coup_gui
