# Rhythmic-diffusion

The goal of this project is to generate visuals based on music rhythm.  
It can be decomposed in two parts:

- The graphic part, which is based on the Gray-Scott reaction-diffusion model is responsible for creating those atypical visuals.  
- The audio part extracts frequency informations from the music and uses it to control the reaction-difusion settings in real-time.

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

To install the required dependencies, open a terminal and type: 
```sudo apt install libsoil-dev libglm-dev libglew-dev libglfw3-dev libxinerama-dev libxcursor-dev libxi-dev libfreetype-dev libgl1-mesa-dev xorg-dev freeglut3-dev libasound2-dev```

To install, build and start the application, you simply have to clone this project and execute the ```start.sh``` file.  
This will create a build directory, compile the source code and launch the newly created executable.


### MacOs
Unfortunately, the use of OpenGl has been deprecated by MacOs, making it impossible (without re-writing parts of the program) to run the application on this system.

## Usage

### Reaction diffusion

reaction diffusion
gray scott model
4 parameters
multiple patterns
noises

The Gray-Scott reaction diffusion model is used to simulate the evolution of two chemicals on a 2D grid, reacting and diffusing over time.  
To visualize what is happening, a color gradient is applied to each pixel, depending on their chemicals B concentration.  
At the beginning of the application the whole grid is filled with chemicals A (shown in a dark blue color), except for the circle in the middle of the screen (in white) which is constantly adding new chemicals B to the simulation, every new frame.

The equation is based on those four parameters wich can be tweaked with the user interface in the "Reaction diffusion" tab:

-  diffusion rate A
-  diffusion rate B
-  feed rate
-  kill rate

It may be a lot to start with. What I advise is to set diffusion rate A to 1.0, the diffusion rate B to 0.5 and simply play with the feed and kill rate to understand how the changes affects the simulation.
 
Even tough those four values can create a variety of patterns, things can be pushed further with the use of noises.  

Initially, every parameter type is set to "number", applying the same value to the whole simulation.  
This type can be changed to "voronoi" or "perlin noise" which are more complex types, allowing value change depending on its position on the screen.  

Noises have the following properties: 
-  Base value : the minimum value applied to the whole simulation (exactly the same thing than the "number" type allowed to do).
-  Strength factor : the value added when the area is in the white area of the noise.
-  Scale : the lower the scale, the bigger the noise pattern will be.
-  Offset : two values to move the along the X and Y axis.
-  Scale moving & Time multiplier : if the scale moving is ticked, the scale will change over time. The speed can be controlled with the time multiplier value.

### Audio player
To play audio in the application, you will need to select a WAV file using the filebrowser (located in the audio player section).
There are many sites to convert mp3 (or other) audio file to WAV. I would recommand [convertio](https://convertio.co/mp3-wav/).

If during file playback, the audio becomes absolute noises, pause the playback a few seconds and resume it.

The graphic displayed, shows the real-time audio frequencies going from 0 to 8000 Hertz.

### Hooks
Hooks are used to link audio informations with reaction diffusion properties or even shapes.

The three things a hook needs are: 

-  an input
-  an output
-  an operation

The input will come from the audio. The three available inputs are:

-  the bass : sensitive to low frequency sound (from 0 to 80 Hertz roughly)
-  the snare : sensitive to medium sound with spreaded out frequencies (from 2000 to 8000 Hertz roughly)
-  the lead : any sound that has a high variation and that is not included by the bass or snare

It is really hard to make a perfect audio detection algorithm that works for any kind of music.  
This one was conceived mostly for techno music, but it was tested with a large variety of music (going from french music to post rock / metal)

The output is what is going to be affected by the audio variations.  
It can be a reaction diffusion propertie or even a shape.

The operation describes how the output will be affected. 

// talking about ratios
Under the hood, the three inputs (bass, snare and lead) are ratios going from 0 to 1.  
If the 

// notes
hooks have :
- input (bass, snare, lead)
- output (rd prop, shape prop)
- operation (a mode, an initial value, an operation value)
