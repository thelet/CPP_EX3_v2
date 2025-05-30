// you@example.com
#pragma once
#include <SFML/Graphics.hpp>
#include <array>
#include "gui/GuiCommon.hpp"

namespace coup_gui {

class CardWidget {
public:
    CardWidget(const sf::Font& f, sf::Vector2f topleft);

    /* dynamic updates */
    void setTitle(const std::string& s) { title_.setString(s); }
    void setCoins(int c)                { coins_.setString("Coins: " + std::to_string(c)); }
    void setColor(sf::Color col)        { panel_.setFillColor(col); }

    Button& action(std::size_t i) { return buttons_.at(i); }

    void draw(sf::RenderTarget& rt);
    void handleClick(const sf::Event::MouseButtonEvent&, SFMLWindow&);
    
private:
    sf::RectangleShape        panel_;
    sf::Text                  title_;
    sf::Text                  coins_;
    std::array<Button,6>      buttons_;   // Gather / Tax / Bribe / Arrest / Sanction / Coup
};

} // namespace coup_gui
