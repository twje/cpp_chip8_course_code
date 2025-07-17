#pragma once

//--------------------------------------------------------------------------------
// System
#include <vector>
#include <string>

//--------------------------------------------------------------------------------
class TextSpinner
{
public:	
	TextSpinner(std::vector<std::string> frames, float frameDurationSeconds)
		: mFrames(std::move(frames))
		, mFrameDuration(frameDurationSeconds)
		, mCurrentIndex(0)
		, mTimeAccumulator(0.0f)
	{
		if (mFrames.empty())
			mFrames.emplace_back(""); // fallback to empty
	}

	const std::string& Update(float deltaTime)
	{
		mTimeAccumulator += deltaTime;

		while (mTimeAccumulator >= mFrameDuration)
		{
			mTimeAccumulator -= mFrameDuration;
			mCurrentIndex = (mCurrentIndex + 1) % mFrames.size();
		}

		return mFrames[mCurrentIndex];
	}

private:
	std::vector<std::string> mFrames;
	float mFrameDuration;
	std::size_t mCurrentIndex;
	float mTimeAccumulator;
};