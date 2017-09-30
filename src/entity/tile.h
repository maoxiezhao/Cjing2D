#pragma once

#include"entity\entity.h"
#include"entity\tilePattern.h"
#include"entity\groundInfo.h"

/**
*	\brief ͼ����Ϣ����
*/
class TileInfo
{
public:
	TileInfo() :mLayer(0), mPos(), mSize(0,0), mPatternID(0),
		mGround(Ground::GROUND_EMPTY), mPattern(nullptr){}

	int mLayer;
	Point2 mPos;
	Size mSize;
	int mPatternID;
	Ground mGround;
	const TilePattern* mPattern;
};


/**
*	\brief tile ��ͼ�Ļ�����ʾ��Ԫ
*/
class Tile : public Entity
{
public:
	Tile(const TileInfo& tileInfo);

	virtual void Build();
	virtual void Update();
	virtual void Draw();

	virtual EntityType GetEntityType()const;

private:
	int mPatternID;
	TilePattern& mPattern;
	SpritePtr mTileSprite;

};

using TilePtr = std::shared_ptr<Tile>;