gtsamAddTestsGlob(LET_OPT "test*.cpp" "" "${CONVENIENCE_LIB_NAME}")


ADD_EXECUTABLE(GenerateTaskSet GenerateTaskSet.cpp)
TARGET_LINK_LIBRARIES(GenerateTaskSet ${CONVENIENCE_LIB_NAME})

# enable_testing()

# add_executable(
#     testGoogleTest
#     testGoogleTest.cpp
# )
# target_link_libraries(
#     testGoogleTest
#     ${CONVENIENCE_LIB_NAME}
# )

# include(GoogleTest)
# gtest_discover_tests(testGoogleTest)
