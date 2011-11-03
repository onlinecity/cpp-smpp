cmake_minimum_required (VERSION 2.6)
project (cpp-smpp) 

set(CMAKE_CXX_FLAGS "-O2 -Wall -pedantic -Wno-long-long")

add_subdirectory (src) 

ENABLE_TESTING()
add_subdirectory (test)
