file(
    GLOB ALL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ut/*.cpp
)

add_executable(json_ut ${ALL_SOURCES})

target_link_libraries(json_ut polip_json gtest gtest_main)
add_test(json_ut json_ut)
