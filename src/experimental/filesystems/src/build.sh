#!/bin/bash

g++ -c SimulatedDevice.cpp
g++ -c FileSystemExt2.cpp
g++ -c main.cpp

g++ main.o SimulatedDevice.o FileSystemExt2.o -o a.out
