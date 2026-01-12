add_library(X STATIC
        Src/AudioSystem.cpp
        Src/Camera.cpp
        Src/Config.cpp
        Src/ConstantBuffer.cpp
        Src/Font.cpp
        Src/GraphicsSystem.cpp
        Src/Gui.cpp
        Src/InputSystem.cpp
        Src/PixelShader.cpp
        Src/Precompiled.cpp
        Src/RenderTarget.cpp
        Src/SimpleDraw.cpp
        Src/SoundEffectManager.cpp
        Src/SpriteRenderer.cpp
        Src/Texture.cpp
        Src/TextureManager.cpp
        Src/Timer.cpp
        Src/Vertex.cpp
        Src/VertexShader.cpp
        Src/VertexUtil.cpp
        Src/XEngine.cpp
        Src/XMath.cpp
)

target_include_directories(X PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Inc
        ${CMAKE_CURRENT_SOURCE_DIR}/External
)

if(APPLE)
    target_link_libraries(X PUBLIC
            "-framework Cocoa"
            "-framework Metal"
            "-framework MetalKit"
            "-framework QuartzCore"
    )
endif()

add_subdirectory(External)

target_link_libraries(X PUBLIC glfw)

# Link the platform-specific DX wrapper so its INTERFACE include directories
# are propagated to target X. On macOS we want the dxmt shim, on Linux use dxvk.
if (NOT WIN32)
    if (APPLE)
        target_link_libraries(X PUBLIC dxmt)
    else()
        target_link_libraries(X PUBLIC dxvk)
    endif()
endif()
