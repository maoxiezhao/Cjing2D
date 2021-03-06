#include "animation.h"
#include "core\debug.h"
#include "core\fileData.h"
#include "core\resourceCache.h"
#include <sstream>


AnimationDirection::AnimationDirection(const std::vector<Rect>& frameRect, const Point2& orgin):
	mFrameRects(frameRect),
	mOrgin(orgin)
{
}

Size AnimationDirection::GetSize() const
{
	Debug::CheckAssertion(mFrameRects.size() > 0, "Invaild animation rect.");
	return {mFrameRects[0].GetSize().width,
			mFrameRects[0].GetSize().height};
}

int AnimationDirection::GetNumFrames() const
{
	return mFrameRects.size();
}

Rect AnimationDirection::GetFrameRect(int frame) const
{
	if (frame < 0 || frame >=  GetNumFrames() )
	{
		std::ostringstream oss;
		oss << "Invaild frame " << frame <<
			",the maxinum frame" << GetNumFrames();
	}
	return mFrameRects[frame];
}

//////////////////////////////////////////////////

Animation::Animation():
	mFrameDelay(0),
	mFrameLoop(-1)
{
}

Animation::Animation(const string & imageName, uint32_t frameDelay, int frameLoop, const std::deque<AnimationDirection>& directions):
	mImageName(imageName),
	mFrameDelay(frameDelay),
	mFrameLoop(frameLoop),
	mDirections(directions)
{
	InitTextureWithImageSrc();
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

int Animation::GetNumFrame(int currDirection) const
{
	if (currDirection < 0 || currDirection >= GetNumDirections())
		Debug::Die("Invaild directions in Animation::GetNumFrame()");

	return mDirections[currDirection].GetNumFrames();
}

/**
*	\brief 返会方向的个数
*/
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
		nextFrame = mFrameLoop;

	return nextFrame;
}

/**
*	\brief 根据信息返回当前矩形
*/
Rect Animation::GetAniamtionRect(int currFrame,int currDirection) const
{
	if (currDirection < 0 || currDirection >= GetNumDirections())
		Debug::Die("Invaild directions in Animation::GetANimationRect.");

	Rect animationRect;
	AnimationDirection currAnimaDirection = mDirections[currDirection];

	return currAnimaDirection.GetFrameRect(currFrame);
}

/**
*	\brief 根据当前srcImage初始化texture,上级目录必然是sprites/
*/
void Animation::InitTextureWithImageSrc()
{
	std::string prefix = "sprites/";
	std::string prefixedFileName = prefix + mImageName;

	if (!FileData::IsFileExists(prefixedFileName) )
	{
		Debug::Error("The srcImage '" + mImageName + "' in animation is not existed.");
		return;
	}

	mTexture = ResourceCache::GetInstance().LoadTexture2D(prefixedFileName);
	if (mTexture == nullptr)
	{
		Debug::Error("The srcImage '" + mImageName + "' in animation initialize texture failed.");
		return;
	}
}
