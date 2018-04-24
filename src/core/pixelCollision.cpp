#include "pixelCollision.h"

PixelCollision::PixelCollision(TexturePtr texture, Rect & clipRect) :
	mTexture(texture),
	mClipRect(clipRect),
	mSize(clipRect.GetSize()),
	mPixelBits()
{
	RecomputeMask();
}

void PixelCollision::SetClipRect(const Rect & rect)
{
	if (rect != mClipRect)
	{
		mClipRect = rect;
		RecomputeMask();
	}
}

Rect PixelCollision::GetClipRect() const
{
	return mClipRect;
}

Size PixelCollision::GetSize() const
{
	return mSize;
}

std::vector<std::vector<int>>& PixelCollision::GetPixelBits() 
{
	return mPixelBits;
}

/**
*	\brief 像素级精灵碰撞检测
*/
bool PixelCollision::TestCollision(PixelCollision & dst, Point2 srcPos, const Point2 & dstPos)
{
	Rect srcRect(srcPos, mSize);
	Rect dstRect(dstPos, dst.GetSize());

	if (!srcRect.Overlaps(dstRect))
		return false;

	// 获取两个矩形的相交区域
	int minX = std::min(srcPos.x, dstPos.x);
	int minY = std::min(srcPos.y, dstPos.y);
	int maxX = std::max(srcPos.x + srcRect.GetSize().width, dstPos.y + dstRect.GetSize().width);
	int maxY = std::max(srcPos.y + srcRect.GetSize().height, dstPos.y + dstRect.GetSize().height);
	Rect intersection(minX, minY, maxX, maxY);
	/*----------------------------
			__________________
			|dx,dy			 |
	________|________	     |
	|sx,sy  |ix,iy	|        |
	|		|       |        |
	|		|	    |        |
	|       |_______|________|
	|               |
	|               |
	|               |
	|_______________|
	*///---------------------------
	auto& dstPixelBits = dst.GetPixelBits();

	// leftRow始终指向的是左边的rect,rightRow同理,对于右边的rect
	// 像素数据始终是从0开始的，则我们以rightRow作为参照
	using row_iterator = std::vector<std::vector<int>>::const_iterator;
	row_iterator leftRow;
	row_iterator rightRow;

	int leftUnusedMaskNum = 0;
	int leftUnusedBitNum = 0;
	if (srcPos.x < dstPos.x)
	{
		leftRow = mPixelBits.begin() + (intersection.GetPos().y - srcPos.y);
		rightRow = dstPixelBits.begin() + (intersection.GetPos().y - dstPos.y);
		
		int offsetX = (intersection.GetPos().x - srcPos.x);
		leftUnusedMaskNum = offsetX >> 5;
		leftUnusedBitNum = 32 - (offsetX & 31);
	}
	else
	{
		rightRow = mPixelBits.begin() + (intersection.GetPos().y - srcPos.y);
		leftRow = dstPixelBits.begin() + (intersection.GetPos().y - dstPos.y);

		int offsetX = (intersection.GetPos().x - dstPos.x);
		leftUnusedMaskNum = offsetX >> 5;
		leftUnusedBitNum = 32 - (offsetX & 31);
	}

	// 一个32位int，每位表示该像素是否存在
	int rightRowMaskNum = intersection.GetSize().width >> 5;
	if ((intersection.GetSize().width & 31) != 0)
		rightRowMaskNum++;

	// 遍历相交区域，检查是否存在叠加像素
	for (int h = 0; h < intersection.GetSize().height; h++)
	{
		const auto& leftRowsData = *leftRow;
		const auto& rightRowsData = *rightRow;

		for (int w = 0; w < rightRowMaskNum; w++)
		{
			// rightMask:			xxxxxxxxxx					
			// leftMask:     yyyyyyyyyy
			// 相差位数             leftUnusedBits
			// leftMaskRMove           zzzzzzzzzzz
			int rightMask = rightRowsData[w];
			int leftMask = leftRowsData[w + leftUnusedMaskNum];	// 未考虑32位内的数据，所有leftValue实际位数偏前
			int rightMaskRMove = rightMask >> leftUnusedBitNum;
			int leftMaskRMove = 0x00;
			if (w < rightRowMaskNum - 1)
				leftMaskRMove = (leftRowsData[w + leftUnusedMaskNum + 1] >> leftUnusedBitNum);

			// 所以位与操作分厂两步
			// 1.xxxxxx右移leftUnusedBits位，可以与yyyy且操作，相当于xxx的左半相交部分
			// 2.zzzzz 右移leftUnusedBits位，可以与xxxxxx操作，相当于xxx的右半相交部分
			if ((rightMaskRMove & leftMask) |
				(leftMaskRMove & rightMask) != 0)
				return true;
		}
	}
	return false;
}

/**
*	\brief 根据srcData和srcClipRect重新计算当前Mask，以32个像素为一个
*	mask，多出的部分填充0
*/
void PixelCollision::RecomputeMask()
{
	mPixelBits.clear();

	int rowMaskNum = mSize.width >> 5;
	if ((mSize.width & 31) != 0)
		rowMaskNum++;

	mPixelBits.resize(mSize.height);
	for (int y = 0; y < mSize.height; y++)
	{
		auto& rowBits = mPixelBits[y];
		rowBits.resize(rowMaskNum);

		int bitIndex = -1;
		int mask = 0x00000000;
		for (int x = 0; x < mSize.width; x++)
		{
			if (mask == 0) {
				mask = 0x80000000;
				bitIndex++;
				rowBits[bitIndex] = 0x00;
			}

			if (true)
				rowBits[x] |= mask;

			mask >> 1;
		}
	}
}
