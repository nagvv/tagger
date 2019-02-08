cmake_minimum_required(VERSION 3.12)
project(tagger)

add_compile_definitions(SQLITE_THREADSAFE=0)

set(CMAKE_CXX_STANDARD 17)

SET(COMPILE_FLAGS "")
SET(LINK_FLAGS "")

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LINK_FLAGS}")

add_executable(tagger main.cpp tagger_db.cpp tagger_db.h common.h tagger_fs.cpp tagger_fs.h)

add_library(sqlite3 STATIC sqlite3.c)

target_link_libraries(tagger -static -lstdc++fs sqlite3)