#pragma once

#include"core\texture.h"
#include"utils\rectangle.h"
#include"utils\size.h"

#include<vector>

class Sprite;
class AnimationSprite;

/**
*	\brief 像素级碰撞检测
*
*	通过int32_t的mask数据的每一位来表示某一个像素是否存在，将像素数据缩减为32倍
*	对每一个int32_t mask碰撞检测时，仅需进行异或操作，当不为0时，即发生了碰撞。
*/
class PixelCollision
{
public:
	PixelCollision(TexturePtr texture, Rect& clipRect);

	/** 设置裁剪范围，一般在animateSprite中会随着帧动画噶边 */
	void SetClipRect(const Rect& rect);
	Rect GetClipRect()const;
	Size GetSize()const;
	std::vector<std::vector<int> >& GetPixelBits();

	bool TestCollision(PixelCollision& dst, Point2 srcPos, const Point2& dstPos);
private:
	void RecomputeMask();

	TexturePtr mTexture;
	Rect mClipRect;
	Size mSize;
	std::vector<std::vector<int> > mPixelBits;
};