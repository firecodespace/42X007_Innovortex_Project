cmake_minimum_required(VERSION 3.21)
project(GladiatorSFML LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# -------------------------
# SFML (2.6.1, shared)
# -------------------------
set(BUILD_SHARED_LIBS ON CACHE BOOL "" FORCE)

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

# Point consumers to the fetched SFML package config
# Visual Studio places it under: <build>/_deps/sfml-build
set(SFML_DIR "${CMAKE_BINARY_DIR}/_deps/sfml-build" CACHE PATH "" FORCE)
# Also add to prefix path in case a consumer does find_package(SFML)
set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};${CMAKE_BINARY_DIR}/_deps/sfml-build")

# -------------------------
# Dear ImGui + ImGui-SFML
# -------------------------

# Dear ImGui
FetchContent_Declare(
  ImGui
  GIT_REPOSITORY https://github.com/ocornut/imgui.git
  GIT_TAG v1.91.0
)
FetchContent_MakeAvailable(ImGui)
# Tell ImGui-SFML where Dear ImGui sources are
set(IMGUI_DIR "${imgui_SOURCE_DIR}" CACHE PATH "" FORCE)

# ImGui-SFML binding
# We already provide SFML; disable its internal "find SFML on system"
set(IMGUI_SFML_FIND_SFML OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  ImGui-SFML
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
  GIT_TAG v2.6
)
FetchContent_MakeAvailable(ImGui-SFML)

# -------------------------
# Executable
# -------------------------
add_executable(gladiator
  src/main.cpp
)

find_package(Threads REQUIRED)

target_link_libraries(gladiator
  PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    ImGui-SFML::ImGui-SFML
    Threads::Threads
)

# Copy runtime DLLs next to the exe (Windows)
if (WIN32)
  add_custom_command(TARGET gladiator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      $<TARGET_FILE:sfml-graphics>
      $<TARGET_FILE:sfml-window>
      $<TARGET_FILE:sfml-system>
      $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
      $<TARGET_FILE_DIR:gladiator>)
endif()

