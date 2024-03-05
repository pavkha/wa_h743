cmake_minimum_required(VERSION 3.20)

get_filename_component(CROSS_TOOLCHAIN_PATH "../arm-gnu-toolchain-stm32-11.3-2023.05/bin" ABSOLUTE)
set(ENV{PATH} "${CROSS_TOOLCHAIN_PATH}:$ENV{PATH}")

set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/cmake/toolchains/toolchain-none-eabi-arm-generic.cmake")
message(STATUS "Cmake toolchain: ${CMAKE_TOOLCHAIN_FILE}")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cxx-common-opts)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Enable/Disable output of compile commands during generation.")

project(weact_h743 VERSION 6.2.0)

configure_file(
    "${CMAKE_SOURCE_DIR}/src/version.in"
    "${CMAKE_SOURCE_DIR}/src/version.h")

find_package(Libopencm3 REQUIRED)

add_subdirectory(src)
add_subdirectory(lib)
