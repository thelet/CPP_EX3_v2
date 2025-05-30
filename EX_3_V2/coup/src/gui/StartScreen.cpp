// src/gui/StartScreen.cpp
#include "gui/StartScreen.hpp"
#include <array>
using namespace coup_gui;

StartScreen::StartScreen(sf::RenderWindow& w) : win_(w)
{
    font_.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    const float lx=120, rx=450, y0=140, dy=70;
    for(int i=0;i<6;++i){
        nameBoxes_.push_back({sf::RectangleShape({220,40}), sf::Text("",font_,18)});
        roleBoxes_.push_back({sf::RectangleShape({220,40}), sf::Text("Governor",font_,18)});
        nameBoxes_[i].box.setPosition(lx, y0+i*dy);
        roleBoxes_[i].box.setPosition(rx, y0+i*dy);
        nameBoxes_[i].box.setFillColor(sf::Color(40,40,40));
        roleBoxes_[i].box.setFillColor(sf::Color(40,40,40));
        nameBoxes_[i].box.setOutlineThickness(2);
        roleBoxes_[i].box.setOutlineThickness(2);
    }
}
// -----------------------------------------------------------------
std::vector<PlayerSpec> StartScreen::choose()
{
    while(win_.isOpen()){
        sf::Event ev;
        while(win_.pollEvent(ev)){
            if(ev.type==sf::Event::Closed){ win_.close(); break; }
            handleEvent(ev);
        }
        draw();
    }
    std::vector<PlayerSpec> out;
    static const std::array<const char*,6> roles={"Governor","Spy","Baron","General","Judge","Merchant"};
    for(int i=0;i<playerCount_;++i){
        PlayerSpec p;
        p.name = nameBoxes_[i].text.getString();
        if(p.name.empty()) p.name = "P"+std::to_string(i+1);
        p.role = roleBoxes_[i].text.getString();
        if(std::find(roles.begin(),roles.end(),p.role)==roles.end()) p.role="Governor";
        out.push_back(std::move(p));
    }
    return out;
}
// -----------------------------------------------------------------
void StartScreen::handleEvent(const sf::Event& ev)
{
    if(ev.type==sf::Event::KeyPressed){
        if(ev.key.code==sf::Keyboard::Up   && playerCount_<6) ++playerCount_;
        if(ev.key.code==sf::Keyboard::Down && playerCount_>2) --playerCount_;
        if(ev.key.code==sf::Keyboard::Enter) win_.close();
    }
    if(ev.type==sf::Event::TextEntered){
        for(auto& e:nameBoxes_)
            if(e.active && ev.text.unicode>=32 && ev.text.unicode<128){
                auto s=e.text.getString(); s+=static_cast<char>(ev.text.unicode);
                e.text.setString(s);
            }else if(e.active && ev.text.unicode==8 && !e.text.getString().isEmpty()){
                auto s=e.text.getString(); s.erase(s.getSize()-1); e.text.setString(s);
            }
    }
    if(ev.type==sf::Event::MouseButtonPressed){
        sf::Vector2f p{static_cast<float>(ev.mouseButton.x),static_cast<float>(ev.mouseButton.y)};
        for(auto& e:nameBoxes_){
            e.active=e.box.getGlobalBounds().contains(p);
            e.box.setOutlineColor(e.active?sf::Color::Yellow:sf::Color::White);
        }
        static const std::array<const char*,6> roles={"Governor","Spy","Baron","General","Judge","Merchant"};
        for(auto& e:roleBoxes_)
            if(e.box.getGlobalBounds().contains(p)){
                std::string cur=e.text.getString();
                std::size_t idx=(std::find(roles.begin(),roles.end(),cur)-roles.begin()+1)%roles.size();
                e.text.setString(roles[idx]);
            }
    }
}
// -----------------------------------------------------------------
void StartScreen::draw()
{
    win_.clear(sf::Color::Black);
    sf::Text t("Coup – Pick players (UP/DOWN to change count, ENTER to start)",
               font_,20); t.setPosition(60,60); win_.draw(t);
    for(int i=0;i<6;++i){
        bool vis=i<playerCount_;
        auto& nb=nameBoxes_[i]; auto& rb=roleBoxes_[i];
        nb.box.setFillColor(vis?sf::Color(60,60,60):sf::Color(20,20,20));
        rb.box.setFillColor(vis?sf::Color(60,60,60):sf::Color(20,20,20));
        win_.draw(nb.box); win_.draw(rb.box);
        if(vis){ win_.draw(nb.text); win_.draw(rb.text); }
    }
    win_.display();
}
