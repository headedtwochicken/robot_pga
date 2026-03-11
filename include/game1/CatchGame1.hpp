#pragma once
#include <random>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include "game1_objects.hpp"
#include "../RobotArm.hpp"
#include "../game2/BaseGame2.hpp"

class CatchGame : public BaseGame {
private:
    std::unique_ptr<RobotArm> arm;
    Catcher catcher;
    std::vector<FallingObject> objects;
    float spawnTimer;
    const float SPAWN_INTERVAL = 0.9f;
    int score;
    int missed;
    const int MAX_MISSED = 5;
    bool gameOver;
    sf::Font font;
    sf::Text scoreText, missedText, gameOverText, restartText, instructionText;
    bool fontLoaded;
    std::mt19937 rng;
    std::uniform_real_distribution<float> xPosDist;


public:
    CatchGame() :
    arm(std::make_unique<RobotArm>(450.0f, 400.0f, 120.0f, 100.0f)),
    spawnTimer(0.0f),
    score(0),
    missed(0),
    gameOver(false),
    scoreText(font),
    missedText(font),
    gameOverText(font),
    restartText(font),
    instructionText(font),
    fontLoaded(false),
    rng(std::random_device{}()),
    xPosDist(50.0f, 850.0f)
    {
        try {
            if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc") &&
                !font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
                throw std::runtime_error("Failed to load fonts!");
                }
            fontLoaded = true;
        } catch (const std::exception& e) {
            printf("EXCEPTION: %s\n", e.what());
            fontLoaded = false;
        }

