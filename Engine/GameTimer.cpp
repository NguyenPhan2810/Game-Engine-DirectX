#include "GameTimer.h"


GameTimer::GameTimer()
: mDeltaTime(-1.0)
, mPausedTime(0.0)
, mBaseTime()
, mStopTime()
, mPrevTime()
, mCurrentTime()
, mStopped(false)
{
}

float GameTimer::TotalTime() const
{
    if (mStopped)
    {
        return TimeDiff(mBaseTime, mStopTime) - mPausedTime;
    }
    else
    {
        return TimeDiff(mBaseTime, mCurrentTime) - mPausedTime;
    }
}

float GameTimer::DeltaTime() const
{
    return mDeltaTime;
}

void GameTimer::Reset()
{
    mDeltaTime = -1;
    mPausedTime = 0;
    mStopped = false;
    mBaseTime = now();
    mCurrentTime = now();
    mPrevTime = mCurrentTime;
}

void GameTimer::Start()
{
    if (mStopped)
    {
        auto startTime = now();

        mPausedTime += TimeDiff(mStopTime, startTime);
        mPrevTime = startTime;
        mStopped = false;
    }
}

void GameTimer::Stop()
{
    if (!mStopped)
    {
        mStopTime = mCurrentTime;
        mStopped = true;
    }
}

void GameTimer::Tick()
{
    if (mStopped)
    {
        mDeltaTime = 0.0;
        return;
    }


    mCurrentTime = now();
    
    mDeltaTime = TimeDiff(mPrevTime, mCurrentTime);

    mPrevTime = mCurrentTime;

    // THIS IS REALLY IMPORTANT
    // Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the 
    // processor goes into a power save mode or we get shuffled to another
    // processor, then mDeltaTime can be negative.
    if (mDeltaTime < 0.0)
    {
        mDeltaTime = 0.0;
    }
}

std::chrono::system_clock::time_point GameTimer::now()
{
    return std::chrono::system_clock::now();
}

double GameTimer::TimeDiff(std::chrono::system_clock::time_point before, std::chrono::system_clock::time_point after)
{
    return std::chrono::duration<double>((after - before)).count();
}
