#pragma once
#include "pga2d.hpp"
#include "RobotArm.hpp"
#include "game2/shape2.hpp"
#include "game2/BaseGame2.hpp"
#include <iostream>
#include <cmath>
#include <memory>
#include <vector>

inline void runMathTests() {
    std::cout << "\n=======================================================\n";
    std::cout << "  RUNNING ADVANCED ARCHITECTURE & MATH TESTS (C++17) \n";
    std::cout << "=======================================================\n";

    int passed = 0;
    int total = 0;

    auto printResult = [&passed, &total](bool condition, const std::string& testName) {
        total++;
        if (condition) {
            std::cout << "[ OK ] " << testName << "\n";
            passed++;
        } else {
            std::cout << "[FAIL] " << testName << " <--- ERROR!\n";
        }
    };

    // === БЛОК 1: ТЕСТЫ ГЕОМЕТРИЧЕСКОЙ АЛГЕБРЫ (PGA) ===
    Multivector p1 = makePoint(0.0f, 0.0f);
    Multivector t1 = makeTranslator(15.0f, -25.0f);
    Multivector res1 = applyMotor(t1, p1);
    float x1, y1; getPoint(res1, x1, y1);
    printResult(std::abs(x1 - 15.0f) < 0.001f && std::abs(y1 - (-25.0f)) < 0.001f,
                "Math: Translation Motor correctness");

    Multivector p2 = makePoint(10.0f, 0.0f);
    Multivector r1 = makeRotor(3.14159265f / 2.0f);
    Multivector res2 = applyMotor(r1, p2);
    float x2, y2; getPoint(res2, x2, y2);
    printResult(std::abs(x2 - 0.0f) < 0.001f && std::abs(y2 - 10.0f) < 0.001f,
                "Math: Rotor 90-degree transformation");

    // === БЛОК 2: ТЕСТЫ std::optional и ИНВЕРСНОЙ КИНЕМАТИКИ ===
    RobotArm testArm(0.0f, 0.0f, 100.0f, 100.0f);

    // Тест 2.1: Цель в зоне досягаемости (150px)
    auto ikSuccess = testArm.solveIK(150.0f, 0.0f);
    printResult(ikSuccess.has_value() == true,
                "std::optional: IK Solver handles reachable target");

    // Тест 2.2: Цель слишком далеко (300px при длине руки 200px)
    auto ikFail = testArm.solveIK(300.0f, 0.0f);
    printResult(ikFail.has_value() == false,
                "std::optional: IK Solver gracefully rejects unreachable target (nullopt)");

    // === БЛОК 3: ТЕСТЫ УМНЫХ УКАЗАТЕЛЕЙ (std::unique_ptr) И ПОЛИМОРФИЗМА ===
    {
        std::vector<std::unique_ptr<Shape>> testShapes;
        testShapes.push_back(std::make_unique<CircleShapeTrace>());
        testShapes.push_back(std::make_unique<SquareShapeTrace>());

        bool polySuccess = true;
        for (const auto& shape : testShapes) {
            if (shape->getPoints().empty()) polySuccess = false;
        }
        printResult(polySuccess && testShapes.size() == 2,
                    "Memory & OOP: std::unique_ptr safely manages polymorphic Shape objects");
    }

    // === БЛОК 4: ТЕСТЫ std::variant ===
    GameResult catchRes = 150;
    GameResult traceRes = 95.5f;
    GameResult emptyRes = std::monostate{};

    bool variantIntOk = std::holds_alternative<int>(catchRes) && std::get<int>(catchRes) == 150;
    bool variantFloatOk = std::holds_alternative<float>(traceRes);
    bool variantEmptyOk = std::holds_alternative<std::monostate>(emptyRes);

    printResult(variantIntOk && variantFloatOk && variantEmptyOk,
                "Modern C++: std::variant correctly stores multiple game result types");

    // === ИТОГИ ===
    std::cout << "-------------------------------------------------------\n";
    std::cout << "RESULT: Passed " << passed << " / " << total << " architecture tests.\n";
    if (passed == total) {
        std::cout << "STATUS: READY FOR DEFENSE (10/10) 😎\n";
    }
    std::cout << "=======================================================\n\n";
}