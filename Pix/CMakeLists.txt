add_executable(PixEditor
        CmdDrawPixel.cpp
        CmdSetColor.cpp
        CmdSetResolution.cpp
        CmdVarFloat.cpp
        CommandDictionary.cpp
        Graphics.cpp
        MathHelper.cpp
        PixEditor.cpp
        Rasterizer.cpp
        ScriptParser.cpp
        TextEditor.cpp
        VariableCache.cpp
        Viewport.cpp
        WinMain.cpp
)

target_include_directories(PixEditor PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(PixEditor
        X
)
