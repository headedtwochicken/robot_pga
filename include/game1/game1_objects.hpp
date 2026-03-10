#pragma once
#include <SFML/Graphics.hpp>
#include <random>

class GameObject {
protected:
    sf::Vector2f position;
    sf::Color color;

public:
    explicit GameObject(float x = 0, float y = 0) : position(x, y), color(sf::Color::White) {}
    virtual ~GameObject() = default;

    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    void setPosition(float x, float y) { position = {x, y}; }
    [[nodiscard]] sf::Vector2f getPosition() const { return position; }
    void setColor(const sf::Color& c) { color = c; }
};
