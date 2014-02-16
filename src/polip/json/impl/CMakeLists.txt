file(GLOB ALL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_library(polip_json ${ALL_SOURCES})
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mod_tests)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/mod_apps)
