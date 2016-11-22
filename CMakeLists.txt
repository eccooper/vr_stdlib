cmake_minimum_required(VERSION 3.5)
project(vrstd C)

include_directories(include)

file(GLOB_RECURSE SOURCES "src/*.c")

add_library(vrstd STATIC ${SOURCES})
