cmake_minimum_required(VERSION 3.28)

project(cai)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release)
endif()
 
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")

include_directories(source)
include_directories(libraries)
include_directories(standard-sauce)
include_directories(nnpp/source)
include_directories(nnpp/pcg-cpp/include/)

add_subdirectory(source)

add_executable(cai source/cai.cpp)
target_link_libraries(cai Game)

add_subdirectory(tests)
