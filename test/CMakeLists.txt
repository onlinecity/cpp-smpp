find_library(gtest_LIB gtest HINTS ${GTEST_LIB_DIR} DOC "GTest lib" REQUIRED)
find_library(gtest_main_LIB gtest_main HINTS ${GTEST_LIB_DIR} DOC "GTest main() lib" REQUIRED)

# Add a single gtest
function(add_gtest name libs)
  add_executable(${name} $<TARGET_OBJECTS:compiled> ${name}_test.cc ${ARGN})
  target_link_libraries(${name} ${link_libs})
  foreach (lib "${libs}")
    target_link_libraries(${name} ${lib})
  endforeach()
endfunction()

# Add a list of gtests, with global SetUp/TearDown
function(add_gtests)
  foreach (testname ${ARGN})
    list (APPEND testsrcs "${testname}_test.cc")
  endforeach()
  add_executable(unittest $<TARGET_OBJECTS:compiled> ${testsrcs} main.cc)
  target_link_libraries(unittest ${link_libs} ${gtest_LIB})
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
