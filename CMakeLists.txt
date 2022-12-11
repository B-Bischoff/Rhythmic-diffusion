cmake_minimum_required(VERSION 3.0)
project(Rythmic-diffusion)

find_package(OpenGL REQUIRED)

set (CMAKE_CXX_STANDARD 11)

# Compile external dependencies
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
cmake_policy(SET CMP0026 OLD)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-fe2273")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	external/glfw-3.3.8/include
	external/glm-0.9.9.8/
	external/glew-2.2.0/include
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_220
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	GLEW_220
)

add_executable(Rythmic-diffusion
	src/main.cpp
	src/Application.cpp
	src/Application.hpp
	src/Shader.cpp
	src/Shader.hpp

	# STB IMAGE
	src/vendor/stb/stb_image.h

	# IMGUI
	src/vendor/imgui/imconfig.h
	src/vendor/imgui/imgui.h
	src/vendor/imgui/imgui.cpp
	src/vendor/imgui/imgui_draw.cpp
	src/vendor/imgui/imgui_impl_glfw.cpp
	src/vendor/imgui/imgui_impl_glfw.h
	src/vendor/imgui/imgui_impl_opengl3.cpp
	src/vendor/imgui/imgui_impl_opengl3.h
	src/vendor/imgui/imgui_impl_opengl3_loader.h
	src/vendor/imgui/imgui_internal.h
	src/vendor/imgui/imgui_tables.cpp
	src/vendor/imgui/imgui_widgets.cpp

)
target_link_libraries(Rythmic-diffusion
	${ALL_LIBS}
)
