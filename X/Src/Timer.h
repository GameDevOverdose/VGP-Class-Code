//====================================================================================================
// Filename:	Timer.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_TIMER_H
#define INCLUDED_XENGINE_TIMER_H

#include <chrono>

namespace X {

class Timer
{
public:
	Timer();

	void Initialize();
	void Update();

	float GetElapsedTime() const;
	float GetTotalTime() const;
	float GetFramesPerSecond() const;

private:
	using Clock = std::chrono::high_resolution_clock;
	using TimePoint = std::chrono::time_point<Clock>;

	TimePoint mStartTime;
	TimePoint mLastTick;
	TimePoint mCurrentTick;
	
	float mElapsedTime;
	float mTotalTime;
	
	float mLastUpdateTime;
	float mFrameSinceLastSecond;
	float mFramesPerSecond;
};

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_TIMER_H
