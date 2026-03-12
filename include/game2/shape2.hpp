#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Shape {
public:
    virtual ~Shape() = default;

    [[nodiscard]] virtual std::vector<sf::Vector2f> getPoints() const = 0;

    virtual void draw(sf::RenderWindow& window) const = 0;
};
