#pragma once

#include <SFML/Graphics.hpp>
#include "../game2/BaseGame2.hpp"
#include "../pga2d.hpp"
#include "PingPongGameView.hpp"

#include <algorithm> //std::clamp std::max std::min
#include <cmath> //std::sqrt, std::ceil, std::acos, std::atan2, std::sin, std::cos, std::abs
#include <memory> //std::shared_ptr std::unique_ptr std::make_shared std::make_unique
#include <optional> //std::optional<PongVec2>
#include <stdexcept> //std::runtime_error std::invalid_argument
#include <string>//строки в исключениях
#include <vector> //для списка виджетов

class BallEntity final { //отдельный класс мяча final-без наследования нам оно не нужно
public:
    Multivector motor; //представление положения мяча
    PongVec2 pos{}; //текущее положение
    PongVec2 prevPos{}; // предыдущее положение вообщн это нужно чтобы знать откуда мяч двигался например падал вниз или летел вверх
    PongVec2 vel{}; //скорость мяча
    float radius = 11.0f; //радиус
    float visualAngle = 0.0f;//угол вращения (визуального)
    float visualSpin = 0.0f;//скорость вращения (визуального)

    void syncMotor() {//чтобы мотор не отставал от пос
        motor = makeTranslator(pos.x, pos.y);
    }

    void launch(float x, float y) { //для функции старт и рестарт чтобы задать начальную скорость и положение мяча
        pos = {x, y};
        prevPos = pos;
        vel = {5.6f, 6.1f};
        visualAngle = 0.0f;
        visualSpin = 0.0f;
        syncMotor();
    }

    void park(float x, float y) { //состояние ожидания старта, почти как launch но скорость нулевая
        pos = {x, y};
        prevPos = pos;
        vel = {0.0f, 0.0f};
        visualAngle = 0.0f;
        visualSpin = 0.0f;
        syncMotor();
    }

    void step(float dt60) { //жизнь мяча
        prevPos = pos; //сохраняет старое положение перед шагом
        pos += vel * dt60; //(сколько мяч должен пройти за этот шаг времени) 60 потому что в мейне ограничение до 60 кадров в секнду
        visualAngle += visualSpin * dt60; //под каким углом рисовать мяч
        visualSpin *= std::clamp(1.0f - 0.045f * dt60, 0.0f, 1.0f); //как быстро этот угол должен менятся
        //clamp - защита значения от и до; с каждым шагом скорость спина меньше
        syncMotor();
    }
};

class PingPongGame : public BaseGame { //наследование
private:
    static constexpr unsigned WINDOW_W = 1120; //размер окна
    static constexpr unsigned WINDOW_H = 700;
    static constexpr float HUD_W = 220.0f; //ширина правой панели
    static constexpr float PLAY_W = WINDOW_W - HUD_W; //ширина игровой части

    static constexpr float WALL_PAD = 22.0f; // мяч отбивается не от краев а от этих отступов право-лево
    static constexpr float TOP_PAD = 28.0f; //отступ сверху
    static constexpr float L1 = 120.0f;// длина костей
    static constexpr float L2 = 100.0f;
    static constexpr float RACKET_HALF = 56.0f; //половина длины ракетки
    static constexpr float RACKET_THICKNESS = 20.0f; //тощина ракетки
    static constexpr float BASE_Y = WINDOW_H - 46.0f;
    static constexpr float COLLISION_RADIUS = 11.0f + RACKET_THICKNESS * 0.78f; //радиус столкновения мяча с ракеткой (вклад толщины ракетки)

    //геометрические хелпер функции
    static float dot(const PongVec2& a, const PongVec2& b) { //скалярное произведение нужно для длины ,проекций ,ближ точки и смещения попадания
        return a.x * b.x + a.y * b.y;
    }

    static float lengthSq(const PongVec2& v) { //для сравнение квадратов длин
        return dot(v, v);
    }

    static float length(const PongVec2& v) {
        return std::sqrt(lengthSq(v));
    }

    static PongVec2 normalize(const PongVec2& v) { //нормализация
        const float len = length(v);
        return (len < 1e-6f) ? PongVec2{0.0f, 0.0f} : v / len; //если длина оооочень маленькая то возвращаем нулевой вектор
        // а сравниваем не с 0 чтобы не ловить нестабильность изза ооочень маленького числа
        //взято из логики пга2д
    }

