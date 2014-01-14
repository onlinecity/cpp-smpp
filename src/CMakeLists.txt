SET(headers
	smpp/exceptions.h
	smpp/gsmencoding.h
	smpp/pdu.h
	smpp/smppclient.h
	smpp/smpp.h
	smpp/sms.h
	smpp/timeformat.h
	smpp/tlv.h
	smpp/hexdump.h
)

SET(sources
	smpp/gsmencoding.cpp
	smpp/pdu.cpp
	smpp/smppclient.cpp
	smpp/smpp.cpp
	smpp/sms.cpp
	smpp/timeformat.cpp
	smpp/hexdump.cpp
)


ADD_LIBRARY(smpp SHARED ${headers} ${sources})
SET_TARGET_PROPERTIES(smpp PROPERTIES VERSION 1.0.0 SOVERSION 1 PUBLIC_HEADER "${headers}")
TARGET_LINK_LIBRARIES(smpp ${link_libs})

SET(CMAKE_INSTALL_LIBDIR lib CACHE PATH "Output directory for libraries")
SET(CMAKE_INSTALL_HEADERDIR include CACHE PATH "Output directory for headers")
INSTALL(TARGETS smpp LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_HEADERDIR}/smpp)
