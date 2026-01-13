//====================================================================================================
// Filename:	SpriteRenderer.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_SPRITERENDERER_H
#define INCLUDED_XENGINE_SPRITERENDERER_H

#include "XMath.h"
#include "XTypes.h"

#ifdef _WIN32
namespace DirectX { class CommonStates; class SpriteBatch; }
#endif

namespace X {

class Texture;

class SpriteRenderer
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static SpriteRenderer* Get();

public:
	SpriteRenderer();
	~SpriteRenderer();

	SpriteRenderer(const SpriteRenderer&) = delete;
	SpriteRenderer& operator=(const SpriteRenderer&) = delete;

	void Initialize();
	void Terminate();

	void SetTransform(const Math::Matrix4& transform);

	void BeginRender();
	void EndRender();

	void Draw(const Texture& texture, const Math::Vector2& pos, float rotation = 0.0f, Pivot pivot = Pivot::Center, Flip flip = Flip::None);
	void Draw(const Texture& texture, const Math::Rect& sourceRect, const Math::Vector2& pos, float rotation = 0.0f, Pivot pivot = Pivot::Center, Flip flip = Flip::None);

private:
	friend class Font;

#ifdef _WIN32
	DirectX::CommonStates* mCommonStates;
	DirectX::SpriteBatch* mSpriteBatch;
#else
	// Sprite vertex format for macOS custom renderer
	struct SpriteVertex
	{
		float x, y;       // Position
		float u, v;       // Texture coordinates
		float r, g, b, a; // Color
	};
	
	static constexpr uint32_t kMaxSprites = 2048;
	
	// Shaders and input layout
	ID3D11VertexShader* mVertexShader;
	ID3D11PixelShader* mPixelShader;
	ID3D11InputLayout* mInputLayout;
	
	// Buffers
	ID3D11Buffer* mVertexBuffer;
	ID3D11Buffer* mIndexBuffer;
	ID3D11Buffer* mConstantBuffer;
	
	// States
	ID3D11SamplerState* mSamplerState;
	ID3D11BlendState* mBlendState;
	ID3D11DepthStencilState* mDepthStencilState;
	ID3D11RasterizerState* mRasterizerState;
	
	// Batching
	SpriteVertex mVertices[kMaxSprites * 4];
	uint32_t mSpriteCount = 0;
	ID3D11ShaderResourceView* mCurrentTexture = nullptr;
	
	void AddSprite(float x, float y, float width, float height,
		float u0, float v0, float u1, float v1,
		float originX, float originY, float rotation, Flip flip);
	void FlushBatch();
#endif

	Math::Matrix4 mTransform;
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_SPRITERENDERER_H
