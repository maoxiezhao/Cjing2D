#pragma once

#include"common\common.h"
#include"entity\groundInfo.h"
#include"game\sprite.h"

class Tileset;

/**
*	\brief ͼ��ģʽ����
*/
class TilePatternData
{
public:
	/**** **** setter/getter **** ****/
	void SetPos(const Point2& pos)
	{
		mPos = pos;
	}
	Point2 GetPos()const
	{
		return mPos;
	}
	void SetSize(const Size& size)
	{
		mSize = size;
	}
	Size GetSize()const
	{
		return mSize;
	}
	void SetPatternID(int id)
	{
		mPatternID = id;
	}
	int GetPatternID()const
	{
		return mPatternID;
	}
	void SetFrameRect(const std::vector<Rect>& frameRect)
	{
		mFramesRect = frameRect;
	}
	const std::vector<Rect>& GetFrameRect()const
	{
		return mFramesRect;
	}
	void SetGround(Ground ground)
	{
		mGround = ground;
	}
	Ground GetGround()const
	{
		return mGround;
	}

private:
	int mPatternID;
	Point2 mPos;
	Size mSize;
	std::vector<Rect> mFramesRect;
	Ground mGround;
};

/**
*	\brief ͼ��ģʽ
*/
class TilePattern
{
public:
	TilePattern(const TilePatternData& data);

	Point2 GetPos()const;
	void SetPos(const Point2& pos);
	Size GetSize()const;
	void SetSize(const Size& size);
	int GetPatternID()const;
	void SetPatternID(int patternID);
	virtual Rect GetTextureRect()const;
	void SetGround(Ground ground);
	Ground GetGround()const;

	virtual void Init();
	static void Update();

private:
	int mPatternID;
	Point2 mPos;
	Size mSize;
	Ground mGround;
};

inline Point2 TilePattern::GetPos()const
{
	return mPos;
}

inline void TilePattern::SetPos(const Point2 & pos)
{
	mPos = pos;
}

inline Size TilePattern::GetSize() const
{
	return mSize;
}

inline void TilePattern::SetSize(const Size & size)
{
	mSize = size;
}

inline int TilePattern::GetPatternID() const
{
	return mPatternID;
}

inline void TilePattern::SetPatternID(int patternID)
{
	mPatternID = patternID;
}

inline Rect TilePattern::GetTextureRect() const
{
	return Rect();
}

inline void TilePattern::SetGround(Ground ground)
{
	mGround = ground;
}

inline Ground TilePattern::GetGround() const
{
	return mGround;
}
