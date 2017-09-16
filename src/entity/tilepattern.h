#pragma once

#include"common\common.h"
#include"game\sprite.h"

class Tileset;

/**
*	\brief 图块模式数据
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

private:
	int mPatternID;
	Point2 mPos;
	Size mSize;
};

/**
*	\brief 图块模式
*/
class TilePattern
{
public:
	TilePattern(const TilePatternData& data);

	virtual void Draw(const Point2& pos, const Size& size, const Tileset& tileset)const;
private:
	int mPatternID;
	Point2 mPos;
	Size mSize;

};