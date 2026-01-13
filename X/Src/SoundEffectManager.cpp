//====================================================================================================
// Filename:	SoundEffectManager.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "SoundEffectManager.h"

#include "AudioSystem.h"

#ifdef _WIN32
#include <DirectXTK/Inc/Audio.h>
using namespace DirectX;
#endif

using namespace X;

namespace
{
	SoundEffectManager* sSoundEffectManager = nullptr;
}

void SoundEffectManager::StaticInitialize(const char* root)
{
	XASSERT(sSoundEffectManager == nullptr, "[SoundEffectManager] Manager already initialized!");
	sSoundEffectManager = new SoundEffectManager();
	sSoundEffectManager->SetRootPath(root);
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::StaticTerminate()
{
	if (sSoundEffectManager != nullptr)
	{
		sSoundEffectManager->Clear();
		SafeDelete(sSoundEffectManager);
	}
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager* SoundEffectManager::Get()
{
	XASSERT(sSoundEffectManager != nullptr, "[SoundEffectManager] No instance registered.");
	return sSoundEffectManager;
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager::SoundEffectManager()
{
}

//----------------------------------------------------------------------------------------------------

SoundEffectManager::~SoundEffectManager()
{
	XASSERT(mInventory.empty(), "[SoundEffectManager] Clear() must be called to clean up.");
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::SetRootPath(const char* path)
{
	mRoot = path;
}

//----------------------------------------------------------------------------------------------------

SoundId SoundEffectManager::Load(const char* fileName)
{
#ifdef _WIN32
	std::string fullName = mRoot + "/" + fileName;

	std::hash<std::string> hasher;
	SoundId hash = hasher(fullName);

	auto result = mInventory.insert({ hash, nullptr });
	if (result.second)
	{
		wchar_t wbuffer[1024];
		mbstowcs_s(nullptr, wbuffer, fullName.c_str(), 1024);

		auto entry = std::make_unique<Entry>();
		entry->effect = std::make_unique<SoundEffect>(AudioSystem::Get()->mAudioEngine, wbuffer);
		entry->instance = entry->effect->CreateInstance();
		result.first->second = std::move(entry);
	}

	return hash;
#else
	(void)fileName;
	return 0;
#endif
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Clear()
{
#ifdef _WIN32
	AudioSystem::Get()->mAudioEngine->Suspend();

	for (auto& item : mInventory)
	{
		if (item.second)
		{
			item.second->instance->Stop();
			item.second->instance.reset();
			item.second->effect.reset();
			item.second.reset();
		}
	}
#endif
	mInventory.clear();
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Play(SoundId id, bool loop)
{
#ifdef _WIN32
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		if (loop)
		{
			iter->second->instance->Play(true);
		}
		else
		{
			iter->second->effect->Play();
		}
	}
#else
	(void)id;
	(void)loop;
#endif
}

//----------------------------------------------------------------------------------------------------

void SoundEffectManager::Stop(SoundId id)
{
#ifdef _WIN32
	auto iter = mInventory.find(id);
	if (iter != mInventory.end())
	{
		iter->second->instance->Stop(true);
	}
#else
	(void)id;
#endif
}
