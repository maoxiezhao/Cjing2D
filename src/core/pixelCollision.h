#pragma once

#include"core\texture.h"
#include"utils\rectangle.h"
#include"utils\size.h"

#include<vector>

class Sprite;
class AnimationSprite;

/**
*	\brief ���ؼ���ײ���
*
*	ͨ��int32_t��mask���ݵ�ÿһλ����ʾĳһ�������Ƿ���ڣ���������������Ϊ32��
*	��ÿһ��int32_t mask��ײ���ʱ���������������������Ϊ0ʱ������������ײ��
*/
class PixelCollision
{
public:
	PixelCollision(TexturePtr texture, Rect& clipRect);

	/** ���òü���Χ��һ����animateSprite�л�����֡�������� */
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