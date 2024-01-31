cmake_minimum_required(VERSION 3.0.0)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_STANDARD 11)
project(wheelKeyTest)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D'__FILE__=\"$(subst $(realpath ${CMAKE_SOURCE_DIR})/,,$(abspath $<))\"'")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D__FILENAME__=\"(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)\"")

file(GLOB CPP_FILES app/*.cpp seed/*.cpp)
message(STATUS ${CPP_FILES})

set(LIBS pthread ssl)
add_executable(wheelKeyTest ${CPP_FILES})
include_directories(seed)
target_link_libraries(wheelKeyTest ${LIBS})
