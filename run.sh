#!/bin/bash

cmake -S . -B build
cd build && make

if [[ $# > 0 && "$1" == "t" ]]
then 
   ./commandprocessortest --gtest_catch_exceptions=0
else
    ./commandprocessor
fi

cd ..