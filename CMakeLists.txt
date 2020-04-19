cmake_minimum_required(VERSION 3.10)

# set the project name
project(Atoms VERSION 1.0)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# add the executable
add_executable(Atoms main_sfml.cc)

add_library(AtomsImpl SHARED atoms.cc)

list(APPEND EXTRA_LIBS AtomsImpl)
list(APPEND EXTRA_LIBS sfml-graphics)
list(APPEND EXTRA_LIBS sfml-window)
list(APPEND EXTRA_LIBS sfml-system)
list(APPEND EXTRA_LIBS sfml-network)
target_link_libraries(Atoms PUBLIC ${EXTRA_LIBS})
