cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable/Disable output of compile commands during generation.")

project(test_weact_h743 VERSION 1.0.0)

find_package(GTest REQUIRED)
enable_testing()

include_directories(
    ../src
)

add_executable(led_test)
target_sources(led_test PRIVATE
    led.cpp
)
target_link_libraries(led_test PRIVATE
    GTest::gtest_main
    GTest::gmock
)
add_test(NAME led_test COMMAND led_test)

add_executable(button_test)
target_sources(button_test PRIVATE
    button.cpp
)
target_link_libraries(button_test PRIVATE
    GTest::gtest_main
)
add_test(NAME button_test COMMAND button_test)
