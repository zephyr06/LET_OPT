# gtsamAddTestsGlob(RunSingles "*RunSingle*.cpp" "" "${CONVENIENCE_LIB_NAME}")


file( GLOB TEST_SOURCES "*RunSingle*.cpp" )
# file(GLOB_RECURSE EXCLUDE_FILE "testEnv.cpp")
# list(REMOVE_ITEM TEST_SOURCES "${EXCLUDE_FILE}")

foreach( testsourcefile ${TEST_SOURCES} )
    # I used a simple string replace, to cut off .cpp.
    file(RELATIVE_PATH RELATIVE_SOURCE_FILE ${CMAKE_SOURCE_DIR}/RunSingleFile ${testsourcefile})
    string( REPLACE ".cpp" ".run" testname ${RELATIVE_SOURCE_FILE} )
    add_executable( ${testname} ${RELATIVE_SOURCE_FILE} )
    # Make sure YourLib is linked to each app
    target_link_libraries( ${testname} ${CONVENIENCE_LIB_NAME} )
endforeach( testsourcefile ${TEST_SOURCES} )