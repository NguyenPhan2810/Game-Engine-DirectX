#pragma once

#include <chrono>

// base unit of this timer's measurement is second
class GameTimer
{
public:
	GameTimer();

	static void SetFixedDeltaTime(float dt);

	static float TotalTime();
	static float DeltaTime();
	static float FixedDeltaTime();

	static void Reset(); // Call before main loop
	static void Start(); // Call when unpaused
	static void Stop(); // Call when paused
	static void Tick(); // Call every frame

protected:
	static std::chrono::system_clock::time_point now();

	// Return time differences in second
	static double TimeDiff(std::chrono::system_clock::time_point before,
		std::chrono::system_clock::time_point after);

protected:
	static double mFixedDeltaTime; // fixed dt for needs
	static double mDeltaTime; // Time since last frame/tick
	static double mPausedTime; // Total time stopped since mBaseTime

	//					<--mPausedTime-->
	// -----*-----------*---------------*---------------*---------
	// mBaseTime	mStopTime		mStartTime		mCurrentTime
	static std::chrono::system_clock::time_point mBaseTime;
	static std::chrono::system_clock::time_point mStopTime;
	static std::chrono::system_clock::time_point mPrevTime;
	static std::chrono::system_clock::time_point mCurrentTime;

	static bool mStopped;

private:
	// Store total time so method TotalTime doesnt have to recalculate
	// everytime it's called during a frame
	static double mTotalTime;
};