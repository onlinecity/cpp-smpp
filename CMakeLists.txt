cmake_minimum_required (VERSION 2.6)
project (cpp-smpp) 

set(CMAKE_CXX_FLAGS "-O2 -Wall -pedantic -Wno-long-long -std=c++0x")

add_subdirectory (src) 

option (ENABLE_TEST "Compile unit testing and CTest target" OFF)

if (ENABLE_TEST)
	ENABLE_TESTING()
	add_subdirectory (test)
endif (ENABLE_TEST)
