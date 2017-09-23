#pragma once

#include"common\common.h"
#include"utils\grid.h"
#include"entity\tile.h"
#include<map>

class Map;

/**
*	\brief tile区域
*
*	tile区域管理所有tile entity(entities不包含tile entity)
*	tile通过网格结构减少大地图tile的遍历次数
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