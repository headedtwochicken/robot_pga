#include <SFML/Graphics.hpp>
#include "include/RobotArm.hpp"
#include "include/game1/CatchGame1.hpp"
#include "include/game2/traceG2.hpp"
#include "include/math_tests.hpp"
#include "include/game3/PingPongGame.hpp"

enum AppMode { FK_MODE, IK_MODE, PHYSICS_MODE, COLLISION_MODE, GAME_LOBBY, MINI_GAME_CATCH, MINI_GAME_TRACE, MINI_GAME_PINGPONG };

// === КЛАСС ДЛЯ КНОПОК ===
class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;

    Button(float x, float y, float w, float h, const std::string& str, const sf::Font& font, sf::Color bgColor = sf::Color(50, 50, 70))
        : text(font, str, 16)
    {
        rect.setPosition({x, y});
        rect.setSize({w, h});
        rect.setFillColor(bgColor);
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2);

        text.setFillColor(sf::Color::White);
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({bounds.position.x + bounds.size.x / 2.0f, bounds.position.y + bounds.size.y / 2.0f});
        text.setPosition({x + w / 2.0f, y + h / 2.0f});
    }

    void draw(sf::RenderWindow& window) const {
        window.draw(rect);
        window.draw(text);
    }

    bool isClicked(sf::Vector2i mousePos, bool mouseClicked) const {
        if (mouseClicked && rect.getGlobalBounds().contains(sf::Vector2f(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)))) {
            return true;
        }return false;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode({1120, 700}), "PGA Robot Engine - Pro UI");
    window.setFramerateLimit(60);

    RobotArm mainArm(400.0f, 300.0f, 120.0f, 100.0f);
    CatchGame catchGameMini;
    TraceGame traceGameMini;
    PingPongGame pingPongGame;

    AppMode currentMode = FK_MODE;

    sf::CircleShape targetPoint(6);
    targetPoint.setFillColor(sf::Color::Transparent);
    targetPoint.setOutlineColor(sf::Color::Magenta);
    targetPoint.setOutlineThickness(2);
    targetPoint.setOrigin({6, 6});

    sf::Font font;
    bool fontLoaded = false;
    try {
        if (!font.openFromFile("/System/Library/Fonts/Helvetica.ttc") &&
            !font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
            throw std::runtime_error("Failed to load fonts!");
        } fontLoaded = true;
    } catch (const std::exception& e) {
        printf("EXCEPTION: %s\n", e.what());
    }

    sf::Text hintText(font);
    if (fontLoaded) {
        hintText.setCharacterSize(16);
        hintText.setFillColor(sf::Color(200, 255, 200));
        hintText.setPosition({10.0f, 10.0f});
    }

    // === ПРАВАЯ ПАНЕЛЬ И КНОПКИ ===
    float panelX = 860.0f;
    Button btnFK(panelX + 20, 100, 200, 50, "1: FK Mode", font);
    Button btnIK(panelX + 20, 170, 200, 50, "2: IK Mode", font);
    Button btnPhys(panelX + 20, 240, 200, 50, "3: Physics", font);
    Button btnColl(panelX + 20, 310, 200, 50, "4: Collision", font);
    Button btnGames(panelX + 20, 450, 200, 60, "PLAY GAMES", font, sf::Color(100, 50, 150));

    // === ЛОББИ: КНОПКИ ПО ЦЕНТРУ ===
    float lobbyBtnX = (1120.0f - 400.0f) / 2.0f; // Идеальный центр для экрана 1120
    Button btnLobbyCatch(lobbyBtnX, 200, 400, 60, "Play Catch Game (Catcher)", font, sf::Color(50, 150, 50));
    Button btnLobbyTrace(lobbyBtnX, 280, 400, 60, "Play Trace Game (Drawing)", font, sf::Color(50, 100, 150));
    Button btnLobbyPingPong(lobbyBtnX, 360, 400, 60, "Play Ping Pong (PGA Physics)", font, sf::Color(150, 100, 50));
    Button btnLobbyBack(1120.0f / 2.0f - 100.0f, 480, 200, 50, "Back to Sandbox", font, sf::Color(150, 50, 50));

    Button btnMenu(1000, 10, 100, 40, "MENU", font, sf::Color(100, 100, 100));

    while (window.isOpen()) {
        bool mouseClicked = false;

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();
            if (event->is<sf::Event::MouseButtonPressed>()) {
                if (event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left) {
                    mouseClicked = true;
                }
            }

            if (event->is<sf::Event::KeyPressed>()) {
                auto keyCode = event->getIf<sf::Event::KeyPressed>()->code;
                if (keyCode == sf::Keyboard::Key::T) runMathTests();
                if (keyCode == sf::Keyboard::Key::I) {
                    auto printResult = [](const GameResult& res, const std::string& name) {
                        std::visit([&name]<typename T0>(T0&& arg) {
                            using T = std::decay_t<T0>;
                            if constexpr (std::is_same_v<T, std::monostate>) printf("%s: Нет результатов\n", name.c_str());
                            else if constexpr (std::is_same_v<T, int>) printf("%s: Очки: %d\n", name.c_str(), arg);
                            else if constexpr (std::is_same_v<T, float>) printf("%s: Точность: %.2f%%\n", name.c_str(), arg);
                        }, res);
                    };
                    printf("\n--- СТАТИСТИКА ---\n");
                    printResult(catchGameMini.getResult(), "Catcher");
                    printResult(traceGameMini.getResult(), "Tracing");
                    printResult(pingPongGame.getResult(), "Ping Pong");
                }
            }
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        // === ОБРАБОТКА НАЖАТИЙ ===
        if (currentMode == GAME_LOBBY) {
            if (btnLobbyCatch.isClicked(mousePos, mouseClicked)) { currentMode = MINI_GAME_CATCH; mainArm.resetVelocities(); }
            if (btnLobbyTrace.isClicked(mousePos, mouseClicked)) { currentMode = MINI_GAME_TRACE; mainArm.resetVelocities(); }
            if (btnLobbyPingPong.isClicked(mousePos, mouseClicked)) { currentMode = MINI_GAME_PINGPONG; mainArm.resetVelocities(); }
            if (btnLobbyBack.isClicked(mousePos, mouseClicked))  { currentMode = FK_MODE; }
        }
        else if (currentMode == MINI_GAME_CATCH || currentMode == MINI_GAME_TRACE || currentMode == MINI_GAME_PINGPONG) {
            if (btnMenu.isClicked(mousePos, mouseClicked)) { currentMode = GAME_LOBBY; }
        }
        else {
            if (btnFK.isClicked(mousePos, mouseClicked))   { currentMode = FK_MODE; mainArm.resetVelocities(); }
            if (btnIK.isClicked(mousePos, mouseClicked))   { currentMode = IK_MODE; mainArm.resetVelocities(); }
            if (btnPhys.isClicked(mousePos, mouseClicked)) { currentMode = PHYSICS_MODE; mainArm.resetVelocities(); }
            if (btnColl.isClicked(mousePos, mouseClicked)) { currentMode = COLLISION_MODE; mainArm.resetVelocities(); }
            if (btnGames.isClicked(mousePos, mouseClicked)){ currentMode = GAME_LOBBY; }
            bool isMouseInSafeZone = (static_cast<float>(mousePos.x) < panelX);

            float dx = 0.0f, dy = 0.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Left))  dx = -2.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Right)) dx = 2.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Up))    dy = -2.0f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Down))  dy = 2.0f;
            mainArm.moveBase(dx, dy);

            if (currentMode == COLLISION_MODE) mainArm.clampBaseY(400.0f);

            if (currentMode == FK_MODE) {
                float dTheta1 = 0, dTheta2 = 0;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A)) dTheta1 = -0.05f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D)) dTheta1 = 0.05f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W)) dTheta2 = -0.05f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S)) dTheta2 = 0.05f;
                mainArm.addAngles(dTheta1, dTheta2);
            }
            else if (currentMode == IK_MODE && isMouseInSafeZone) {
                auto ikRes = mainArm.solveIK(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y));
                targetPoint.setOutlineColor(ikRes.has_value() ? sf::Color::Magenta : sf::Color::Red);
                targetPoint.setPosition({static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)});
            }
            else if (currentMode == PHYSICS_MODE || currentMode == COLLISION_MODE) {
                bool checkCols = (currentMode == COLLISION_MODE);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && isMouseInSafeZone) {
                    auto targetY = static_cast<float>(mousePos.y);
                    if (checkCols && targetY > 400.0f) targetY = 400.0f;
                    [[maybe_unused]] auto ikRes = mainArm.solveIK(static_cast<float>(mousePos.x), targetY);
                    mainArm.resetVelocities();
                } else mainArm.updatePhysics(checkCols, 400.0f);
            }
        }

        // === ОТРИСОВКА ===
        window.clear(sf::Color(20, 20, 25));

        if (currentMode == GAME_LOBBY) {
            sf::Text title(font, "SELECT MINI-GAME", 36);
            title.setPosition({ lobbyBtnX + 30, 100 });
            window.draw(title);

            btnLobbyCatch.draw(window);
            btnLobbyTrace.draw(window);
            btnLobbyPingPong.draw(window);
            btnLobbyBack.draw(window);
        }
        else if (currentMode == MINI_GAME_CATCH) {
            catchGameMini.update(window);
            catchGameMini.draw(window);
            btnMenu.draw(window);
        }
        else if (currentMode == MINI_GAME_TRACE) {
            traceGameMini.update(window);
            traceGameMini.draw(window);
            btnMenu.draw(window);
        }
        else if (currentMode == MINI_GAME_PINGPONG) {
            pingPongGame.update(window);
            pingPongGame.draw(window);
            btnMenu.draw(window);
        }
        else {
            if (currentMode == COLLISION_MODE) {
                sf::RectangleShape floorShape(sf::Vector2f(1120.0f, 300.0f));
                floorShape.setPosition({0.0f, 400.0f});
                floorShape.setFillColor(sf::Color(100, 100, 100, 150));
                window.draw(floorShape);
            }

            mainArm.draw(window);
            if (currentMode == IK_MODE) window.draw(targetPoint);

            if (fontLoaded) {
                std::string hints = "Press T to run Unit Tests | Press I for Stats\n";
                if (currentMode == FK_MODE) hints += "Mode 1 [FK]: Control joints with W/S, A/D";
                else if (currentMode == IK_MODE) hints += "Mode 2 [IK]: Arm follows mouse cursor";
                else if (currentMode == PHYSICS_MODE) hints += "Mode 3 [Physics]: Free fall | Hold LClick to grab";
                else hints += "Mode 4 [Collisions]: Floor bounce | Hold LClick to grab";
                hintText.setString(hints);
                window.draw(hintText);
            }

            // === ПРАВАЯ ПАНЕЛЬ ===
            sf::RectangleShape rightPanel(sf::Vector2f(1120.0f - panelX, 700.0f));
            rightPanel.setPosition({panelX, 0.0f});
            rightPanel.setFillColor(sf::Color(30, 30, 40));
            window.draw(rightPanel);

            // Линия-разделитель
            sf::RectangleShape divider(sf::Vector2f(2.0f, 700.0f));
            divider.setPosition({panelX, 0.0f});
            divider.setFillColor(sf::Color(70, 70, 90));
            window.draw(divider);
            btnFK.draw(window);
            btnIK.draw(window);
            btnPhys.draw(window);
            btnColl.draw(window);
            btnGames.draw(window);
        }
        window.display();
    } return 0;
}