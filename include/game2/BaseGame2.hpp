#pragma once
#include <SFML/Graphics.hpp>
#include <variant>
#include <string>

using GameResult = std::variant<std::monostate, int, float, std::string>;

class BaseGame {
public:
    virtual ~BaseGame() = default;
    virtual void update(sf::RenderWindow& window) = 0;
    virtual void draw(sf::RenderWindow& window) = 0;

    virtual void printRes() {}
    virtual void randomShape() {}

    [[nodiscard]]  virtual GameResult getResult() const = 0;
};