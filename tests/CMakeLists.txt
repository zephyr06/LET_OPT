
string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_CASED_BUILD_TYPE)
if(UPPER_CASED_BUILD_TYPE STREQUAL "DEBUG")
message("########## Debug test")
gtsamAddTestsGlob(LET_OPT "test*.cpp" "testEnv.cpp" "${CONVENIENCE_LIB_NAME}")
endif()


ADD_EXECUTABLE(GenerateTaskSet GenerateTaskSet.cpp)
TARGET_LINK_LIBRARIES(GenerateTaskSet ${CONVENIENCE_LIB_NAME})

# If necessary, use the RELATIVE flag, otherwise each source file may be listed 
# with full pathname. RELATIVE may makes it easier to extract an executable name
# automatically.
# file( GLOB APP_SOURCES RELATIVE app/*.cxx )
# file( GLOB TEST_SOURCES "test*.cpp" )
# file(GLOB_RECURSE EXCLUDE_FILE "testEnv.cpp")
# list(REMOVE_ITEM TEST_SOURCES "${EXCLUDE_FILE}")

# foreach( testsourcefile ${TEST_SOURCES} )
#     # I used a simple string replace, to cut off .cpp.
#     file(RELATIVE_PATH RELATIVE_SOURCE_FILE ${CMAKE_SOURCE_DIR}/tests ${testsourcefile})
#     string( REPLACE ".cpp" ".run2" testname ${RELATIVE_SOURCE_FILE} )
#     add_executable( ${testname} ${RELATIVE_SOURCE_FILE} )
#     # Make sure YourLib is linked to each app
#     target_link_libraries( ${testname} ${CONVENIENCE_LIB_NAME} )
#     string(TOUPPER ${CMAKE_BUILD_TYPE} UPPER_CASED_BUILD_TYPE)
#     if(UPPER_CASED_BUILD_TYPE STREQUAL "DEBUG")
#     target_link_libraries(${testname}  GTest::GTest GTest::Main)
#     gtest_discover_tests(${testname})
#     endif()
# endforeach( testsourcefile ${TEST_SOURCES} )
