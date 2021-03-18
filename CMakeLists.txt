cmake_minimum_required(VERSION 2.8.12)
project(journald-bot)

add_definitions("-std=c++11")

add_subdirectory(lib/nlohmann_json)
add_subdirectory(lib/curlcpp)
include_directories(${CURLCPP_SOURCE_DIR}/include)

add_executable(main src/journald-bot.cpp)
target_link_libraries(main PRIVATE nlohmann_json::nlohmann_json curlcpp)

