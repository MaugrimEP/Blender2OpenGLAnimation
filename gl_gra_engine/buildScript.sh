#!/bin/bash
cd sfmlGraphicsPipeline
cd extlib
make -j6 # compile the external libraries
cd ../
mkdir build
cd build
cmake ../
# generate the Makefile using CMakeLists.txt
make -j6 # compile the framework (static library)
cd ../../
cd sampleProject
mkdir build
cd build
cmake ../ # generate the Makefile using CMakeLists.txt
make -j6 # compile the front program (executable)
./main
