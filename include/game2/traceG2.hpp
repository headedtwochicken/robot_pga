#pragma once
#include "BaseGame2.hpp"
#include "shape2.hpp"
#include "../RobotArm.hpp"
#include <memory>
#include <vector>

class TraceGame : public BaseGame {
private:
    std::vector<std::unique_ptr<Shape>> shapes;
    Shape* currentShape = nullptr;
    std::vector<sf::Vector2f> userTrace;

    bool tracing = false;
    bool finished = false;
    float score = 0;

    sf::Font font;
    sf::Text resText;

    RobotArm arm;
    sf::CircleShape targetPoint;

    bool rPressed = false;
    bool sPressed = false;

public:
    TraceGame();
    void update(sf::RenderWindow& window) override;
    void draw(sf::RenderWindow& window) override;
    void randomShape() override;
    float calculateAccuracy() const;
    void printRes() override;

    GameResult getResult() const override {
        return score;
    }
};