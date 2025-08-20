cmake_minimum_required(VERSION 4.0.3)
set(CMAKE_MODULE_PATH "/Users/marselmacevans/Downloads/MusicSynth/build/_deps/eigen3-src/cmake")
project(CheckFortran Fortran)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/result.cmake"
  "set(CMAKE_Fortran_COMPILER \"${CMAKE_Fortran_COMPILER}\")\n"
  
  )