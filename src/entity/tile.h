#pragma once

#include"entity\entity.h"
#include"entity\tilePattern.h"

/**
*	\brief ͼ����Ϣ����
*/
class TileInfo
{
public:
	TileInfo() :mLayer(0), mPos(), mSize(0,0), mPatternID(0), mPattern(nullptr){}

	int mLayer;
	Point2 mPos;
	Size mSize;
	int mPatternID;
	const TilePattern* mPattern;
};


/**
*	\brief tile ��ͼ�Ļ�����ʾ��Ԫ
*/
class Tile : public Entity
{
public:
	Tile(const TileInfo& tileInfo);

	virtual void Update();
	virtual void Draw();
	virtual void Initalized();

private:
	int mPatternID;
	TilePattern& mPattern;
	SpritePtr mTileSprite;

};

using TilePtr = std::shared_ptr<Tile>;