cmake_minimum_required(VERSION 3.18)
project(GladiatorArena LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLADIATOR_SOURCES
    Animation.cpp
    Arena.cpp
    BackgroundVideo.cpp
    BattleLog.cpp
    BridgeConfig.cpp
    Fighter.cpp
    Game.cpp
    GamePlayState.cpp
    json_util.cpp
    MainMenuState.cpp
    MapSelectionState.cpp
    PauseGameState.cpp
    PvP_BlockChain.cpp
    ResourcePaths.cpp
    State.cpp
    StoreState.cpp
    Wallets.cpp
    http_client.cpp
)

add_executable(PvP_BlockChain ${GLADIATOR_SOURCES})
target_include_directories(PvP_BlockChain PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

set(GLADIATOR_LOCAL_SFML_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/../GladiatorSFML/out/build/x64-debug/_deps/sfml-build"
)
if(MSVC AND EXISTS "${GLADIATOR_LOCAL_SFML_DIR}/SFMLConfig.cmake")
    set(SFML_DIR "${GLADIATOR_LOCAL_SFML_DIR}" CACHE PATH "Local SFML config")
endif()

find_package(SFML 2.5 COMPONENTS graphics window system audio QUIET)
if(NOT SFML_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        sfml
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
    )
    FetchContent_MakeAvailable(sfml)
endif()

target_link_libraries(
    PvP_BlockChain
    PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        winhttp
)
