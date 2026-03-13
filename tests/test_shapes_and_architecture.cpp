#include <gtest/gtest.h>

#include "game2/shape2.hpp"
#include "game2/BaseGame2.hpp"

#include <cmath>
#include <memory>
#include <variant>
#include <vector>

namespace {
bool allPointsFinite(const std::vector<sf::Vector2f>& pts) {
    for (const auto& p : pts) {
        if (!std::isfinite(p.x) || !std::isfinite(p.y)) {
            return false;
        }
    }
    return true;
}

class DummyGame final : public BaseGame {
public:
    explicit DummyGame(GameResult result) : result_(result) {}

    void update(sf::RenderWindow&) override {}
    void draw(sf::RenderWindow&) override {}
    [[nodiscard]] GameResult getResult() const override { return result_; }

private:
    GameResult result_;
};
}

TEST(Shapes, CircleShapeProducesNonEmptyFinitePointSet) {
    CircleShapeTrace circle;
    const auto pts = circle.getPoints();

    EXPECT_FALSE(pts.empty());
    EXPECT_TRUE(allPointsFinite(pts));
    EXPECT_GE(pts.size(), 8u);
}

TEST(Shapes, SquareShapeProducesFinitePointSet) {
    SquareShapeTrace square;
    const auto pts = square.getPoints();

    EXPECT_GE(pts.size(), 4u);
    EXPECT_TRUE(allPointsFinite(pts));
}

TEST(Shapes, PolymorphicContainerStoresAndUsesDifferentShapes) {
    std::vector<std::unique_ptr<Shape>> shapes;
    shapes.push_back(std::make_unique<CircleShapeTrace>());
    shapes.push_back(std::make_unique<SquareShapeTrace>());

    ASSERT_EQ(shapes.size(), 2u);
    EXPECT_FALSE(shapes[0]->getPoints().empty());
    EXPECT_FALSE(shapes[1]->getPoints().empty());
}

TEST(GameArchitecture, GameResultVariantStoresIntFloatAndEmpty) {
    GameResult catchResult = 150;
    GameResult traceResult = 95.5f;
    GameResult emptyResult = std::monostate{};

    ASSERT_TRUE(std::holds_alternative<int>(catchResult));
    EXPECT_EQ(std::get<int>(catchResult), 150);

    ASSERT_TRUE(std::holds_alternative<float>(traceResult));
    EXPECT_FLOAT_EQ(std::get<float>(traceResult), 95.5f);

    EXPECT_TRUE(std::holds_alternative<std::monostate>(emptyResult));
}

TEST(GameArchitecture, BaseGamePolymorphismWorksThroughBasePointer) {
    std::unique_ptr<BaseGame> game = std::make_unique<DummyGame>(GameResult{42});

    ASSERT_TRUE(std::holds_alternative<int>(game->getResult()));
    EXPECT_EQ(std::get<int>(game->getResult()), 42);
}
