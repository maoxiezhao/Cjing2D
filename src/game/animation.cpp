#include "animation.h"

Animation::Animation()
{
}

Animation::Animation(const string & imageName, uint32_t frameDelay, int frameLoop, const std::deque<AnimationDirectionData>& directions):
	mImageName(imageName),
	mFrameDelay(frameDelay),
	mFrameLoop(frameLoop),
	mDirections(directions)
{
}

string Animation::GetImageName() const
{
	return mImageName;
}

TexturePtr Animation::GetTexture() const
{
	return mTexture;
}

uint32_t Animation::GetFrameDelay() const
{
	return mFrameDelay;
}

int Animation::GetFrameLoop() const
{
	return mFrameLoop;
}

int Animation::GetNumDirections() const
{
	return mDirections.size();
}

/**
*	\brief 返回下一帧的值
*/
int Animation::GetNextFrame(int currFrame, int currDirection) const
{
	if (currDirection < 0 || currDirection >= GetNumDirections())
		Debug::Die("Invaild directions in Animation::GetNextFrame()");

	int nextFrame = currFrame + 1;
	if (nextFrame == mDirections[currDirection].GetNumFrames())
		nextFrame == mFrameLoop;

	return nextFrame;
}

/**
*	\brief 根据信息返回当前矩形
*/
Rect Animation::GetAniamtionRect() const
{
	return Rect();
}

