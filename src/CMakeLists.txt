cmake_minimum_required(VERSION 2.8)
project(polip)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -pedantic")
add_definitions(-DBOOST_SPIRIT_USE_PHOENIX_V3=1)

enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTest_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(${CMAKE_SOURCE_DIR}/polip/json/impl)
