cmake_minimum_required (VERSION 2.6)
project (cpp-smpp)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -pedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long -Wno-variadic-macros") # warnings as errors

# Find Boost library
set(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost 1.41 COMPONENTS date_time system filesystem regex REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Find LibEvent
find_path(EVENT_INCLUDE_DIR event.h)
include_directories(${EVENT_INCLUDE_DIR})
find_library(EVENT_LIB event)

# Find Thrift
find_path(THRIFT_INCLUDE_DIR Thrift.h PATH_SUFFIXES thrift)
include_directories(${THRIFT_INCLUDE_DIR})
find_library(THRIFT_LIB thrift)
find_library(THRIFT_NB_LIB thriftnb)

# Find oclog
find_path(OCLOG_INCLUDE_DIR Log.h PATH_SUFFIXES oclog)
include_directories(${OCLOG_INCLUDE_DIR})

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
	${GFLAGS}
)

include_directories(src)
add_subdirectory (src)

option (ENABLE_TEST "Compile unit testing and CTest target" OFF)

if (ENABLE_TEST)
	ENABLE_TESTING()
	add_subdirectory (test)
endif (ENABLE_TEST)
