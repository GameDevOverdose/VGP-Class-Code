//====================================================================================================
// Filename:	Timer.cpp
// Created by:	Peter Chan
//====================================================================================================

#include "Precompiled.h"
#include "Timer.h"

using namespace X;

//----------------------------------------------------------------------------------------------------

Timer::Timer()
	: mElapsedTime(0.0f)
	, mTotalTime(0.0f)
	, mLastUpdateTime(0.0f)
	, mFrameSinceLastSecond(0.0f)
	, mFramesPerSecond(0.0f)
{
}

//----------------------------------------------------------------------------------------------------

void Timer::Initialize()
{
	mStartTime = Clock::now();
	mCurrentTick = mStartTime;
	mLastTick = mCurrentTick;
	
	// Reset
	mElapsedTime = 0.0f;
	mTotalTime = 0.0f;
	mLastUpdateTime = 0.0f;
	mFrameSinceLastSecond = 0.0f;
	mFramesPerSecond = 0.0f;
}

//----------------------------------------------------------------------------------------------------

void Timer::Update()
{
	// Get the current tick count
	mCurrentTick = Clock::now();

	// Calculate elapsed time in seconds
	auto elapsed = std::chrono::duration<float>(mCurrentTick - mLastTick);
	mElapsedTime = elapsed.count();
	
	// Calculate total time
	auto total = std::chrono::duration<float>(mCurrentTick - mStartTime);
	mTotalTime = total.count();

	// Update the last tick count
	mLastTick = mCurrentTick;

	// Calculate the FPS
	mFrameSinceLastSecond += 1.0f;
	if (mTotalTime >= mLastUpdateTime + 1.0f)
	{
		mFramesPerSecond = mFrameSinceLastSecond / (mTotalTime - mLastUpdateTime);
		mFrameSinceLastSecond = 0.0f;
		mLastUpdateTime = mTotalTime;
	}
}

//----------------------------------------------------------------------------------------------------

float Timer::GetElapsedTime() const
{
	return mElapsedTime;
}

//----------------------------------------------------------------------------------------------------

float Timer::GetTotalTime() const
{
	return mTotalTime;
}

//----------------------------------------------------------------------------------------------------

float Timer::GetFramesPerSecond() const
{
	return mFramesPerSecond;
}
