cmake_minimum_required (VERSION 2.8.8)
project (cpp-smpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -fPIC")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wno-variadic-macros") # warnings as errors

# Set a default build type if none was specified (we are not using an IDE)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to 'Debug' as none was specified.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build." FORCE)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Add src before libs to make use it don't use installed headers
include_directories(src)

# ASIO lib
find_package(Asio)
include_directories(${ASIO_INCLUDE_DIR})

# Find Boost library
set(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.41 COMPONENTS date_time REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Google flags
find_library(GFLAGS_LIB gflags REQUIRED)
find_path(GFLAGS_INCLUDE gflags.h PATH_SUFFIXES gflags)
include_directories(${GFLAGS_INCLUDE})

# Google log
find_library(GLOG_LIB glog REQUIRED)
find_path(GLOG_INCLUDE logging.h PATH_SUFFIXES glog)
include_directories(${GLOG_INCLUDE})

set(link_libs
	${Boost_LIBRARIES}
	${GLOG_LIB}
	${GFLAGS_LIB}
)

add_subdirectory (src)

option (ENABLE_TEST "Compile unit testing" ON)

if (ENABLE_TEST)
  enable_testing()
  find_package(GTest)
  include_directories(${GTEST_INCLUDE_DIR})
  add_subdirectory (test)
endif (ENABLE_TEST)
