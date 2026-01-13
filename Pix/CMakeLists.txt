# Automatically find source and header files
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

# Create Windows GUI application
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/X/Inc
    ${CMAKE_SOURCE_DIR}/X/External
)

# Link to XEngine library
target_link_libraries(${PROJECT_NAME} PRIVATE XEngine)

# Preprocessor definitions
target_compile_definitions(${PROJECT_NAME} PRIVATE
    UNICODE
    _UNICODE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    _WINDOWS
)

# Copy xconfig.json
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/xconfig.json"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/xconfig.json"
)
