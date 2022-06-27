cmake_minimum_required(VERSION 3.8)
project(3D_Fractals)

include(Cmake/CPM.cmake)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Src")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Libs")

# Include files
set(INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Include")

# Config file
configure_file(${SRC_DIR}/helpers/RootDir.h.in ${SRC_DIR}/helpers/RootDir.h)

# IMGUI
CPMAddPackage(
  NAME IMGUI
  GIT_REPOSITORY "https://github.com/ocornut/imgui.git"
  GIT_TAG "v1.80"
  )

# Source files
file(GLOB SOURCES 
	${SRC_DIR}/*.cpp 
#${IMGUI_SOURCE_DIR}/*.cpp
	${IMGUI_SOURCE_DIR}/*.cpp
	${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp)

# Executable definitions and properties
add_executable(${PROJECT_NAME} ${SOURCES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 14)
if(NOT WIN32)
	target_link_libraries(${PROJECT_NAME} stdc++fs)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE "${INCLUDE_DIR}")
target_include_directories(${PROJECT_NAME} PRIVATE "${IMGUI_SOURCE_DIR}" "${IMGUI_SOURCE_DIR}/backends")

# GLFW
CPMAddPackage(
  NAME glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.2"
  OPTIONS
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_DOCS OFF"
    "GLFW_INSTALL OFF"
  )

#target_link_libraries(${PROJECT_NAME} "glfw" "${GLFW_LIBRARIES}")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLFW_SOURCE_DIR}/include")
target_compile_definitions(${PROJECT_NAME} PRIVATE "GLFW_INCLUDE_NONE")

# GLM
CPMAddPackage("https://github.com/g-truc/glm.git#0.9.9.8")

# GLAD
set(GLAD_DIR "${LIB_DIR}/glad")
add_library("glad" "${GLAD_DIR}/src/glad.c")
target_include_directories("glad" PRIVATE "${GLAD_DIR}/include")
target_include_directories(${PROJECT_NAME} PRIVATE "${GLAD_DIR}/include")
target_link_libraries(${PROJECT_NAME} "glad" "glfw" "glm::glm" "${CMAKE_DL_LIBS}")
