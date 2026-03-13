cmake_minimum_required(VERSION 3.10)
project(robot_pga)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(SFML_DIR "C:/libs/SFML-3.0.2-vc17-64/SFML-3.0.2/lib/cmake/SFML")

find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

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
        src/game3/PingPongGame.cpp
        include/game3/PingPongGame.hpp
        src/game3/PingPongGameView.cpp
        include/game3/PingPongGameView.hpp
)


target_include_directories(robot_pga PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(robot_pga PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)

add_executable(robot_pga_tests
        tests/test_pga2d.cpp
        tests/test_robot_arm.cpp
        tests/test_shapes_and_architecture.cpp
        tests/test_pingpong_logic.cpp
        tests/test_pingpong_view_support.cpp
        tests/test_game1_logic.cpp

        src/pga2d.cpp
        src/RobotArm.cpp
        src/game2/BaseGame2.cpp
        src/game2/shape2.cpp
        src/game2/traceG2.cpp
        src/game3/PingPongGame.cpp
        src/game3/PingPongGameView.cpp
)

target_include_directories(robot_pga_tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(robot_pga_tests PRIVATE
        GTest::gtest_main
        SFML::Graphics
        SFML::Window
        SFML::System
)

include(GoogleTest)
gtest_discover_tests(robot_pga_tests)