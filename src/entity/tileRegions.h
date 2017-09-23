#pragma once

#include"common\common.h"
#include"utils\grid.h"
#include"entity\tile.h"
#include<map>

class Map;

/**
*	\brief tile����
*
*	tile�����������tile entity(entities������tile entity)
*	tileͨ������ṹ���ٴ��ͼtile�ı�������
*/

class TileRegions
{
public:
	TileRegions(Map& map, int layer);

	void AddTile(const TileInfo& tileInfo);
	void Build();
	void Draw();

private:
	int mLayer;
	Map& mMap;
	std::vector<TileInfo> mTileInfos;
	util::Grid<std::shared_ptr<Tile> >mTiles;
};