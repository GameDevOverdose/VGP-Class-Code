# GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

if(APPLE)
    add_subdirectory(dxmt)
    add_subdirectory(vkd3d)
endif()

# DirectXTK - Windows only (uses WIC, XAudio2, etc.)
if(WIN32)
    add_subdirectory(DirectXTK)
endif()

add_subdirectory(ImGui)
add_subdirectory(RapidJSON)
