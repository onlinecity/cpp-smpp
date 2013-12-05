SET(headers
	exceptions.h
	gsmencoding.h
	pdu.h
	smppclient.h
	smpp.h
	sms.h
	timeformat.h
	tlv.h
	hexdump.h
	log.h
)

SET(sources
	gsmencoding.cpp
	pdu.cpp
	smppclient.cpp
	smpp.cpp
	sms.cpp
	timeformat.cpp
	hexdump.cpp
)	

SET(Boost_USE_STATIC_LIBS OFF) # Or we get errors with -fPIC
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)

# Boost
FIND_PACKAGE(Boost 1.41 COMPONENTS date_time system filesystem regex REQUIRED)
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# LibEvent
FIND_PATH(EVENT_INCLUDE_DIR event.h)
INCLUDE_DIRECTORIES(${EVENT_INCLUDE_DIR})
FIND_LIBRARY(EVENT_LIB event)

# Thrift
FIND_PATH(THRIFT_INCLUDE_DIR Thrift.h PATH_SUFFIXES thrift)
INCLUDE_DIRECTORIES(${THRIFT_INCLUDE_DIR}) 
FIND_LIBRARY(THRIFT_LIB thrift)
FIND_LIBRARY(THRIFT_NB_LIB thriftnb)

# oclog
FIND_PATH(OCLOG_INCLUDE_DIR Log.h PATH_SUFFIXES oclog)
INCLUDE_DIRECTORIES(${OCLOG_INCLUDE_DIR})

ADD_LIBRARY(smpp SHARED ${headers} ${sources})
SET_TARGET_PROPERTIES(smpp PROPERTIES VERSION 1.0.0 SOVERSION 1 PUBLIC_HEADER "${headers}")
TARGET_LINK_LIBRARIES(smpp ${Boost_LIBRARIES} oclog)
	
	
SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
SET(CMAKE_INSTALL_HEADERDIR include CACHE PATH "Output directory for headers")
INSTALL(TARGETS smpp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_HEADERDIR}/smpp)
