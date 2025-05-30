// you@example.com
#include "gui/CardWidget.hpp"

using namespace coup_gui;


CardWidget::CardWidget(const sf::Font& f, sf::Vector2f pos)
: panel_  ()
, title_  ()
, coins_  ()
, buttons_{
      /* row-1 */
      Button(f, {pos.x+20.f , pos.y+70.f }, 80,28, "Gather"),
      Button(f, {pos.x+120.f, pos.y+70.f }, 80,28, "Tax"   ),
      /* row-2 */
      Button(f, {pos.x+20.f , pos.y+110.f}, 80,28, "Bribe" ),
      Button(f, {pos.x+120.f, pos.y+110.f}, 80,28, "Arrest"),
      /* row-3 */
      Button(f, {pos.x+20.f , pos.y+150.f}, 80,28, "Sanction"),
      Button(f, {pos.x+120.f, pos.y+150.f}, 80,28, "Coup"    )
  }
  {
    /* panel and static texts */
    panel_.setPosition(pos);
    panel_.setSize({210.f, 300.f});
    panel_.setFillColor(sf::Color(30,30,60));

    title_.setFont(f);
    title_.setCharacterSize(16);
    title_.setFillColor(sf::Color::White);
    title_.setPosition(pos.x + 8, pos.y + 6);

    coins_.setFont(f);
    coins_.setCharacterSize(14);
    coins_.setFillColor(sf::Color::Yellow);
    coins_.setPosition(pos.x + 8, pos.y + 28);
  }

void CardWidget::draw(sf::RenderTarget& rt)
{
    rt.draw(panel_);
    rt.draw(title_);
    rt.draw(coins_);
    for (auto& b : buttons_) b.draw(rt);
}
void CardWidget::handleClick(const sf::Event::MouseButtonEvent& ev,SFMLWindow& gui)
{
for (auto& b : buttons_)
if (b.contains(ev)) b.onClick();
}

