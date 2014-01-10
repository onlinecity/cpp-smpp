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

set(TEST3 smppclient_test)
add_executable(${TEST3} $<TARGET_OBJECTS:source_files> smppclient_test.cpp smppclient_test.h)
target_link_libraries(${TEST3} ${link_libs} ${test_libs})
add_test(${TEST3} ${testbin}/${TEST3})

set(TEST4 sms_test)
add_executable(${TEST4} $<TARGET_OBJECTS:source_files> sms_test.cpp)
target_link_libraries(${TEST4} ${link_libs} ${test_libs})
add_test(${TEST4} ${testbin}/${TEST4})

set(TEST5 time_test)
add_executable(${TEST5} $<TARGET_OBJECTS:source_files> time_test.cpp)
target_link_libraries(${TEST5} ${link_libs} ${test_libs})
add_test(${TEST5} ${testbin}/${TEST5})