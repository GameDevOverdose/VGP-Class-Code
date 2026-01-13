//====================================================================================================
// Filename:	Font.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_FONT_H
#define INCLUDED_XENGINE_FONT_H

namespace X {

class Font
{
public:
	Font();
	~Font();

	void Initialize();
	void Terminate();

	float GetStringWidth(const wchar_t* str, float size) const;
	void Draw(const wchar_t* str, float x, float y, float size, uint32_t color);
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_FONT_H
