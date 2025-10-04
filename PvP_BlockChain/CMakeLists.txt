cmake_minimum_required(VERSION 3.10)
project(PvP_BlockChain)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(PvP_BlockChain ${SOURCES})

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
target_link_libraries(PvP_BlockChain sfml-graphics sfml-window sfml-system)