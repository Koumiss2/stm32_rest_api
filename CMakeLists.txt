add_library(rest_api STATIC)

file (GLOB_RECURSE REST_SOURCES 
    "api/*.cpp"
    "http/*.cpp"
    "endpoints/*.cpp"
)

target_sources(rest_api PRIVATE ${REST_SOURCES})

target_include_directories(rest_api PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}/api
    ${CMAKE_CURRENT_SOURCE_DIR}/http
    ${CMAKE_CURRENT_SOURCE_DIR}/endpoints
)

target_link_libraries(rest_api PUBLIC
    stm32cubemx
    LwIP
    FreeRTOS
)