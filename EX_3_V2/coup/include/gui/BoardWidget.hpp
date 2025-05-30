// you@example.com
#pragma once
#include <vector>
#include <memory>
#include "gui/CardWidget.hpp"

namespace coup_gui {

class BoardWidget {
public:
    explicit BoardWidget(const sf::Font&);

    CardWidget& card(std::size_t i) { return *cards_.at(i); }
    void        draw(sf::RenderTarget&);
    void            handleClick(const sf::Event::MouseButtonEvent&, SFMLWindow&);
private:
    std::vector<std::unique_ptr<CardWidget>> cards_;
};

} // namespace coup_gui
