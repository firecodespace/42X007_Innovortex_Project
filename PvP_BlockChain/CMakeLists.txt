cmake_minimum_required(VERSION 3.10)
project(PvP_BlockChain)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)
file(GLOB SOURCES "src/*.cpp")

add_executable(PvP_BlockChain ${SOURCES})

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)
find_package(CURL REQUIRED)

# Pull nlohmann/json using FetchContent if not available
include(FetchContent)
FetchContent_Declare(
	json
	URL https://github.com/nlohmann/json/releases/download/v3.11.2/nlohmann-json-3.11.2.tar.gz
)
FetchContent_MakeAvailable(json)

target_link_libraries(PvP_BlockChain PRIVATE sfml-graphics sfml-window sfml-system CURL::libcurl nlohmann_json::nlohmann_json)