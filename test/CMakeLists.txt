set(unitheaders
	encodertest.h
	smstest.h
	timetest.h
)

set(liveheaders
    clienttest.h
	connectionsetting.h
)

set(test_libs 
	gtest
	gtest_main
)	

set(testbin ${CMAKE_BINARY_DIR}/bin)

#include project source
include_directories(${CMAKE_SOURCE_DIR}/src/smpp)
aux_source_directory(${CMAKE_SOURCE_DIR}/src/smpp smpp_sources)
add_library(source_files OBJECT ${smpp_sources})

set(TEST1 pdu_test)
add_executable(${TEST1} $<TARGET_OBJECTS:source_files> pdu_test.cpp)
target_link_libraries(${TEST1} ${link_libs} ${test_libs})
add_test(${TEST1} ${testbin}/${TEST1})

set(TEST2 encoder_test)
add_executable(${TEST2} $<TARGET_OBJECTS:source_files> encoder_test.cpp)
target_link_libraries(${TEST2} ${link_libs} ${test_libs})
add_test(${TEST2} ${testbin}/${TEST2})
