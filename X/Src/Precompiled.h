//====================================================================================================
// Filename:	Precompiled.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_PRECOMPILED_H
#define INCLUDED_XENGINE_PRECOMPILED_H

#include <algorithm>
#include <codecvt>
#include <list>
#include <locale>
#include <map>
#include <string>
#include <vector>

#ifdef _WIN32
    #include <d3d11_1.h>
    #include <d3dcompiler.h>
    #include <DirectXMath.h>

    #pragma comment(lib, "d3d11.lib")
    #pragma comment(lib, "D3DCompiler.lib")
#else
    // macOS - DXMT headers
    #include <windef.h>
    #include <d3d11.h>
    #include <d3d11_1.h>
    #include <d3dcompiler.h>
    #include <DirectXMath.h>
#endif

#include <GLFW/glfw3.h>

#include "XCore.h"
#include "Forward.h"

#endif // #ifndef INCLUDED_XENGINE_PRECOMPILED_H
