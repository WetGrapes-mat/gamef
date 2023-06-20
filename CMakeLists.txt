cmake_minimum_required(VERSION 3.26)
project(opengl-minimal CXX C)

file(COPY ${CMAKE_SOURCE_DIR}/shader DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "wave_list.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
file(COPY "config_game.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

file(GLOB IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.cpp)
file(GLOB IMGUI_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.h)

add_library(
  engine SHARED
  engine.cxx
  engine.hxx
  glad/glad.c
  glad/glad.h
  glad/KHR/khrplatform.h
  ${IMGUI_SRC}
  ${IMGUI_INCLUDE})
target_compile_features(engine PUBLIC cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(engine PRIVATE SDL3::SDL3-shared OpenGL::GL glm::glm)

add_executable(
  game
  main.cxx
  game.cxx
  game.hxx
  player.cxx
  player.hxx
  bullet.cxx
  bullet.hxx
  enemy.hxx
  enemy.cxx
  nlohmann/json.hpp)
target_compile_features(game PUBLIC cxx_std_20)
target_link_libraries(game PRIVATE engine)