    static PongVec2 pointFromMultivector(const Multivector& p) {//чтобы работать не с мультивектором а с обычными х у
        float x = 0.0f;
        float y = 0.0f;
        getPoint(p, x, y);
        return {x, y};
    }

    static PongVec2 closestPointOnSegment(const PongVec2& a, const PongVec2& b, const PongVec2& p) {//ключ столкновения мяча с ракеткой
        const PongVec2 ab = b - a; //вектор вдоль ракетки
        const float abLenSq = lengthSq(ab); //квадрат длины ракетки const потому что не меняем а просто сравниваем
        if (abLenSq < 1e-6f) {return a;} //проверка если как то оказалось что длина ракетки 0 для избежения ошибок
        const float t = std::clamp(dot(p - a, ab) / abLenSq, 0.0f, 1.0f); // проекция точки p  на ракетку и ограничиваем ее длиной ракетки
        return a + ab * t; // возвращаем ближайщую точку на отрезке
    }

    static float clampAbsMin(float v, float minAbs) {//чтобы скорость мяча не была слишком медленной
        if (std::abs(v) >= minAbs) {
            return v;
        }
        return (v < 0.0f ? -minAbs : minAbs);
    }
//конец хелпер функций
    static bool solveIJOlya(float targetX,// функция почти как в робо арм только здесь проверяет на достижимость цели и если она достижима то в ссылки кладет углы
                            float targetY,
                            float len1,
                            float len2,
                            float& t1,
                            float& t2,
                            bool elbowUp = true) {
        const float d2 = targetX * targetX + targetY * targetY;
        const float d = std::sqrt(d2);
        if (d > len1 + len2 || d < std::abs(len1 - len2)) { //слишком адлеко или слишком близко, что рука так не может сложится
            return false;
        }

        const float cosTheta2 = std::clamp(
            (d2 - len1 * len1 - len2 * len2) / (2.0f * len1 * len2),
            -1.0f,
            1.0f
        );

        t2 = elbowUp ? std::acos(cosTheta2) : -std::acos(cosTheta2);

        const float alpha = std::atan2(targetY, targetX);
        const float beta = std::atan2(len2 * std::sin(t2), len1 + len2 * std::cos(t2));
        t1 = alpha - beta;
        return true;
    }

    sf::Clock clock; // для измерения времени между кадрами

    float baseX = PLAY_W * 0.5f; //горизонтальная позиция базы
    float theta1 = -1.2f;//угол первого звена
    float theta2 = 1.15f;//угол второго звена
    float racketTilt = 0.0f;//доп наклон ракетки

    std::optional<PongVec2> racketCenterPrev; //предыдущее положение центра ракетки, опшинал потому что в первом кадре предыдущего положения нет
    ScreenState screen = WaitingStartState{}; //состояние экрана через вариант

    std::shared_ptr<PixelBitmapFont> bitmapFont; //шрифт деоится между игрой ии виджитами
    std::unique_ptr<BallEntity> ball; //игра единсвенный "владелец" мяча

    std::unique_ptr<RestartButtonWidget> restartButton;
    std::vector<std::unique_ptr<UiWidget>> hudWidgets; //уникальное владение элементами интерефеса  пользователя

    int score = 0; //счет
    float paddleHitCooldown = 0.0f;//защита от многократкного столкновения на соседних шагах

    bool prevR = false;
    bool prevSpace = false;
    bool prevMouseLeft = false;
    //хранят состояние на предыдущем кадре чтобы лоавить нажатие клавиши а не ее удержание

    void setupScene() { //подготовка к начальному состоянию
        score = 0;
        screen = WaitingStartState{};
        baseX = PLAY_W * 0.5f;
        theta1 = -1.2f;
        theta2 = 1.15f;
        racketTilt = 0.0f;
        ball->park(PLAY_W * 0.5f, 155.0f);
        racketCenterPrev.reset();
        paddleHitCooldown = 0.0f;
        if (restartButton) {
            restartButton->setPressed(false);
        }
    }

