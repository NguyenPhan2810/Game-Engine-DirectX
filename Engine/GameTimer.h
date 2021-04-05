#pragma once

#include <chrono>

// base unit of this timer's measurement is second
class GameTimer
{
public:
	GameTimer();

	float TotalTime() const;
	float DeltaTime() const;

	void Reset(); // Call before main loop
	void Start(); // Call when unpaused
	void Stop(); // Call when paused
	void Tick(); // Call every frame

protected:
	std::chrono::system_clock::time_point now();

	// Return time differences in second
	static double TimeDiff(std::chrono::system_clock::time_point before,
		std::chrono::system_clock::time_point after);

protected:
	double mDeltaTime; // Time since last frame/tick
	double mPausedTime; // Total time stopped since mBaseTime

	//					<--mPausedTime-->
	// -----*-----------*---------------*---------------*---------
	// mBaseTime	mStopTime		mStartTime		mCurrentTime
	std::chrono::system_clock::time_point mBaseTime;
	std::chrono::system_clock::time_point mStopTime;
	std::chrono::system_clock::time_point mPrevTime;
	std::chrono::system_clock::time_point mCurrentTime;

	bool mStopped;
};