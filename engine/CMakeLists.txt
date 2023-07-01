cmake_minimum_required(VERSION 3.16)
project(engine CXX C)

file(GLOB IMGUI_SRC ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.cpp)
file(GLOB IMGUI_INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/imgui-src/*.h)

set(ENGINE_SRC src/engine.cxx)
set(ENGINE_INCLUDE include/engine.hxx include/picopng.hxx glad/glad.c
                   glad/glad.h glad/KHR/khrplatform.h nlohmann/json.hpp)

add_library(engine SHARED ${ENGINE_INCLUDE} ${ENGINE_SRC} ${IMGUI_INCLUDE}
                          ${IMGUI_SRC})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  target_compile_definitions(engine PUBLIC IMGUI_IMPL_OPENGL_ES3)
endif()

target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glad)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/glm)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/imgui-src)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/nlohmann)
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

target_compile_features(engine PUBLIC cxx_std_20)
target_include_directories(engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

if(${CMAKE_SYSTEM_NAME} STREQUAL "Android")
  target_link_libraries(engine PRIVATE SDL3::SDL3-shared android log EGL GLESv3)
else()
  find_package(SDL3 REQUIRED)
  find_package(OpenGL REQUIRED)
  find_package(glm REQUIRED)
  target_link_libraries(engine PRIVATE SDL3::SDL3-shared OpenGL::GL glm::glm)
endif()
