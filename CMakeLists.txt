cmake_minimum_required(VERSION 3.26)
project(opengl-minimal CXX C)

list(
  APPEND
  SHADERS
  "${CMAKE_CURRENT_SOURCE_DIR}/shader/texture_vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/shader/texture_fragment.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/shader/triangle_vertex.shader"
  "${CMAKE_CURRENT_SOURCE_DIR}/shader/triangle_fragment.shader")
list(
  APPEND
  IMAGES
  "${CMAKE_CURRENT_SOURCE_DIR}/img/pixel_ship_yellow.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/img/pixel_laser_yellow.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/img/background-black.png"
  "${CMAKE_CURRENT_SOURCE_DIR}/img/pixel_ship_green_big.png")
list(APPEND SOUNDS "${CMAKE_CURRENT_SOURCE_DIR}/sound/main.wav"
     "${CMAKE_CURRENT_SOURCE_DIR}/sound/E1.wav"
     "${CMAKE_CURRENT_SOURCE_DIR}/sound/laser.wav")

file(COPY ${SOUNDS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/sound")
file(COPY ${SHADERS} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/shader")
file(COPY ${IMAGES} DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/img")
file(COPY "wave_list.json" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")

add_library(engine SHARED engine.cxx engine.hxx glad/glad.c glad/glad.h
                          glad/KHR/khrplatform.h)
target_compile_features(engine PUBLIC cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

target_link_libraries(engine PRIVATE SDL3::SDL3-shared OpenGL::GL glm::glm)

add_executable(
  game
  game.cxx
  player.cxx
  player.hxx
  bullet.cxx
  bullet.hxx
  enemy.hxx
  enemy.cxx
  nlohmann/json.hpp)
target_compile_features(game PUBLIC cxx_std_20)
target_link_libraries(game PRIVATE engine)
