//====================================================================================================
// Filename:	GraphicsSystem.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "GraphicsSystem.h"

// Platform-specific native window access
#if defined(_WIN32)
    #define GLFW_EXPOSE_NATIVE_WIN32
    #include <GLFW/glfw3native.h>
#elif defined(__APPLE__)
    // Save and undefine DXMT Windows macros that conflict with macOS system headers
    #pragma push_macro("interface")
    #pragma push_macro("BOOL")
    #undef interface
    #undef BOOL

    #define GLFW_EXPOSE_NATIVE_COCOA
    #include <GLFW/glfw3native.h>

    // Restore DXMT Windows macros
    #pragma pop_macro("BOOL")
    #pragma pop_macro("interface")
#endif

using namespace X;

namespace
{
	const D3D_DRIVER_TYPE kDriverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	const UINT kNumDriverTypes = static_cast<UINT>(std::size(kDriverTypes));

	const D3D_FEATURE_LEVEL kFeatureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	const UINT kNumFeatureLevels = static_cast<UINT>(std::size(kFeatureLevels));

	GraphicsSystem* sGraphicsSystem = nullptr;
}

void GraphicsSystem::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	if (sGraphicsSystem != nullptr && width > 0 && height > 0)
	{
		sGraphicsSystem->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
	}
}

void GraphicsSystem::StaticInitialize(GLFWwindow* window, bool fullscreen)
{
	XASSERT(sGraphicsSystem == nullptr, "[GraphicsSystem] System already initialized!");
	sGraphicsSystem = new GraphicsSystem();
	sGraphicsSystem->Initialize(window, fullscreen);
}

void GraphicsSystem::StaticTerminate()
{
	if (sGraphicsSystem != nullptr)
	{
		sGraphicsSystem->Terminate();
		SafeDelete(sGraphicsSystem);
	}
}

GraphicsSystem* GraphicsSystem::Get()
{
	XASSERT(sGraphicsSystem != nullptr, "[GraphicsSystem] No system registered.");
	return sGraphicsSystem;
}

GraphicsSystem::GraphicsSystem()
	: mD3DDevice(nullptr)
	, mImmediateContext(nullptr)
	, mSwapChain(nullptr)
	, mRenderTargetView(nullptr)
	, mDepthStencilBuffer(nullptr)
	, mDepthStencilView(nullptr)
	, mDisableDepthStencil(nullptr)
	, mDriverType(D3D_DRIVER_TYPE_NULL)
	, mFeatureLevel(D3D_FEATURE_LEVEL_11_0)
{
}

GraphicsSystem::~GraphicsSystem()
{
	XASSERT(mD3DDevice == nullptr, "[GraphicsSystem] Terminate() must be called to clean up!");
}

void GraphicsSystem::Initialize(GLFWwindow* window, bool fullscreen)
{
	// Get window dimensions from GLFW
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	// Get native window handle for swap chain
#if defined(_WIN32)
	HWND nativeWindow = glfwGetWin32Window(window);
#elif defined(__APPLE__)
	HWND nativeWindow = (HWND)glfwGetCocoaWindow(window);
#else
	#error "Unsupported platform"
#endif

	UINT createDeviceFlags = 0;

	DXGI_SWAP_CHAIN_DESC descSwapChain;
	ZeroMemory(&descSwapChain, sizeof(descSwapChain));
	descSwapChain.BufferCount = 1;
	descSwapChain.BufferDesc.Width = width;
	descSwapChain.BufferDesc.Height = height;
	descSwapChain.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descSwapChain.BufferDesc.RefreshRate.Numerator = 60;
	descSwapChain.BufferDesc.RefreshRate.Denominator = 1;
	descSwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	descSwapChain.OutputWindow = nativeWindow;
	descSwapChain.SampleDesc.Count = 1;
	descSwapChain.SampleDesc.Quality = 0;
	descSwapChain.Windowed = !fullscreen;

	HRESULT hr = S_OK;
	for (UINT driverTypeIndex = 0; driverTypeIndex < kNumDriverTypes; ++driverTypeIndex)
	{
		mDriverType = kDriverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain
		(
			nullptr,
			mDriverType,
			nullptr,
			createDeviceFlags,
			kFeatureLevels,
			kNumFeatureLevels,
			D3D11_SDK_VERSION,
			&descSwapChain,
			&mSwapChain,
			&mD3DDevice,
			&mFeatureLevel,
			&mImmediateContext
		);
		if (SUCCEEDED(hr))
		{
			break;
		}
	}
	XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to create device or swap chain.");

	// Cache swap chain description
	mSwapChain->GetDesc(&mSwapChainDesc);

	// Initialize render target and depth stencil views
	Resize(GetBackBufferWidth(), GetBackBufferHeight());

	// Set GLFW resize callback
	glfwSetFramebufferSizeCallback(window, FramebufferSizeCallback);
}

