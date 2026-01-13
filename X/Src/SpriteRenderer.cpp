//====================================================================================================
// Filename:	SpriteRenderer.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "SpriteRenderer.h"

#include "GraphicsSystem.h"
#include "Texture.h"

#ifdef _WIN32
#include <DirectXTK/Inc/CommonStates.h>
#include <DirectXTK/Inc/SpriteBatch.h>
#endif

using namespace X;

namespace
{
	struct Float2 { float x, y; };

	Float2 GetOrigin(uint32_t width, uint32_t height, Pivot pivot)
	{
		auto index = static_cast<std::underlying_type_t<Pivot>>(pivot);
		constexpr Float2 offsets[] =
		{
			{ 0.0f, 0.0f }, // TopLeft
			{ 0.5f, 0.0f }, // Top
			{ 1.0f, 0.0f }, // TopRight
			{ 0.0f, 0.5f }, // Left
			{ 0.5f, 0.5f }, // Center
			{ 1.0f, 0.5f }, // Right
			{ 0.0f, 1.0f }, // BottomLeft
			{ 0.5f, 1.0f }, // Bottom
			{ 1.0f, 1.0f }, // BottomRight
		};
		return { width * offsets[index].x, height * offsets[index].y };
	}

#ifdef _WIN32
	DirectX::SpriteEffects GetSpriteEffects(Flip flip)
	{
		switch (flip)
		{
		case Flip::Horizontal:
			return DirectX::SpriteEffects_FlipHorizontally;
		case Flip::Vertical:
			return DirectX::SpriteEffects_FlipVertically;
		case Flip::Both:
			return DirectX::SpriteEffects_FlipBoth;
		default:
			break;
		}
		return DirectX::SpriteEffects_None;
	}

	DirectX::XMFLOAT2 ToXMFLOAT2(const X::Math::Vector2& v)
	{
		return { v.x, v.y };
	}
#endif

	SpriteRenderer* sSpriteRenderer = nullptr;
}

void SpriteRenderer::StaticInitialize()
{
	XASSERT(sSpriteRenderer == nullptr, "[SpriteRenderer] System already initialized!");
	sSpriteRenderer = new SpriteRenderer();
	sSpriteRenderer->Initialize();
}

//----------------------------------------------------------------------------------------------------

void SpriteRenderer::StaticTerminate()
{
	if (sSpriteRenderer != nullptr)
	{
		sSpriteRenderer->Terminate();
		SafeDelete(sSpriteRenderer);
	}
}

//----------------------------------------------------------------------------------------------------

SpriteRenderer* SpriteRenderer::Get()
{
	XASSERT(sSpriteRenderer != nullptr, "[SpriteRenderer] No instance registered.");
	return sSpriteRenderer;
}

//----------------------------------------------------------------------------------------------------

SpriteRenderer::SpriteRenderer()
#ifdef _WIN32
	: mCommonStates(nullptr)
	, mSpriteBatch(nullptr)
#else
	: mVertexShader(nullptr)
	, mPixelShader(nullptr)
	, mInputLayout(nullptr)
	, mVertexBuffer(nullptr)
	, mIndexBuffer(nullptr)
	, mConstantBuffer(nullptr)
	, mSamplerState(nullptr)
	, mBlendState(nullptr)
	, mDepthStencilState(nullptr)
	, mRasterizerState(nullptr)
#endif
{
}

//----------------------------------------------------------------------------------------------------

