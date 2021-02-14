#!/bin/bash

if [ ! -d "build"  ];then
     mkdir build
  
  fi
cd build
make clean
cmake -D CMAKE_BUILD_TYPE=Debug   ..
make
