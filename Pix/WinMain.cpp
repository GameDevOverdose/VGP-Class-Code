//====================================================================================================
// Filename:	WinMain.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "PixEditor.h"
#include <filesystem>

#ifdef _WIN32
int CALLBACK WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// Set working directory to executable location
	wchar_t path[MAX_PATH];
	GetModuleFileNameW(nullptr, path, MAX_PATH);
	std::filesystem::current_path(std::filesystem::path(path).parent_path());
#else
int main(int argc, char* argv[])
{
	// Set working directory to executable location
	std::filesystem::current_path(std::filesystem::path(argv[0]).parent_path());
#endif

	PixEditor editor;

	X::Start("xconfig.json");
	editor.Initialize();

	X::Run([&](float deltaTime)
	{
		return editor.Run(deltaTime);
	});

	editor.Terminate();
	X::Stop();
	return 0;
}