SpriteRenderer::~SpriteRenderer()
{
#ifdef _WIN32
	XASSERT(mSpriteBatch == nullptr, "[SpriteRenderer] Renderer not freed.");
#else
	XASSERT(mVertexShader == nullptr, "[SpriteRenderer] Renderer not freed.");
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::Initialize()
{
#ifdef _WIN32
	XASSERT(mSpriteBatch == nullptr, "[SpriteRenderer] Already initialized.");
	GraphicsSystem* gs = GraphicsSystem::Get();
	mCommonStates = new DirectX::CommonStates(gs->GetDevice());
	mSpriteBatch = new DirectX::SpriteBatch(gs->GetContext());
#else
	XASSERT(mVertexShader == nullptr, "[SpriteRenderer] Already initialized.");
	
	GraphicsSystem* gs = GraphicsSystem::Get();
	ID3D11Device* device = gs->GetDevice();
	
	// Sprite shader for textured quads
	const char* spriteShader =
		"cbuffer CBSprite : register(b0)\n"
		"{\n"
		"    matrix transform;\n"
		"};\n"
		"struct VS_INPUT\n"
		"{\n"
		"    float2 position : POSITION;\n"
		"    float2 texCoord : TEXCOORD;\n"
		"    float4 color : COLOR;\n"
		"};\n"
		"struct PS_INPUT\n"
		"{\n"
		"    float4 position : SV_POSITION;\n"
		"    float2 texCoord : TEXCOORD;\n"
		"    float4 color : COLOR;\n"
		"};\n"
		"Texture2D spriteTexture : register(t0);\n"
		"SamplerState spriteSampler : register(s0);\n"
		"PS_INPUT VS(VS_INPUT input)\n"
		"{\n"
		"    PS_INPUT output;\n"
		"    output.position = mul(float4(input.position, 0.0f, 1.0f), transform);\n"
		"    output.texCoord = input.texCoord;\n"
		"    output.color = input.color;\n"
		"    return output;\n"
		"}\n"
		"float4 PS(PS_INPUT input) : SV_Target\n"
		"{\n"
		"    return spriteTexture.Sample(spriteSampler, input.texCoord) * input.color;\n"
		"}\n";
	
	// Compile vertex shader
	ID3DBlob* vsBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	HRESULT hr = D3DCompile(spriteShader, strlen(spriteShader), nullptr, nullptr, nullptr,
		"VS", "vs_5_0", 0, 0, &vsBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			XLOG("[SpriteRenderer] Vertex shader compile error: %s", (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		XASSERT(false, "[SpriteRenderer] Failed to compile vertex shader.");
	}
	
	hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShader);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create vertex shader.");
	
	// Create input layout
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	hr = device->CreateInputLayout(inputLayout, 3, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayout);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create input layout.");
	vsBlob->Release();
	
	// Compile pixel shader
	ID3DBlob* psBlob = nullptr;
	hr = D3DCompile(spriteShader, strlen(spriteShader), nullptr, nullptr, nullptr,
		"PS", "ps_5_0", 0, 0, &psBlob, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			XLOG("[SpriteRenderer] Pixel shader compile error: %s", (char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		XASSERT(false, "[SpriteRenderer] Failed to compile pixel shader.");
	}
	
	hr = device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShader);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create pixel shader.");
	psBlob->Release();
	
	// Create vertex buffer (dynamic, for batching)
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(SpriteVertex) * kMaxSprites * 4;
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer(&vbDesc, nullptr, &mVertexBuffer);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create vertex buffer.");
	
	// Create index buffer (static, for quads)
	std::vector<uint16_t> indices(kMaxSprites * 6);
	for (uint32_t i = 0; i < kMaxSprites; ++i)
	{
		indices[i * 6 + 0] = static_cast<uint16_t>(i * 4 + 0);
		indices[i * 6 + 1] = static_cast<uint16_t>(i * 4 + 1);
		indices[i * 6 + 2] = static_cast<uint16_t>(i * 4 + 2);
		indices[i * 6 + 3] = static_cast<uint16_t>(i * 4 + 2);
		indices[i * 6 + 4] = static_cast<uint16_t>(i * 4 + 1);
		indices[i * 6 + 5] = static_cast<uint16_t>(i * 4 + 3);
	}
	
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = static_cast<UINT>(sizeof(uint16_t) * indices.size());
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	
	D3D11_SUBRESOURCE_DATA ibData = {};
	ibData.pSysMem = indices.data();
	hr = device->CreateBuffer(&ibDesc, &ibData, &mIndexBuffer);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create index buffer.");
	
	// Create constant buffer
	D3D11_BUFFER_DESC cbDesc = {};
	cbDesc.Usage = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth = sizeof(Math::Matrix4);
	cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = device->CreateBuffer(&cbDesc, nullptr, &mConstantBuffer);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create constant buffer.");
	
	// Create sampler state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = device->CreateSamplerState(&sampDesc, &mSamplerState);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create sampler state.");
	
	// Create blend state (non-premultiplied alpha)
	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendDesc, &mBlendState);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create blend state.");
	
	// Create depth stencil state (depth disabled)
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = device->CreateDepthStencilState(&dsDesc, &mDepthStencilState);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create depth stencil state.");
	
	// Create rasterizer state
	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_NONE;
	rsDesc.ScissorEnable = FALSE;
	rsDesc.DepthClipEnable = TRUE;
	hr = device->CreateRasterizerState(&rsDesc, &mRasterizerState);
	XASSERT(SUCCEEDED(hr), "[SpriteRenderer] Failed to create rasterizer state.");
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::Terminate()
{
#ifdef _WIN32
	XASSERT(mSpriteBatch != nullptr, "[SpriteRenderer] Already terminated.");
	SafeDelete(mSpriteBatch);
	SafeDelete(mCommonStates);
#else
	XASSERT(mVertexShader != nullptr, "[SpriteRenderer] Already terminated.");
	SafeRelease(mRasterizerState);
	SafeRelease(mDepthStencilState);
	SafeRelease(mBlendState);
	SafeRelease(mSamplerState);
	SafeRelease(mConstantBuffer);
	SafeRelease(mIndexBuffer);
	SafeRelease(mVertexBuffer);
	SafeRelease(mInputLayout);
	SafeRelease(mPixelShader);
	SafeRelease(mVertexShader);
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::SetTransform(const Math::Matrix4& transform)
{
	mTransform = transform;
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::BeginRender()
{
#ifdef _WIN32
	XASSERT(mSpriteBatch != nullptr, "[SpriteRenderer] Not initialized.");
	mSpriteBatch->Begin(
		DirectX::SpriteSortMode_Deferred,
		mCommonStates->NonPremultiplied(),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
		DirectX::XMMATRIX(
			mTransform._11, mTransform._12, mTransform._13, mTransform._14,
			mTransform._21, mTransform._22, mTransform._23, mTransform._24,
			mTransform._31, mTransform._32, mTransform._33, mTransform._34,
			mTransform._41, mTransform._42, mTransform._43, mTransform._44
		));
#else
	XASSERT(mVertexShader != nullptr, "[SpriteRenderer] Not initialized.");
	mSpriteCount = 0;
	mCurrentTexture = nullptr;
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::EndRender()
{
#ifdef _WIN32
	XASSERT(mSpriteBatch != nullptr, "[SpriteRenderer] Not initialized.");
	mSpriteBatch->End();

	// Restore state objects
	auto blendState = mCommonStates->Opaque();
	auto depthStencilState = mCommonStates->DepthDefault();
	auto rasterizerState = mCommonStates->CullCounterClockwise();

	ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();
	context->OMSetBlendState(blendState, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(depthStencilState, 0);
	context->RSSetState(rasterizerState);
#else
	XASSERT(mVertexShader != nullptr, "[SpriteRenderer] Not initialized.");
	
	// Flush any remaining sprites
	FlushBatch();
	
	// Restore default states
	ID3D11DeviceContext* context = GraphicsSystem::Get()->GetContext();
	context->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::Draw(const Texture& texture, const Math::Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
#ifdef _WIN32
	XASSERT(mSpriteBatch != nullptr, "[SpriteRenderer] Not initialized.");
	DirectX::XMFLOAT2 origin = { GetOrigin(texture.GetWidth(), texture.GetHeight(), pivot).x,
	                             GetOrigin(texture.GetWidth(), texture.GetHeight(), pivot).y };
	DirectX::SpriteEffects effects = GetSpriteEffects(flip);
	mSpriteBatch->Draw(texture.mShaderResourceView, ToXMFLOAT2(pos), nullptr, DirectX::Colors::White, rotation, origin, 1.0f, effects);
#else
	XASSERT(mVertexShader != nullptr, "[SpriteRenderer] Not initialized.");
	
	// If texture changed, flush current batch
	if (mCurrentTexture != texture.mShaderResourceView)
	{
		FlushBatch();
		mCurrentTexture = texture.mShaderResourceView;
	}
	
	// If batch is full, flush
	if (mSpriteCount >= kMaxSprites)
	{
		FlushBatch();
	}
	
	Float2 origin = GetOrigin(texture.GetWidth(), texture.GetHeight(), pivot);
	float width = static_cast<float>(texture.GetWidth());
	float height = static_cast<float>(texture.GetHeight());
	
	AddSprite(pos.x, pos.y, width, height, 0.0f, 0.0f, 1.0f, 1.0f, origin.x, origin.y, rotation, flip);
#endif
}

//----------------------------------------------------------------------------------------------------
void SpriteRenderer::Draw(const Texture& texture, const Math::Rect& sourceRect, const Math::Vector2& pos, float rotation, Pivot pivot, Flip flip)
{
#ifdef _WIN32
	XASSERT(mSpriteBatch != nullptr, "[SpriteRenderer] Not initialized.");
	RECT rect;
	rect.left = static_cast<LONG>(sourceRect.left);
	rect.top = static_cast<LONG>(sourceRect.top);
	rect.right = static_cast<LONG>(sourceRect.right);
	rect.bottom = static_cast<LONG>(sourceRect.bottom);
	DirectX::XMFLOAT2 origin = { GetOrigin(rect.right - rect.left, rect.bottom - rect.top, pivot).x,
	                             GetOrigin(rect.right - rect.left, rect.bottom - rect.top, pivot).y };
	DirectX::SpriteEffects effects = GetSpriteEffects(flip);
	mSpriteBatch->Draw(texture.mShaderResourceView, ToXMFLOAT2(pos), &rect, DirectX::Colors::White, rotation, origin, 1.0f, effects);
#else
	XASSERT(mVertexShader != nullptr, "[SpriteRenderer] Not initialized.");
	
	// If texture changed, flush current batch
	if (mCurrentTexture != texture.mShaderResourceView)
	{
		FlushBatch();
		mCurrentTexture = texture.mShaderResourceView;
	}
	
	// If batch is full, flush
	if (mSpriteCount >= kMaxSprites)
	{
		FlushBatch();
	}
	
	float texWidth = static_cast<float>(texture.GetWidth());
	float texHeight = static_cast<float>(texture.GetHeight());
	
	float srcWidth = sourceRect.right - sourceRect.left;
	float srcHeight = sourceRect.bottom - sourceRect.top;
	
	Float2 origin = GetOrigin(static_cast<uint32_t>(srcWidth), static_cast<uint32_t>(srcHeight), pivot);
	
	float u0 = sourceRect.left / texWidth;
	float v0 = sourceRect.top / texHeight;
	float u1 = sourceRect.right / texWidth;
	float v1 = sourceRect.bottom / texHeight;
	
	AddSprite(pos.x, pos.y, srcWidth, srcHeight, u0, v0, u1, v1, origin.x, origin.y, rotation, flip);
#endif
}

#ifndef _WIN32
void SpriteRenderer::AddSprite(float x, float y, float width, float height,
	float u0, float v0, float u1, float v1,
	float originX, float originY, float rotation, Flip flip)
{
	// Handle flip
	if (flip == Flip::Horizontal || flip == Flip::Both)
	{
		std::swap(u0, u1);
	}
	if (flip == Flip::Vertical || flip == Flip::Both)
	{
		std::swap(v0, v1);
	}
	
	// Calculate corner positions relative to origin
	float left = -originX;
	float top = -originY;
	float right = width - originX;
	float bottom = height - originY;
	
	// Apply rotation
	float cosR = cosf(rotation);
	float sinR = sinf(rotation);
	
	auto rotatePoint = [cosR, sinR, x, y](float px, float py) -> std::pair<float, float> {
		float rx = px * cosR - py * sinR + x;
		float ry = px * sinR + py * cosR + y;
		return { rx, ry };
	};
	
	auto [x0, y0] = rotatePoint(left, top);
	auto [x1, y1] = rotatePoint(right, top);
	auto [x2, y2] = rotatePoint(left, bottom);
	auto [x3, y3] = rotatePoint(right, bottom);
	
	// White color (tint)
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
	
	// Add vertices (4 per sprite)
	uint32_t baseVertex = mSpriteCount * 4;
	mVertices[baseVertex + 0] = { x0, y0, u0, v0, r, g, b, a };
	mVertices[baseVertex + 1] = { x1, y1, u1, v0, r, g, b, a };
	mVertices[baseVertex + 2] = { x2, y2, u0, v1, r, g, b, a };
	mVertices[baseVertex + 3] = { x3, y3, u1, v1, r, g, b, a };
	
	++mSpriteCount;
}

void SpriteRenderer::FlushBatch()
{
	if (mSpriteCount == 0 || mCurrentTexture == nullptr)
		return;
	
	GraphicsSystem* gs = GraphicsSystem::Get();
	ID3D11DeviceContext* context = gs->GetContext();
	
	// Update vertex buffer
	D3D11_MAPPED_SUBRESOURCE mapped;
	context->Map(mVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, mVertices, sizeof(SpriteVertex) * mSpriteCount * 4);
	context->Unmap(mVertexBuffer, 0);
	
	// Update constant buffer with transform
	uint32_t screenWidth = gs->GetBackBufferWidth();
	uint32_t screenHeight = gs->GetBackBufferHeight();
	
	// Create orthographic projection and apply user transform
	Math::Matrix4 projection(
		2.0f / screenWidth, 0.0f, 0.0f, 0.0f,
		0.0f, -2.0f / screenHeight, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 1.0f
	);
	Math::Matrix4 finalTransform = Math::Transpose(mTransform * projection);
	
	context->Map(mConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	memcpy(mapped.pData, &finalTransform, sizeof(Math::Matrix4));
	context->Unmap(mConstantBuffer, 0);
	
	// Set pipeline state
	context->IASetInputLayout(mInputLayout);
	UINT stride = sizeof(SpriteVertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	context->VSSetShader(mVertexShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &mConstantBuffer);
	
	context->PSSetShader(mPixelShader, nullptr, 0);
	context->PSSetShaderResources(0, 1, &mCurrentTexture);
	context->PSSetSamplers(0, 1, &mSamplerState);
	
	context->OMSetBlendState(mBlendState, nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(mDepthStencilState, 0);
	context->RSSetState(mRasterizerState);
	
	// Draw
	context->DrawIndexed(mSpriteCount * 6, 0, 0);
	
	// Reset batch
	mSpriteCount = 0;
}
#endif
