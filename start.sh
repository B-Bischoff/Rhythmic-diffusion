#!/bin/bash

make -C build/;
if [ "$?" == 0 ]
then
	echo "-------------------- COMPILATION END --------------------";
	./build/Rythmic-diffusion
else
	echo "COMPILATION FAILED"
fi
