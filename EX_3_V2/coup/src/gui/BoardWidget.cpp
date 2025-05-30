// you@example.com
#include "gui/BoardWidget.hpp"
#include "gui/SFMLWindow.hpp"     // need the full type for implementation
#include "gui/CardWidget.hpp"
#include <array>

using namespace coup_gui;

BoardWidget::BoardWidget(const sf::Font& f)
{
    const std::array<sf::Vector2f,6> pos = {
        sf::Vector2f{10,10},  {240,10},  {470,10},
        sf::Vector2f{10,330}, {240,330}, {470,330}
    };
    for (auto p : pos)
        cards_.push_back(std::make_unique<CardWidget>(f, p));
}

void BoardWidget::draw(sf::RenderTarget& rt)
{
    for (auto& c : cards_) c->draw(rt);
}
void BoardWidget::handleClick(const sf::Event::MouseButtonEvent& ev,SFMLWindow& gui)
{
for (auto& c : cards_) c->handleClick(ev, gui);
}

