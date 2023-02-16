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

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include(${CMAKE_CURRENT_SOURCE_DIR}/external/stk-4.6.2/CMakeLists.txt)

include_directories(
	external/glfw-3.3.8/include
	external/glm-0.9.9.8/
	external/glew-2.2.0/include
	external/stk-4.6.2/
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
	src/UserInterface.cpp
	src/UserInterface.hpp
	src/Adapter.hpp
	src/Adapter.cpp

	src/Graphic/ReactionDiffusionSimulator.hpp
	src/Graphic/ReactionDiffusionSimulator.cpp
	src/Graphic/Shader.cpp
	src/Graphic/Shader.hpp
	src/Graphic/ComputeShader.cpp
	src/Graphic/ComputeShader.hpp
	src/Graphic/Object.hpp
	src/Graphic/Object.cpp
	src/Graphic/Texture.cpp
	src/Graphic/Texture.hpp
	src/Graphic/InputParameter.cpp
	src/Graphic/InputParameter.hpp

	src/Audio/AudioPlayer.hpp
	src/Audio/AudioPlayer.cpp
	src/Audio/AudioAnalyzer.hpp
	src/Audio/AudioAnalyzer.cpp
	src/Audio/SoundGroup.hpp
	src/Audio/SoundGroup.cpp

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

	# STK
	src/vendor/stk/WvIn.h
	src/vendor/stk/Stk.h
	src/vendor/stk/Stk.cpp
	src/vendor/stk/FileWvIn.h
	src/vendor/stk/FileWvIn.cpp
	src/vendor/stk/RtAudio.h
	src/vendor/stk/RtAudio.cpp
	src/vendor/stk/FileRead.h
	src/vendor/stk/FileRead.cpp

	# KISSFFT
	external/kissfft/kiss_fftr.h
	external/kissfft/kiss_fftr.c
	external/kissfft/kiss_fft.h
	external/kissfft/kiss_fft.c

)
target_link_libraries(Rythmic-diffusion
	${ALL_LIBS}
)
