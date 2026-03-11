#include <SFML/Graphics.hpp>
#include <string>

// === КЛАСС ДЛЯ КНОПОК ===
class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;

    Button(float x, float y, float w, float h, const std::string& str, const sf::Font& font, sf::Color bgColor = sf::Color(50, 50, 70))
        : text(font, str, 16)
    {
        rect.setPosition({x, y});
        rect.setSize({w, h});
        rect.setFillColor(bgColor);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2);

        text.setFillColor(sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
        text.setPosition({x + w / 2.0f, y + h / 2.0f});
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(rect);
        window.draw(text);
    }

    bool isClicked(sf::Vector2i mousePos, bool mouseClicked) const {
        if (mouseClicked && rect.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
            return true;
        }
        return false;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({1120, 700}), "PGA Robot Engine - Pro UI");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        bool mouseClicked = false;
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (event->is<sf::Event::MouseButtonPressed>()) {
                if (event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    mouseClicked = true;
                }
            }
        }

        window.clear(sf::Color(20, 20, 25));
        window.display();
    }
    return 0;
}