cmake_minimum_required(VERSION 3.10)
project(robot_pga)

set(CMAKE_CXX_STANDARD 23)

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

add_executable(robot_pga main.cpp
        src/pga2d.cpp
        src/game1/game1_objects.cpp
        include/game1/game1_objects.hpp
        src/game1/CatchGame1.cpp
        include/game1/CatchGame1.hpp
        src/RobotArm.cpp
        include/RobotArm.hpp
        src/game2/BaseGame2.cpp
        include/game2/BaseGame2.hpp
        src/game2/shape2.cpp
        include/game2/shape2.hpp
        src/game2/traceG2.cpp
        include/game2/traceG2.hpp
        src/math_tests.cpp
        include/math_tests.hpp
        src/game3/PingPongGame.cpp
        include/game3/PingPongGame.hpp
        src/game3/PingPongGameView.cpp
        include/game3/PingPongGameView.hpp
)

target_link_libraries(robot_pga SFML::Graphics SFML::Window SFML::System)