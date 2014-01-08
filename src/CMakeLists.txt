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


ADD_LIBRARY(smpp SHARED ${headers} ${sources})
SET_TARGET_PROPERTIES(smpp PROPERTIES VERSION 2.0.0 SOVERSION 1 PUBLIC_HEADER "${headers}")
TARGET_LINK_LIBRARIES(smpp ${link_libs} oclog)
	
SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
SET(CMAKE_INSTALL_HEADERDIR include CACHE PATH "Output directory for headers")
INSTALL(TARGETS smpp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_HEADERDIR}/smpp)
