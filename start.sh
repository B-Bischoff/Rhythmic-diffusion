#!/bin/bash

BUILD_DIRECTORY="build";
PRESET_DIRECTORY="presets/";

# Generate Cmake file
if [ ! -d $BUILD_DIRECTORY ]
then
	mkdir $BUILD_DIRECTORY;
	cmake -S . -B $BUILD_DIRECTORY
fi

# Create application preset directory
if [ ! -d $PRESET_DIRECTORY ]
then
	mkdir $PRESET_DIRECTORY;
fi

# Compile and launch application
make -C $BUILD_DIRECTORY;
if [ "$?" == 0 ]
then
	echo "-------------------- COMPILATION END --------------------";
	./$BUILD_DIRECTORY/Rythmic-diffusion
else
	echo "-------------------- COMPILATION FAILED --------------------";
fi
