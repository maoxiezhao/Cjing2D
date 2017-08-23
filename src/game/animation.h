#ifndef _CJING_ANIMATION_H_
#define _CJING_ANIMATION_H_

#include"common\common.h"
#include"game\animationData.h"
#include"core\texture.h"
#include"utils\rectangle.h"
#include"core\debug.h"
#include<deque>

/**
*	\brief animation方向数据
*/
class AnimationDirection
{
public:
	AnimationDirection(const std::vector<Rect>& frameRect,const Point2& orgin);

	Size GetSize()const;
	int GetNumFrames()const;
	Rect GetFrameRect(int frame)const;

private:
	Point2 mOrgin;
	std::vector<Rect> mFrameRects;

};

/**
*	\brief animation动画信息
*/
class Animation
{
public:
	Animation();
	Animation(const string& imageName, uint32_t frameDelay, int frameLoop, const std::deque<AnimationDirection>& directions);

	/***** ***** ****** getter/setter ****** ***** *****/
	string GetImageName()const;
	TexturePtr GetTexture()const;
	uint32_t GetFrameDelay()const;
	int GetFrameLoop()const;
	int GetNumFrame(int currDirection)const;
	Rect GetAniamtionRect(int currFrame,int currDirection)const;
	int  GetNumDirections()const;
	int  GetNextFrame(int currFrame,int currDirection)const;
	const AnimationDirection& GetDirection(int direction)const;

private:
	void InitTextureWithImageSrc();

	string mImageName;
	TexturePtr mTexture;
	uint32_t mFrameDelay;
	int mFrameLoop;
	int mCurrDirection;
	std::deque<AnimationDirection> mDirections;

};

inline const AnimationDirection& Animation::GetDirection(int direction)const
{
	Debug::CheckAssertion(direction >= 0 && direction < GetNumDirections(),
		"Invalid animation direction.");
	return mDirections[direction];
}



#endif