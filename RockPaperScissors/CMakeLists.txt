cmake_minimum_required (VERSION 2.6)

project (RockPaperScissors)

# the version number
set (RockPaperScissors_VERSION_MAJOR 0)
set (RockPaperScissors_VERSION_MINOR 1)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/RockPaperScissorsConfig.hxx.in"
  "${PROJECT_BINARY_DIR}/RockPaperScissorsConfig.hxx"
  )
 
# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories ("${PROJECT_BINARY_DIR}")
 
# add dependencies
find_package (Boost COMPONENTS system program_options REQUIRED)
include_directories (${Boost_INCLUDE_DIR})
find_package (Threads)

# add the executable
add_definitions ("-std=c++11 -Wall -g")
add_executable (RockPaperScissors RockPaperScissors.cxx CommandLine.cxx)
target_link_libraries (RockPaperScissors ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