    void startRound() { //запускает раунд
        score = 0;
        screen = PlayingState{};
        ball->launch(PLAY_W * 0.5f, 155.0f);
        paddleHitCooldown = 0.0f;
    }

    void restartNow() {// сначала полноная подготовка а потом сразу запуск после рестарта
        setupScene();
        screen = PlayingState{};
        ball->launch(PLAY_W * 0.5f, 155.0f);
    }

public:
    PingPongGame() try
        : bitmapFont(std::make_shared<PixelBitmapFont>()), //создается общий шрифт
          ball(std::make_unique<BallEntity>()) { //создается шрифт
        restartButton = std::make_unique<RestartButtonWidget>( //создается кнопка рестарт (прямоугольник и передаем шрифт)
            sf::FloatRect({PLAY_W + 24.0f, 508.0f}, {HUD_W - 48.0f, 58.0f}),
            bitmapFont
        );

        hudWidgets.push_back(std::make_unique<SmileyWidget>( //виджет смайлика
            PLAY_W + HUD_W * 0.5f - 25.0f,
            427.0f,
            5.0f,
            bitmapFont
        ));

        setupScene();
    } catch (const std::invalid_argument& ex) {// если invalid_argument в виджетах или шрифт
        throw std::runtime_error(std::string("PingPongGame initialization failed: ") + ex.what());
    }

    void update(sf::RenderWindow& window) override { // грубо говоря вся реализация игру тут, то есть именно здесь мы испольуем все то что прописывали выше
        const float dt = clock.restart().asSeconds();
        const float dt60 = std::clamp(dt * 60.0f, 0.35f, 2.0f);

        const bool rNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::R); //обработка R
        if (rNow && !prevR) {
            restartNow();
        }
        prevR = rNow;

        const bool spaceNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Space);//обработка пробела
        if (std::holds_alternative<WaitingStartState>(screen) && spaceNow && !prevSpace) { //раунд запускается только если у нас режим ожидания и нажали пробел
            startRound();
        }
        prevSpace = spaceNow;

        const bool mouseLeftNow = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left); //нажата ли левая кнопка мыши
        const sf::Vector2i mousePos = sf::Mouse::getPosition(window); //координаты мыши int
        const sf::Vector2f mousePosF(static_cast<float>(mousePos.x), static_cast<float>(mousePos.y)); //координаты мыши float для виджетов

        if (restartButton) {//если кнопка сущ обновляем внутри кнопки курсор или нет
            restartButton->setHovered(restartButton->contains(mousePosF));

            if (mouseLeftNow && !prevMouseLeft && restartButton->contains(mousePosF)) {// если ЛКМ нажали внутри кнопки то кнопка работает
                restartButton->setPressed(true);
            }

            if (!mouseLeftNow && prevMouseLeft) {
                if (restartButton->contains(mousePosF)) {
                    restartNow();
                }
                restartButton->setPressed(false);
            }
        }
        prevMouseLeft = mouseLeftNow; //сохр сост мыши

        paddleHitCooldown = std::max(0.0f, paddleHitCooldown - dt60); //постепенно уменьшается до 0

        const float desiredBase = std::clamp(static_cast<float>(mousePos.x), 100.0f, PLAY_W - 100.0f);
        baseX += (desiredBase - baseX) * 0.24f * std::min(dt60, 1.0f); //чтобы база не телепортировалась а плавно следовала за мышью

        const float targetX = std::clamp(static_cast<float>(mousePos.x), WALL_PAD + 32.0f, PLAY_W - WALL_PAD - 32.0f);//не слишком близко к стенам
        const float targetY = std::clamp(static_cast<float>(mousePos.y), TOP_PAD + 30.0f, BASE_Y - 50.0f);//не ниже базы

        const bool elbowUp = targetX >= baseX; // выбор позации локтя, то есть куда он согнется в зависимости от цели
        if (!solveIJOlya(targetX - baseX, targetY - BASE_Y, L1, L2, theta1, theta2, elbowUp)) {
            const float clampedX = std::clamp(targetX - baseX, -(L1 + L2 - 10.0f), L1 + L2 - 10.0f);
            const float clampedY = std::clamp(targetY - BASE_Y, -(L1 + L2 - 10.0f), 0.0f);
            theta1 = std::atan2(clampedY, clampedX);
            theta2 = 0.0f;//выпрямляется
        }
