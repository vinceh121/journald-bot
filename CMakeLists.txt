include(FetchContent)

cmake_minimum_required(VERSION 4.2)

project(journald-bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions("-std=c++20")

set(ENV{CPR_BUILD_TESTS} OFF)
set(ENV{CPR_FORCE_USE_SYSTEM_CURL} ON)

find_package(nlohmann_json)

find_package(PkgConfig REQUIRED) 
pkg_check_modules(CURLPP REQUIRED IMPORTED_TARGET curlpp)

# https://discourse.cmake.org/t/removing-a-bad-include-from-pkg-config-package/8760/4
get_property(__curlpp_includes TARGET PkgConfig::CURLPP PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
list(REMOVE_ITEM __curlpp_includes include)
set_property(TARGET PkgConfig::CURLPP PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${__curlpp_includes})

add_executable(journald-bot src/journald-bot.cpp)
target_link_libraries(journald-bot PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(journald-bot PRIVATE PkgConfig::CURLPP)

install(TARGETS journald-bot RUNTIME)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "vinceh121")
INCLUDE(CPack)

