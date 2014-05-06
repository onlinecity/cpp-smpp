set(gtest_LIB ${GTEST_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a)
set(gtest_main_LIB ${GTEST_LIB_DIR}/${CMAKE_FIND_LIBRARY_PREFIXES}gtest_main.a)

# Add a single gtest
function(add_gtest name libs)
  set (test_bin ${name}_test)
  add_executable(${test_bin} $<TARGET_OBJECTS:compiled> ${name}_test.cc ${ARGN})
  target_link_libraries(${test_bin} ${link_libs})
  foreach (lib "${libs}")
    target_link_libraries(${test_bin} ${lib} )
  endforeach()
  add_dependencies(${test_bin} googletest)
  add_test(${name} ${CMAKE_BINARY_DIR}/bin/${test_bin})
endfunction()

# Add a list of gtests, with global SetUp/TearDown
function(add_gtests)
  foreach (testname ${ARGN})
    list (APPEND testsrcs "${testname}_test.cc")
  endforeach()
  add_executable(unittest $<TARGET_OBJECTS:compiled> ${testsrcs} main.cc)
  target_link_libraries(unittest ${link_libs} ${gtest_LIB})
  add_dependencies(unittest googletest)
  add_test(unittest ${CMAKE_BINARY_DIR}/bin/unittest)
endfunction()

add_custom_target(test ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/unittest DEPENDS compiled)

## Use these to add a single target with optional extra libs and sources
# add_gtest(redis ${gtest_LIB} main.cc)
# add_gtest(other "${gtest_LIB};${gtest_main_LIB}")

# Build a single binary (unittest) that performs all unit tests
add_gtests(
  sms
  time
  encoder
  pdu
  )

add_gtest(smppclient ${gtest_LIB})
