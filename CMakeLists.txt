cmake_minimum_required(VERSION 3.12)
project(nTagger)

set(CMAKE_CXX_STANDARD 17)

SET(COMPILE_FLAGS "")
SET(LINK_FLAGS "")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

add_executable(nTagger main.cpp)

target_link_libraries(nTagger -lstdc++fs)