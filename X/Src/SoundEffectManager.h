//====================================================================================================
// Filename:	SoundEffectManager.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_SOUNDEFFECTMANAGER_H
#define INCLUDED_XENGINE_SOUNDEFFECTMANAGER_H

#include "XTypes.h"

#ifdef _WIN32
namespace DirectX { class SoundEffect; class SoundEffectInstance; }
#endif

namespace X {

class SoundEffectManager
{
public:
	static void StaticInitialize(const char* root);
	static void StaticTerminate();
	static SoundEffectManager* Get();

public:
	SoundEffectManager();
	~SoundEffectManager();

	SoundEffectManager(const SoundEffectManager&) = delete;
	SoundEffectManager& operator=(const SoundEffectManager&) = delete;

	void SetRootPath(const char* path);

	SoundId Load(const char* fileName);
	void Clear();

	void Play(SoundId id, bool loop = false);
	void Stop(SoundId id);

private:
#ifdef _WIN32
	struct Entry
	{
		std::unique_ptr<DirectX::SoundEffect> effect;
		std::unique_ptr<DirectX::SoundEffectInstance> instance;
	};
	std::unordered_map<std::size_t, std::unique_ptr<Entry>> mInventory;
#else
	std::unordered_map<std::size_t, int> mInventory; // Stub
#endif

	std::string mRoot;
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_SOUNDEFFECTMANAGER_H
