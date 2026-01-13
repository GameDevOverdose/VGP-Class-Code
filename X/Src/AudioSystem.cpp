//====================================================================================================
// Filename:	AudioSystem.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "AudioSystem.h"

#ifdef _WIN32
#include <DirectXTK/Inc/Audio.h>
#endif

using namespace X;

namespace
{
	AudioSystem* sAudioSystem = nullptr;
}

void AudioSystem::StaticInitialize()
{
	XASSERT(sAudioSystem == nullptr, "[AudioSystem] System already initialized!");
	sAudioSystem = new AudioSystem();
	sAudioSystem->Initialize();
}

//----------------------------------------------------------------------------------------------------

void AudioSystem::StaticTerminate()
{
	if (sAudioSystem != nullptr)
	{
		sAudioSystem->Terminate();
		SafeDelete(sAudioSystem);
	}
}

//----------------------------------------------------------------------------------------------------

AudioSystem* AudioSystem::Get()
{
	XASSERT(sAudioSystem != nullptr, "[AudioSystem] No system registered.");
	return sAudioSystem;
}

//----------------------------------------------------------------------------------------------------

AudioSystem::AudioSystem()
	: mAudioEngine(nullptr)
{
}

//----------------------------------------------------------------------------------------------------

AudioSystem::~AudioSystem()
{
	XASSERT(mAudioEngine == nullptr, "[AudioSystem] Terminate() must be called to clean up!");
}

//----------------------------------------------------------------------------------------------------

void AudioSystem::Initialize()
{
	XASSERT(mAudioEngine == nullptr, "[AudioSystem] System already initialized.");

#ifdef _WIN32
	DirectX::AUDIO_ENGINE_FLAGS flags = DirectX::AudioEngine_Default;
#if defined(_DEBUG)
	flags = flags | DirectX::AudioEngine_Debug;
#endif
	mAudioEngine = new DirectX::AudioEngine(flags);
#else
	// Audio not supported on this platform
	XLOG("[AudioSystem] Audio not supported on this platform.");
#endif
}

//----------------------------------------------------------------------------------------------------

void AudioSystem::Terminate()
{
#ifdef _WIN32
	SafeDelete(mAudioEngine);
#endif
	mAudioEngine = nullptr;
}

//----------------------------------------------------------------------------------------------------

void AudioSystem::Update()
{
#ifdef _WIN32
	if (mAudioEngine && !mAudioEngine->Update())
	{
		// No audio device is active
		if (mAudioEngine->IsCriticalError())
		{
			XLOG("[AudioSystem] Critical Error. Shutting down.");
			SafeDelete(mAudioEngine);
		}
	}
#endif
}
