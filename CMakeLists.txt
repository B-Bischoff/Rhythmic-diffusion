cmake_minimum_required(VERSION 3.0)
IF (WIN32)
	set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
ENDIF()
project(Rythmic-diffusion)

find_package(OpenGL REQUIRED)

set (CMAKE_CXX_STANDARD 17)

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

include(${CMAKE_CURRENT_SOURCE_DIR}/external/stk-4.6.2/CMakeLists.txt)

include_directories(
	external/glfw-3.3.8/include
	external/glm-0.9.9.8/
	external/glew-2.2.0/include
	external/stk-4.6.2/
	external/cereal/include
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
	src/Application.hpp
	src/Application.cpp
	src/Adapter.hpp
	src/Adapter.cpp
	src/Preset.hpp
	src/Preset.cpp

	src/UserInterface/UserInterface.hpp
	src/UserInterface/UserInterface.cpp
	src/UserInterface/AudioPlayerUI.hpp
	src/UserInterface/AudioPlayerUI.cpp
	src/UserInterface/HooksUI.hpp
	src/UserInterface/HooksUI.cpp
	src/UserInterface/InitialConditionsUI.hpp
	src/UserInterface/InitialConditionsUI.cpp
	src/UserInterface/PresetUI.hpp
	src/UserInterface/PresetUI.cpp
	src/UserInterface/RDOptionsUI.hpp
	src/UserInterface/RDOptionsUI.cpp
	src/UserInterface/GradientUI.hpp
	src/UserInterface/GradientUI.cpp

	src/Graphic/ReactionDiffusion/ReactionDiffusionSimulator.hpp
	src/Graphic/ReactionDiffusion/ReactionDiffusionSimulator.cpp
	src/Graphic/ReactionDiffusion/InputParameter.hpp
	src/Graphic/ReactionDiffusion/InputParameter.cpp
	src/Graphic/ReactionDiffusion/InitialConditions.hpp
	src/Graphic/ReactionDiffusion/InitialConditions.cpp
	src/Graphic/ReactionDiffusion/PostProcessing.hpp
	src/Graphic/ReactionDiffusion/PostProcessing.cpp

	src/Graphic/OpenGlHelper/Shader.hpp
	src/Graphic/OpenGlHelper/Shader.cpp
	src/Graphic/OpenGlHelper/ComputeShader.hpp
	src/Graphic/OpenGlHelper/ComputeShader.cpp
	src/Graphic/OpenGlHelper/Object.hpp
	src/Graphic/OpenGlHelper/Object.cpp
	src/Graphic/OpenGlHelper/Texture.hpp
	src/Graphic/OpenGlHelper/Texture.cpp

	src/Audio/AudioPlayer.hpp
	src/Audio/AudioPlayer.cpp
	src/Audio/AudioAnalyzer.hpp
	src/Audio/AudioAnalyzer.cpp
	src/Audio/SoundGroup.hpp
	src/Audio/SoundGroup.cpp

	# ------------ EXTERNAL LIBS ------------

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
	src/vendor/imgui/imfilebrowser.h
	src/vendor/imgui/imgui_color_gradient.h
	src/vendor/imgui/imgui_color_gradient.cpp

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
