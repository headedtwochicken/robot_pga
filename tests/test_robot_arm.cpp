#include <gtest/gtest.h>
#include "RobotArm.hpp"

#include <cmath>

namespace {
constexpr float EPS = 1e-3f;

bool isFinite(float v) {
    return std::isfinite(v);
}
}

TEST(RobotArm, ConstructorProducesFiniteEndPoint) { // на корректный конец
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    EXPECT_TRUE(isFinite(arm.getEndX()));
    EXPECT_TRUE(isFinite(arm.getEndY()));
}

TEST(RobotArm, MoveBaseShiftsEndPointBySameOffset) { // проверяет движение
    RobotArm arm(10.0f, 20.0f, 100.0f, 100.0f);

    const float oldX = arm.getEndX();
    const float oldY = arm.getEndY();

    arm.moveBase(15.0f, -5.0f);

    EXPECT_NEAR(arm.getEndX(), oldX + 15.0f, EPS);
    EXPECT_NEAR(arm.getEndY(), oldY - 5.0f, EPS);
}

TEST(RobotArm, ClampBaseYMovesArmDownToFloorIfNeeded) { //проверка ограничения базы по высоты
    RobotArm arm(0.0f, 500.0f, 100.0f, 100.0f);

    arm.clampBaseY(400.0f);

    EXPECT_LE(arm.getBaseY(), 400.0f + EPS);
}

TEST(RobotArm, AddAnglesChangesEndPoint) {// проверка что изменение углов меняет положение руки
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    const float oldX = arm.getEndX();
    const float oldY = arm.getEndY();

    arm.addAngles(0.2f, -0.15f);

    const bool changed =
        std::abs(arm.getEndX() - oldX) > EPS ||
        std::abs(arm.getEndY() - oldY) > EPS;

    EXPECT_TRUE(changed);
}

TEST(RobotArm, SolveIKReturnsValueForCurrentEndPoint) {// проверка на достижение конечной текущей точки
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    const float targetX = arm.getEndX();
    const float targetY = arm.getEndY();

    const auto result = arm.solveIK(targetX, targetY);

    EXPECT_TRUE(result.has_value());
}

TEST(RobotArm, SolveIKRejectsClearlyUnreachableTarget) { // проверка недостижимой цели
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    const auto result = arm.solveIK(1000.0f, 1000.0f);

    EXPECT_FALSE(result.has_value());
}

TEST(RobotArm, UpdatePhysicsKeepsEndPointFinite) { // проверка что физика не ломает кинематику
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    arm.updatePhysics(false);

    EXPECT_TRUE(isFinite(arm.getEndX()));
    EXPECT_TRUE(isFinite(arm.getEndY()));
}

TEST(RobotArm, GrabWithMouseDoesNotBreakKinematics) { // проверка что мышь не ломает руку
    RobotArm arm(0.0f, 0.0f, 100.0f, 100.0f);

    arm.grabWithMouse(50.0f, 80.0f, false);
    arm.updatePhysics(false);

    EXPECT_TRUE(isFinite(arm.getEndX()));
    EXPECT_TRUE(isFinite(arm.getEndY()));
}
