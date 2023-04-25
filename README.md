# Rhythmic-diffusion

The goal of this project is to generate visuals according to music rythm.  
It can be decomposed in two parts:

- The graphic part, is based on the Gray-Scott reaction-diffusion model which is responsible for creating those atypical visuals.  
- The audio part, breaks down the audio informations to extract its frequencies, and spot variations in the song.

Those variations are then used to change the reaction-diffusion settings in realtime, to create a sensation of rythme in the visuals.

A documentation on how the project was made is currently beeing done and will soon arrive.

## Table of contents
1. [Downloads](#downloads-and-installation)
     1. [Windows](#windows)
     2. [Linux](#linux)
     3. [MacOs](#macos)
2. [Usage](#usage)
    1. [Reaction diffusion](#reaction-diffusion)
    2. [Audio player](#audio-player)
    3. [Hooks](#hooks)
    4. [Presets](#presets)
3. [Examples](#examples)
    1. [ARTIST NAME AND SONG](#to-fill)

## Downloads and installation

### Windows
[TO COMPLETE] release (executable or installer?) or open the cmake project in visual studio
### Linux
Make sure CMake, Git and GCC are installed by typing ```sudo apt install cmake git g++```

You may also need to install the required packages : 
```sudo apt install libsoil-dev libglm-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev libxi-dev libfreetype-dev libgl1-mesa-dev xorg-dev freeglut3-dev libasound2-dev```

To install, build and start the application, you simply have to clone this project and then execute the ```start.sh``` file.  
That file will create a build directory, compile the source code and launch the newly created executable.


### MacOs
Unfortunately, the use of OpenGl has been deprecated by MacOs, making impossible (without re-writing parts of the program) to run the application on this system.

## Usage

### Reaction diffusion

reaction diffusion
gray scott model
4 parameters
multiple patterns
noises

The Gray-Scott reaction diffusion model is used to simulate the evolution of two chemicals on a 2D grid, reacting and diffusing over time.  
The equation uses four parameters:

-  diffusion rate A
-  diffusion rate B
-  feed rate
-  kill rate
