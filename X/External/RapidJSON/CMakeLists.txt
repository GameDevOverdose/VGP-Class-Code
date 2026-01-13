# RapidJSON is header-only, create an interface library
add_library(RapidJSON INTERFACE)

# Include directories
target_include_directories(RapidJSON INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
)
