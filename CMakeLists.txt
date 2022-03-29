cmake_minimum_required(VERSION 3.10)

project(cai)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

include_directories(source)
include_directories(libraries)
include_directories(nnpp/source)

add_subdirectory(source)

add_executable(cai source/cai.cpp)
target_link_libraries(cai Game)

add_subdirectory(tests)
