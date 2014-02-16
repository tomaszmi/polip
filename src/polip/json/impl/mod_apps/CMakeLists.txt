file(
    GLOB ALL_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_executable(basic_usage
    ${ALL_SOURCES}
)
target_link_libraries(basic_usage polip_json)
