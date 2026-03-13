#include <gtest/gtest.h>

#include "include/game1/game1_objects.hpp"
#include "include/game1/CatchGame1.hpp"

#include <cmath>
#include <optional>


TEST(FallingObject, UpdateMovesObjectDownWhenNotCaught) {// проверка на правильное падение вниз мячиков
    FallingObject obj(100.0f, 50.0f, 12.0f);

    const float oldY = obj.getPosition().y;
    obj.update(0.5f);

    EXPECT_GT(obj.getPosition().y, oldY);
}

TEST(FallingObject, CheckCatchReturnsTrueWhenObjectTouchesCatcher) { // проверка на успешную поимку обьекта
    FallingObject obj(100.0f, 100.0f, 12.0f);

    const bool caught = obj.checkCatch(100.0f, 100.0f, 18.0f);

    EXPECT_TRUE(caught);
    EXPECT_TRUE(obj.isCaught());
}

TEST(FallingObject, CheckCatchReturnsFalseWhenObjectIsFarAway) {// проверка на ложные поимки
    FallingObject obj(100.0f, 100.0f, 12.0f);

    const bool caught = obj.checkCatch(300.0f, 300.0f, 18.0f);

    EXPECT_FALSE(caught);
    EXPECT_FALSE(obj.isCaught());
}

TEST(FallingObject, IsOffScreenReturnsTrueOnlyAfterLeavingScreen) {//проверка на правильность ухода обьекта за экран
    FallingObject obj(100.0f, 50.0f, 12.0f);

    EXPECT_FALSE(obj.isOffScreen(700.0f));

    obj.setPosition(100.0f, 800.0f);
    EXPECT_TRUE(obj.isOffScreen(700.0f));
}

TEST(FallingObject, CaughtObjectStaysCaught) {// проверка что обьект не ловитс повторно
    FallingObject obj(100.0f, 100.0f, 12.0f);

    EXPECT_TRUE(obj.checkCatch(100.0f, 100.0f, 18.0f));
    EXPECT_TRUE(obj.isCaught());
    EXPECT_FALSE(obj.checkCatch(100.0f, 100.0f, 18.0f));
}

TEST(Catcher, HasPositiveRadius) {
    Catcher catcher;

    EXPECT_GT(catcher.getRadius(), 0.0f);
}

TEST(CatchGame, StartsWithZeroScore) { //проверка на корректную инициализацию CacthGame
    CatchGame game;

    ASSERT_TRUE(std::holds_alternative<int>(game.getResult()));
    EXPECT_EQ(std::get<int>(game.getResult()), 0);
}
