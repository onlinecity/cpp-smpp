set(unitheaders
	pdutest.h
	encodertest.h
	smstest.h
	timetest.h
)

set(liveheaders
    clienttest.h
	connectionsetting.h
)

SET(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE(Boost 1.41 COMPONENTS date_time system filesystem regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

FIND_LIBRARY(cppunitpath cppunit)

include_directories(${CMAKE_SOURCE_DIR}/src)

# oclog
FIND_PATH(OCLOG_INCLUDE_DIR Log.h PATH_SUFFIXES oclog)
INCLUDE_DIRECTORIES(${OCLOG_INCLUDE_DIR})
FIND_LIBRARY(OCLOG_LIB oclog)

# LibEvent
FIND_PATH(EVENT_INCLUDE_DIR event.h)
INCLUDE_DIRECTORIES(${EVENT_INCLUDE_DIR})

# Thrift
FIND_PATH(THRIFT_INCLUDE_DIR Thrift.h PATH_SUFFIXES thrift)
INCLUDE_DIRECTORIES(${THRIFT_INCLUDE_DIR})

add_executable(unittest ${unitheaders} testrun.cpp)
target_link_libraries(unittest ${cppunitpath} smpp ${Boost_LIBRARIES} oclog)
add_test(cpp-unit-test unittest)

add_executable(livetest ${liveheaders} livetest.cpp)
target_link_libraries(livetest ${cppunitpath} smpp ${Boost_LIBRARIES} oclog)
add_test(live-test livetest)
