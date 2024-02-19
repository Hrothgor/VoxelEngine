# Get CMake PreDefined Variables https://cmake.org/cmake/help/latest/manual/cmake-variables.7.html

cmake_minimum_required(VERSION 3.20.0 FATAL_ERROR)

project(voxel-engine VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "/O2")

add_compile_options(/fp:fast /fp:precise)

# Variables for Paths of External Libraries
set(GLFW_ROOT_DIR external/glfw)
set(GLM_ROOT_DIR external/glm)
set(FASTNOISE_ROOT_DIR external/FastNoise2)
set(IMGUI_ROOT_DIR external/imgui)

# Add all Source Files
file(GLOB_RECURSE SOURCE_FILES
	${CMAKE_SOURCE_DIR}/src/*.cpp

	${IMGUI_ROOT_DIR}/imgui.cpp
	${IMGUI_ROOT_DIR}/imgui_draw.cpp
	${IMGUI_ROOT_DIR}/imgui_widgets.cpp
	${IMGUI_ROOT_DIR}/imgui_tables.cpp
	${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.cpp
	${IMGUI_ROOT_DIR}/backends/imgui_impl_glfw.cpp
	${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl3.cpp
)

# Add all Header Files
file(GLOB_RECURSE HEADER_FILES 
	${CMAKE_SOURCE_DIR}/src/*.h
	${CMAKE_SOURCE_DIR}/src/*.hpp
	
	${IMGUI_ROOT_DIR}/imgui.h
	${IMGUI_ROOT_DIR}/imgui_internal.h
	${IMGUI_ROOT_DIR}/misc/cpp/imgui_stdlib.h
	${IMGUI_ROOT_DIR}/backends/imgui_impl_glfw.h
	${IMGUI_ROOT_DIR}/backends/imgui_impl_opengl3.h
)

# Add src/Include.hpp as -I
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${IMGUI_ROOT_DIR})
include_directories(${IMGUI_ROOT_DIR}/backends)


add_executable(${PROJECT_NAME} 
	${HEADER_FILES} ${SOURCE_FILES}
)

# Find the Required Packages (if Any -> OpenGL/Vulkan or Any)
find_package(OpenGL REQUIRED)

######################################
# GLAD CONFIGURATION
#
# Configure GLAD loader generation variables (lines below pulled from the GLAD top-level
# CMakeLists.txt file, with slight modifications.
# Refer to https://github.com/Dav1dde/glad for more information)
######################################

set(GLAD_OUT_DIR        "${CMAKE_CURRENT_BINARY_DIR}" CACHE STRING "Output directory" FORCE)
set(GLAD_PROFILE        "core" CACHE STRING "OpenGL profile" FORCE)
set(GLAD_API            "gl=4.6" CACHE STRING "API type/version pairs, like \"gl=3.2,gles=\", no version means latest" FORCE)
set(GLAD_GENERATOR      "c" CACHE STRING "Language to generate the binding for" FORCE)
set(GLAD_EXTENSIONS     "" CACHE STRING "Path to extensions file or comma separated list of extensions, if missing all extensions are included" FORCE)
set(GLAD_SPEC           "gl" CACHE STRING "Name of the spec" FORCE)
set(GLAD_ALL_EXTENSIONS OFF CACHE BOOL "Include all extensions instead of those specified by GLAD_EXTENSIONS" FORCE)
set(GLAD_NO_LOADER      OFF CACHE BOOL "No loader" FORCE)
set(GLAD_REPRODUCIBLE   OFF CACHE BOOL "Reproducible build" FORCE)

######################################

include(external/glad/LinkGLAD.cmake)
LinkGLAD(voxel-engine PRIVATE)

# Variable for the Libs to add to the Linkers
set(LIBS glfw opengl32 glm FastNoise)

# Add the External Libraries / Files Directory
add_subdirectory(${GLFW_ROOT_DIR})
add_subdirectory(${GLM_ROOT_DIR})
add_subdirectory(${FASTNOISE_ROOT_DIR})

######################################

# Add the Libs to the Linker for the Project
target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBS})


