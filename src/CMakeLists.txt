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
	pdu_test.cpp
	smppclient.cpp
	smpp.cpp
	sms.cpp
	timeformat.cpp
)	

ADD_LIBRARY(smpp SHARED ${headers} ${sources})

FIND_PACKAGE(Boost)
IF (BOOST_FOUND)
    INCLUDE_DIRECTORIES(${BOOST_INCLUDE_DIR})
    ADD_DEFINITIONS( "-DHAS_BOOST" )
ENDIF()

TARGET_LINK_LIBRARIES(smpp
	${Boost_SYSTEM_LIBRARY}
	${Boost_REGEX_LIBRARY}
)
	
set_target_properties(smpp PROPERTIES VERSION 1.0.0 SOVERSION 1)
SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
install(TARGETS smpp DESTINATION ${CMAKE_INSTALL_LIBDIR})