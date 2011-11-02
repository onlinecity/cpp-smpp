set(unitheaders
	pdutest.h
	encodertest.h
	smstest.h
	timetest.h
)

set(liveheaders
	connectionsetting.h
)

SET(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE(Boost 1.41 COMPONENTS date_time system filesystem regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(unittest ${unitheaders} testrun.cpp)
target_link_libraries(unittest cppunit smpp ${Boost_LIBRARIES})
add_test(cpp-unit-test unittest)

add_executable(livetest ${liveheaders} livetest.cpp)
target_link_libraries(livetest cppunit smpp ${Boost_LIBRARIES})
add_test(live-test livetest)
