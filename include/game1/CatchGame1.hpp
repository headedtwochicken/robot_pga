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
    std::vector<FallingObject> objects;//все шарики
    float spawnTimer;
    const float SPAWN_INTERVAL = 0.9f;// каждые 0.9 сек будет падать шарик
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
    xPosDist(50.0f, 850.0f)//задаем диапазон для x координат
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
            scoreText.setPosition({920.0f, 80.0f});//почти правый край и ближе к верхнему

            missedText.setCharacterSize(22);
            missedText.setFillColor(sf::Color(255, 100, 100));
            missedText.setPosition({920.0f, 120.0f});

            instructionText.setCharacterSize(16);
            instructionText.setFillColor(sf::Color(200, 200, 200));
            instructionText.setPosition({920.0f, 200.0f});
            instructionText.setString("CONTROLS:\n\nMouse - Move Arm\nA/D - Move Base\n\nCatch the falling\n objects!");

            gameOverText.setCharacterSize(48);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition({300.0f, 250.0f});//центр
            gameOverText.setString("GAME OVER");

            restartText.setCharacterSize(24);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition({350.0f, 320.0f});
            restartText.setString("Press R to restart");
        }
    }
  void update(sf::RenderWindow& window) override {
        if (gameOver && sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R)) {
            gameOver = false;
            score = 0;
            missed = 0;
            objects.clear();
            if (arm) arm->resetVelocities();// проверяем существует ли объект руки и обнуляем ее угловые скорости методом из RobotArm
        }

        if (!gameOver) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left)) {//если нажата А или этя стрелочка влево двигвем основу руки влево
                if (arm) arm->moveBase(-6.0f);//метод из RobotArm, он здвигает основу руки на указанное расстояние
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) {
                if (arm) arm->moveBase(6.0f);/тож самое про право
            }

            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            auto targetX = static_cast<float>(mousePos.x);
            auto targetY = static_cast<float>(mousePos.y);

            if (targetX > 880) targetX = 880;//ограничиваем тут по x, ниже по y
            if (targetY > 680) targetY = 680;
            if (targetY < 100) targetY = 100;

            auto ikRes = arm->solveIK(targetX, targetY);//targetX, targetY- координаты точки, до которой мы хотим дотянуться, основной прикол solveIK, я веду свою руку в  точку с этими координатами, а выводится парой через std::optional: выводятся угол поворота плеча и угол сгиба локтя
            if (!ikRes.has_value()) {}//если есть что-то в ikRes, то правда иначе не правда

            spawnTimer += 1.0f / 60.0f;//это секундомер, считаем скок времени прошло с появления посл шарика( тип все работает со скоростью 60 кадров в сек, значит 1/60 сек на кадр)
            if (spawnTimer >= SPAWN_INTERVAL) {// хотим шарик каждые 0,9 сек
                spawnTimer = 0.0f;
                float x = xPosDist(rng);// это от 50 до 850 диапазон по линии x, чтобы шарики могли спавниться в рандомных местах нашего поля
                objects.emplace_back(x, 30.0f, 14.0f);
            }

            for (auto& obj : objects) {
                obj.update(1.0f / 60.0f);//update из FallingObject( говорим шару обновить свое состояние c учетом, что прошла 1/60 сек( та йоу функция с dt))
                if (!obj.isCaught() && obj.checkCatch(arm->getEndX(), arm->getEndY(), catcher.getRadius())) score += 10;
                if (!obj.isCaught() && obj.isOffScreen(700.0f)) missed++;//isOffScreen(700.0f))- упал ниже экрана
            }

            std::erase_if(objects, [](const FallingObject& obj) {
                return obj.isCaught() || obj.isOffScreen(700.0f);
            });

            if (missed >= MAX_MISSED) gameOver = true;

            if (fontLoaded) {
                scoreText.setString("Score: " + std::to_string(score));
                missedText.setString("Missed: " + std::to_string(missed) + "/" + std::to_string(MAX_MISSED));
            }
        } else {
            if (arm) arm->updatePhysics(false);//она падает
        }
    }
    void draw(sf::RenderWindow& window) override {
        sf::RectangleShape sky(sf::Vector2f(900, 400));
        sky.setFillColor(sf::Color(70, 70, 100));//темно синий
        window.draw(sky);

        sf::RectangleShape floor(sf::Vector2f(900, 300));
        floor.setFillColor(sf::Color(100, 100, 120));
        floor.setPosition({0, 400});
        window.draw(floor);//нарисовали пол

        for (const auto& obj : objects) obj.draw(window);
        if (arm) arm->draw(window);
        catcher.draw(window, arm->getEndX(), arm->getEndY());//рисуем ловца на конце руки

        sf::RectangleShape rightPanel(sf::Vector2f(220.0f, 700.0f));
        rightPanel.setPosition({900.0f, 0.0f});
        rightPanel.setFillColor(sf::Color(30, 30, 40));
        window.draw(rightPanel);

        sf::RectangleShape divider(sf::Vector2f(2.0f, 700.0f));
        divider.setPosition({900.0f, 0.0f});
        divider.setFillColor(sf::Color(70, 70, 90));
        window.draw(divider);

        if (fontLoaded) {
            window.draw(scoreText);
            window.draw(missedText);
            window.draw(instructionText);
            if (gameOver) {
                window.draw(gameOverText);
                window.draw(restartText);
            }
        }
    }
    GameResult getResult() const override { return score; }
};

