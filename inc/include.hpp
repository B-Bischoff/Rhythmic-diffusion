#pragma once

#ifdef WIN32
#define PATH_PREFIX "../../../"
#else
#define PATH_PREFIX "./"
#endif

// OPENGL LIBRARY
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// GLM
#include "../external/glm-0.9.9.8/glm/glm.hpp"
#include "../external/glm-0.9.9.8/glm/gtc/matrix_transform.hpp"

#include "../src/vendor/imgui/imgui.h"
#include "../src/vendor/imgui/imgui_impl_glfw.h"
#include "../src/vendor/imgui/imgui_impl_opengl3.h"
#include "../src/vendor/imgui/imfilebrowser.h"
#include "../src/vendor/imgui/imgui_color_gradient.h"

#include "../src/vendor/stb/stb_image.h"

#include "../external/kissfft/kiss_fftr.h"
#include "../external/kissfft/kiss_fft.h"

#include "../src/vendor/stk/FileWvIn.h"
#include "../src/vendor/stk/RtAudio.h"

#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/xml.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <list>
#include <exception>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <signal.h>
#include <filesystem>

#include <thread>
#include <mutex>

#include <chrono>
#include <ctime>
