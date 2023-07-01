cmake_minimum_required(VERSION 3.16)
project(space_game CXX C)

if(SDL3_SRC_DIR)
  add_subdirectory(
    # Specifies the directory of the CMakeLists.txt file.
    ${SDL3_SRC_DIR}
    # Specifies the directory for the build outputs.
    ${CMAKE_CURRENT_BINARY_DIR}/SDL_${ANDROID_ABI})
endif()

file(COPY ${CMAKE_SOURCE_DIR}/assets/sounds
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets/images
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/shader DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

file(COPY ${CMAKE_SOURCE_DIR}/assets/wave_list.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets/config_game.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_subdirectory(engine)

set(GAME_SRC src/game.cxx src/player.cxx src/bullet.cxx src/enemy.cxx
             src/main.cxx)
set(GAME_INCLUDE include/player.hxx include/bullet.hxx include/enemy.hxx
                 include/ientity.hxx)

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  add_library(game SHARED ${GAME_SRC} ${GAME_INCLUDE})
else()
  add_executable(game ${GAME_SRC} ${GAME_INCLUDE})
endif()

target_compile_features(game PUBLIC cxx_std_20)
target_link_libraries(game PRIVATE engine)
target_include_directories(game PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
