// d3dcompiler_bridge.cpp - Provides D3DCompileFromFileA implementation for macOS
#if defined(__APPLE__)

#include <fstream>
#include <sstream>
#include <string>
#include <cstdio>

#include "../include/directx/d3dcompiler.h"

extern "C" {

__attribute__((visibility("default")))
HRESULT WINAPI D3DCompileFromFileA(
    const char *filename,
    const D3D_SHADER_MACRO *defines,
    ID3DInclude *includes,
    const char *entrypoint,
    const char *target,
    UINT flags1,
    UINT flags2,
    ID3DBlob **code,
    ID3DBlob **errors)
{
    // Read the shader file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        fprintf(stderr, "D3DCompileFromFileA: Failed to open file: %s\n", filename);
        return E_FAIL;
    }

    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::string source(size, '\0');
    if (!file.read(&source[0], size)) {
        fprintf(stderr, "D3DCompileFromFileA: Failed to read file: %s\n", filename);
        return E_FAIL;
    }

    // Call D3DCompile with the loaded source
    return D3DCompile(
        source.c_str(),
        source.size(),
        filename,
        defines,
        includes,
        entrypoint,
        target,
        flags1,
        flags2,
        code,
        errors
    );
}

} // extern "C"

#endif // __APPLE__
