SET(headers
	exceptions.h
	gsmencoding.h
	pdu.h
	smppclient.h
	smpp.h
	sms.h
	timeformat.h
	tlv.h
)

SET(sources
	gsmencoding.cpp
	pdu.cpp
	smppclient.cpp
	smpp.cpp
	sms.cpp
	timeformat.cpp
)	

SET(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

FIND_PACKAGE(Boost 1.41 COMPONENTS date_time system filesystem regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

ADD_LIBRARY(smpp SHARED ${headers} ${sources})
SET_TARGET_PROPERTIES(smpp PROPERTIES VERSION 1.0.0 SOVERSION 1)
TARGET_LINK_LIBRARIES(smpp ${Boost_LIBRARIES})
	
SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
INSTALL(TARGETS smpp DESTINATION ${CMAKE_INSTALL_LIBDIR})
