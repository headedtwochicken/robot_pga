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
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)) {
        if (!rPressed) { printRes(); rPressed = true; }
    } else rPressed = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) {
        if (!sPressed) { randomShape(); sPressed = true; }
    } else sPressed = false;

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

void TraceGame::printRes() {
    if (!finished && !userTrace.empty()){
        finished = true;
        score = calculateAccuracy();
        resText.setString("RESULT:\n\nAccuracy:\n" + std::to_string(static_cast<int>(score)) + "%\n\nPress S for\n new shape");
    }
}

float TraceGame::calculateAccuracy() const {
    if (!currentShape || userTrace.empty()) return 0;

    auto shapePts = currentShape->getPoints();
    std::vector<sf::Vector2f> denseShape;

    for (size_t i = 0; i < shapePts.size() - 1; i++) {
        sf::Vector2f p1 = shapePts[i];
        sf::Vector2f p2 = shapePts[i+1];
        float dx = p2.x - p1.x;
        float dy = p2.y - p1.y;
        float segLen = std::sqrt(dx * dx + dy * dy);
        int steps = std::max(1, static_cast<int>(segLen / 5.0f));
        for (int j = 0; j < steps; j++) {
            auto t = static_cast<float>(j) / static_cast<float>(steps);
            denseShape.emplace_back(p1.x + t * (p2.x - p1.x), p1.y + t * (p2.y - p1.y));
        }
    }
    denseShape.push_back(shapePts.back());

    float totalPrecisionDist = 0;
    for (auto& p : userTrace) {
        float best = 999999;
        for (auto& s : denseShape) {
            float dx = p.x - s.x;
            float dy = p.y - s.y;
            float d = std::sqrt(dx * dx + dy * dy);
            if (d < best) best = d;
        }
        totalPrecisionDist += best;
    }
    float avgPrecision = totalPrecisionDist / static_cast<float>(userTrace.size());

    float totalCoverageDist = 0;
    for (auto& s : denseShape) {
        float best = 999999;
        for (auto& p : userTrace) {
            float dx = s.x - p.x;
            float dy = s.y - p.y;
            float d = std::sqrt(dx * dx + dy * dy);
            if (d < best) best = d;
        }
        totalCoverageDist += best;
    }
    float avgCoverage = totalCoverageDist / static_cast<float>(denseShape.size());

    float finalScoreDist = (avgPrecision + avgCoverage) / 2.0f;
    float maxDist = 40.0f;

    float accuracy = 100.0f * (1.0f - (finalScoreDist / maxDist));

    float traceLength = 0;
    for (size_t i = 1; i < userTrace.size(); i++) {
        float dx = userTrace[i].x - userTrace[i-1].x;
        float dy = userTrace[i].y - userTrace[i-1].y;
        traceLength += std::sqrt(dx*dx + dy*dy);
    }

    float idealLength = 0;
    for (size_t i = 1; i < denseShape.size(); i++) {
        float dx = denseShape[i].x - denseShape[i-1].x;
        float dy = denseShape[i].y - denseShape[i-1].y;
        idealLength += std::sqrt(dx*dx + dy*dy);
    }

    float inkRatio = traceLength / (idealLength + 0.001f);

    if (inkRatio > 1.5f) { accuracy -= (inkRatio - 1.5f) * 30.0f; }
    if (accuracy < 0) accuracy = 0;
    if (accuracy > 100) accuracy = 100;

    return accuracy;
}

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