//построение руки с ракеткой(платформой) через пга
        const Multivector origin = makePoint(0.0f, 0.0f);
        const Multivector mBase = makeTranslator(baseX, BASE_Y);
        const Multivector mJoint1 = mBase * makeRotor(theta1);
        const Multivector mJoint2 = mJoint1 * makeTranslator(L1, 0.0f) * makeRotor(theta2);
        const Multivector mEnd = mJoint2 * makeTranslator(L2, 0.0f);

        const PongVec2 endPoint = pointFromMultivector(applyMotor(mEnd, origin));
        const Multivector racketMotor = makeTranslator(endPoint.x, endPoint.y) * makeRotor(racketTilt);

        const PongVec2 racketA = pointFromMultivector(applyMotor(racketMotor, makePoint(-RACKET_HALF, 0.0f)));
        const PongVec2 racketB = pointFromMultivector(applyMotor(racketMotor, makePoint( RACKET_HALF, 0.0f)));
        const PongVec2 racketCenter = (racketA + racketB) * 0.5f;
//скорость ракетки
        if (!racketCenterPrev.has_value()) {
            racketCenterPrev = racketCenter;
        }
        const PongVec2 racketVelocity = (racketCenter - *racketCenterPrev) / std::max(dt60, 0.001f);
        racketCenterPrev = racketCenter;

        if (std::holds_alternative<PlayingState>(screen)) {
            const int substeps = std::clamp(static_cast<int>(std::ceil(length(ball->vel) * dt60 / 2.5f)), 2, 10); //если мяч слишком быстрый чтобы он не пропустил стену или ракетку
            const float subDt = dt60 / static_cast<float>(substeps);

            for (int i = 0; i < substeps && std::holds_alternative<PlayingState>(screen); ++i) { //на каждом подшаге проверка на движение мяча, на стены, на ракетку, на game over
                ball->step(subDt);//движение мяча
                PongVec2 v = ball->vel;

                if (ball->pos.x - ball->radius < WALL_PAD) {//стены например если зашел за границу возвращаем его и меняем знак линейной скорости
                    ball->pos.x = WALL_PAD + ball->radius;//это конкретно левая
                    v.x = std::abs(v.x);
                }
                if (ball->pos.x + ball->radius > PLAY_W - WALL_PAD) {//правая
                    ball->pos.x = PLAY_W - WALL_PAD - ball->radius;
                    v.x = -std::abs(v.x);
                }
                if (ball->pos.y - ball->radius < TOP_PAD) {//верхняя
                    ball->pos.y = TOP_PAD + ball->radius;
                    v.y = std::abs(v.y);
                }
                ball->vel = v;

                const PongVec2 closest = closestPointOnSegment(racketA, racketB, ball->pos);
                const PongVec2 delta = ball->pos - closest;
                const float d2 = lengthSq(delta);
                const bool downward = v.y > 0.0f;
                const bool fromAbove = ball->prevPos.y <= std::max(racketA.y, racketB.y) + COLLISION_RADIUS + 4.0f;

                if (downward && fromAbove && d2 <= COLLISION_RADIUS * COLLISION_RADIUS && paddleHitCooldown <= 0.0f) { //условие столкновения
                    PongVec2 tangent = normalize(racketB - racketA);
                    if (lengthSq(tangent) < 1e-6f) {
                        tangent = {1.0f, 0.0f};
                    }

                    PongVec2 upNormal = {-tangent.y, tangent.x};
                    if (upNormal.y > 0.0f) {
                        upNormal = upNormal * -1.0f;
                    }
                    if (dot(v, upNormal) > -0.02f) {
                        upNormal = upNormal * -1.0f;
                    }
                    if (upNormal.y > -0.08f) {
                        upNormal.y = -0.08f;
                        upNormal = normalize(upNormal);
                    }

                    const float hitOffset = std::clamp(dot(ball->pos - racketCenter, tangent) / RACKET_HALF, -1.0f, 1.0f);// угол отскока зависит от положения попадания по ракетке
                    const float tangentBoost = hitOffset * 1.15f + racketVelocity.x * 0.07f + racketTilt * 0.65f;

                    PongVec2 outDir = normalize(upNormal * 1.25f + tangent * tangentBoost);//итоговое направление вылета
                    if (outDir.y > -0.25f) {//гарантируем что мяч точно полетит вверх
                        outDir.y = -0.25f;
                        outDir = normalize(outDir);
                    }

                    const float speed = std::clamp(length(v) + 0.24f, 7.2f, 12.8f);//скорость после удара, берем старую и чуть чуть увеличиваем
                    PongVec2 bounced = outDir * speed;//новая скорость
                    bounced.x = clampAbsMin(bounced.x, 1.55f);//х не слишком маленький
                    bounced.y = -std::max(std::abs(bounced.y), 3.3f);//у вверх и не слишкком маленький
                    //после удара мяч чуть выше ракетки чтобы он не застрял внутри нее
                    const float dist = std::sqrt(std::max(d2, 1e-6f));
                    ball->pos = closest + upNormal * (COLLISION_RADIUS + 1.8f + std::max(0.0f, COLLISION_RADIUS - dist));
                    //запись результатов удара
                    ball->vel = bounced;
                    ball->visualSpin = std::clamp(hitOffset * 0.28f + racketTilt * 0.55f, -0.45f, 0.45f);
                    paddleHitCooldown = 6.0f;
                    ++score;
                }

                if (length(ball->vel) < 6.8f) {//стабилизация скорости
                    ball->vel = normalize(ball->vel) * 6.8f;
                }
                ball->vel.x = clampAbsMin(ball->vel.x, 1.25f);//страховка для горизонтальной скорости
                if (std::abs(ball->vel.y) < 2.6f) { //страховка для вертикальной скорости
                    ball->vel.y = (ball->vel.y < 0.0f ? -1.0f : 1.0f) * 2.6f;
                }

                if (ball->pos.y - ball->radius > WINDOW_H) { // проверка на проигрыш
                    screen = GameOverState{score};
                }

                ball->syncMotor(); //синхронизация мотора после подшага
            }
        }
    }

    void draw(sf::RenderWindow& window) override {
        const Multivector origin = makePoint(0.0f, 0.0f);
        const Multivector mBase = makeTranslator(baseX, BASE_Y);
        const Multivector mJoint1 = mBase * makeRotor(theta1);
        const Multivector mJoint2 = mJoint1 * makeTranslator(L1, 0.0f) * makeRotor(theta2);
        const Multivector mEnd = mJoint2 * makeTranslator(L2, 0.0f);
        // извлечение точек для рендера
        const PongVec2 base = pointFromMultivector(applyMotor(mBase, origin));
        const PongVec2 joint = pointFromMultivector(applyMotor(mJoint1 * makeTranslator(L1, 0.0f), origin));
        const PongVec2 end = pointFromMultivector(applyMotor(mEnd, origin));

        PongRenderSnapshot snapshot;//в эту структуру складывается все для видимого слоя, то есть как все наши данные будут отображаться на экране
        //заполнение данных:
        snapshot.playW = PLAY_W;
        snapshot.hudW = HUD_W;
        snapshot.windowH = static_cast<float>(WINDOW_H);
        snapshot.wallPad = WALL_PAD;
        snapshot.topPad = TOP_PAD;
        snapshot.baseY = BASE_Y;
        snapshot.racketHalf = RACKET_HALF;
        snapshot.racketThickness = RACKET_THICKNESS;
        snapshot.base = base;
        snapshot.joint = joint;
        snapshot.end = end;
        snapshot.ballPos = ball->pos;
        snapshot.ballRadius = ball->radius;
        snapshot.ballVisualAngle = ball->visualAngle;
        snapshot.score = score;
        snapshot.screen = screen;
        snapshot.font = bitmapFont;
        snapshot.restartButton = restartButton.get();
        snapshot.hudWidgets = &hudWidgets;

        PingPongGameView::draw(window, snapshot);//наш файл собирает всю инфу а view рисует на экране
    }

    [[nodiscard]] GameResult getResult() const override {//nodiscard - необходимо учитывать
        return score;// очки
    }
};