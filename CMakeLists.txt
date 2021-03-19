include(FetchContent)

cmake_minimum_required(VERSION 2.8.12)
project(journald-bot)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_definitions("-std=c++11")

add_subdirectory(lib/nlohmann_json)

set(ENV{BUILD_CPR_TESTS} OFF)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG e4bca4bada2301f58b3f5310218889bdede7ecf9) # use snapshot because we need callbacks API
FetchContent_MakeAvailable(cpr)

add_executable(main src/journald-bot.cpp)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json cpr::cpr)