void GraphicsSystem::Terminate()
{
	SafeRelease(mDisableDepthStencil);
	SafeRelease(mDepthStencilView);
	SafeRelease(mDepthStencilBuffer);
	SafeRelease(mRenderTargetView);
	SafeRelease(mSwapChain);
	SafeRelease(mImmediateContext);
	SafeRelease(mD3DDevice);
}

void GraphicsSystem::BeginRender(const Color& clearColor)
{
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
	mImmediateContext->ClearRenderTargetView(mRenderTargetView, (const FLOAT*)&clearColor);
	mImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsSystem::EndRender()
{
	mSwapChain->Present(1, 0);
}

void GraphicsSystem::ToggleFullscreen()
{
	BOOL fullscreen;
	mSwapChain->GetFullscreenState(&fullscreen, nullptr);
	mSwapChain->SetFullscreenState(!fullscreen, nullptr);
}

void GraphicsSystem::Resize(uint32_t width, uint32_t height)
{
	mImmediateContext->OMSetRenderTargets(0, nullptr, nullptr);

	SafeRelease(mRenderTargetView);
	SafeRelease(mDepthStencilView);

	HRESULT hr;
	if (width != GetBackBufferWidth() || height != GetBackBufferHeight())
	{
		hr = mSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to resize swap chain buffer.");

		// Cache swap chain description
		mSwapChain->GetDesc(&mSwapChainDesc);
	}

	// Create a render target view
	ID3D11Texture2D* backBuffer = nullptr;
	hr = mSwapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
	XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to access swap chain buffer.");

	hr = mD3DDevice->CreateRenderTargetView(backBuffer, nullptr, &mRenderTargetView);
	SafeRelease(backBuffer);
	XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to create render target view.");

	// Create depth stencil texture
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = GetBackBufferWidth();
	descDepth.Height = GetBackBufferHeight();
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 1;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;
	hr = mD3DDevice->CreateTexture2D(&descDepth, nullptr, &mDepthStencilBuffer);
	XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to create depth stencil buffer.");

	// Create the depth stencil view
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = mD3DDevice->CreateDepthStencilView(mDepthStencilBuffer, &descDSV, &mDepthStencilView);
	XASSERT(SUCCEEDED(hr), "[GraphicsSystem] Failed to create depth stencil view.");

	// Set the render target view and depth stencil view
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Setup the viewport
	mViewport.Width = static_cast<float>(GetBackBufferWidth());
	mViewport.Height = static_cast<float>(GetBackBufferHeight());
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;
	mViewport.TopLeftX = 0;
	mViewport.TopLeftY = 0;
	mImmediateContext->RSSetViewports(1, &mViewport);
}

void GraphicsSystem::ResetRenderTarget()
{
	XASSERT(mImmediateContext != nullptr, "[GraphicsSystem] Failed to reset render target.");
	mImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);
}

void GraphicsSystem::ResetViewport()
{
	XASSERT(mImmediateContext != nullptr, "[GraphicsSystem] Failed to reset viewport.");
	mImmediateContext->RSSetViewports(1, &mViewport);
}

void GraphicsSystem::EnableDepthTesting(bool enable)
{
	XASSERT(mImmediateContext != nullptr, "[GraphicsSystem] Failed to set depth stencil state.");
	mImmediateContext->OMSetDepthStencilState(enable ? nullptr : mDisableDepthStencil, 0);
}

uint32_t GraphicsSystem::GetBackBufferWidth() const
{
	XASSERT(mSwapChain != nullptr, "[GraphicsSystem] Failed to get swap chain buffer width.");
	return mSwapChainDesc.BufferDesc.Width;
}

uint32_t GraphicsSystem::GetBackBufferHeight() const
{
	XASSERT(mSwapChain != nullptr, "[GraphicsSystem] Failed to get swap chain buffer width.");
	return mSwapChainDesc.BufferDesc.Height;
}

float GraphicsSystem::GetBackBufferAspectRatio() const
{
	return static_cast<float>(GetBackBufferWidth()) / static_cast<float>(GetBackBufferHeight());
}
