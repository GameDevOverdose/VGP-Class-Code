# Add external dependencies
add_subdirectory(External)

# X Engine source files
file(GLOB X_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.cpp")
file(GLOB X_HEADERS_SRC "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.h")
file(GLOB X_HEADERS_INC "${CMAKE_CURRENT_SOURCE_DIR}/Inc/*.h")

# Platform-specific source files
if(APPLE)
    file(GLOB X_OBJC_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Src/*.mm")
endif()

# Create static library
add_library(XEngine STATIC
    ${X_SOURCES}
    ${X_OBJC_SOURCES}
    ${X_HEADERS_SRC}
    ${X_HEADERS_INC}
)

# Platform-specific includes (BEFORE common includes so they take precedence)
if(APPLE)
    # DXMT headers must come first for Windows API compatibility
    target_include_directories(XEngine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/External/dxmt/include/windows
        ${CMAKE_CURRENT_SOURCE_DIR}/External/dxmt/include/directx
    )
endif()

# Common include directories
target_include_directories(XEngine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/External
    ${CMAKE_CURRENT_SOURCE_DIR}/External/DirectXMath/Inc
)

target_include_directories(XEngine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/Src
)

# Link external libraries (cross-platform)
target_link_libraries(XEngine PUBLIC
    glfw
    RapidJSON
    ImGui
)

# Platform-specific libraries
if(WIN32)
    target_link_libraries(XEngine PUBLIC
        d3d11
        dxgi
        d3dcompiler
        xinput
        xaudio2
    )
elseif(APPLE)
    target_link_libraries(XEngine PUBLIC
        dxmt
    )
    find_library(METAL_LIBRARY Metal)
    find_library(QUARTZCORE_LIBRARY QuartzCore)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(UNIFORMTYPEIDENTIFIERS_LIBRARY UniformTypeIdentifiers)
    target_link_libraries(XEngine PUBLIC
        ${METAL_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${COCOA_LIBRARY}
        ${IOKIT_LIBRARY}
        ${UNIFORMTYPEIDENTIFIERS_LIBRARY}
    )
endif()
