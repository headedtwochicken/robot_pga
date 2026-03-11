#include "../../include/game2/traceG2.hpp"
#include <cstdlib>
#include <cmath>
#include <iostream>
#include <random>

TraceGame::TraceGame() : resText(font), arm(450.0f, 380.0f, 120.0f, 100.0f) {
    shapes.emplace_back(std::make_unique<CircleShapeTrace>());
    shapes.emplace_back(std::make_unique<SquareShapeTrace>());
    shapes.emplace_back(std::make_unique<TriangleShapeTrace>());
    shapes.emplace_back(std::make_unique<RectangularShapeTrace>());
    shapes.emplace_back(std::make_unique<OvalShapeTrace>());
    shapes.emplace_back(std::make_unique<StarShapeTrace>());

    TraceGame::randomShape();

    if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc") &&
        !font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        throw std::runtime_error("Failed to load fonts!");
    }

    resText.setFont(font);
    resText.setCharacterSize(16);
    resText.setFillColor(sf::Color::White);
    resText.setPosition({915, 80});

    targetPoint.setRadius(6);
    targetPoint.setFillColor(sf::Color::Transparent);
    targetPoint.setOutlineColor(sf::Color::Magenta);
    targetPoint.setOutlineThickness(2);
    targetPoint.setOrigin({6.0f, 6.0f});
}

void TraceGame::randomShape() {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, shapes.size() - 1);

    std::size_t r = dist(rng);
    currentShape = shapes[r].get();

    userTrace.clear();
    finished = false;
    score = 0;

    resText.setString("CONTROLS:\n\nLClick - Draw\nA/D - Move Base\n\nR - Result\nS - New Shape\n\nDon't break\n the line!");
}

void TraceGame::update(sf::RenderWindow& window) {
    if (finished) return;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {
        arm.moveBase(-4.0f);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
        arm.moveBase(4.0f);
    }

    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    auto targetX = static_cast<float>(mousePos.x);
    if (targetX > 880) targetX = 880;

    targetPoint.setPosition({targetX, static_cast<float>(mousePos.y)});
    [[maybe_unused]] auto ikRes = arm.solveIK(targetX, static_cast<float>(mousePos.y));

    sf::Vector2f brushPos(arm.getEndX(), arm.getEndY());

    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && mousePos.x < 900) {
        tracing = true;
        userTrace.push_back(brushPos);
    } else {
        tracing = false;
    }
}

void TraceGame::printRes() {}
float TraceGame::calculateAccuracy() const { return 0.0f; }

void TraceGame::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(20, 20, 25));

    if (currentShape) currentShape->draw(window);

    for (size_t i = 1; i < userTrace.size(); i++) {
        sf::Vertex line[] = {
                sf::Vertex{userTrace[i - 1], sf::Color::Green},
                sf::Vertex{userTrace[i], sf::Color::Green}
        };
        window.draw(line, 2, sf::PrimitiveType::Lines);
    }

    arm.draw(window);
    window.draw(targetPoint);

    sf::RectangleShape rightPanel(sf::Vector2f(220.0f, 700.0f));
    rightPanel.setPosition({900.0f, 0.0f});
    rightPanel.setFillColor(sf::Color(30, 30, 40));
    window.draw(rightPanel);

    sf::RectangleShape divider(sf::Vector2f(2.0f, 700.0f));
    divider.setPosition({900.0f, 0.0f});
    divider.setFillColor(sf::Color(70, 70, 90));
    window.draw(divider);

    window.draw(resText);
}