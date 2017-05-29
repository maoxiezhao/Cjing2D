#ifndef _CJING_ANIMATION_H_
#define _CJING_ANIMATION_H_

#include"common\common.h"
#include"game\animationData.h"
#include"core\texture.h"
#include"utils\rectangle.h"
#include"core\debug.h"
#include<deque>

/**
*	\brief animation∂Øª≠–≈œ¢
*/
class Animation
{
public:
	Animation();
	Animation(const string& imageName, uint32_t frameDelay, int frameLoop, const std::deque<AnimationDirectionData>& directions);

	string GetImageName()const;
	TexturePtr GetTexture()const;
	uint32_t GetFrameDelay()const;
	int GetFrameLoop()const;
	Rect GetAniamtionRect(int currDirection)const;
	int  GetNumDirections()const;
	int  GetNextFrame(int currFrame,int currDirection)const;
	const AnimationDirectionData& GetDirection(int direction)const;

private:
	string mImageName;
	TexturePtr mTexture;
	uint32_t mFrameDelay;
	int mFrameLoop;
	int mCurrDirection;
	std::deque<AnimationDirectionData> mDirections;

};

inline const AnimationDirectionData& Animation::GetDirection(int direction)const
{
	Debug::CheckAssertion(direction >= 0 && direction < GetNumDirections(),
		"Invalid animation direction.");
	return mDirections[direction];
}


#endif