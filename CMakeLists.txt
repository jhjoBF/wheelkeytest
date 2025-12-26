cmake_minimum_required(VERSION 3.5.0)
set(CMAKE_CXX_STANDARD 11)
project(cesShow)

# 컴파일러별 플래그 설정
if(NOT WIN32)
    set(CMAKE_CXX_COMPILER g++)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-builtin-macro-redefined -D__FILENAME__=\"(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)\"")
endif()

file(GLOB CPP_FILES app/*.cpp seed/*.cpp)
message(STATUS ${CPP_FILES})

# 플랫폼별 라이브러리 설정
if(WIN32)
    set(LIBS ws2_32)
else()
    set(LIBS pthread)
endif()

add_executable(cesShow ${CPP_FILES})
include_directories(seed)
target_link_libraries(cesShow ${LIBS})
