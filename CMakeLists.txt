cmake_minimum_required(VERSION 3.26)
project(Three_Body LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

add_executable(Three_Body main.cpp)

find_package(OpenGL REQUIRED)
target_include_directories(Three_Body PUBLIC ${OPENGL_INCLUDE_DIR})
target_link_libraries(Three_Body PUBLIC ${OPENGL_LIBRARIES})
message(STATUS "OpenGL include directory: ${OPENGL_INCLUDE_DIRS}")
if (OpenGL_FOUND)
    message(STATUS "OpenGL found")
    message(STATUS "OpenGL include directory: ${OPENGL_INCLUDE_DIRS}")
else()
    message(STATUS "OpenGL not found")
endif()

find_package(glm REQUIRED)
target_include_directories(Three_Body PUBLIC ${GLM_INCLUDE_DIR})
target_link_libraries(Three_Body PUBLIC ${GLM_LIBRARIES})
message(STATUS "GLM include directory: ${GLM_INCLUDE_DIRS}")

find_package(glfw3 REQUIRED)
target_include_directories(Three_Body PUBLIC ${GLFW_INCLUDE_DIR})
target_link_libraries(Three_Body PUBLIC ${GLFW_LIBRARIES})
message(STATUS "GLM include directory: ${GLFM_INCLUDE_DIRS}")

target_link_libraries(Three_Body PUBLIC glfw)