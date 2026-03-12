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
        if (fontLoaded) {
            scoreText.setCharacterSize(22);
            scoreText.setFillColor(sf::Color::White);
            scoreText.setPosition({920.0f, 80.0f});

            missedText.setCharacterSize(22);
            missedText.setFillColor(sf::Color(255, 100, 100));
            missedText.setPosition({920.0f, 120.0f});

            instructionText.setCharacterSize(16);
            instructionText.setFillColor(sf::Color(200, 200, 200));
            instructionText.setPosition({920.0f, 200.0f});
            instructionText.setString("CONTROLS:\n\nMouse - Move Arm\nA/D - Move Base\n\nCatch the falling\n objects!");

            gameOverText.setCharacterSize(48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition({300.0f, 250.0f});
            gameOverText.setString("GAME OVER");

            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition({350.0f, 320.0f});
            restartText.setString("Press R to restart");
        }
    }

