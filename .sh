#!/bin/bash

if [ ! -d "build"  ];then
     mkdir build
  
  fi
cd build
cmake -D CMAKE_BUILD_TYPE=Debug   ..
make
