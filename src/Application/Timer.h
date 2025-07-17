#pragma once

// Includes
//--------------------------------------------------------------------------------
#include <cstddef>
#include <cmath>
#include <algorithm>

//--------------------------------------------------------------------------------
class Timer
{
public:
	Timer(float frequencyHz, float baselineFps = 60.0f, float overrunFactor = 2.0f)
		: mFrequencyHz(frequencyHz)
		, mIntervalSec(1.0f / frequencyHz)
		, mElapsedTime(0.0f)
	{
		mMaxStepsPerFrame = static_cast<size_t>(
			std::ceil((frequencyHz / baselineFps) * overrunFactor));
	}

	size_t ComputeStepCount(float deltaTime)
	{
		mElapsedTime += deltaTime;
		size_t steps = static_cast<size_t>(mElapsedTime / mIntervalSec);
		steps = std::min(steps, mMaxStepsPerFrame);
		mElapsedTime -= steps * mIntervalSec;

		return steps;
	}

private:
	float mFrequencyHz;
	float mIntervalSec;
	float mElapsedTime;
	size_t mMaxStepsPerFrame;
};