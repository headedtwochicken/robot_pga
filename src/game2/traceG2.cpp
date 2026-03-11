#include "../../include/game2/traceG2.hpp"
#include <cstdlib>
#include <iostream>

TraceGame::TraceGame() : resText(font), arm(450.0f, 380.0f, 120.0f, 100.0f) {
    shapes.emplace_back(std::make_unique<CircleShapeTrace>());
    shapes.emplace_back(std::make_unique<SquareShapeTrace>());
    shapes.emplace_back(std::make_unique<TriangleShapeTrace>());
    shapes.emplace_back(std::make_unique<RectangularShapeTrace>());
    shapes.emplace_back(std::make_unique<OvalShapeTrace>());
    shapes.emplace_back(std::make_unique<StarShapeTrace>());

    if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc") &&
        !font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        throw std::runtime_error("Failed to load fonts!");
    }

    resText.setFont(font);
    resText.setCharacterSize(16);
    resText.setFillColor(sf::Color::White);
    resText.setPosition({915, 80});
    resText.setString("CONTROLS:\n\nLClick - Draw\nA/D - Move Base\n\nR - Result\nS - New Shape");

    targetPoint.setRadius(6);
    targetPoint.setFillColor(sf::Color::Transparent);
    targetPoint.setOutlineColor(sf::Color::Magenta);
    targetPoint.setOutlineThickness(2);
    targetPoint.setOrigin({6.0f, 6.0f});
}

void TraceGame::randomShape() {}
void TraceGame::printRes() {}
float TraceGame::calculateAccuracy() const { return 0.0f; }

void TraceGame::update(sf::RenderWindow& window) {
}

void TraceGame::draw(sf::RenderWindow& window) {
    window.clear(sf::Color(20, 20, 25));

    if (currentShape) currentShape->draw(window);

    arm.draw(window);
    window.draw(targetPoint);

    // === ПРАВАЯ ПАНЕЛЬ ИНТЕРФЕЙСА ===
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