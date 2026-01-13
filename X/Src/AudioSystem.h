//====================================================================================================
// Filename:	AudioSystem.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_AUDIOSYSTEM_H
#define INCLUDED_XENGINE_AUDIOSYSTEM_H

#ifdef _WIN32
namespace DirectX { class AudioEngine; }
#endif

namespace X {

class AudioSystem
{
public:
	static void StaticInitialize();
	static void StaticTerminate();
	static AudioSystem* Get();

public:
	AudioSystem();
	~AudioSystem();

	AudioSystem(const AudioSystem&) = delete;
	AudioSystem& operator=(const AudioSystem&) = delete;

	void Initialize();
	void Terminate();

	void Update();

private:
	friend class SoundEffectManager;
	
#ifdef _WIN32
	DirectX::AudioEngine* mAudioEngine;
#else
	void* mAudioEngine;
#endif
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_AUDIOSYSTEM_H
