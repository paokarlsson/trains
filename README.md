# Project: Train simulation

## Environment & Tools
This project is built in Visual Studio Community 2019 on a Windows 10 computer.

## Purpose
This is the final project in a C++ course in object oriented programming. 
The project is about simulating a train environment with, train stations, trains, and a time table. 

## Build
To build the project you need MVSC, Microsoft Visual Stutio Compiler. The simplest way to build it is to, after cloning the git project, open the CMakeLists.txt file from within Visual Studio. File -> Open -> CMake...
There are three input files that the program is dependent on, TrainMap, Trains and TrainStatings. These files are located in the subfolder trains-data and are referensed from the main.cpp file in the constructor of the app. Depending of your build setup, this path might need to be adjusted. 
Now it should work building and running the program Trains.exe. Enjoy!
