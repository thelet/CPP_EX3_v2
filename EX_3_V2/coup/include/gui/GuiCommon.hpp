// include/gui/GuiCommon.hpp
#pragma once
#include <SFML/Graphics.hpp>

namespace coup_gui {
class SFMLWindow;

struct Button {
    sf::RectangleShape shape;
    sf::Text           label;
    std::function<void()> onClick;

    Button(const sf::Font& f,
           sf::Vector2f    pos,
           float           w,
           float           h,
           const std::string& txt,
           sf::Color       col = sf::Color(100,100,100))
    {
        shape.setSize({w,h});
        shape.setPosition(pos);
        shape.setFillColor(col);
        shape.setOutlineThickness(2);
        label.setFont(f);
        label.setString(txt);
        label.setCharacterSize(16);
        label.setPosition(pos.x+6, pos.y+4);
    }
    bool contains(const sf::Event::MouseButtonEvent& e) const {
        return shape.getGlobalBounds().contains(
            static_cast<float>(e.x), static_cast<float>(e.y));
    }
    void draw(sf::RenderTarget& rt) const {
        rt.draw(shape);
        rt.draw(label);
    }
};

} // namespace coup_gui
