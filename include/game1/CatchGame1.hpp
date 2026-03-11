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



public:
    CatchGame() :
    arm(std::make_unique<RobotArm>(450.0f, 400.0f, 120.0f, 100.0f)),
    spawnTimer(0.0f),
    score(0),
    missed(0),
    gameOver(false){}
