//====================================================================================================
// Filename:	XTypes.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_TYPES_H
#define INCLUDED_XENGINE_TYPES_H

namespace X
{
	using GameLoop = std::function<bool(float)>;

	using SoundId = std::size_t;
	using TextureId = std::size_t;

	enum class Pivot
	{
		TopLeft,
		Top,
		TopRight,
		Left,
		Center,
		Right,
		BottomLeft,
		Bottom,
		BottomRight
	};

	enum class Flip
	{
		None,
		Horizontal,
		Vertical,
		Both
	};

	namespace Keys
	{
		// Key codes - using ASCII values where applicable for cross-platform compatibility
		// Letters use ASCII codes (65-90 for A-Z)
		// Numbers use ASCII codes (48-57 for 0-9)

		// Function keys (using values above normal key range)
		const int F1			= 290;
		const int F2			= 291;
		const int F3			= 292;
		const int F4			= 293;
		const int F5			= 294;
		const int F6			= 295;
		const int F7			= 296;
		const int F8			= 297;
		const int F9			= 298;
		const int F10			= 299;
		const int F11			= 300;
		const int F12			= 301;

		// Keyboard roll 1
		const int ESCAPE		= 256;

		// Keyboard roll 2
		const int GRAVE			= 96;  // `
		const int ONE			= '1';
		const int TWO			= '2';
		const int THREE			= '3';
		const int FOUR			= '4';
		const int FIVE			= '5';
		const int SIX			= '6';
		const int SEVEN			= '7';
		const int EIGHT			= '8';
		const int NINE			= '9';
		const int ZERO			= '0';
		const int MINUS			= 45;  // -
		const int EQUALS		= 61;  // =
		const int BACKSPACE		= 259;

		// Keyboard roll 3
		const int TAB			= 258;
		const int Q				= 'Q';
		const int W				= 'W';
		const int E				= 'E';
		const int R				= 'R';
		const int T				= 'T';
		const int Y				= 'Y';
		const int U				= 'U';
		const int I				= 'I';
		const int O				= 'O';
		const int P				= 'P';
		const int LBRACKET		= 91;  // [
		const int RBRACKET		= 93;  // ]
		const int BACKSLASH		= 92;  // backslash

		// Keyboard roll 4
		const int A				= 'A';
		const int S				= 'S';
		const int D				= 'D';
		const int F				= 'F';
		const int G				= 'G';
		const int H				= 'H';
		const int J				= 'J';
		const int K				= 'K';
		const int L				= 'L';
		const int SEMICOLON		= 59;  // ;
		const int APOSTROPHE	= 39;  // '
		const int RETURN		= 257;
		const int ENTER			= 257;

		// Keyboard roll 5
		const int Z				= 'Z';
		const int X				= 'X';
		const int C				= 'C';
		const int V				= 'V';
		const int B				= 'B';
		const int N				= 'N';
		const int M				= 'M';
		const int COMMA			= 44;  // ,
		const int PERIOD		= 46;  // .
		const int SLASH			= 47;  // /

		// Lock keys
		const int CAPSLOCK		= 280;
		const int NUMLOCK		= 282;
		const int SCROLLLOCK	= 281;

		// Numpad
		const int NUMPAD0		= 320;
		const int NUMPAD1		= 321;
		const int NUMPAD2		= 322;
		const int NUMPAD3		= 323;
		const int NUMPAD4		= 324;
		const int NUMPAD5		= 325;
		const int NUMPAD6		= 326;
		const int NUMPAD7		= 327;
		const int NUMPAD8		= 328;
		const int NUMPAD9		= 329;
		const int ADD			= 334;
		const int SUBTRACT		= 333;
		const int MULTIPLY		= 332;
		const int DIVIDE		= 331;
		const int NUMPADENTER	= 335;
		const int DECIMAL		= 330;
		// Legacy names
		const int NUM_ADD		= ADD;
		const int NUM_SUB		= SUBTRACT;
		const int NUM_MUL		= MULTIPLY;
		const int NUM_DIV		= DIVIDE;
		const int NUM_ENTER		= NUMPADENTER;
		const int NUM_DECIMAL	= DECIMAL;

		// Navigation keys
		const int INSERT		= 260;
		const int DEL			= 261;
		const int HOME			= 268;
		const int END			= 269;
		const int PGUP			= 266;
		const int PGDN			= 267;
		// Legacy name
		const int INS			= INSERT;

		// Support keys
		const int LSHIFT		= 340;
		const int RSHIFT		= 344;
		const int LCONTROL		= 341;
		const int RCONTROL		= 345;
		const int LALT			= 342;
		const int RALT			= 346;
		const int LWIN			= 343;
		const int RWIN			= 347;
		const int SPACE			= 32;

		// Arrow keys
		const int UP			= 265;
		const int DOWN			= 264;
		const int LEFT			= 263;
		const int RIGHT			= 262;
	}

	namespace Mouse
	{
		const int LBUTTON = 0;
		const int RBUTTON = 1;
		const int MBUTTON = 2;
	}
}

#endif // #ifndef INCLUDED_XENGINE_TYPES_H
