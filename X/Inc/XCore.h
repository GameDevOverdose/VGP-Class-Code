//====================================================================================================
// Filename:	XCore.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_CORE_H
#define INCLUDED_XENGINE_CORE_H

#ifdef _WIN32
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#define NOGDI
	#include <Windows.h>
#else
	// Define MAX_PATH for non-Windows platforms
	#ifndef MAX_PATH
		#define MAX_PATH 260
	#endif
#endif

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>
#include <vector>

//----------------------------------------------------------------------------------------------------

#if defined(_DEBUG)
	#ifdef _WIN32
		#define XLOG(format, ...)\
			do {\
				char _buffer[1024];\
				int ret = _snprintf_s(_buffer, std::size(_buffer), _TRUNCATE, format, __VA_ARGS__);\
				OutputDebugStringA(_buffer);\
				if (ret == -1) OutputDebugStringA("** message truncated **\n");\
				OutputDebugStringA("\n");\
			} while (false)

		#define XASSERT(condition, format, ...)\
			do {\
				if (!(condition))\
				{\
					XLOG("%s(%d) "##format, __FILE__, __LINE__, __VA_ARGS__);\
					DebugBreak();\
				}\
			} while (false)

		#define XVERIFY(condition, format, ...)\
			do {\
				if (!(condition))\
				{\
					XLOG(format, __VA_ARGS__);\
					DebugBreak();\
				}\
			} while (false)
	#else
		// macOS/Linux debug macros
		#include <cstdio>
		#include <cassert>

		#define XLOG(format, ...)\
			do {\
				fprintf(stderr, format, ##__VA_ARGS__);\
				fprintf(stderr, "\n");\
			} while (false)

		#define XASSERT(condition, format, ...)\
			do {\
				if (!(condition))\
				{\
					fprintf(stderr, "%s(%d) " format, __FILE__, __LINE__, ##__VA_ARGS__);\
					fprintf(stderr, "\n");\
					assert(false);\
				}\
			} while (false)

		#define XVERIFY(condition, format, ...)\
			do {\
				if (!(condition))\
				{\
					fprintf(stderr, format, ##__VA_ARGS__);\
					fprintf(stderr, "\n");\
					assert(false);\
				}\
			} while (false)
	#endif
#else
	#define XLOG(format, ...)
	#define XASSERT(condition, format, ...) do { (void)(sizeof(condition)); } while (false)
	#define XVERIFY(condition, format, ...) condition
#endif

//----------------------------------------------------------------------------------------------------

namespace X {

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeDelete(T*& ptr)
{
	delete ptr;
	ptr = nullptr;
}

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeDeleteArray(T*& ptr)
{
	delete[] ptr;
	ptr = nullptr;
}

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeRelease(T*& ptr)
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_CORE_H
