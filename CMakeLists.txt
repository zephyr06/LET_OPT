cmake_minimum_required(VERSION 3.0)
project(LET_OPT CXX C)
set (CMAKE_CXX_STANDARD 17)
# set(CMAKE_BUILD_TYPE Debug)

include_directories(${CMAKE_SOURCE_DIR})

add_definitions(-DPROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}")

# find_package(GTSAMCMakeTools)
# include(GtsamMakeConfigFile)
# include(GtsamBuildTypes)
# include(GtsamTesting)
# set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${GTSAM_DIR}/../GTSAMCMakeTools")

set(CMAKE_WARN_DEPRECATED OFF)
set(CMAKE_ERROR_DEPRECATED OFF)
add_compile_options(-Wdeprecated-declarations)
unset(CMAKE_WARN_DEPRECATED)

# Boost - same requirement as gtsam
find_package(Boost 1.50 REQUIRED)
find_package(yaml-cpp)
include_directories(${Boost_INCLUDE_DIR})
include_directories(/usr/local/lib)
file(GLOB_RECURSE sources "sources/*.cpp" "sources/*.h")
set(CONVENIENCE_LIB_NAME ${PROJECT_NAME})


# Config Cplex
set(ENV{CPLEX_ROOT_DIR} "./CPLEX")
if (DEFINED ENV{CPLEX_ROOT_DIR})
  message("Find Cplex installed at " $ENV{CPLEX_ROOT_DIR})
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
  set(CPLEX_ROOT_DIR $ENV{CPLEX_ROOT_DIR})
  find_package(Cplex)
  include_directories(SYSTEM ${CPLEX_INCLUDE_DIRS})
else()
  # exclude files denpends on cplex
  message( FATAL_ERROR "Error: Cplex not found! The project will not work without Cplex.")
endif()



# google test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/912db742531bf82efb01194bc08140416e3b3467.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
# find_package(GTest REQUIRED)
# include(GoogleTest)

add_library(${CONVENIENCE_LIB_NAME} SHARED ${sources})

# linking with dependencies
target_link_libraries(${CONVENIENCE_LIB_NAME} ${Boost_LIBRARIES})
target_link_libraries(${CONVENIENCE_LIB_NAME} yaml-cpp)
target_link_libraries(${CONVENIENCE_LIB_NAME} "stdc++fs")
target_link_libraries(${CONVENIENCE_LIB_NAME} gtest_main)
target_link_libraries(${CONVENIENCE_LIB_NAME}  gmock)
find_package(GTest REQUIRED)
target_link_libraries(${CONVENIENCE_LIB_NAME}  GTest::GTest GTest::Main) # discover and run all the tests
find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})
if (CPLEX_FOUND)
  target_link_libraries(${CONVENIENCE_LIB_NAME} ${CPLEX_LIBRARIES} ${CMAKE_DL_LIBS})
endif()

add_subdirectory(sources)
enable_testing()
add_subdirectory(tests)
# add_subdirectory(scripts)
add_subdirectory(RunSingleFile)

# include(GoogleTest)
# gtest_discover_tests(LET_OPT)